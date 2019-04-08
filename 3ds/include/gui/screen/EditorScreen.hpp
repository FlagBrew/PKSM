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

#ifndef EDITORSCREEN_HPP
#define EDITORSCREEN_HPP

#include "Screen.hpp"
#include "Button.hpp"
#include "ViewOverlay.hpp"
#include "PKX.hpp"

class EditorScreen : public Screen
{
public:
    ~EditorScreen()
    {
        // Delete shared back button, followed by all the others
        delete buttons[0][0];
        for (auto vec : buttons)
        {
            for (size_t i = 1; i < vec.size(); i++)
            {
                delete vec[i];
            }
        }
    }
    EditorScreen(std::shared_ptr<PKX> pkm, int box, int index);
    void draw() const override;
    void update(touchPosition* touch) override;
    ScreenType type() const override { return ScreenType::EDITOR; }
private:
    bool changeLevel(bool up);
    void setLevel();
    bool selectNature();
    bool selectAbility();
    bool selectItem();
    bool selectForm();
    bool selectBall();
    bool selectSpecies();
    bool togglePokerus();
    void setOT();
    void setNick();
    bool changeFriendship(bool up);
    void setFriendship();
    bool hexEdit();
    bool save();
    bool genderSwitch();
    void partyUpdate();
    
    bool changeIV(int which, bool up);
    void setIV(int which);
    // Either EV or Awakened value
    bool changeSecondaryStat(int which, bool up);
    void setSecondaryStat(int which);
    bool setHP();
    bool goBack();
    void changeMove();
    bool setSaveInfo();
    std::array<std::vector<Button*>, 3> buttons;
    std::shared_ptr<PKX> pkm;
    int currentTab = 0;
    int moveSelected = 0;
    int box = 0;
    int index = 0;
    int origPartyStats[6] = {0};
    int origPartyLevel = 0;
    int origPartyCurrHP = 0;
    int origPartyCP = 0;
    bool justSwitched = true;
    bool saved = true;
};

#endif
