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
#include "LanguageStrings.hpp"
#include <atomic>
#include <unistd.h>

static LanguageStrings* jp = nullptr;
std::atomic_flag jpInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* en = nullptr;
std::atomic_flag enInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* fr = nullptr;
std::atomic_flag frInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* it = nullptr;
std::atomic_flag itInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* de = nullptr;
std::atomic_flag deInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* es = nullptr;
std::atomic_flag esInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* ko = nullptr;
std::atomic_flag koInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* zh = nullptr;
std::atomic_flag zhInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* tw = nullptr;
std::atomic_flag twInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* nl = nullptr;
std::atomic_flag nlInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* pt = nullptr;
std::atomic_flag ptInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* ru = nullptr;
std::atomic_flag ruInit    = ATOMIC_FLAG_INIT;
static LanguageStrings* ro = nullptr;
std::atomic_flag roInit    = ATOMIC_FLAG_INIT;

static const std::string emptyString                = "";
static const std::vector<std::string> emptyVector   = {};
static const std::map<u16, std::string> emptyU16Map = {};
static const std::map<u8, std::string> emptyU8Map   = {};

void i18n::init(Language lang)
{
    switch (lang)
    {
        case Language::JP:
            if (!jpInit.test_and_set())
            {
                jp = new LanguageStrings(Language::JP);
            }
            break;
        case Language::EN:
            if (!enInit.test_and_set())
            {
                en = new LanguageStrings(Language::EN);
            }
            break;
        case Language::FR:
            if (!frInit.test_and_set())
            {
                fr = new LanguageStrings(Language::FR);
            }
            break;
        case Language::IT:
            if (!itInit.test_and_set())
            {
                it = new LanguageStrings(Language::IT);
            }
            break;
        case Language::DE:
            if (!deInit.test_and_set())
            {
                de = new LanguageStrings(Language::DE);
            }
            break;
        case Language::ES:
            if (!esInit.test_and_set())
            {
                es = new LanguageStrings(Language::ES);
            }
            break;
        case Language::KO:
            if (!koInit.test_and_set())
            {
                ko = new LanguageStrings(Language::KO);
            }
            break;
        case Language::ZH:
            if (!zhInit.test_and_set())
            {
                zh = new LanguageStrings(Language::ZH);
            }
            break;
        case Language::TW:
            if (!twInit.test_and_set())
            {
                tw = new LanguageStrings(Language::TW);
            }
            break;
        case Language::NL:
            if (!nlInit.test_and_set())
            {
                nl = new LanguageStrings(Language::NL);
            }
            break;
        case Language::PT:
            if (!ptInit.test_and_set())
            {
                pt = new LanguageStrings(Language::PT);
            }
            break;
        case Language::RU:
            if (!ruInit.test_and_set())
            {
                ru = new LanguageStrings(Language::RU);
            }
            break;
        case Language::RO:
            if (!roInit.test_and_set())
            {
                ro = new LanguageStrings(Language::RO);
            }
            break;
        case Language::UNUSED:
            break;
    }
}

void i18n::exit(void)
{
    if (jpInit.test_and_set())
    {
        while (!jp)
        {
            usleep(100);
        }
        delete jp;
        jpInit.clear();
    }
    if (enInit.test_and_set())
    {
        while (!en)
        {
            usleep(100);
        }
        delete en;
        enInit.clear();
    }
    if (frInit.test_and_set())
    {
        while (!fr)
        {
            usleep(100);
        }
        delete fr;
        frInit.clear();
    }
    if (itInit.test_and_set())
    {
        while (!it)
        {
            usleep(100);
        }
        delete it;
        itInit.clear();
    }
    if (deInit.test_and_set())
    {
        while (!de)
        {
            usleep(100);
        }
        delete de;
        deInit.clear();
    }
    if (esInit.test_and_set())
    {
        while (!es)
        {
            usleep(100);
        }
        delete es;
        esInit.clear();
    }
    if (koInit.test_and_set())
    {
        while (!ko)
        {
            usleep(100);
        }
        delete ko;
        koInit.clear();
    }
    if (zhInit.test_and_set())
    {
        while (!zh)
        {
            usleep(100);
        }
        delete zh;
        zhInit.clear();
    }
    if (twInit.test_and_set())
    {
        while (!tw)
        {
            usleep(100);
        }
        delete tw;
        twInit.clear();
    }
    if (nlInit.test_and_set())
    {
        while (!nl)
        {
            usleep(100);
        }
        delete nl;
        nlInit.clear();
    }
    if (ptInit.test_and_set())
    {
        while (!pt)
        {
            usleep(100);
        }
        delete pt;
        ptInit.clear();
    }
    if (ruInit.test_and_set())
    {
        while (!ru)
        {
            usleep(100);
        }
        delete ru;
        ruInit.clear();
    }
}

