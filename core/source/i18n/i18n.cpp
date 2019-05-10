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

#include "i18n.hpp"

static LanguageStrings* jp = nullptr;
static LanguageStrings* en = nullptr;
static LanguageStrings* fr = nullptr;
static LanguageStrings* it = nullptr;
static LanguageStrings* de = nullptr;
static LanguageStrings* es = nullptr;
static LanguageStrings* ko = nullptr;
static LanguageStrings* zh = nullptr;
static LanguageStrings* tw = nullptr;
static LanguageStrings* nl = nullptr;
static LanguageStrings* pt = nullptr;
static LanguageStrings* ru = nullptr;

static const std::string emptyString                = "";
static const std::vector<std::string> emptyVector   = {};
static const std::map<u16, std::string> emptyU16Map = {};

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
    ru = new LanguageStrings(Language::RU);
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
    delete ru;
}

const std::string& i18n::ability(u8 lang, u8 val)
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
        case Language::RU:
            return ru->ability(val);
    }
    return emptyString;
}

const std::string& i18n::ball(u8 lang, u8 val)
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
        case Language::RU:
            return ru->ball(val);
    }
    return emptyString;
}

const std::string& i18n::form(u8 lang, u16 species, u8 form, Generation generation)
{
    switch (lang)
    {
        case Language::DE:
            return de->form(species, form, generation);
        case Language::EN:
            return en->form(species, form, generation);
        case Language::ES:
            return es->form(species, form, generation);
        case Language::FR:
            return fr->form(species, form, generation);
        case Language::IT:
            return it->form(species, form, generation);
        case Language::JP:
            return jp->form(species, form, generation);
        case Language::KO:
            return ko->form(species, form, generation);
        case Language::NL:
            return nl->form(species, form, generation);
        case Language::PT:
            return pt->form(species, form, generation);
        case Language::ZH:
            return zh->form(species, form, generation);
        case Language::TW:
            return tw->form(species, form, generation);
        case Language::RU:
            return ru->form(species, form, generation);
    }
    return emptyString;
}

const std::string& i18n::hp(u8 lang, u8 val)
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
        case Language::RU:
            return ru->hp(val);
    }
    return emptyString;
}

const std::string& i18n::item(u8 lang, u16 val)
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
        case Language::RU:
            return ru->item(val);
    }
    return emptyString;
}

const std::string& i18n::move(u8 lang, u16 val)
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
        case Language::RU:
            return ru->move(val);
    }
    return emptyString;
}

const std::string& i18n::nature(u8 lang, u8 val)
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
        case Language::RU:
            return ru->nature(val);
    }
    return emptyString;
}

const std::string& i18n::species(u8 lang, u16 val)
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
        case Language::RU:
            return ru->species(val);
    }
    return emptyString;
}

const std::string& i18n::localize(Language lang, const std::string& val)
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
        case Language::RU:
            return ru->localize(val);
        default:
            return emptyString;
    }
    return emptyString;
}

const std::string& i18n::localize(const std::string& index)
{
    return localize(Configuration::getInstance().language(), index);
}

const std::string& i18n::langString(Language l)
{
    static const std::string JPN = "JPN";
    static const std::string ENG = "ENG";
    static const std::string FRE = "FRE";
    static const std::string ITA = "ITA";
    static const std::string GER = "GER";
    static const std::string SPA = "SPA";
    static const std::string KOR = "KOR";
    static const std::string CHS = "CHS";
    static const std::string CHT = "CHT";
    switch (l)
    {
        case Language::JP:
            return JPN;
        case Language::EN:
            return ENG;
        case Language::FR:
            return FRE;
        case Language::IT:
            return ITA;
        case Language::DE:
            return GER;
        case Language::ES:
            return SPA;
        case Language::KO:
            return KOR;
        case Language::ZH:
            return CHS;
        case Language::TW:
            return CHT;
        default:
            return ENG;
    }
}

Language i18n::langFromString(const std::string& value)
{
    if (value == "JPN")
    {
        return Language::JP;
    }
    if (value == "ENG")
    {
        return Language::EN;
    }
    if (value == "FRE")
    {
        return Language::FR;
    }
    if (value == "ITA")
    {
        return Language::IT;
    }
    if (value == "GER")
    {
        return Language::DE;
    }
    if (value == "SPA")
    {
        return Language::ES;
    }
    if (value == "KOR")
    {
        return Language::KO;
    }
    if (value == "CHS")
    {
        return Language::ZH;
    }
    if (value == "CHT")
    {
        return Language::TW;
    }
    return Language::EN;
}

