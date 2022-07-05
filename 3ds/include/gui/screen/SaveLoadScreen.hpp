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

#ifndef SAVELOADSCREEN_HPP
#define SAVELOADSCREEN_HPP

#include "Screen.hpp"
#include "enums/Language.hpp"
#include <arpa/inet.h>
#include <array>
#include <errno.h>
#include <fcntl.h>
#include <list>
#include <malloc.h>
#include <memory>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

#define SOC_ALIGN 0x1000
#define SOC_BUFFERSIZE 0x100000

class Button;

class SaveLoadScreen : public Screen
{
public:
    SaveLoadScreen();
    void drawTop() const override;
    void drawBottom() const override;
    void update(touchPosition* touch) override;
    void makeInstructions();

private:
    bool setSelectedSave(int i);
    bool loadSave(void);
    void updateTitles(void);

    // Has to be mutable because no const operator[]
    static constexpr size_t NUM_GROUPS = 12;
    std::array<std::vector<std::pair<std::string, std::string>>, NUM_GROUPS> saves;
    std::vector<std::unique_ptr<Button>> buttons;
    int firstSave    = 0;
    int selectedSave = -1;
    int saveGroup    = 0;
    pksm::Language oldLang;
    bool selectedGroup = false;

    bool mustUpdateTitles = false;
};

#endif
