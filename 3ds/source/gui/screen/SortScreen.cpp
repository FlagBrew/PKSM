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

#include "SortScreen.hpp"
#include "ClickButton.hpp"
#include "banks.hpp"
#include "gui.hpp"
#include "loader.hpp"

SortScreen::SortScreen(bool storage) : storage(storage)
{
    for (int i = 0; i < 5; i++)
    {
        buttons.push_back(std::make_unique<ClickButton>(106, 15 + i * 35, 108, 28,
            [this, i]() {
                pickSort(i);
                return false;
            },
            ui_sheet_button_editor_idx, "", 0.0f, 0));
    }
    buttons.push_back(std::make_unique<ClickButton>(212, 210, 108, 28,
        [this]() {
            justSwitched = true;
            this->sort();
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("SORT"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(1, 211, 34, 28,
        []() {
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, 0));
}

void SortScreen::drawTop() const
{
    Gui::backgroundTop(false);
    Gui::backgroundAnimatedTop();
}

void SortScreen::drawBottom() const
{
    Gui::backgroundBottom(false);
    Gui::backgroundAnimatedBottom();

    for (auto& button : buttons)
    {
        button->draw();
    }

    for (size_t i = 0; i < 5; i++)
    {
        if (i >= sortTypes.size())
        {
            Gui::text(i18n::localize(std::string(sortTypeToString(NONE))), 160, 29 + 35 * i, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK,
                TextPosX::CENTER, TextPosY::CENTER);
        }
        else
        {
            Gui::text(i18n::localize(std::string(sortTypeToString(sortTypes[i]))), 160, 29 + 35 * i, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK,
                TextPosX::CENTER, TextPosY::CENTER);
        }
    }
}

void SortScreen::update(touchPosition* touch)
{
    if (justSwitched)
    {
        if (hidKeysHeld() & KEY_TOUCH)
        {
            return;
        }
        else
        {
            justSwitched = false;
        }
    }
    for (auto& button : buttons)
    {
        if (button->update(touch))
        {
            return;
        }
    }

    if (hidKeysDown() & KEY_B)
    {
        Gui::screenBack();
    }
}

void SortScreen::pickSort(size_t number)
{
    if (number >= sortTypes.size())
    {
        number = sortTypes.size();
        sortTypes.push_back(NONE);
    }
    addOverlay<SortOverlay>(sortTypes[number]);
}

void SortScreen::sort()
{
    while (!sortTypes.empty() && sortTypes.back() == NONE)
    {
        sortTypes.pop_back();
    }
    if (!sortTypes.empty())
    {
        if (std::find(sortTypes.begin(), sortTypes.end(), DEX) == sortTypes.end())
        {
            sortTypes.push_back(DEX);
        }
        std::vector<std::shared_ptr<PKX>> sortMe;
        if (storage)
        {
            for (int i = 0; i < Banks::bank->boxes() * 30; i++)
            {
                std::shared_ptr<PKX> pkm = Banks::bank->pkm(i / 30, i % 30);
                if (pkm->encryptionConstant() != 0 && pkm->species() != 0)
                {
                    sortMe.push_back(pkm);
                }
            }
        }
        else
        {
            for (int i = 0; i < TitleLoader::save->maxSlot(); i++)
            {
                std::shared_ptr<PKX> pkm = TitleLoader::save->pkm(i / 30, i % 30);
                if (pkm->encryptionConstant() != 0 && pkm->species() != 0)
                {
                    sortMe.push_back(pkm);
                }
            }
        }
        std::stable_sort(sortMe.begin(), sortMe.end(), [this](const std::shared_ptr<PKX>& pkm1, const std::shared_ptr<PKX>& pkm2) {
            for (auto type : sortTypes)
            {
                switch (type)
                {
                    case DEX:
                        if (pkm1->species() < pkm2->species())
                            return true;
                        if (pkm2->species() < pkm1->species())
                            return false;
                        break;
                    case FORM:
                        if (pkm1->alternativeForm() < pkm2->alternativeForm())
                            return true;
                        if (pkm2->alternativeForm() < pkm1->alternativeForm())
                            return false;
                        break;
                    case TYPE1:
                        if (pkm1->type1() < pkm2->type1())
                            return true;
                        if (pkm2->type1() < pkm1->type1())
                            return false;
                        break;
                    case TYPE2:
                        if (pkm1->type2() < pkm2->type2())
                            return true;
                        if (pkm2->type2() < pkm1->type2())
                            return false;
                        break;
                    case HP:
                        if (pkm1->stat(0) < pkm2->stat(0))
                            return true;
                        if (pkm2->stat(0) < pkm1->stat(0))
                            return false;
                        break;
                    case ATK:
                        if (pkm1->stat(1) < pkm2->stat(1))
                            return true;
                        if (pkm2->stat(1) < pkm1->stat(1))
                            return false;
                        break;
                    case DEF:
                        if (pkm1->stat(2) < pkm2->stat(2))
                            return true;
                        if (pkm2->stat(2) < pkm1->stat(2))
                            return false;
                        break;
                    case SATK:
                        if (pkm1->stat(4) < pkm2->stat(4))
                            return true;
                        if (pkm2->stat(4) < pkm1->stat(4))
                            return false;
                        break;
                    case SDEF:
                        if (pkm1->stat(5) < pkm2->stat(5))
                            return true;
                        if (pkm2->stat(5) < pkm1->stat(5))
                            return false;
                        break;
                    case SPE:
                        if (pkm1->stat(3) < pkm2->stat(3))
                            return true;
                        if (pkm2->stat(3) < pkm1->stat(3))
                            return false;
                        break;
                    case NATURE:
                        if (pkm1->nature() < pkm2->nature())
                            return true;
                        if (pkm2->nature() < pkm1->nature())
                            return false;
                        break;
                    case LEVEL:
                        if (pkm1->level() < pkm2->level())
                            return true;
                        if (pkm2->level() < pkm1->level())
                            return false;
                        break;
                    case TID:
                        if (pkm1->TID() < pkm2->TID())
                            return true;
                        if (pkm2->TID() < pkm1->TID())
                            return false;
                        break;
                    case HPIV:
                        if (pkm1->iv(0) < pkm2->iv(0))
                            return true;
                        if (pkm2->iv(0) < pkm1->iv(0))
                            return false;
                        break;
                    case ATKIV:
                        if (pkm1->iv(1) < pkm2->iv(1))
                            return true;
                        if (pkm2->iv(1) < pkm1->iv(1))
                            return false;
                        break;
                    case DEFIV:
                        if (pkm1->iv(2) < pkm2->iv(2))
                            return true;
                        if (pkm2->iv(2) < pkm1->iv(2))
                            return false;
                        break;
                    case SATKIV:
                        if (pkm1->iv(4) < pkm2->iv(4))
                            return true;
                        if (pkm2->iv(4) < pkm1->iv(4))
                            return false;
                        break;
                    case SDEFIV:
                        if (pkm1->iv(5) < pkm2->iv(5))
                            return true;
                        if (pkm2->iv(5) < pkm1->iv(5))
                            return false;
                        break;
                    case SPEIV:
                        if (pkm1->iv(3) < pkm2->iv(3))
                            return true;
                        if (pkm2->iv(3) < pkm1->iv(3))
                            return false;
                        break;
                    case HIDDENPOWER:
                        if (pkm1->hpType() < pkm2->hpType())
                            return true;
                        if (pkm2->hpType() < pkm1->hpType())
                            return false;
                        break;
                    case FRIENDSHIP:
                        if (pkm1->currentFriendship() < pkm2->currentFriendship())
                            return true;
                        if (pkm2->currentFriendship() < pkm1->currentFriendship())
                            return false;
                        break;
                    case NICKNAME:
                        if (pkm1->nickname() < pkm2->nickname())
                            return true;
                        if (pkm2->nickname() < pkm1->nickname())
                            return false;
                        break;
                    case SPECIESNAME:
                        if (i18n::species(Configuration::getInstance().language(), pkm1->species()) <
                            i18n::species(Configuration::getInstance().language(), pkm2->species()))
                            return true;
                        if (i18n::species(Configuration::getInstance().language(), pkm2->species()) <
                            i18n::species(Configuration::getInstance().language(), pkm1->species()))
                            return false;
                        break;
                    case OTNAME:
                        if (pkm1->otName() < pkm2->otName())
                            return true;
                        if (pkm2->otName() < pkm1->otName())
                            return false;
                        break;
                    case SHINY:
                        if (pkm1->shiny() && !pkm2->shiny())
                            return true;
                        if (pkm2->shiny() && !pkm1->shiny())
                            return false;
                        break;
                    default:
                        break;
                }
            }
            return false;
        });

        if (storage)
        {
            for (size_t i = 0; i < sortMe.size(); i++)
            {
                Banks::bank->pkm(sortMe[i], i / 30, i % 30);
            }
            for (int i = sortMe.size(); i < Banks::bank->boxes() * 30; i++)
            {
                Banks::bank->pkm(TitleLoader::save->emptyPkm(), i / 30, i % 30);
            }
        }
        else
        {
            for (size_t i = 0; i < sortMe.size(); i++)
            {
                TitleLoader::save->pkm(sortMe[i], i / 30, i % 30, false);
            }
            for (int i = sortMe.size(); i < TitleLoader::save->maxSlot(); i++)
            {
                TitleLoader::save->pkm(TitleLoader::save->emptyPkm(), i / 30, i % 30, false);
            }
        }
    }
}
