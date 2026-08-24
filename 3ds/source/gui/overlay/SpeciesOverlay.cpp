/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2022 Bernardo Giordano, Admiral Fish, piepie62
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
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
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

SpeciesOverlay::SpeciesOverlay(ReplaceableScreen& screen, pksm::IPKFilterable& object, u8 origLevel)
    : SearchableOverlay(screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK"),
          i18n::localize("SPECIES"), 40, 8),
      object(object),
      origLevel(origLevel)
{
    const std::set<pksm::Species>& species = availableSpecies();
    dispPkm = std::vector<pksm::Species>(species.begin(), species.end());
    std::sort(dispPkm.begin(), dispPkm.end());
    hid.update(dispPkm.size());

    auto it = std::find(dispPkm.begin(), dispPkm.end(), object.species());
    if (it == dispPkm.end())
    {
        it = dispPkm.begin();
    }
    hid.select(object.species() == pksm::Species::None ? 0 : std::distance(dispPkm.begin(), it));
}

const std::set<pksm::Species>& SpeciesOverlay::availableSpecies() const
{
    if (TitleLoader::save)
    {
        return TitleLoader::save->availableSpecies();
    }
    if (object.isFilter())
    {
        throw SpeciesException{};
    }
    return pksm::VersionTables::availableSpecies(
        pksm::GameVersion::oldestVersion(object.generation()));
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
            pksm::Gender gender   = object.gender();
            pksm::Generation gen;
            if (!object.isFilter())
            {
                gen = object.generation();
            }
            else if (TitleLoader::save)
            {
                gen = TitleLoader::save->generation();
            }
            else
            {
                throw SpeciesException{};
            }
            Gui::pkm(species, 0, gen, gender, x * 50 + 7, y * 48 + 2);
            Gui::text(std::to_string(size_t(species)), x * 50 + 25, y * 48 + 34, FONT_SIZE_9,
                COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        }
    }
}

void SpeciesOverlay::filter(const std::string& search)
{
    const std::set<pksm::Species>& species = availableSpecies();
    dispPkm.clear();
    if (search.empty())
    {
        dispPkm.insert(dispPkm.begin(), species.begin(), species.end());
    }
    else
    {
        for (auto i = species.begin(); i != species.end(); i++)
        {
            std::string speciesName =
                i18n::species(Configuration::getInstance().language(), *i).substr(0, search.size());
            StringUtils::toLower(speciesName);
            if (speciesName == search)
            {
                dispPkm.push_back(*i);
            }
        }
    }
    std::sort(dispPkm.begin(), dispPkm.end());
}

bool SpeciesOverlay::commit()
{
    if (dispPkm.size() > 0)
    {
        pksm::Species species = dispPkm[hid.fullIndex()];
        if (!object.isFilter())
        {
            pksm::PKX& pkm = static_cast<pksm::PKX&>(object);
            if (pkm.species() == pksm::Species::None || !pkm.nicknamed())
            {
                std::string nick = species.localize(pkm.language());
                if (pkm.generation() <= pksm::Generation::FOUR)
                {
                    nick = StringUtils::toUpper(nick);
                }
                pkm.nickname(nick);
            }
            pkm.species(species);
            pkm.alternativeForm(0);
            pkm.setAbility(0);
            pkm.PID(pksm::PKX::getRandomPID(pkm.species(), pkm.gender(), pkm.version(),
                pkm.nature(), pkm.alternativeForm(), pkm.abilityNumber(), pkm.shiny(), pkm.TSV(),
                pkm.PID(), pkm.generation()));
            if (origLevel != 0)
            {
                pkm.level(origLevel);
            }
        }
        else
        {
            object.species(species);
        }
    }
    return object.isFilter() || object.species() != pksm::Species::None;
}
