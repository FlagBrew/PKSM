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

#ifndef LOADER_HPP
#define LOADER_HPP

#include <3ds.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Sav;
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
    bool load(std::shared_ptr<Title> title);
    bool load(std::shared_ptr<Title> title, const std::string& path);
    bool load(std::shared_ptr<u8[]> data, size_t size);
    void backupSave(const std::string& id);
    void saveChanges(void);
    void saveToTitle(bool ask);
    void init(void);
    void exit(void);

    // Title list
    inline std::vector<std::shared_ptr<Title>> nandTitles;
    inline std::shared_ptr<Title> cardTitle = nullptr;
    inline std::unordered_map<std::string, std::vector<std::string>> sdSaves;
    inline std::shared_ptr<Sav> save;
}

#endif
