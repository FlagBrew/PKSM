/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef GPSSSCREEN_HPP
#define GPSSSCREEN_HPP

#include "GpssBrowser.hpp"
#include "pkx/PKFilter.hpp"
#include "pkx/PKX.hpp"
#include "Screen.hpp"
#include <array>
#include <memory>
#include <optional>
#include <string>

class Button;

// What the two GPSS browsers do identically: where the cursor is, which bank box is under it, how
// a page turn is asked for, and what is said when one fails. The screens themselves differ in
// what a slot on the page holds - one Pokémon, or a bundle of them - and in little else.
class GpssScreen : public Screen
{
protected:
    GpssScreen(
        const std::string& instructions, std::shared_ptr<pksm::PKFilter> filter, int storageBox);

    // The paging the screen is browsing. Its access object owns it.
    virtual GpssBrowser& paging()             = 0;
    virtual const GpssBrowser& paging() const = 0;

    // Picks up what is under the cursor, or puts down what is being carried.
    virtual void pickup() = 0;

    bool showViewer();
    bool dumpPkm();
    bool prevBox(bool forceBottom = false);
    bool nextBox(bool forceBottom = false);
    bool prevBoxTop();
    bool nextBoxTop();
    bool jumpBoxTopBy(int delta);
    bool jumpBoxTop();
    bool clickBottomIndex(int index);

    // GPSS could not give us the page: says why - the server's code, or nothing when it never
    // answered - and leaves the browser. True when it did, which is when the caller has to stop
    // touching the page.
    bool leftOnPageError(std::optional<int> error);

    // Says what the server made of an upload. Silent when it took it.
    void reportUploadStatus(long status);

    std::array<std::unique_ptr<Button>, 7> mainButtons;
    std::array<std::unique_ptr<Button>, 31> clickButtons;
    std::unique_ptr<pksm::PKX> infoMon = nullptr;
    std::shared_ptr<pksm::PKFilter> filter;
    int cursorIndex           = 0;
    int storageBox            = 0;
    int pendingPageJumpFrames = 0;
    bool justSwitched         = true;
    bool cloudChosen          = false;
    bool pendingPageJump      = false;
    std::string websiteURL;
};

#endif
