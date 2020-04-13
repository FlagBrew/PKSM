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

#ifndef GROUPCLOUDACCESS_HPP
#define GROUPCLOUDACCESS_HPP

#include "Generation.hpp"
#include "nlohmann/json_fwd.hpp"
#include <atomic>
#include <memory>

class PKX;

class GroupCloudAccess
{
public:
    static constexpr int NUM_GROUPS = 5;
    GroupCloudAccess();
    std::vector<std::shared_ptr<PKX>> group(size_t groupIndex) const;
    std::vector<std::shared_ptr<PKX>> fetchGroup(size_t groupIndex) const;
    long group(std::vector<std::shared_ptr<PKX>> pokemon);
    std::shared_ptr<PKX> pkm(size_t groupIndex, size_t pkm) const;
    std::shared_ptr<PKX> fetchPkm(size_t groupIndex, size_t pkm) const;
    bool isLegal(size_t groupIndex, size_t pkm) const;

    int pages() const;
    int page() const { return pageNumber; }
    bool nextPage();
    bool prevPage();

    bool filterLegal() const { return legal; }
    void filterLegal(bool v)
    {
        if (v != legal)
        {
            legal = v;
            refreshPages();
        }
    }

    bool good() const { return isGood; }
    nlohmann::json grabPage(int page);
    static std::string makeURL(int page, bool legal, Generation low, Generation high, bool LGPE);

private:
    struct Page
    {
        ~Page();
        std::unique_ptr<nlohmann::json> data;
        std::atomic<bool> available = false;
    };
    void refreshPages();
    static void downloadGroupPage(std::shared_ptr<Page> page, int number, bool legal, Generation low, Generation high, bool LGPE);
    std::shared_ptr<Page> current, next, prev;
    int pageNumber;
    bool isGood = false;
    bool legal  = false;
    // Currently not changeable
    Generation high = Generation::EIGHT;
    Generation low  = Generation::THREE;
    bool LGPE       = true;
};

#endif
