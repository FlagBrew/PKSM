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

#include "app.hpp"
#include "gui.hpp"
#include "revision.h"
#include <3ds.h>

namespace
{
    class AbortException : public std::exception
    {
    public:
        AbortException() noexcept {}

        const char* what() const noexcept override { return "Abort called"; }
    };

    void consoleDisplayError(const char* message)
    {
        consoleInit(GFX_TOP, nullptr);
        printf("\x1b[2;16H\x1b[34mPKSM v%d.%d.%d-%s\x1b[0m", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO, GIT_REV);
        printf("\x1b[5;1HAn exception has occurred during execution\x1b[0m");
        printf("\x1b[8;1HDescription: \x1b[33m%s\x1b[0m", message);
        printf("\x1b[29;16HPress START to exit.");
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        while (aptMainLoop() && !(hidKeysDown() & KEY_START))
        {
            hidScanInput();
        }
    }

    bool prettyAbort = true;
}

// These are necessary because of the 3DS's bad exit semantics.
// We *have* to clean up; not doing so ends in terrible, terrible things happening (mainly infinite hangs)
extern "C" {
extern void __real_abort();

void __wrap_abort(void)
{
    if (prettyAbort)
    {
        prettyAbort = false;
        throw AbortException();
    }
    else
    {
        __real_abort();
    }
}
}

int main(int argc, char* argv[])
{
    Result res = App::init(argc > 0 ? argv[0] : "");
    if (R_FAILED(res))
    {
        App::exit();
        return res;
    }

    try
    {
        Gui::mainLoop();
    }
    catch (std::exception& e)
    {
        consoleDisplayError(e.what());
    }

    return App::exit();
}
