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

#ifndef INJECTORSCREEN_HPP
#define INJECTORSCREEN_HPP

#include "HidHorizontal.hpp"
#include "Language.hpp"
#include "Sav.hpp"
#include "Screen.hpp"
#include "json.hpp"
#include <memory>
#include <string>
#include <vector>

class Button;
class WCX;

class InjectorScreen : public Screen
{
public:
    InjectorScreen(nlohmann::json ids);
    InjectorScreen(std::unique_ptr<WCX> card);
    void update(touchPosition* touch) override;
    void drawTop(void) const override;
    void drawBottom(void) const override;

private:
    bool setLanguage(Language lang);
    bool isLangAvailable(Language lang) const;
    void changeDate();

    std::vector<std::unique_ptr<Button>> buttons;
    std::unique_ptr<WCX> wondercard;
    std::string game;
    HidHorizontal hid;
    nlohmann::json ids;
    const std::vector<Sav::giftData> gifts;
    const int emptySlot;
    int slot;
    // For multi-item injects
    int item = 0;
    Language lang = Language::JP;
    bool overwriteCard = false;
    bool adaptLanguage = false;
    bool choosingSlot  = false;
};

#endif
