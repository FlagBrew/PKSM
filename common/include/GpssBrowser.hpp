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

#ifndef GPSSBROWSER_HPP
#define GPSSBROWSER_HPP

#include "GpssPage.hpp"
#include "pkx/PKX.hpp"
#include <atomic>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

// Paging over a GPSS search: which page is on screen, which two are being prefetched, and what
// happens when one of them fails. One state machine for both searches - the single-Pokémon
// browser and the bundle browser differ only in the Query they hand it.
//
// A page is published exactly once. The thread that fetches it decodes it in full and then makes
// it visible; nothing ever writes into a page a caller may be reading, and a page that is being
// refetched is a new page, not the old one emptied out.
class GpssBrowser
{
public:
    // Everything that decides which results come back. Two Queries that compare equal name the
    // same search, so changing one is what forces a refetch.
    struct Query
    {
        Gpss::Kind kind = Gpss::Kind::Pokemon;
        // Bundles are always newest-first; the single-Pokémon browser lets the user choose.
        bool popular          = false;
        bool ascending        = true;
        bool legal            = false;
        pksm::Generation low  = pksm::Generation::ONE;
        pksm::Generation high = pksm::Generation::EIGHT;
        bool lgpe             = true;

        bool operator==(const Query&) const = default;
    };

    // A page as the screens read it: decoded once, then immutable.
    struct Mon
    {
        std::unique_ptr<pksm::PKX> pkm;
        bool legal = false;
        std::string downloadCode;
    };

    struct Entry
    {
        std::string downloadCode;
        std::vector<Mon> pokemon;
    };

    struct Contents
    {
        int total = 0;
        int pages = 1;
        std::vector<Entry> entries;
    };

    explicit GpssBrowser(Query query);

    const Query& query() const { return searchQuery; }

    // Applies a new search and reloads from the current page. Does nothing if the search is the
    // same one.
    void query(const Query& query);

    bool good() const { return isGood; }

    // The code the server gave for the page that is on screen, or 0 if it never gave one.
    int errorCode() const;

    int page() const { return pageNumber; }

    int pages() const;

    // Valid whenever good(). An empty page when it isn't, so a caller that draws before it checks
    // reads empty rather than freed memory.
    const Contents& contents() const { return *currentContents(); }

    // Each returns nullopt on success, or the server's error code (0 if it gave none) when the
    // page could not be loaded, in which case good() is false.
    std::optional<int> nextPage();
    std::optional<int> prevPage();
    std::optional<int> jumpPage(int page);

    // Refetches the current page and its neighbours. Call it after an upload changes the results.
    void refresh();

    // The URL and POST body one page of a search lives at. Public because it is the whole
    // description of the request and worth testing on its own.
    static std::pair<std::string, std::string> makeURL(const Query& query, int page);

    // Tells the server that a download happened, for its counter. Fire and forget: nothing waits
    // on the answer, and nothing here reads it.
    static void countDownload(const std::string& what, const std::string& code);

private:
    // A page in flight. Written once by the fetching thread, then read by the caller's thread;
    // ready is what publishes it, so it is also what the caller waits on.
    struct Slot
    {
        std::atomic<bool> ready    = false;
        std::atomic<int> errorCode = 0;
        std::shared_ptr<const Contents> contents;
    };

    // Fetches page number into a fresh slot, in the background.
    std::shared_ptr<Slot> fetch(int number);
    // Fetches page number on this thread.
    std::shared_ptr<Slot> fetchNow(int number);
    // Blocks until the slot is published, then makes it the current page. Returns what
    // nextPage()/prevPage() return.
    std::optional<int> adopt(std::shared_ptr<Slot> slot, int number);
    std::shared_ptr<const Contents> currentContents() const;

    Query searchQuery;
    std::shared_ptr<Slot> current, next, prev;
    int pageNumber = 1;
    bool isGood    = false;
};

#endif
