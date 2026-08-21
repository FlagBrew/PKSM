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

#ifndef CLOUDACCESS_HPP
#define CLOUDACCESS_HPP

#include "GpssBrowser.hpp"
#include "pkx/PKX.hpp"
#include <memory>

// The single-Pokémon half of GPSS: what a slot on the current page holds, and how to put a
// Pokémon up there. The paging behind it is GpssBrowser's.
class CloudAccess
{
public:
    enum SortType
    {
        LATEST,
        POPULAR
    };

    CloudAccess();

    std::unique_ptr<pksm::PKX> pkm(size_t slot) const;
    // The Pokémon in a slot without copying it, or nullptr if the slot is empty. For the draw
    // path, which asks for all thirty of them every frame.
    const pksm::PKX* peek(size_t slot) const;
    bool isLegal(size_t slot) const;
    // The Pokémon, plus a word to the server that it was downloaded
    std::unique_ptr<pksm::PKX> fetchPkm(size_t slot) const;
    // Uploads, and answers with the HTTP status the server gave
    long pkm(std::unique_ptr<pksm::PKX> pk);

    // Which page is on screen and how to turn it. Paging is the browser's job; this class only
    // says what a slot on the page holds.
    GpssBrowser& paging() { return browser; }

    const GpssBrowser& paging() const { return browser; }

    void sortType(SortType type);

    SortType sortType() const { return browser.query().popular ? POPULAR : LATEST; }

    void sortDir(bool ascend);

    bool sortAscending() const { return browser.query().ascending; }

    void filterLegal(bool legal);

    bool filterLegal() const { return browser.query().legal; }

private:
    // The Pokémon in a slot, or nullptr if the page has no such slot.
    const GpssBrowser::Mon* mon(size_t slot) const;

    GpssBrowser browser;
};

#endif
