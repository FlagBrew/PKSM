/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef THREAD_HPP
#define THREAD_HPP

#include "coretypes.h"
#include <optional>

namespace Threads
{
    static constexpr int MAX_THREADS = 32;

    bool init(u8 workers);
    // stackSize will be ignored on systems that don't provide explicit setting of it. KEEP THIS IN
    // MIND IF YOU ARE PORTING
    bool create(void (*entrypoint)(void*), void* arg = nullptr,
        std::optional<size_t> stackSize = std::nullopt);
    // Executes task on a worker thread with stack size of 0x8000 (if settable).
    void executeTask(void (*task)(void*), void* arg);
    void exit(void);
}

#endif
