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

#include "i18n.hpp"

static u8 systemLanguage = 1;

static LanguageStrings* jp;
static LanguageStrings* en;
static LanguageStrings* fr;
static LanguageStrings* it;
static LanguageStrings* de;
static LanguageStrings* es;
static LanguageStrings* ko;
static LanguageStrings* zh;
static LanguageStrings* tw;
static LanguageStrings* nl;
static LanguageStrings* pt;

void i18n::init(void)
{
    jp = new LanguageStrings(Language::JP);
    en = new LanguageStrings(Language::EN);
    fr = new LanguageStrings(Language::FR);
    it = new LanguageStrings(Language::IT);
    de = new LanguageStrings(Language::DE);
    es = new LanguageStrings(Language::ES);
    ko = new LanguageStrings(Language::KO);
    zh = new LanguageStrings(Language::ZH);
    tw = new LanguageStrings(Language::TW);
    nl = new LanguageStrings(Language::NL);
    pt = new LanguageStrings(Language::PT);

    CFGU_GetSystemLanguage(&systemLanguage);
}

void i18n::exit(void)
{
    delete jp;
    delete en;
    delete fr;
    delete it;
    delete de;
    delete es;
    delete ko;
    delete zh;
    delete tw;
    delete nl;
    delete pt;
}

std::string i18n::ability(u8 lang, u8 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->ability(val);
        case Language::EN:
            return en->ability(val);
        case Language::ES:
            return es->ability(val);
        case Language::FR:
            return fr->ability(val);
        case Language::IT:
            return it->ability(val);
        case Language::JP:
            return jp->ability(val);
        case Language::KO:
            return ko->ability(val);
        case Language::NL:
            return nl->ability(val);
        case Language::PT:
            return pt->ability(val);
        case Language::ZH:
            return zh->ability(val);
        case Language::TW:
            return tw->ability(val);
    }
    return "";
}

std::string i18n::ball(u8 lang, u8 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->ball(val);
        case Language::EN:
            return en->ball(val);
        case Language::ES:
            return es->ball(val);
        case Language::FR:
            return fr->ball(val);
        case Language::IT:
            return it->ball(val);
        case Language::JP:
            return jp->ball(val);
        case Language::KO:
            return ko->ball(val);
        case Language::NL:
            return nl->ball(val);
        case Language::PT:
            return pt->ball(val);
        case Language::ZH:
            return zh->ball(val);
        case Language::TW:
            return tw->ball(val);
    }
    return "";
}

std::string i18n::form(u8 lang, u16 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->form(val);
        case Language::EN:
            return en->form(val);
        case Language::ES:
            return es->form(val);
        case Language::FR:
            return fr->form(val);
        case Language::IT:
            return it->form(val);
        case Language::JP:
            return jp->form(val);
        case Language::KO:
            return ko->form(val);
        case Language::NL:
            return nl->form(val);
        case Language::PT:
            return pt->form(val);
        case Language::ZH:
            return zh->form(val);
        case Language::TW:
            return tw->form(val);
    }
    return "";
}

std::string i18n::hp(u8 lang, u8 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->hp(val);
        case Language::EN:
            return en->hp(val);
        case Language::ES:
            return es->hp(val);
        case Language::FR:
            return fr->hp(val);
        case Language::IT:
            return it->hp(val);
        case Language::JP:
            return jp->hp(val);
        case Language::KO:
            return ko->hp(val);
        case Language::NL:
            return nl->hp(val);
        case Language::PT:
            return pt->hp(val);
        case Language::ZH:
            return zh->hp(val);
        case Language::TW:
            return tw->hp(val);
    }
    return "";
}

std::string i18n::item(u8 lang, u16 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->item(val);
        case Language::EN:
            return en->item(val);
        case Language::ES:
            return es->item(val);
        case Language::FR:
            return fr->item(val);
        case Language::IT:
            return it->item(val);
        case Language::JP:
            return jp->item(val);
        case Language::KO:
            return ko->item(val);
        case Language::NL:
            return nl->item(val);
        case Language::PT:
            return pt->item(val);
        case Language::ZH:
            return zh->item(val);
        case Language::TW:
            return tw->item(val);
    }
    return "";
}

std::string i18n::move(u8 lang, u16 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->move(val);
        case Language::EN:
            return en->move(val);
        case Language::ES:
            return es->move(val);
        case Language::FR:
            return fr->move(val);
        case Language::IT:
            return it->move(val);
        case Language::JP:
            return jp->move(val);
        case Language::KO:
            return ko->move(val);
        case Language::NL:
            return nl->move(val);
        case Language::PT:
            return pt->move(val);
        case Language::ZH:
            return zh->move(val);
        case Language::TW:
            return tw->move(val);
    }
    return "";
}

