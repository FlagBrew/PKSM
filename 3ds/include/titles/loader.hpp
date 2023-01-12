/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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

#ifndef LOADER_HPP
#define LOADER_HPP

#include "sav/Sav.hpp"
#include <3ds.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Title;

bool isLoadedSaveFromBridge(void);
bool receiveSaveFromBridge(void);
bool sendSaveToBridge(void);
void setLoadedSaveFromBridge(bool v);
void backupBridgeChanges(void);

namespace TitleLoader
{
    void scanTitles(void);
    bool scanCard(void);
    bool cardWasUpdated(void);
    void scanSaves(void);
    bool load(const std::shared_ptr<Title>& title);
    bool load(const std::shared_ptr<Title>& title, const std::string& path);
    bool load(const std::shared_ptr<u8[]>& data, size_t size);
    void backupSave(const std::string& id);
    void saveChanges(void);
    void saveToTitle(bool ask);
    void setRebootToTitle(void);
    bool titleIsRebootable(void);
    void init(void);
    void exit(void);
    std::string savePath(void);
    void reloadTitleIds(void);

    // Title lists
    // Note that there can only be up to 12 installed titles of either type, which means a threaded
    // push_back can only edit the end pointer and will not change the storage used
    inline std::vector<std::shared_ptr<Title>> ctrTitles = std::invoke(
        []()
        {
            std::vector<std::shared_ptr<Title>> ret;
            ret.reserve(12);
            return ret;
        });
    inline std::vector<std::shared_ptr<Title>> vcTitles = std::invoke(
        []()
        {
            std::vector<std::shared_ptr<Title>> ret;
            ret.reserve(12);
            return ret;
        });
    inline std::shared_ptr<Title> cardTitle = nullptr;
    inline std::unordered_map<std::string, std::vector<std::string>> sdSaves;
    inline std::shared_ptr<pksm::Sav> save;
}

#endif
