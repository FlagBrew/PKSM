/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#include "LanguageStrings.hpp"

std::string LanguageStrings::folder(Language lang) const
{
    switch (lang)
    {
        case Language::EN: return "en";
        case Language::ES: return "es";
        case Language::DE: return "de";
        case Language::FR: return "fr";
        case Language::IT: return "it";
        case Language::JP: return "jp";
        case Language::KO: return "ko";
        case Language::NL: return "nl";
        case Language::PT: return "pt";
        case Language::ZH: return "zh";
        case Language::TW: return "tw";
        default: return "en";
    }

    return "en";
}

LanguageStrings::LanguageStrings(Language lang)
{
    load(lang, "/abilities.txt", abilities);
    load(lang, "/balls.txt", balls);
    load(lang, "/forms.txt", forms);
    load(lang, "/hp.txt", hps);
    load(lang, "/items.txt", items);
    load(lang, "/moves.txt", moves);
    load(lang, "/natures.txt", natures);
    load(lang, "/species.txt", speciess);
    loadGui(lang);
    
    std::pair<std::string, int> tmp;
    for (size_t i = 1; i < items.size(); i++)
    {
        if (items[i].find("？？？") != std::string::npos || items[i].find("???") != std::string::npos) continue;
        tmp = std::make_pair(items[i], i);
        sortedItems.push_back(tmp);
    }
    for (size_t i = 0; i < moves.size(); i++)
    {
        if (i >= 622 && i <= 658) continue;
        tmp = std::make_pair(moves[i], i);
        sortedMoves.push_back(tmp);
    }
    static const auto less = [](const std::pair<std::string, int>& pair1, const std::pair<std::string, int>& pair2){ return pair1.first < pair2.first; };
    std::sort(sortedItems.begin(), sortedItems.end(), less);
    std::sort(sortedMoves.begin(), sortedMoves.end(), less);
    sortedItems.insert(sortedItems.begin(), std::make_pair(items[0], 0));
}

void LanguageStrings::load(Language lang, const std::string name, std::vector<std::string>& array)
{
    static const std::string base = "romfs:/i18n/";
    std::string path = io::exists(base + folder(lang) + name) ? base + folder(lang) + name : base + folder(Language::EN) + name;
    
    std::string tmp;
    std::ifstream values(path);
    if (values.bad()) return;
    while (std::getline(values, tmp))
    {
        array.push_back(tmp.substr(0, tmp.find('\r')));
    }
    values.close();
}

void LanguageStrings::loadGui(Language lang)
{
    static const std::string base = "romfs:/i18n/";
    std::string path = io::exists(base + folder(lang) + "/gui.json") ? base + folder(lang) + "/gui.json" : base + folder(Language::EN) + "/gui.json";

    std::ifstream values(path);
    gui << values;
    values.close();
}

std::string LanguageStrings::ability(u8 v) const
{
    return v < abilities.size() ? abilities.at(v) : "Invalid";
}

std::string LanguageStrings::ball(u8 v) const
{
    return v < balls.size() ? balls.at(v) : "Invalid";
}

std::string LanguageStrings::form(u8 v) const
{
    return v < forms.size() ? forms.at(v) : "Invalid";
}

std::string LanguageStrings::hp(u8 v) const
{
    return v < hps.size() ? hps.at(v) : "Invalid";
}

std::string LanguageStrings::item(u16 v) const
{
    return v < items.size() ? items.at(v) : "Invalid";
}

std::string LanguageStrings::move(u16 v) const
{
    return v < moves.size() ? moves.at(v) : "Invalid";
}

std::string LanguageStrings::nature(u8 v) const
{
    return v < natures.size() ? natures.at(v) : "Invalid";
}

std::string LanguageStrings::species(u16 v) const
{
    return v < speciess.size() ? speciess.at(v) : "Invalid";
}

std::string LanguageStrings::sortedItem(u16 v) const
{
    return v < sortedItems.size() ? sortedItems[v].first : items[0];
}

std::string LanguageStrings::sortedMove(u16 v) const
{
    return v < sortedMoves.size() ? sortedMoves[v].first : moves[0];
}

int LanguageStrings::item(std::string v) const
{
    if (v == items[0] || v == "")
    {
        return 0;
    }
    int index = -1, min = 0, mid = 0, max = sortedItems.size();
    while (min <= max)
    {
        mid = min + (max-min)/2;
        if (sortedItems[mid].first == v)
        {
            index = mid;
            break;
        }
        if (sortedItems[mid].first < v)
        {
            min = mid + 1;
        }
        else
        {
            max = mid - 1;
        }
    }
    return index >= 0 ? sortedItems[index].second : 0;
}

int LanguageStrings::move(std::string v) const
{
    if (v == moves[0] || v == "")
    {
        return 0;
    }
    int index = -1, min = 0, mid = 0, max = sortedMoves.size();
    while (min <= max)
    {
        mid = min + (max-min)/2;
        if (sortedMoves[mid].first == v)
        {
            index = mid;
            break;
        }
        if (sortedMoves[mid].first < v)
        {
            min = mid + 1;
        }
        else
        {
            max = mid - 1;
        }
    }
    return index >= 0 ? sortedMoves[index].second : 0;
}

std::string LanguageStrings::localize(const std::string& v) const
{
    return gui.value(v, "Invalid");
}