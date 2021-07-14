/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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
#include "Configuration.hpp"
#include "SortOverlay.hpp"
#include "banks.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "pkx/PKX.hpp"
#include "sav/Sav.hpp"

SortScreen::SortScreen(bool storage) : storage(storage)
{
    for (int i = 0; i < 5; i++)
    {
        buttons.push_back(std::make_unique<ClickButton>(
            106, 15 + i * 35, 108, 28,
            [this, i]()
            {
                pickSort(i);
                return false;
            },
            ui_sheet_button_editor_idx, "", 0.0f, COLOR_BLACK));
    }
    buttons.push_back(std::make_unique<ClickButton>(
        212, 210, 108, 28,
        [this]()
        {
            justSwitched = true;
            this->sort();
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_editor_idx, i18n::localize("SORT"), FONT_SIZE_12, COLOR_BLACK));
    buttons.push_back(std::make_unique<ClickButton>(
        1, 211, 34, 28,
        []()
        {
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, COLOR_BLACK));
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

    for (const auto& button : buttons)
    {
        button->draw();
    }

    for (size_t i = 0; i < 5; i++)
    {
        if (i >= sortTypes.size())
        {
            Gui::text(i18n::localize(sortTypeToString(SortType::NONE)), 160, 29 + 35 * i,
                FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::CENTER);
        }
        else
        {
            Gui::text(i18n::localize(sortTypeToString(sortTypes[i])), 160, 29 + 35 * i,
                FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::CENTER);
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
        sortTypes.push_back(SortType::NONE);
    }
    addOverlay<SortOverlay>(sortTypes[number]);
}

void SortScreen::sort()
{
    while (!sortTypes.empty() && sortTypes.back() == SortType::NONE)
    {
        sortTypes.pop_back();
    }
    if (!sortTypes.empty())
    {
        if (std::find(sortTypes.begin(), sortTypes.end(), SortType::DEX) == sortTypes.end())
        {
            sortTypes.push_back(SortType::DEX);
        }
        std::vector<std::shared_ptr<pksm::PKX>> sortMe;
        if (storage)
        {
            for (int i = 0; i < Banks::bank->boxes() * 30; i++)
            {
                std::shared_ptr<pksm::PKX> pkm = Banks::bank->pkm(i / 30, i % 30);
                if (pkm->species() != pksm::Species::None)
                {
                    sortMe.push_back(pkm);
                }
            }
        }
        else
        {
            for (int i = 0; i < TitleLoader::save->maxSlot(); i++)
            {
                std::shared_ptr<pksm::PKX> pkm = TitleLoader::save->pkm(i / 30, i % 30);
                if (pkm->species() != pksm::Species::None)
                {
                    sortMe.push_back(pkm);
                }
            }
        }
        std::stable_sort(sortMe.begin(), sortMe.end(),
            [this](const std::shared_ptr<pksm::PKX>& pkm1, const std::shared_ptr<pksm::PKX>& pkm2)
            {
                for (const auto& type : sortTypes)
                {
                    switch (type)
                    {
                        case SortType::DEX:
                            if (pkm1->species() < pkm2->species())
                                return true;
                            if (pkm2->species() < pkm1->species())
                                return false;
                            break;
                        case SortType::FORM:
                            if (pkm1->alternativeForm() < pkm2->alternativeForm())
                                return true;
                            if (pkm2->alternativeForm() < pkm1->alternativeForm())
                                return false;
                            break;
                        case SortType::TYPE1:
                            if (pkm1->type1() < pkm2->type1())
                                return true;
                            if (pkm2->type1() < pkm1->type1())
                                return false;
                            break;
                        case SortType::TYPE2:
                            if (pkm1->type2() < pkm2->type2())
                                return true;
                            if (pkm2->type2() < pkm1->type2())
                                return false;
                            break;
                        case SortType::HP:
                            if (pkm1->stat(pksm::Stat::HP) < pkm2->stat(pksm::Stat::HP))
                                return true;
                            if (pkm2->stat(pksm::Stat::HP) < pkm1->stat(pksm::Stat::HP))
                                return false;
                            break;
                        case SortType::ATK:
                            if (pkm1->stat(pksm::Stat::ATK) < pkm2->stat(pksm::Stat::ATK))
                                return true;
                            if (pkm2->stat(pksm::Stat::ATK) < pkm1->stat(pksm::Stat::ATK))
                                return false;
                            break;
                        case SortType::DEF:
                            if (pkm1->stat(pksm::Stat::DEF) < pkm2->stat(pksm::Stat::DEF))
                                return true;
                            if (pkm2->stat(pksm::Stat::DEF) < pkm1->stat(pksm::Stat::DEF))
                                return false;
                            break;
                        case SortType::SATK:
                            if (pkm1->stat(pksm::Stat::SPATK) < pkm2->stat(pksm::Stat::SPATK))
                                return true;
                            if (pkm2->stat(pksm::Stat::SPATK) < pkm1->stat(pksm::Stat::SPATK))
                                return false;
                            break;
                        case SortType::SDEF:
                            if (pkm1->stat(pksm::Stat::SPDEF) < pkm2->stat(pksm::Stat::SPDEF))
                                return true;
                            if (pkm2->stat(pksm::Stat::SPDEF) < pkm1->stat(pksm::Stat::SPDEF))
                                return false;
                            break;
                        case SortType::SPE:
                            if (pkm1->stat(pksm::Stat::SPDEF) < pkm2->stat(pksm::Stat::SPD))
                                return true;
                            if (pkm2->stat(pksm::Stat::SPDEF) < pkm1->stat(pksm::Stat::SPD))
                                return false;
                            break;
                        case SortType::NATURE:
                            if (pkm1->nature() < pkm2->nature())
                                return true;
                            if (pkm2->nature() < pkm1->nature())
                                return false;
                            break;
                        case SortType::LEVEL:
                            if (pkm1->level() < pkm2->level())
                                return true;
                            if (pkm2->level() < pkm1->level())
                                return false;
                            break;
                        case SortType::TID:
                            if (pkm1->TID() < pkm2->TID())
                                return true;
                            if (pkm2->TID() < pkm1->TID())
                                return false;
                            break;
                        case SortType::HPIV:
                            if (pkm1->iv(pksm::Stat::HP) < pkm2->iv(pksm::Stat::HP))
                                return true;
                            if (pkm2->iv(pksm::Stat::HP) < pkm1->iv(pksm::Stat::HP))
                                return false;
                            break;
                        case SortType::ATKIV:
                            if (pkm1->iv(pksm::Stat::ATK) < pkm2->iv(pksm::Stat::ATK))
                                return true;
                            if (pkm2->iv(pksm::Stat::ATK) < pkm1->iv(pksm::Stat::ATK))
                                return false;
                            break;
                        case SortType::DEFIV:
                            if (pkm1->iv(pksm::Stat::DEF) < pkm2->iv(pksm::Stat::DEF))
                                return true;
                            if (pkm2->iv(pksm::Stat::DEF) < pkm1->iv(pksm::Stat::DEF))
                                return false;
                            break;
                        case SortType::SATKIV:
                            if (pkm1->iv(pksm::Stat::SPATK) < pkm2->iv(pksm::Stat::SPATK))
                                return true;
                            if (pkm2->iv(pksm::Stat::SPATK) < pkm1->iv(pksm::Stat::SPATK))
                                return false;
                            break;
                        case SortType::SDEFIV:
                            if (pkm1->iv(pksm::Stat::SPDEF) < pkm2->iv(pksm::Stat::SPDEF))
                                return true;
                            if (pkm2->iv(pksm::Stat::SPDEF) < pkm1->iv(pksm::Stat::SPDEF))
                                return false;
                            break;
                        case SortType::SPEIV:
                            if (pkm1->iv(pksm::Stat::SPD) < pkm2->iv(pksm::Stat::SPD))
                                return true;
                            if (pkm2->iv(pksm::Stat::SPD) < pkm1->iv(pksm::Stat::SPD))
                                return false;
                            break;
                        case SortType::HIDDENPOWER:
                            if (pkm1->hpType() < pkm2->hpType())
                                return true;
                            if (pkm2->hpType() < pkm1->hpType())
                                return false;
                            break;
                        case SortType::FRIENDSHIP:
                            if (pkm1->currentFriendship() < pkm2->currentFriendship())
                                return true;
                            if (pkm2->currentFriendship() < pkm1->currentFriendship())
                                return false;
                            break;
                        case SortType::NICKNAME:
                            if (pkm1->nickname() < pkm2->nickname())
                                return true;
                            if (pkm2->nickname() < pkm1->nickname())
                                return false;
                            break;
                        case SortType::SPECIESNAME:
                            if (pkm1->species().localize(Configuration::getInstance().language()) <
                                pkm2->species().localize(Configuration::getInstance().language()))
                                return true;
                            if (pkm2->species().localize(Configuration::getInstance().language()) <
                                pkm1->species().localize(Configuration::getInstance().language()))
                                return false;
                            break;
                        case SortType::OTNAME:
                            if (pkm1->otName() < pkm2->otName())
                                return true;
                            if (pkm2->otName() < pkm1->otName())
                                return false;
                            break;
                        case SortType::SHINY:
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
                Banks::bank->pkm(*sortMe[i], i / 30, i % 30);
            }
            for (int i = sortMe.size(); i < Banks::bank->boxes() * 30; i++)
            {
                Banks::bank->pkm(*TitleLoader::save->emptyPkm(), i / 30, i % 30);
            }
        }
        else
        {
            for (size_t i = 0; i < sortMe.size(); i++)
            {
                TitleLoader::save->pkm(*sortMe[i], i / 30, i % 30, false);
            }
            for (int i = sortMe.size(); i < TitleLoader::save->maxSlot(); i++)
            {
                TitleLoader::save->pkm(*TitleLoader::save->emptyPkm(), i / 30, i % 30, false);
            }
        }
    }
}
