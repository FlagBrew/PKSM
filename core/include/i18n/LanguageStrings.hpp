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

#ifndef LANGUAGESTRINGS_HPP
#define LANGUAGESTRINGS_HPP

#include "Language.hpp"
#include "coretypes.h"
#include "generation.hpp"
#include "io.hpp"
#include "nlohmann/json.hpp"
#include <algorithm>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>

class LanguageStrings
{
protected:
    std::vector<std::string> abilities;
    std::vector<std::string> balls;
    std::vector<std::string> forms;
    std::vector<std::string> hps;
    std::vector<std::string> items;
    std::vector<std::string> moves;
    std::vector<std::string> natures;
    std::vector<std::string> speciess;
    std::vector<std::string> games;
    std::map<u16, std::string> locations4;
    std::map<u16, std::string> locations5;
    std::map<u16, std::string> locations6;
    std::map<u16, std::string> locations7;
    std::map<u16, std::string> locationsLGPE;
    std::map<u16, std::string> locations8;
    std::map<u8, std::string> countries;
    std::map<u8, std::map<u8, std::string>> subregions;
    nlohmann::json gui;

    static void load(Language lang, const std::string& name, std::vector<std::string>& array);
    template <typename T>
    static void load(Language lang, const std::string& name, std::map<T, std::string>& map)
    {
        static constexpr const char* base = "romfs:/i18n/";
        std::string path                  = io::exists(base + folder(lang) + name) ? base + folder(lang) + name : base + folder(Language::EN) + name;

        std::string tmp;
        FILE* values = fopen(path.c_str(), "rt");
        if (values)
        {
            if (ferror(values))
            {
                fclose(values);
                return;
            }
            char* data  = (char*)malloc(128);
            size_t size = 0;
            while (!feof(values) && !ferror(values))
            {
                size = std::max(size, (size_t)128);
                if (__getline(&data, &size, values) >= 0)
                {
                    tmp = std::string(data);
                    tmp = tmp.substr(0, tmp.find('\n'));
                    // 0 automatically deduces the base: 0x prefix makes it hexadecimal, 0 prefix makes it octal
                    T val    = std::stoi(tmp.substr(0, tmp.find('|')), 0, 0);
                    map[val] = tmp.substr(0, tmp.find('\r')).substr(tmp.find('|') + 1);
                }
                else
                {
                    break;
                }
            }
            fclose(values);
            free(data);
        }
    }
    static void load(Language lang, const std::string& name, nlohmann::json& json);

public:
    LanguageStrings(Language lang);
    static std::string folder(Language lang);

    const std::vector<std::string>& rawItems() const;
    const std::vector<std::string>& rawMoves() const;
    const std::map<u16, std::string>& locations(Generation g) const;
    const std::map<u8, std::string>& rawCountries() const;
    const std::map<u8, std::string>& rawSubregions(u8 country) const;
    size_t numGameStrings() const;

    const std::string& ability(u16 v) const;
    const std::string& ball(u8 v) const;
    const std::string& hp(u8 v) const;
    const std::string& item(u16 v) const;
    const std::string& move(u16 v) const;
    const std::string& nature(u8 v) const;
    const std::string& species(u16 v) const;
    const std::string& form(u16 species, u16 form, Generation generation) const;
    const std::string& location(u16 v, Generation generation) const;
    const std::string& game(u8 v) const;
    const std::string& subregion(u8 country, u8 v) const;
    const std::string& country(u8 v) const;

    const std::string& localize(const std::string& v) const;
};

#endif
