/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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

// Response bodies in, pages or errors out. Runs on the host: no 3DS, no libctru, no network.

#include "GpssPage.hpp"
#include <cstdio>
#include <string>

namespace
{
    int failures = 0;

    void check(bool condition, const char* what)
    {
        if (!condition)
        {
            failures++;
            std::printf("FAIL: %s\n", what);
        }
    }

    const Gpss::Page* pageOf(const std::variant<Gpss::Page, Gpss::Error>& result)
    {
        return std::get_if<Gpss::Page>(&result);
    }

    bool isError(const std::variant<Gpss::Page, Gpss::Error>& result, Gpss::Error expected)
    {
        const auto* error = std::get_if<Gpss::Error>(&result);
        return error && *error == expected;
    }

    // "AQID" is base64 for the three bytes 01 02 03.
    const std::string MON =
        R"({"base_64":"AQID","generation":"7","legal":true,"code":"0123456789"})";

    std::string pokemonPage(const std::string& mons, int pages = 3)
    {
        return R"({"total":42,"pages":)" + std::to_string(pages) + R"(,"pokemon":[)" + mons + "]}";
    }

    std::string bundlePage(const std::string& bundles)
    {
        return R"({"total":9,"pages":2,"bundles":[)" + bundles + "]}";
    }

    void pokemonPageParses()
    {
        const auto result = Gpss::parse(pokemonPage(MON), Gpss::Kind::Pokemon);
        const auto* page  = pageOf(result);
        check(page != nullptr, "a well-formed Pokémon page parses");
        if (!page)
        {
            return;
        }
        check(page->total == 42 && page->pages == 3, "total and page count come from the body");
        check(page->entries.size() == 1, "one entry per Pokémon");
        check(page->entries[0].pokemon.size() == 1, "a Pokémon entry holds one Pokémon");

        const auto& mon = page->entries[0].pokemon[0];
        check(mon.data == std::vector<u8>{1, 2, 3}, "base_64 is decoded once, on parse");
        check(mon.generation == pksm::Generation::SEVEN, "the generation is read");
        check(mon.legal, "legality is read from legal");
        check(mon.downloadCode == "0123456789", "the Pokémon carries its own download code");
    }

    void bundlePageParses()
    {
        const std::string bundle =
            R"({"download_code":"9876543210","count":2,)"
            R"("download_codes":["aaaaaaaaaa","bbbbbbbbbb"],"pokemons":[)" +
            std::string(R"({"base_64":"AQID","generation":"8","legality":false},)") +
            R"({"base_64":"AQID","generation":"6","legality":true}]})";

        const auto result = Gpss::parse(bundlePage(bundle), Gpss::Kind::Bundles);
        const auto* page  = pageOf(result);
        check(page != nullptr, "a well-formed bundle page parses");
        if (!page)
        {
            return;
        }
        check(page->entries.size() == 1, "one entry per bundle");
        check(page->entries[0].downloadCode == "9876543210", "the bundle carries its own code");
        check(page->entries[0].pokemon.size() == 2, "the bundle holds both Pokémon");
        check(!page->entries[0].pokemon[0].legal && page->entries[0].pokemon[1].legal,
            "legality is read from legality");
        check(page->entries[0].pokemon[1].downloadCode == "bbbbbbbbbb",
            "each Pokémon in a bundle keeps its own code");
    }

    void emptySearchIsOnePage()
    {
        const auto result =
            Gpss::parse(R"({"total":0,"pages":0,"pokemon":[]})", Gpss::Kind::Pokemon);
        const auto* page = pageOf(result);
        check(page != nullptr, "an empty result set is still a page");
        // Page arithmetic is modular in the page count, so zero pages would divide by zero.
        check(page && page->pages == 1, "zero pages is reported as one empty page");
        check(page && page->entries.empty(), "an empty result set has no entries");
    }

