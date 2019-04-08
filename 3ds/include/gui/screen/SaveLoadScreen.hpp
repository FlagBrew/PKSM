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

#include <fcntl.h>
#include <errno.h>
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <malloc.h>
#include <memory>
#include <list>
#include "Screen.hpp"
#include "Title.hpp"
#include "Directory.hpp"
#include "Button.hpp"
#include "loader.hpp"

#ifndef SAVELOADSCREEN_HPP
#define SAVELOADSCREEN_HPP

#define PKSM_PORT 34567
#define SOC_ALIGN      0x1000
#define SOC_BUFFERSIZE 0x100000

class SaveLoadScreen : public Screen
{
public:
    SaveLoadScreen();
    ~SaveLoadScreen()
    {
        for (auto b : buttons)
        {
            delete b;
        }
    }
    void drawSelector(int x, int y) const;
    void draw() const override;
    ScreenType type() const override { return ScreenType::TITLELOAD; }
    void update(touchPosition* touch) override;

private:
    int saveGroup = 0;
    // Has to be mutable because no const operator[]
    mutable std::unordered_map<int, std::vector<std::pair<std::string, std::string>>> saves;
    int firstSave = 0;
    std::vector<Button*> buttons;
    int selectedSave = -1;
    bool selectedGroup = false;
    bool setSelectedSave(int i);
    bool increaseFirstSave()
    {
        if (firstSave < (int) saves[saveGroup].size() - 1)
        {
            firstSave++;
        }
        return false;
    }
    bool decreaseFirstSave()
    {
        if (firstSave > 0)
        {
            firstSave--;
        }
        return false;
    }
    static constexpr std::string_view titleName(int index);

    bool loadSave(void);
};

#endif
