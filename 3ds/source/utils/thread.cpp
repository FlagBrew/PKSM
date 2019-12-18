/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2019 Bernardo Giordano, Admiral Fish, piepie62
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
#include <3ds.h>
#include <list>

struct ThreadRecord
{
    ThreadRecord(void (*entrypoint)(void*), void* arg, Thread thread) : entrypoint(entrypoint), arg(arg), thread(thread) {}
    void (*entrypoint)(void*);
    void* arg;
    Thread thread;
    std::list<ThreadRecord>::iterator listPos;
};

static std::list<ThreadRecord> threads;
LightLock listLock;

void Threads::init()
{
    LightLock_Init(&listLock);
}

static void threadWrap(void* arg)
{
    ThreadRecord* record = (ThreadRecord*)arg;
    record->entrypoint(record->arg);
    LightLock_Lock(&listLock);
    std::list<ThreadRecord>::iterator it = record->listPos;
    threads.erase(it);
    LightLock_Unlock(&listLock);
}

bool Threads::create(void (*entrypoint)(void*), void* arg, std::optional<size_t> stackSize)
{
    s32 prio = 0;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    LightLock_Lock(&listLock);
    threads.emplace_front(entrypoint, arg, nullptr);
    threads.front().listPos = threads.begin();
    threads.front().thread  = threadCreate(threadWrap, (void*)&threads.front(), stackSize.value_or(4 * 1024), prio - 1, -2, true);
    if (!threads.front().thread)
    {
        threads.erase(threads.begin());
        LightLock_Unlock(&listLock);
        return false;
    }
    else
    {
        LightLock_Unlock(&listLock);
        return true;
    }
}

void Threads::exit(void)
{
    while (!threads.empty())
    {
        threadJoin(threads.back().thread, U64_MAX);
        // All detached, so no deallocation necessary
    }
    // All remove themselves, so no extra removal necessary
}
