/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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

#include "enums/Generation.hpp"
#include "nlohmann/json_fwd.hpp"
#include "pkx/PKX.hpp"
#include <atomic>
#include <memory>
#include <optional>

class GroupCloudAccess
{
public:
    static constexpr int NUM_GROUPS = 5;
    GroupCloudAccess();
    std::vector<std::unique_ptr<pksm::PKX>> group(size_t groupIndex) const;
    std::vector<std::unique_ptr<pksm::PKX>> fetchGroup(size_t groupIndex) const;
    long group(std::vector<std::unique_ptr<pksm::PKX>> pokemon);
    std::unique_ptr<pksm::PKX> pkm(size_t groupIndex, size_t pkm) const;
    std::unique_ptr<pksm::PKX> fetchPkm(size_t groupIndex, size_t pkm) const;
    bool isLegal(size_t groupIndex, size_t pkm) const;

    int pages() const;
    int page() const { return pageNumber; }
    std::optional<int> nextPage();
    std::optional<int> prevPage();

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
    int currentPageError() const { return current->siteJsonErrorCode; }
    nlohmann::json grabPage(int page);
    static std::pair<std::string, std::string> makeURL(
        int page, bool legal, pksm::Generation low, pksm::Generation high, bool LGPE);

private:
    struct Page
    {
        ~Page();
        std::unique_ptr<nlohmann::json> data;
        std::atomic<bool> available        = false;
        std::atomic<int> siteJsonErrorCode = 0;
    };
    void refreshPages();
    static void downloadGroupPage(std::shared_ptr<Page> page, int number, bool legal,
        pksm::Generation low, pksm::Generation high, bool LGPE);
    static bool pageIsGood(const nlohmann::json& page);
    std::shared_ptr<Page> current, next, prev;
    int pageNumber;
    bool isGood = false;
    bool legal  = false;
    // Currently not changeable
    pksm::Generation high = pksm::Generation::EIGHT;
    pksm::Generation low  = pksm::Generation::THREE;
    bool LGPE             = true;
};

#endif
