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

#ifndef GROUPCLOUDACCESS_HPP
#define GROUPCLOUDACCESS_HPP

#include "GpssBrowser.hpp"
#include "pkx/PKX.hpp"
#include <memory>
#include <vector>

// The bundle half of GPSS: groups of up to six Pokémon. The paging behind it is GpssBrowser's,
// the same state machine the single-Pokémon browser uses.
class GroupCloudAccess
{
public:
    static constexpr int NUM_GROUPS = 5;

    GroupCloudAccess();

    std::vector<std::unique_ptr<pksm::PKX>> group(size_t groupIndex) const;
    // The group, plus a word to the server that it was downloaded
    std::vector<std::unique_ptr<pksm::PKX>> fetchGroup(size_t groupIndex) const;
    // Uploads, and answers with the HTTP status the server gave
    long group(std::vector<std::unique_ptr<pksm::PKX>> pokemon);
    std::unique_ptr<pksm::PKX> pkm(size_t groupIndex, size_t pkm) const;
    // A Pokémon without copying it, or nullptr if there is none there. For the draw path, which
    // asks for every Pokémon on the page every frame.
    const pksm::PKX* peek(size_t groupIndex, size_t pkm) const;
    bool isLegal(size_t groupIndex, size_t pkm) const;

    // Which page is on screen and how to turn it. Paging is the browser's job; this class only
    // says what a bundle on the page holds.
    GpssBrowser& paging() { return browser; }

    const GpssBrowser& paging() const { return browser; }

    void filterLegal(bool legal);

    bool filterLegal() const { return browser.query().legal; }

private:
    const GpssBrowser::Entry* entry(size_t groupIndex) const;
    const GpssBrowser::Mon* mon(size_t groupIndex, size_t pkm) const;

    GpssBrowser browser;
};

#endif
