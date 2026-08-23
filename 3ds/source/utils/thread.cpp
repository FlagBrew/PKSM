/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
 *       * Requiring preservation of specified reasonable legal notices or
 *         author attributions in that material or in the Appropriate Legal
 *         Notices displayed by works containing it.
 *       * Prohibiting misrepresentation of the origin of that material,
 *         or requiring that modified versions of such material be marked in
 *         reasonable ways as different from the original version.
 */

#include "thread.hpp"
#include "DataMutex.hpp"
#include "SmallVector.hpp"
#include <3ds.h>
#include <algorithm>
#include <atomic>
#include <vector>

namespace
{
    constexpr int MIN_HANDLES           = 2;
    constexpr s32 HIGHEST_USER_PRIORITY = 0x18;
    constexpr s32 LOWEST_USER_PRIORITY  = 0x3F;
    Thread reaperThread;
    // Exit event, "update your list" event, and threads themselves
    DataMutex<std::pair<SmallVector<Thread, Threads::MAX_THREADS>,
        SmallVector<Handle, MIN_HANDLES + Threads::MAX_THREADS>>>
        threads;

    void reapThread(void* arg)
    {
        while (true)
        {
            s32 signaledHandle;
            {
                u32 size;
                const Handle* handles;
                // Letting the lock expire is fine because the only thing that could happen between
                // then and the use is adding a handle, which won't change anything since we only
                // use the original size
                // In an ideal world, svcWaitSynchronizationN would atomically release and regain
                // the lock, but we can't have nice things
                {
                    auto lockedThreadData                            = threads.lock();
                    const auto& [lockedThreads, reaperThreadHandles] = *lockedThreadData;
                    size                                             = lockedThreads.size();
                    handles                                          = reaperThreadHandles.data();
                }
                svcWaitSynchronizationN(
                    &signaledHandle, handles, MIN_HANDLES + size, false, U64_MAX);
            }
            switch (signaledHandle)
            {
                case 0:
                {
                    auto lockedThreads = threads.lock();
                    for (size_t i = 0; i < lockedThreads->first.size(); i++)
                    {
                        svcWaitSynchronization(lockedThreads->second[MIN_HANDLES + i], U64_MAX);
                        threadFree(lockedThreads->first[i]);
                    }
                    return;
                }
                case 1:
                    continue;
                default:
                {
                    auto lockedThreads = threads.lock();
                    threadFree(lockedThreads->first[signaledHandle - 2]);
                    lockedThreads->first.erase(lockedThreads->first.begin() + signaledHandle - 2);
                    lockedThreads->second.erase(lockedThreads->second.begin() + signaledHandle);
                }
                break;
            }
        }
    }

    struct Task
    {
        void (*entrypoint)(void*);
        void* arg;
    };

    DataMutex<std::vector<Task>> workerTasks;
    LightSemaphore moreTasks;
    std::atomic<u8> numWorkers  = 0;
    std::atomic<u8> freeWorkers = 0;
    u8 maxWorkers               = 0;
    u8 minWorkers               = 0;

    bool createThread(
        void (*entrypoint)(void*), void* arg, std::optional<size_t> stackSize, int priority)
    {
        auto lockedThreads = threads.lock();
        if (lockedThreads->first.size() >= Threads::MAX_THREADS)
        {
            return false;
        }
        Thread thread = threadCreate(
            entrypoint, arg, stackSize.value_or(Threads::DEFAULT_STACK), priority, -2, false);

        if (thread)
        {
            lockedThreads->first.emplace_back(thread);
            lockedThreads->second.emplace_back(threadGetHandle(thread));
            svcSignalEvent(lockedThreads->second[1]);
            return true;
        }

        return false;
    }

    bool createRelative(
        void (*entrypoint)(void*), void* arg, std::optional<size_t> stackSize, s32 priorityOffset)
    {
        s32 callerPriority;
        if (R_FAILED(svcGetThreadPriority(&callerPriority, CUR_THREAD_HANDLE)))
        {
            return false;
        }
        return createThread(entrypoint, arg, stackSize,
            std::clamp(
                callerPriority + priorityOffset, HIGHEST_USER_PRIORITY, LOWEST_USER_PRIORITY));
    }

