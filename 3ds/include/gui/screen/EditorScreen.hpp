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

#include "Button.hpp"
#include "PKX.hpp"
#include "Screen.hpp"
#include "ViewOverlay.hpp"
#include "sha256.h"

class EditorScreen : public Screen
{
public:
    ~EditorScreen()
    {
        for (auto button : buttons)
        {
            delete button;
        }
    }
    EditorScreen(std::shared_ptr<PKX> pkm, int box, int index, bool emergency = false);
    // Done with Overlay
    void drawTop() const override {}
    void drawBottom() const override;
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

    bool goBack();
    bool advanceMon(bool forward);
    bool setSaveInfo();
    bool saved();
    std::vector<Button*> buttons;
    std::shared_ptr<PKX> pkm;
    std::array<u8, SHA256_BLOCK_SIZE> origHash;
    int box               = 0;
    int index             = 0;
    int origPartyStats[6] = {0};
    int origPartyLevel    = 0;
    int origPartyCurrHP   = 0;
    int origPartyCP       = 0;
    bool justSwitched     = true;
    bool emergency;
};

#endif
