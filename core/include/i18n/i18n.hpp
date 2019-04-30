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

#ifndef I18N_HPP
#define I18N_HPP

#include "Configuration.hpp"
#include "LanguageStrings.hpp"

namespace i18n
{
    void init(void);
    void exit(void);

    const std::vector<std::string>& rawItems(u8 lang);
    const std::vector<std::string>& rawMoves(u8 lang);
    const std::map<u16, std::string>& locations(u8 lang, Generation g);
    size_t numGameStrings(u8 lang);

    const std::string& ability(u8 lang, u8 value);
    const std::string& ball(u8 lang, u8 value);
    const std::string& hp(u8 lang, u8 value);
    const std::string& item(u8 lang, u16 value);
    const std::string& move(u8 lang, u16 value);
    const std::string& nature(u8 lang, u8 value);
    const std::string& species(u8 lang, u16 value);
    const std::string& form(u8 lang, u16 species, u8 form, Generation generation);
    const std::string& location(u8 lang, u16 value, Generation generation);
    const std::string& location(u8 lang, u16 value, u8 originGame);
    const std::string& game(u8 lang, u8 value);

    // Used for general GUI stuff; not for PKM values
    const std::string& localize(Language lang, const std::string& index);
    const std::string& localize(const std::string& index);
    const std::string& langString(Language l);
    Language langFromString(const std::string& value);
};

#endif
