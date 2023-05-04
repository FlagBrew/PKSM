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
#include <3ds.h>
#include <algorithm>
#include <atomic>
#include <vector>

namespace
{
    constexpr int MIN_HANDLES = 2;
    Thread reaperThread;
    // Exit event, "update your list" event, and threads themselves
    Handle reaperThreadHandles[MIN_HANDLES + Threads::MAX_THREADS];
    Thread threads[Threads::MAX_THREADS];
    s32 currentThreads = 0;
    LightLock currentThreadsLock;

    void reapThread(void* arg)
    {
        while (true)
        {
            s32 signaledHandle;
            svcWaitSynchronizationN(
                &signaledHandle, reaperThreadHandles, MIN_HANDLES + currentThreads, false, U64_MAX);
            switch (signaledHandle)
            {
                case 0:
                    for (int i = 0; i < currentThreads; i++)
                    {
                        svcWaitSynchronization(reaperThreadHandles[MIN_HANDLES + i], U64_MAX);
                        threadFree(threads[i]);
                    }
                    return;
                case 1:
                    continue;
                default:
                    LightLock_Lock(&currentThreadsLock);
                    currentThreads--;
                    threadFree(threads[signaledHandle - 2]);
                    std::swap(threads[signaledHandle - 2], threads[currentThreads]);
                    reaperThreadHandles[signaledHandle] = 0xFFFFFFFF;
                    std::swap(reaperThreadHandles[signaledHandle],
                        reaperThreadHandles[MIN_HANDLES + currentThreads]);
                    LightLock_Unlock(&currentThreadsLock);
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
    LightLock_Init(&currentThreadsLock);
    if (R_FAILED(svcCreateEvent(&reaperThreadHandles[0], RESET_ONESHOT)))
    {
        return false;
    }
    if (R_FAILED(svcCreateEvent(&reaperThreadHandles[1], RESET_ONESHOT)))
    {
        return false;
    }
    s32 prio = 0;
    if (R_FAILED(svcGetThreadPriority(&prio, CUR_THREAD_HANDLE)))
    {
        return false;
    }
    reaperThread = threadCreate(reapThread, nullptr, 0x400, prio - 3, -2, false);
    if (!reaperThread)
    {
        return false;
    }

    LightSemaphore_Init(&moreTasks, 0, 10000);
    for (int i = 0; i < minWorkers; i++)
    {
        if (!Threads::create(WORKER_STACK, taskWorkerThread))
        {
            return false;
        }
    }
    return true;
}

bool Threads::create(void (*entrypoint)(void*), void* arg, std::optional<size_t> stackSize)
{
    if (currentThreads >= Threads::MAX_THREADS)
    {
        return false;
    }
    s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    Thread thread =
        threadCreate(entrypoint, arg, stackSize.value_or(DEFAULT_STACK), prio - 1, -2, false);

    if (thread)
    {
        LightLock_Lock(&currentThreadsLock);
        reaperThreadHandles[currentThreads++] = threadGetHandle(thread);
        LightLock_Unlock(&currentThreadsLock);
        svcSignalEvent(reaperThreadHandles[1]);
        return true;
    }

    return false;
}

void Threads::executeTask(void (*task)(void*), void* arg)
{
    workerTasks.lock()->emplace_back(task, arg);
    LightSemaphore_Release(&moreTasks, 1);
    if (numWorkers < maxWorkers && freeWorkers == 0)
    {
        Threads::create(WORKER_STACK, taskWorkerThread);
    }
}

void Threads::exit(void)
{
    workerTasks.lock()->clear();
    LightSemaphore_Release(&moreTasks, numWorkers);
    svcSignalEvent(reaperThreadHandles[0]);
    threadJoin(reaperThread, U64_MAX);
    threadFree(reaperThread);
    svcCloseHandle(reaperThreadHandles[0]);
    svcCloseHandle(reaperThreadHandles[1]);
}
