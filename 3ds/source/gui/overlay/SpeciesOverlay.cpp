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

#include "SpeciesOverlay.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "pkx/PKFilter.hpp"
#include "pkx/PKX.hpp"
#include "sav/Sav.hpp"
#include "utils/utils.hpp"

namespace
{
    class SpeciesException : public std::exception
    {
    public:
        SpeciesException() {}

        const char* what() { return "SpeciesException: If you are seeing this, piepie62 is dumb."; }
    };
}

SpeciesOverlay::SpeciesOverlay(ReplaceableScreen& screen, const std::variant<std::shared_ptr<pksm::PKX>, std::shared_ptr<pksm::PKFilter>>& object)
    : ReplaceableScreen(&screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK")),
      object(object),
      hid(40, 8),
      dispPkm(TitleLoader::save->availableSpecies().begin(), TitleLoader::save->availableSpecies().end())
{
    if (TitleLoader::save)
    {
        dispPkm = std::vector<pksm::Species>(TitleLoader::save->availableSpecies().begin(), TitleLoader::save->availableSpecies().end());
    }
    else
    {
        if (object.index() == 0)
        {
            dispPkm = std::vector<pksm::Species>(
                pksm::VersionTables::availableSpecies(pksm::GameVersion::oldestVersion(std::get<0>(object)->generation())).begin(),
                pksm::VersionTables::availableSpecies(pksm::GameVersion::oldestVersion(std::get<0>(object)->generation())).end());
        }
        else
        {
            throw SpeciesException{};
        }
    }
    std::sort(dispPkm.begin(), dispPkm.end());
    instructions.addBox(false, 75, 30, 170, 23, COLOR_GREY, i18n::localize("SEARCH"), COLOR_WHITE);
    searchButton = std::make_unique<ClickButton>(75, 30, 170, 23,
        [this]() {
            searchBar();
            return false;
        },
        ui_sheet_emulated_box_search_idx, "", 0, COLOR_BLACK);
    hid.update(dispPkm.size());
    if (object.index() == 0)
    {
        auto& pkm = std::get<0>(object);
        auto it   = std::find(dispPkm.begin(), dispPkm.end(), pkm->species());
        if (it == dispPkm.end())
        {
            it = dispPkm.begin();
        }
        hid.select(pkm->species() == pksm::Species::None ? 0 : std::distance(dispPkm.begin(), it));
    }
    else
    {
        auto& filter = std::get<1>(object);
        auto it      = std::find(dispPkm.begin(), dispPkm.end(), filter->species());
        if (it == dispPkm.end())
        {
            it = dispPkm.begin();
        }
        hid.select(filter->species() == pksm::Species::None ? 0 : std::distance(dispPkm.begin(), it));
    }
}

void SpeciesOverlay::drawBottom() const
{
    dim();
    Gui::text(i18n::localize("EDITOR_INST"), 160, 115, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    searchButton->draw();
    Gui::sprite(ui_sheet_icon_search_idx, 79, 33);
    Gui::text(searchString, 95, 32, FONT_SIZE_12, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
}

void SpeciesOverlay::drawTop() const
{
    Gui::sprite(ui_sheet_part_mtx_5x8_idx, 0, 0);

    int selectorX = (hid.index() % 8) * 50;
    int selectorY = (hid.index() / 8) * 48;
    // Selector
    if (dispPkm.size() > 0)
    {
        Gui::drawSolidRect(selectorX, selectorY, 49, 47, COLOR_MASKBLACK);
        Gui::drawSolidRect(selectorX, selectorY, 49, 1, COLOR_YELLOW);
        Gui::drawSolidRect(selectorX, selectorY, 1, 47, COLOR_YELLOW);
        Gui::drawSolidRect(selectorX + 48, selectorY, 1, 47, COLOR_YELLOW);
        Gui::drawSolidRect(selectorX, selectorY + 46, 49, 1, COLOR_YELLOW);
    }

    for (int y = 0; y < 5; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            size_t pkmIndex = hid.page() * hid.maxVisibleEntries() + x + y * 8;
            if (pkmIndex >= dispPkm.size())
            {
                break;
            }
            pksm::Species species = dispPkm[pkmIndex];
            pksm::Gender gender   = object.index() == 0 ? std::get<0>(object)->gender() : std::get<1>(object)->gender();
            pksm::Generation gen;
            if (object.index() == 0)
            {
                gen = std::get<0>(object)->generation();
            }
            else if (object.index() == 1 && TitleLoader::save)
            {
                gen = TitleLoader::save->generation();
            }
            else
            {
                throw SpeciesException{};
            }
            Gui::pkm(species, 0, gen, gender, x * 50 + 7, y * 48 + 2);
            Gui::text(std::to_string(size_t(species)), x * 50 + 25, y * 48 + 34, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        }
    }
}

void SpeciesOverlay::update(touchPosition* touch)
{
    if (justSwitched && (hidKeysHeld() & KEY_TOUCH))
    {
        return;
    }
    else if (justSwitched)
    {
        justSwitched = false;
    }

    if (hidKeysDown() & KEY_X)
    {
        searchBar();
    }
    searchButton->update(touch);
    if (!searchString.empty() && searchString != oldSearchString)
    {
        dispPkm.clear();
        const std::set<pksm::Species>& set =
            TitleLoader::save ? TitleLoader::save->availableSpecies()
                              : pksm::VersionTables::availableSpecies(pksm::GameVersion::oldestVersion(std::get<0>(object)->generation()));
        for (auto i = set.begin(); i != set.end(); i++)
        {
            std::string speciesName = i18n::species(Configuration::getInstance().language(), *i).substr(0, searchString.size());
            StringUtils::toLower(speciesName);
            if (speciesName == searchString)
            {
                dispPkm.push_back(*i);
            }
        }
        std::sort(dispPkm.begin(), dispPkm.end());
        oldSearchString = searchString;
    }
    else if (searchString.empty() && !oldSearchString.empty())
    {
        dispPkm.clear();
        const std::set<pksm::Species>& set =
            TitleLoader::save ? TitleLoader::save->availableSpecies()
                              : pksm::VersionTables::availableSpecies(pksm::GameVersion::oldestVersion(std::get<0>(object)->generation()));
        dispPkm.insert(dispPkm.begin(), set.begin(), set.end());
        std::sort(dispPkm.begin(), dispPkm.end());
        oldSearchString = searchString = "";
    }
    if (hid.fullIndex() >= dispPkm.size())
    {
        hid.select(0);
    }
    hid.update(dispPkm.size());
    u32 downKeys = hidKeysDown();
    if (downKeys & KEY_A)
    {
        if (dispPkm.size() > 0)
        {
            pksm::Species species = dispPkm[hid.fullIndex()];
            if (object.index() == 0)
            {
                auto pkm = std::get<0>(object);
                if (pkm->species() == pksm::Species::None || !pkm->nicknamed())
                {
                    std::string nick = i18n::species(pkm->language(), species);
                    if (pkm->generation() == pksm::Generation::FOUR)
                    {
                        nick = StringUtils::toUpper(nick);
                    }
                    pkm->nickname(nick);
                }
                pkm->species(species);
                pkm->alternativeForm(0);
                pkm->setAbility(0);
                pkm->PID(pksm::PKX::getRandomPID(pkm->species(), pkm->gender(), pkm->version(), pkm->nature(), pkm->alternativeForm(),
                    pkm->abilityNumber(), pkm->PID(), pkm->generation()));
            }
            else
            {
                std::get<1>(object)->species(species);
            }
        }
        if (object.index() == 1 || std::get<0>(object)->species() != pksm::Species::None)
        {
            parent->removeOverlay();
        }
        return;
    }
    else if (downKeys & KEY_B)
    {
        parent->removeOverlay();
        return;
    }
}

void SpeciesOverlay::searchBar()
{
    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 2, 20);
    swkbdSetHintText(&state, i18n::localize("SPECIES").c_str());
    swkbdSetValidation(&state, SWKBD_ANYTHING, 0, 0);
    char input[25]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    input[24]       = '\0';
    if (ret == SWKBD_BUTTON_CONFIRM)
    {
        searchString = input;
        StringUtils::toLower(searchString);
    }
}