static LanguageStrings* stringsFor(Language lang)
{
    switch (lang)
    {
        case Language::JP:
            if (!jpInit.test_and_set())
            {
                jpInit.clear();
                i18n::init(lang);
            }
            while (!jp)
            {
                usleep(100);
            }
            return jp;
        case Language::EN:
            if (!enInit.test_and_set())
            {
                enInit.clear();
                i18n::init(lang);
            }
            while (!en)
            {
                usleep(100);
            }
            return en;
        case Language::FR:
            if (!frInit.test_and_set())
            {
                frInit.clear();
                i18n::init(lang);
            }
            while (!fr)
            {
                usleep(100);
            }
            return fr;
        case Language::IT:
            if (!itInit.test_and_set())
            {
                itInit.clear();
                i18n::init(lang);
            }
            while (!it)
            {
                usleep(100);
            }
            return it;
        case Language::DE:
            if (!deInit.test_and_set())
            {
                deInit.clear();
                i18n::init(lang);
            }
            while (!de)
            {
                usleep(100);
            }
            return de;
        case Language::ES:
            if (!esInit.test_and_set())
            {
                esInit.clear();
                i18n::init(lang);
            }
            while (!es)
            {
                usleep(100);
            }
            return es;
        case Language::KO:
            if (!koInit.test_and_set())
            {
                koInit.clear();
                i18n::init(lang);
            }
            while (!ko)
            {
                usleep(100);
            }
            return ko;
        case Language::ZH:
            if (!zhInit.test_and_set())
            {
                zhInit.clear();
                i18n::init(lang);
            }
            while (!zh)
            {
                usleep(100);
            }
            return zh;
        case Language::TW:
            if (!twInit.test_and_set())
            {
                twInit.clear();
                i18n::init(lang);
            }
            while (!tw)
            {
                usleep(100);
            }
            return tw;
        case Language::NL:
            if (!nlInit.test_and_set())
            {
                nlInit.clear();
                i18n::init(lang);
            }
            while (!nl)
            {
                usleep(100);
            }
            return nl;
        case Language::PT:
            if (!ptInit.test_and_set())
            {
                ptInit.clear();
                i18n::init(lang);
            }
            while (!pt)
            {
                usleep(100);
            }
            return pt;
        case Language::RU:
            if (!ruInit.test_and_set())
            {
                ruInit.clear();
                i18n::init(lang);
            }
            while (!ru)
            {
                usleep(100);
            }
            return ru;
        case Language::RO:
            if (!roInit.test_and_set())
            {
                roInit.clear();
                i18n::init(lang);
            }
            while (!ro)
            {
                usleep(100);
            }
            return ro;
        case Language::UNUSED:
            break;
    }
    if (!enInit.test_and_set())
    {
        enInit.clear();
        i18n::init(lang);
    }
    while (!en)
    {
        usleep(100);
    }
    return en;
}

const std::string& i18n::ability(Language lang, u16 val)
{
    return stringsFor(lang)->ability(val);
}

const std::string& i18n::ball(Language lang, u8 val)
{
    return stringsFor(lang)->ball(val);
}

const std::string& i18n::form(Language lang, u16 species, u16 form, Generation generation)
{
    return stringsFor(lang)->form(species, form, generation);
}

const std::string& i18n::hp(Language lang, u8 val)
{
    return stringsFor(lang)->hp(val);
}

const std::string& i18n::item(Language lang, u16 val)
{
    return stringsFor(lang)->item(val);
}

const std::string& i18n::move(Language lang, u16 val)
{
    return stringsFor(lang)->move(val);
}

const std::string& i18n::nature(Language lang, u8 val)
{
    return stringsFor(lang)->nature(val);
}

const std::string& i18n::species(Language lang, u16 val)
{
    return stringsFor(lang)->species(val);
}

const std::string& i18n::localize(Language lang, const std::string& val)
{
    return stringsFor(lang)->localize(val);
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

const std::vector<std::string>& i18n::rawItems(Language lang)
{
    return stringsFor(lang)->rawItems();
}

const std::vector<std::string>& i18n::rawMoves(Language lang)
{
    return stringsFor(lang)->rawMoves();
}

const std::string& i18n::location(Language lang, u16 v, Generation generation)
{
    return stringsFor(lang)->location(v, generation);
}

const std::string& i18n::location(Language lang, u16 v, u8 originGame)
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
        case 44:
        case 45:
            return location(lang, v, Generation::EIGHT);
    }
    return emptyString;
}

const std::string& i18n::game(Language lang, u8 v)
{
    return stringsFor(lang)->game(v);
}

const std::map<u16, std::string>& i18n::locations(Language lang, Generation g)
{
    return stringsFor(lang)->locations(g);
}

size_t i18n::numGameStrings(Language lang)
{
    return stringsFor(lang)->numGameStrings();
}

const std::string& i18n::subregion(Language lang, u8 country, u8 value)
{
    return stringsFor(lang)->subregion(country, value);
}

const std::string& i18n::country(Language lang, u8 value)
{
    return stringsFor(lang)->country(value);
}

const std::map<u8, std::string>& i18n::rawCountries(Language lang)
{
    return stringsFor(lang)->rawCountries();
}

const std::map<u8, std::string>& i18n::rawSubregions(Language lang, u8 country)
{
    return stringsFor(lang)->rawSubregions(country);
}
