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

#ifndef CLOUDACCESS_HPP
#define CLOUDACCESS_HPP

#include "json.hpp"
#include <memory>

class PKX;

class CloudAccess
{
public:
    enum SortType
    {
        LATEST,
        POPULAR
    };
    CloudAccess();
    std::shared_ptr<PKX> pkm(size_t slot) const;
    bool isLegal(size_t slot) const;
    // Gets the Pokémon and increments the server-side download counter
    std::shared_ptr<PKX> fetchPkm(size_t slot) const;
    long pkm(std::shared_ptr<PKX> pk);
    int pages() const { return current->data["pages"].get<int>(); }
    int page() const { return pageNumber; }
    bool nextPage();
    bool prevPage();
    void sortType(SortType type)
    {
        if (sort != type)
        {
            sort = type;
            refreshPages();
        }
    }
    SortType sortType() const { return sort; }
    void sortDir(bool ascend)
    {
        if (this->ascend != ascend)
        {
            this->ascend = ascend;
            refreshPages();
        }
    }
    bool sortAscending() const { return ascend; }
    void filterLegal(bool v)
    {
        if (v != legal)
        {
            legal = v;
            refreshPages();
        }
    }
    bool filterLegal() const { return legal; }
    bool good() const { return isGood; }
    static std::string makeURL(int page, SortType type, bool ascend, bool legal);
    nlohmann::json grabPage(int page);

private:
    struct Page
    {
        nlohmann::json data = {};
        bool available      = false;
    };
    struct PageDownloadInfo
    {
        PageDownloadInfo(std::shared_ptr<Page> page, int number, SortType type, bool ascend, bool legal)
            : page(page), number(number), type(type), ascend(ascend), legal(legal)
        {
        }
        std::shared_ptr<Page> page;
        int number;
        SortType type;
        bool ascend, legal;
    };
    void refreshPages();
    std::shared_ptr<Page> current;
    int pageNumber;
    SortType sort = LATEST;
    bool isGood = false;
    bool ascend   = true;
    bool legal    = false;

    friend void incrementPkmDownloadCount(std::string* num);
    friend void downloadCloudPage(CloudAccess::PageDownloadInfo* di);
};

#endif