std::string i18n::nature(u8 lang, u8 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->nature(val);
        case Language::EN:
            return en->nature(val);
        case Language::ES:
            return es->nature(val);
        case Language::FR:
            return fr->nature(val);
        case Language::IT:
            return it->nature(val);
        case Language::JP:
            return jp->nature(val);
        case Language::KO:
            return ko->nature(val);
        case Language::NL:
            return nl->nature(val);
        case Language::PT:
            return pt->nature(val);
        case Language::ZH:
            return zh->nature(val);
        case Language::TW:
            return tw->nature(val);
    }
    return "";
}

std::string i18n::species(u8 lang, u16 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->species(val);
        case Language::EN:
            return en->species(val);
        case Language::ES:
            return es->species(val);
        case Language::FR:
            return fr->species(val);
        case Language::IT:
            return it->species(val);
        case Language::JP:
            return jp->species(val);
        case Language::KO:
            return ko->species(val);
        case Language::NL:
            return nl->species(val);
        case Language::PT:
            return pt->species(val);
        case Language::ZH:
            return zh->species(val);
        case Language::TW:
            return tw->species(val);
    }
    return "";
}

std::string i18n::sortedItem(u8 lang, u16 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->sortedItem(val);
        case Language::EN:
            return en->sortedItem(val);
        case Language::ES:
            return es->sortedItem(val);
        case Language::FR:
            return fr->sortedItem(val);
        case Language::IT:
            return it->sortedItem(val);
        case Language::JP:
            return jp->sortedItem(val);
        case Language::KO:
            return ko->sortedItem(val);
        case Language::NL:
            return nl->sortedItem(val);
        case Language::PT:
            return pt->sortedItem(val);
        case Language::ZH:
            return zh->sortedItem(val);
        case Language::TW:
            return tw->sortedItem(val);
    }
    return "";
}

std::string i18n::sortedMove(u8 lang, u16 val)
{
    switch (lang)
    {
        case Language::DE:
            return de->sortedMove(val);
        case Language::EN:
            return en->sortedMove(val);
        case Language::ES:
            return es->sortedMove(val);
        case Language::FR:
            return fr->sortedMove(val);
        case Language::IT:
            return it->sortedMove(val);
        case Language::JP:
            return jp->sortedMove(val);
        case Language::KO:
            return ko->sortedMove(val);
        case Language::NL:
            return nl->sortedMove(val);
        case Language::PT:
            return pt->sortedMove(val);
        case Language::ZH:
            return zh->sortedMove(val);
        case Language::TW:
            return tw->sortedMove(val);
    }
    return "";
}

int i18n::item(u8 lang, std::string val)
{
    switch (lang)
    {
        case Language::DE:
            return de->item(val);
        case Language::EN:
            return en->item(val);
        case Language::ES:
            return es->item(val);
        case Language::FR:
            return fr->item(val);
        case Language::IT:
            return it->item(val);
        case Language::JP:
            return jp->item(val);
        case Language::KO:
            return ko->item(val);
        case Language::NL:
            return nl->item(val);
        case Language::PT:
            return pt->item(val);
        case Language::ZH:
            return zh->item(val);
        case Language::TW:
            return tw->item(val);
    }
    return 0;
}

int i18n::move(u8 lang, std::string val)
{
    switch (lang)
    {
        case Language::DE:
            return de->move(val);
        case Language::EN:
            return en->move(val);
        case Language::ES:
            return es->move(val);
        case Language::FR:
            return fr->move(val);
        case Language::IT:
            return it->move(val);
        case Language::JP:
            return jp->move(val);
        case Language::KO:
            return ko->move(val);
        case Language::NL:
            return nl->move(val);
        case Language::PT:
            return pt->move(val);
        case Language::ZH:
            return zh->move(val);
        case Language::TW:
            return tw->move(val);
    }
    return 0;
}

std::string i18n::localize(Language lang, const std::string& val)
{
    switch (lang)
    {
        case Language::DE:
            return de->localize(val);
        case Language::EN:
            return en->localize(val);
        case Language::ES:
            return es->localize(val);
        case Language::FR:
            return fr->localize(val);
        case Language::IT:
            return it->localize(val);
        case Language::JP:
            return jp->localize(val);
        case Language::KO:
            return ko->localize(val);
        case Language::NL:
            return nl->localize(val);
        case Language::PT:
            return pt->localize(val);
        case Language::ZH:
            return zh->localize(val);
        case Language::TW:
            return tw->localize(val);
        case UNUSED:
        default:
            return 0;
    }
    return 0;
}

std::string i18n::localize(const std::string& index)
{
    Language l = Language::EN;
    switch (systemLanguage) {
        case 0x0: l = Language::JP; break;
        case 0x1: l = Language::EN; break;
        case 0x2: l = Language::FR; break;
        case 0x3: l = Language::DE; break;
        case 0x4: l = Language::IT; break;
        case 0x5: l = Language::ES; break;
        case 0x7: l = Language::KO; break;
        case 0x8: l = Language::NL; break;
        case 0x9: l = Language::PT; break;
        case 0xA: l = Language::RU; break;
        case 0xB: l = Language::ZH; break;
        default: l = Language::EN;
    }
    return localize(l, index);
}