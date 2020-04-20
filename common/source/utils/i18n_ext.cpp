/*
 *   This file is part of PKSM-Core
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

#include "i18n_ext.hpp"
#include "../../../core/source/i18n/i18n_internal.hpp"
#include "nlohmann/json.hpp"

namespace i18n
{
    std::unordered_map<Language, nlohmann::json> gui;

    void initGui(Language lang)
    {
        nlohmann::json j;
        load(lang, "/gui.json", j);
        gui.insert_or_assign(lang, std::move(j));
    }

    void exitGui(Language lang) { gui.erase(lang); }

    const std::string& localize(Language lang, const std::string& v)
    {
        checkInitialized(lang);
        auto it = gui.find(lang);
        if (it != gui.end())
        {
            if (!it->second.contains(v))
            {
                it->second[v] = "MISSING: " + v;
            }
            return it->second[v].get_ref<const std::string&>();
        }
        return emptyString;
    }

    const std::string& pouch(Language lang, Sav::Pouch pouch)
    {
        checkInitialized(lang);
        switch (pouch)
        {
            case Sav::Pouch::NormalItem:
                return localize(lang, "ITEMS");
            case Sav::Pouch::KeyItem:
                return localize(lang, "KEY_ITEMS");
            case Sav::Pouch::Ball:
                return localize(lang, "BALLS");
            case Sav::Pouch::TM:
                return localize(lang, "TMHM");
            case Sav::Pouch::Berry:
                return localize(lang, "BERRIES");
            case Sav::Pouch::PCItem:
                return localize(lang, "PC_ITEMS");
            case Sav::Pouch::Mail:
                return i18n::localize(lang, "MAIL");
            case Sav::Pouch::Medicine:
                return i18n::localize(lang, "MEDICINE");
            case Sav::Pouch::Battle:
                return i18n::localize(lang, "BATTLE_ITEMS");
            case Sav::Pouch::ZCrystals:
                return i18n::localize(lang, "ZCRYSTALS");
            case Sav::Pouch::RotomPower:
                return i18n::localize(lang, "ROTOM_POWERS");
            case Sav::Pouch::Candy:
                return i18n::localize(lang, "CANDIES");
            case Sav::Pouch::CatchingItem:
                return i18n::localize(lang, "CATCHING_ITEMS");
            case Sav::Pouch::Treasure:
                return i18n::localize(lang, "TREASURES");
            case Sav::Pouch::Ingredient:
                return i18n::localize(lang, "INGREDIENTS");
        }
        return emptyString;
    }

    const std::string& badTransfer(Language lang, Sav::BadTransferReason reason)
    {
        checkInitialized(lang);
        switch (reason)
        {
            case Sav::BadTransferReason::MOVE:
                return i18n::localize(lang, "STORAGE_BAD_MOVE");
            case Sav::BadTransferReason::SPECIES:
                return i18n::localize(lang, "STORAGE_BAD_SPECIES");
            case Sav::BadTransferReason::FORM:
                return i18n::localize(lang, "STORAGE_BAD_FORM");
            case Sav::BadTransferReason::ABILITY:
                return i18n::localize(lang, "STORAGE_BAD_ABILITY");
            case Sav::BadTransferReason::ITEM:
                return i18n::localize(lang, "STORAGE_BAD_ITEM");
            case Sav::BadTransferReason::BALL:
                return i18n::localize(lang, "STORAGE_BAD_BALL");
            default:
                break;
        }
        return emptyString;
    }
}