    void badBodiesAreTyped()
    {
        check(isError(Gpss::parse("<html>nope</html>", Gpss::Kind::Pokemon), Gpss::Error::NotJson),
            "a body that is not JSON is NotJson");
        check(isError(Gpss::parse("[]", Gpss::Kind::Pokemon), Gpss::Error::NotAPage),
            "a JSON array is not a page");
        check(isError(Gpss::parse(R"({"total":1,"pages":1})", Gpss::Kind::Pokemon),
                  Gpss::Error::NotAPage),
            "a page without its item array is NotAPage");
        check(isError(Gpss::parse(R"({"total":1,"pages":"3","pokemon":[]})", Gpss::Kind::Pokemon),
                  Gpss::Error::NotAPage),
            "a page count that is not a number is NotAPage");
        // The bundle search's array is named differently, and that is the whole difference.
        check(isError(Gpss::parse(pokemonPage(MON), Gpss::Kind::Bundles), Gpss::Error::NotAPage),
            "a Pokémon page is not a bundle page");
    }

    void unreadableEntriesAreTyped()
    {
        check(isError(Gpss::parse(
                          pokemonPage(R"({"generation":"7","legal":true})"), Gpss::Kind::Pokemon),
                  Gpss::Error::BadEntry),
            "a Pokémon with no data is BadEntry");
        check(isError(Gpss::parse(
                          pokemonPage(R"({"base_64":"AQID","legal":true})"), Gpss::Kind::Pokemon),
                  Gpss::Error::BadEntry),
            "a Pokémon with no generation is BadEntry");
        check(isError(
                  Gpss::parse(bundlePage(R"({"download_code":"9876543210"})"), Gpss::Kind::Bundles),
                  Gpss::Error::BadEntry),
            "a bundle with no Pokémon array is BadEntry");
    }

    void missingExtrasDegradeInstead()
    {
        // Fields the screens only display are allowed to go missing: the page still works.
        const auto result =
            Gpss::parse(pokemonPage(R"({"base_64":"AQID","generation":"7"})"), Gpss::Kind::Pokemon);
        const auto* page = pageOf(result);
        check(page != nullptr, "a Pokémon with no legality flag still parses");
        check(page && !page->entries[0].pokemon[0].legal, "a missing legality flag reads illegal");
        check(page && page->entries[0].pokemon[0].downloadCode.empty(),
            "a missing download code reads empty");

        const std::string shortCodes =
            R"({"download_code":"9876543210","count":2,"download_codes":["aaaaaaaaaa"],)"
            R"("pokemons":[{"base_64":"AQID","generation":"8"},)"
            R"({"base_64":"AQID","generation":"8"}]})";
        const auto bundles     = Gpss::parse(bundlePage(shortCodes), Gpss::Kind::Bundles);
        const auto* bundlePage = pageOf(bundles);
        check(bundlePage != nullptr, "a bundle with fewer codes than Pokémon still parses");
        check(bundlePage && bundlePage->entries[0].pokemon.size() == 2,
            "the Pokémon are taken from the array, not from count");
        check(bundlePage && bundlePage->entries[0].pokemon[1].downloadCode.empty(),
            "a Pokémon the server gave no code for reads empty");
    }

    void errorCodesAreReadPerSearch()
    {
        check(Gpss::errorCode(R"({"code":7})", Gpss::Kind::Pokemon) == 7,
            "the Pokémon search reports its code");
        check(Gpss::errorCode(R"({"error_code":9})", Gpss::Kind::Bundles) == 9,
            "the bundle search reports its error_code");
        check(!Gpss::errorCode(R"({"error_code":9})", Gpss::Kind::Pokemon),
            "each search reads only its own field");
        check(!Gpss::errorCode("Bad Gateway", Gpss::Kind::Pokemon),
            "a body that is not JSON carries no code");
        check(!Gpss::errorCode(R"({"code":"banned"})", Gpss::Kind::Pokemon),
            "a code that is not a number is no code");
    }
}

int main()
{
    pokemonPageParses();
    bundlePageParses();
    emptySearchIsOnePage();
    badBodiesAreTyped();
    unreadableEntriesAreTyped();
    missingExtrasDegradeInstead();
    errorCodesAreReadPerSearch();

    if (failures == 0)
    {
        std::printf("GpssPage: all checks passed\n");
        return 0;
    }
    std::printf("GpssPage: %d check(s) failed\n", failures);
    return 1;
}
