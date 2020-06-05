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
    std::unordered_map<pksm::Language, nlohmann::json> gui;

    void load(pksm::Language lang, const std::string& name, nlohmann::json& json)
    {
        std::string path = io::exists(_PKSMCORE_LANG_FOLDER + folder(lang) + name) ? _PKSMCORE_LANG_FOLDER + folder(lang) + name
                                                                                   : _PKSMCORE_LANG_FOLDER + folder(pksm::Language::ENG) + name;

        FILE* values = fopen(path.c_str(), "rt");
        if (values)
        {
            json = nlohmann::json::parse(values, nullptr, false);
            fclose(values);
        }
    }

    void initGui(pksm::Language lang)
    {
        nlohmann::json j;
        load(lang, "/gui.json", j);
        gui.insert_or_assign(lang, std::move(j));
    }

    void exitGui(pksm::Language lang) { gui.erase(lang); }

    const std::string& localize(pksm::Language lang, const std::string& v)
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

    const std::string& pouch(pksm::Language lang, pksm::Sav::Pouch pouch)
    {
        checkInitialized(lang);
        switch (pouch)
        {
            case pksm::Sav::Pouch::NormalItem:
                return localize(lang, "ITEMS");
            case pksm::Sav::Pouch::KeyItem:
                return localize(lang, "KEY_ITEMS");
            case pksm::Sav::Pouch::Ball:
                return localize(lang, "BALLS");
            case pksm::Sav::Pouch::TM:
                return localize(lang, "TMHM");
            case pksm::Sav::Pouch::Berry:
                return localize(lang, "BERRIES");
            case pksm::Sav::Pouch::PCItem:
                return localize(lang, "PC_ITEMS");
            case pksm::Sav::Pouch::Mail:
                return i18n::localize(lang, "MAIL");
            case pksm::Sav::Pouch::Medicine:
                return i18n::localize(lang, "MEDICINE");
            case pksm::Sav::Pouch::Battle:
                return i18n::localize(lang, "BATTLE_ITEMS");
            case pksm::Sav::Pouch::ZCrystals:
                return i18n::localize(lang, "ZCRYSTALS");
            case pksm::Sav::Pouch::RotomPower:
                return i18n::localize(lang, "ROTOM_POWERS");
            case pksm::Sav::Pouch::Candy:
                return i18n::localize(lang, "CANDIES");
            case pksm::Sav::Pouch::CatchingItem:
                return i18n::localize(lang, "CATCHING_ITEMS");
            case pksm::Sav::Pouch::Treasure:
                return i18n::localize(lang, "TREASURES");
            case pksm::Sav::Pouch::Ingredient:
                return i18n::localize(lang, "INGREDIENTS");
        }
        return emptyString;
    }

    const std::string& badTransfer(pksm::Language lang, pksm::Sav::BadTransferReason reason)
    {
        checkInitialized(lang);
        switch (reason)
        {
            case pksm::Sav::BadTransferReason::MOVE:
                return i18n::localize(lang, "STORAGE_BAD_MOVE");
            case pksm::Sav::BadTransferReason::SPECIES:
                return i18n::localize(lang, "STORAGE_BAD_SPECIES");
            case pksm::Sav::BadTransferReason::FORM:
                return i18n::localize(lang, "STORAGE_BAD_FORM");
            case pksm::Sav::BadTransferReason::ABILITY:
                return i18n::localize(lang, "STORAGE_BAD_ABILITY");
            case pksm::Sav::BadTransferReason::ITEM:
                return i18n::localize(lang, "STORAGE_BAD_ITEM");
            case pksm::Sav::BadTransferReason::BALL:
                return i18n::localize(lang, "STORAGE_BAD_BALL");
            default:
                break;
        }
        return emptyString;
    }

    const std::string& language(pksm::Language lang)
    {
        static const std::string JPN = "日本語";
        static const std::string ENG = "English";
        static const std::string FRE = "Français";
        static const std::string GER = "Deutsch";
        static const std::string ITA = "Italiano";
        static const std::string SPA = "Español";
        static const std::string CHN = "中文";
        static const std::string KOR = "한국어";
        static const std::string NL  = "Nederlands";
        static const std::string PT  = "Português";
        static const std::string RO  = "Română";
        switch (lang)
        {
            case pksm::Language::JPN:
                return JPN;
            case pksm::Language::ENG:
                return ENG;
            case pksm::Language::FRE:
                return FRE;
            case pksm::Language::GER:
                return GER;
            case pksm::Language::ITA:
                return ITA;
            case pksm::Language::SPA:
                return SPA;
            case pksm::Language::CHS:
            case pksm::Language::CHT:
                return CHN;
            case pksm::Language::KOR:
                return KOR;
            case pksm::Language::NL:
                return NL;
            case pksm::Language::PT:
                return PT;
            case pksm::Language::RO:
                return RO;
        }
        return emptyString;
    }
}
