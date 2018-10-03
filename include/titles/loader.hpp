/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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
#include <vector>
#include <memory>
#include "Hid.hpp"
#include "gui.hpp"
#include "thread.hpp"
#include "Title.hpp"

#include "Sav.hpp"
#include "SavB2W2.hpp"
#include "SavBW.hpp"
#include "SavDP.hpp"
#include "SavHGSS.hpp"
#include "SavORAS.hpp"
#include "SavPT.hpp"
#include "SavSUMO.hpp"
#include "SavUSUM.hpp"
#include "SavXY.hpp"

namespace TitleLoader
{
    void scanTitles(void);
    void scanSaves(void);
    void load(std::shared_ptr<Title> title);
    void load(std::shared_ptr<Title> title, std::string path);
    void backupSave(void);
    void saveChanges(void);
    void exit(void);
    
    extern std::vector<std::shared_ptr<Title>> nandTitles;
    extern std::shared_ptr<Title> cardTitle;
    extern std::unordered_map<std::string, std::vector<std::string>> sdSaves;
    extern std::shared_ptr<Sav> save;
}

#endif