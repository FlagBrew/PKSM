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

#ifndef INJECTORSCREEN_HPP
#define INJECTORSCREEN_HPP

#include "Hid.hpp"
#include "Language.hpp"
#include "Sav.hpp"
#include "Screen.hpp"
#include "mysterygift.hpp"
#include "nlohmann/json_fwd.hpp"
#include "wcx/WCX.hpp"
#include <memory>
#include <string>
#include <vector>

class Button;

class InjectorScreen : public Screen
{
public:
    InjectorScreen(nlohmann::json ids);
    InjectorScreen(std::unique_ptr<pksm::WCX> card);
    ~InjectorScreen();
    void update(touchPosition* touch) override;
    void drawTop(void) const override;
    void drawBottom(void) const override;

private:
    bool setLanguage(pksm::Language lang);
    bool isLangAvailable(pksm::Language lang) const;
    void changeDate();

    std::vector<std::unique_ptr<Button>> buttons;
    std::unique_ptr<pksm::WCX> wondercard;
    std::string game;
    Hid<HidDirection::HORIZONTAL, HidDirection::HORIZONTAL> hid;
    std::unique_ptr<nlohmann::json> ids;
    std::vector<MysteryGift::giftData> gifts;
    int emptySlot;
    int slot;
    // For multi-item injects
    int item            = 0;
    pksm::Language lang = pksm::Language::JPN;
    bool overwriteCard  = false;
    bool adaptLanguage  = false;
    bool choosingSlot   = false;
};

#endif
