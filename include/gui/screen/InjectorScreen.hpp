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

#include "Screen.hpp"
#include "WCX.hpp"
#include "Button.hpp"
#include "i18n.hpp"
#include "Configuration.hpp"
#include "Hid.hpp"
#include "mysterygift.hpp"
#include <vector>

#ifndef INJECTORSCREEN_HPP
#define INJECTORSCREEN_HPP

class InjectorScreen : public Screen
{
public:
    InjectorScreen(std::unique_ptr<WCX> card, MysteryGift::giftData& data);
    void update(touchPosition* touch) override;
    void draw(void) const override;
    ScreenType type() const override { return ScreenType::INJECTOR; }
private:
    std::vector<Button*> buttons;
    std::unique_ptr<WCX> wondercard;
    std::string game;
    bool overwriteCard = false;
    bool adaptLanguage = false;
    bool choosingSlot = false;
    int slot;
    // For multi-item injects
    int item = 0;
    Hid hid;
    Language lang = Language::JP;

    bool setLanguage(Language lang);
};

#endif