    void taskWorkerThread()
    {
        numWorkers++;
        while (true)
        {
            if (LightSemaphore_TryAcquire(&moreTasks, 1))
            {
                if (numWorkers <= minWorkers)
                {
                    freeWorkers++;
                    LightSemaphore_Acquire(&moreTasks, 1);
                    freeWorkers--;
                }
                else
                {
                    break;
                }
            }

            Task t = std::invoke(
                []
                {
                    auto tasks = workerTasks.lock();
                    if (tasks->size() == 0)
                    {
                        return Task{nullptr, nullptr};
                    }
                    else
                    {
                        Task ret = tasks.get()[0];
                        tasks->erase(tasks->begin());
                        return ret;
                    }
                });

            if (!t.entrypoint)
            {
                break;
            }

            t.entrypoint(t.arg);
        }
        numWorkers--;
    }
}

bool Threads::init(u8 min, u8 max)
{
    minWorkers = min;
    maxWorkers = max;

    {
        auto lockedThreads = threads.lock();
        auto& handles      = lockedThreads->second;

        // A partial init leaves nothing behind. The caller is told the subsystem never came
        // up, so its teardown never runs - which is what used to happen here, against a
        // handle vector left shorter than the two events Threads::exit indexes.
        const auto unwind = [&handles]
        {
            for (auto& handle : handles)
            {
                svcCloseHandle(handle);
            }
            handles.clear();
            return false;
        };

        handles.emplace_back();
        if (R_FAILED(svcCreateEvent(&handles[0], RESET_ONESHOT)))
        {
            handles.clear();
            return false;
        }
        handles.emplace_back();
        if (R_FAILED(svcCreateEvent(&handles[1], RESET_ONESHOT)))
        {
            handles.pop_back();
            return unwind();
        }
        s32 prio = 0;
        if (R_FAILED(svcGetThreadPriority(&prio, CUR_THREAD_HANDLE)))
        {
            return unwind();
        }
        reaperThread = threadCreate(reapThread, nullptr, 0x400, prio - 3, -2, false);
        if (!reaperThread)
        {
            return unwind();
        }

        LightSemaphore_Init(&moreTasks, 0, 10000);
    }

    // Outside the lock: Threads::background takes it too, so spawning the initial workers while
    // still holding it deadlocked for any min above zero.
    for (int i = 0; i < minWorkers; i++)
    {
        if (!Threads::background(WORKER_STACK, taskWorkerThread))
        {
            // The reaper is up by now, so this unwinds the same way any shutdown does.
            Threads::exit();
            return false;
        }
    }
    return true;
}

bool Threads::background(void (*entrypoint)(void*), void* arg, std::optional<size_t> stackSize)
{
    return createRelative(entrypoint, arg, stackSize, 1);
}

bool Threads::foreground(void (*entrypoint)(void*), void* arg, std::optional<size_t> stackSize)
{
    return createRelative(entrypoint, arg, stackSize, -1);
}

bool Threads::atPriority(
    void (*entrypoint)(void*), void* arg, std::optional<size_t> stackSize, int priority)
{
    return createThread(entrypoint, arg, stackSize, priority);
}

void Threads::executeTask(void (*task)(void*), void* arg)
{
    workerTasks.lock()->emplace_back(task, arg);
    LightSemaphore_Release(&moreTasks, 1);
    if (numWorkers < maxWorkers && freeWorkers == 0)
    {
        Threads::background(WORKER_STACK, taskWorkerThread);
    }
}

void Threads::exit(void)
{
    if (!reaperThread)
    {
        return;
    }
    workerTasks.lock()->clear();
    LightSemaphore_Release(&moreTasks, numWorkers);
    svcSignalEvent(threads.lock()->second[0]);
    threadJoin(reaperThread, U64_MAX);
    threadFree(reaperThread);
    reaperThread = nullptr;

    auto lockedThreads = threads.lock();
    // Only the two events are owned here. The rest are thread handles belonging to the
    // Thread objects the reaper has already freed.
    svcCloseHandle(lockedThreads->second[0]);
    svcCloseHandle(lockedThreads->second[1]);
    lockedThreads->second.clear();
    lockedThreads->first.clear();
}
