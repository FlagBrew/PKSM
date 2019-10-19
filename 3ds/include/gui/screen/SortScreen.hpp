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

#ifndef SORTSCREEN_HPP
#define SORTSCREEN_HPP

#include "Screen.hpp"

class Button;

class SortScreen : public Screen
{
public:
    enum class SortType
    {
        NONE,
        DEX,
        SPECIESNAME,
        FORM,
        TYPE1,
        TYPE2,
        HP,
        ATK,
        DEF,
        SATK,
        SDEF,
        SPE,
        HPIV,
        ATKIV,
        DEFIV,
        SATKIV,
        SDEFIV,
        SPEIV,
        NATURE,
        LEVEL,
        TID,
        HIDDENPOWER,
        FRIENDSHIP,
        NICKNAME,
        OTNAME,
        SHINY
    };

    SortScreen(bool storage);
    void drawTop() const override;
    void drawBottom() const override;
    void update(touchPosition* touch) override;

    static const char* sortTypeToString(SortType type)
    {
        switch (type)
        {
            case SortType::NONE:
            default:
                return "NONE";
            case SortType::DEX:
                return "SPECIES";
            case SortType::FORM:
                return "FORM";
            case SortType::TYPE1:
                return "TYPE1";
            case SortType::TYPE2:
                return "TYPE2";
            case SortType::HP:
                return "HP";
            case SortType::ATK:
                return "ATTACK";
            case SortType::DEF:
                return "DEFENSE";
            case SortType::SATK:
                return "SPATK";
            case SortType::SDEF:
                return "SPDEF";
            case SortType::SPE:
                return "SPEED";
            case SortType::HPIV:
                return "HP_IV";
            case SortType::ATKIV:
                return "ATTACK_IV";
            case SortType::DEFIV:
                return "DEFENSE_IV";
            case SortType::SATKIV:
                return "SPATK_IV";
            case SortType::SDEFIV:
                return "SPDEF_IV";
            case SortType::SPEIV:
                return "SPEED_IV";
            case SortType::NATURE:
                return "NATURE";
            case SortType::LEVEL:
                return "LEVEL";
            case SortType::TID:
                return "TID";
            case SortType::HIDDENPOWER:
                return "HIDDEN_POWER";
            case SortType::FRIENDSHIP:
                return "FRIENDSHIP";
            case SortType::NICKNAME:
                return "NICKNAME";
            case SortType::SPECIESNAME:
                return "SPECIES_NAME";
            case SortType::OTNAME:
                return "OT_NAME";
            case SortType::SHINY:
                return "SHINY";
        }
    }

private:
    void pickSort(size_t number);
    void sort();
    std::vector<std::unique_ptr<Button>> buttons;
    std::vector<SortType> sortTypes;
    bool justSwitched = true;
    bool storage;
};

#endif
