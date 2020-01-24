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

#include "Language.hpp"
#include "generation.hpp"
#include <algorithm>
#include <array>

namespace
{
    constexpr std::array<Language, 6> G3Langs = {Language::JP, Language::EN, Language::FR, Language::DE, Language::ES, Language::IT};
    constexpr std::array<Language, 7> G4Langs = {Language::JP, Language::EN, Language::FR, Language::DE, Language::ES, Language::IT, Language::KO};
    constexpr std::array<Language, 9> G7Langs = {
        Language::JP, Language::EN, Language::FR, Language::DE, Language::ES, Language::IT, Language::KO, Language::ZH, Language::TW};
}

Language getSafeLanguage(Generation gen, Language orig)
{
    switch (gen)
    {
        // case Generation::ONE
        // case Generation::TWO
        case Generation::THREE:
            if (std::find(G3Langs.begin(), G3Langs.end(), orig) != G3Langs.end())
            {
                return orig;
            }
            return Language::EN;
        case Generation::FOUR:
        case Generation::FIVE:
        case Generation::SIX:
            if (std::find(G4Langs.begin(), G4Langs.end(), orig) != G4Langs.end())
            {
                return orig;
            }
            return Language::EN;

        case Generation::SEVEN:
        case Generation::LGPE:
        case Generation::EIGHT:
            if (std::find(G7Langs.begin(), G7Langs.end(), orig) != G7Langs.end())
            {
                return orig;
            }
            return Language::EN;
    }
    return Language::EN;
}