const std::vector<std::string>& i18n::rawItems(u8 lang)
{
    switch (lang)
    {
        case Language::DE:
            return de->rawItems();
        case Language::EN:
            return en->rawItems();
        case Language::ES:
            return es->rawItems();
        case Language::FR:
            return fr->rawItems();
        case Language::IT:
            return it->rawItems();
        case Language::JP:
            return jp->rawItems();
        case Language::KO:
            return ko->rawItems();
        case Language::NL:
            return nl->rawItems();
        case Language::PT:
            return pt->rawItems();
        case Language::ZH:
            return zh->rawItems();
        case Language::TW:
            return tw->rawItems();
        case Language::RU:
            return ru->rawItems();
    }
    return emptyVector;
}

const std::vector<std::string>& i18n::rawMoves(u8 lang)
{
    switch (lang)
    {
        case Language::DE:
            return de->rawMoves();
        case Language::EN:
            return en->rawMoves();
        case Language::ES:
            return es->rawMoves();
        case Language::FR:
            return fr->rawMoves();
        case Language::IT:
            return it->rawMoves();
        case Language::JP:
            return jp->rawMoves();
        case Language::KO:
            return ko->rawMoves();
        case Language::NL:
            return nl->rawMoves();
        case Language::PT:
            return pt->rawMoves();
        case Language::ZH:
            return zh->rawMoves();
        case Language::TW:
            return tw->rawMoves();
        case Language::RU:
            return ru->rawMoves();
    }
    return emptyVector;
}

const std::string& i18n::location(u8 lang, u16 v, Generation generation)
{
    switch (lang)
    {
        case Language::DE:
            return de->location(v, generation);
        case Language::EN:
            return en->location(v, generation);
        case Language::ES:
            return es->location(v, generation);
        case Language::FR:
            return fr->location(v, generation);
        case Language::IT:
            return it->location(v, generation);
        case Language::JP:
            return jp->location(v, generation);
        case Language::KO:
            return ko->location(v, generation);
        case Language::NL:
            return nl->location(v, generation);
        case Language::PT:
            return pt->location(v, generation);
        case Language::ZH:
            return zh->location(v, generation);
        case Language::TW:
            return tw->location(v, generation);
        case Language::RU:
            return ru->location(v, generation);
    }
    return emptyString;
}

const std::string& i18n::location(u8 lang, u16 v, u8 originGame)
{
    switch (originGame)
    {
        case 7:
        case 8:
        case 10:
        case 11:
        case 12:
            return location(lang, v, Generation::FOUR);
        case 20:
        case 21:
        case 22:
        case 23:
            return location(lang, v, Generation::FIVE);
        case 24:
        case 25:
        case 26:
        case 27:
            return location(lang, v, Generation::SIX);
        case 30:
        case 31:
        case 32:
        case 33:
            return location(lang, v, Generation::SEVEN);
        case 42:
        case 43:
            return location(lang, v, Generation::LGPE);
    }
    return emptyString;
}

const std::string& i18n::game(u8 lang, u8 v)
{
    switch (lang)
    {
        case Language::DE:
            return de->game(v);
        case Language::EN:
            return en->game(v);
        case Language::ES:
            return es->game(v);
        case Language::FR:
            return fr->game(v);
        case Language::IT:
            return it->game(v);
        case Language::JP:
            return jp->game(v);
        case Language::KO:
            return ko->game(v);
        case Language::NL:
            return nl->game(v);
        case Language::PT:
            return pt->game(v);
        case Language::ZH:
            return zh->game(v);
        case Language::TW:
            return tw->game(v);
        case Language::RU:
            return ru->game(v);
    }
    return emptyString;
}

const std::map<u16, std::string>& i18n::locations(u8 lang, Generation g)
{
    switch (lang)
    {
        case Language::DE:
            return de->locations(g);
        case Language::EN:
            return en->locations(g);
        case Language::ES:
            return es->locations(g);
        case Language::FR:
            return fr->locations(g);
        case Language::IT:
            return it->locations(g);
        case Language::JP:
            return jp->locations(g);
        case Language::KO:
            return ko->locations(g);
        case Language::NL:
            return nl->locations(g);
        case Language::PT:
            return pt->locations(g);
        case Language::ZH:
            return zh->locations(g);
        case Language::TW:
            return tw->locations(g);
        case Language::RU:
            return ru->locations(g);
    }
    return emptyU16Map;
}

size_t i18n::numGameStrings(u8 lang)
{
    switch (lang)
    {
        case Language::DE:
            return de->numGameStrings();
        case Language::EN:
            return en->numGameStrings();
        case Language::ES:
            return es->numGameStrings();
        case Language::FR:
            return fr->numGameStrings();
        case Language::IT:
            return it->numGameStrings();
        case Language::JP:
            return jp->numGameStrings();
        case Language::KO:
            return ko->numGameStrings();
        case Language::NL:
            return nl->numGameStrings();
        case Language::PT:
            return pt->numGameStrings();
        case Language::ZH:
            return zh->numGameStrings();
        case Language::TW:
            return tw->numGameStrings();
        case Language::RU:
            return ru->numGameStrings();
    }
    return 0;
}
