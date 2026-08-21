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

#include "GpssPage.hpp"
#include "nlohmann/json.hpp"
#include "utils/base64.hpp"
#include <algorithm>

namespace
{
    // Fields the screens dereference have to be there and have to be the right type; fields they
    // only display are allowed to be missing, so a server that stops sending one degrades the page
    // instead of emptying the browser.
    bool isString(const nlohmann::json& json, const char* field)
    {
        return json.contains(field) && json[field].is_string();
    }

    bool isInt(const nlohmann::json& json, const char* field)
    {
        return json.contains(field) && json[field].is_number_integer();
    }

    std::string stringOr(const nlohmann::json& json, const char* field, std::string fallback = "")
    {
        return isString(json, field) ? json[field].get<std::string>() : fallback;
    }

    bool boolOr(const nlohmann::json& json, const char* field, bool fallback)
    {
        return json.contains(field) && json[field].is_boolean() ? json[field].get<bool>()
                                                                : fallback;
    }

    // The base_64/generation pair every Pokémon in either shape carries.
    std::optional<Gpss::Mon> parseMon(
        const nlohmann::json& json, const char* legalField, std::string downloadCode)
    {
        if (!json.is_object() || !isString(json, "base_64") || !isString(json, "generation"))
        {
            return std::nullopt;
        }

        Gpss::Mon mon;
        mon.generation   = pksm::Generation::fromString(json["generation"].get<std::string>());
        mon.data         = base64_decode(json["base_64"].get_ref<const std::string&>());
        mon.legal        = boolOr(json, legalField, false);
        mon.downloadCode = std::move(downloadCode);
        return mon;
    }

    const char* itemField(Gpss::Kind kind)
    {
        return kind == Gpss::Kind::Pokemon ? "pokemon" : "bundles";
    }
}

namespace Gpss
{
    std::variant<Page, Error> parse(std::string_view body, Kind kind)
    {
        const nlohmann::json json = nlohmann::json::parse(body, nullptr, false);
        if (json.is_discarded())
        {
            return Error::NotJson;
        }

        const char* items = itemField(kind);
        // clang-format off
        if (!json.is_object() ||
            !isInt(json, "total") ||
            !isInt(json, "pages") ||
            !json.contains(items) || !json[items].is_array())
        // clang-format on
        {
            return Error::NotAPage;
        }

        Page page;
        page.total = json["total"].get<int>();
        // A search with no results reports zero pages. Calling that one empty page keeps every
        // caller's page arithmetic - which is modular in the page count - defined.
        page.pages = std::max(1, json["pages"].get<int>());

        for (const auto& item : json[items])
        {
            Entry entry;
            if (kind == Kind::Pokemon)
            {
                // The single-Pokémon search puts the download code on the Pokémon itself.
                auto mon = parseMon(item, "legal", stringOr(item, "code"));
                if (!mon)
                {
                    return Error::BadEntry;
                }
                entry.pokemon.emplace_back(std::move(*mon));
            }
            else
            {
                if (!item.is_object() || !item.contains("pokemons") || !item["pokemons"].is_array())
                {
                    return Error::BadEntry;
                }
                entry.downloadCode = stringOr(item, "download_code");

                const bool haveCodes =
                    item.contains("download_codes") && item["download_codes"].is_array();
                size_t index = 0;
                for (const auto& mon : item["pokemons"])
                {
                    std::string code;
                    if (haveCodes && index < item["download_codes"].size() &&
                        item["download_codes"][index].is_string())
                    {
                        code = item["download_codes"][index].get<std::string>();
                    }
                    auto parsed = parseMon(mon, "legality", std::move(code));
                    if (!parsed)
                    {
                        return Error::BadEntry;
                    }
                    entry.pokemon.emplace_back(std::move(*parsed));
                    index++;
                }
            }
            page.entries.emplace_back(std::move(entry));
        }

        return page;
    }

    std::optional<int> errorCode(std::string_view body, Kind kind)
    {
        // Only ever read out of a rejection body, which is why the Pokémon search can name this
        // field the same thing it names a download code without the two colliding.
        const char* field = kind == Kind::Pokemon ? "code" : "error_code";

        const nlohmann::json json = nlohmann::json::parse(body, nullptr, false);
        if (json.is_discarded() || !json.is_object() || !isInt(json, field))
        {
            return std::nullopt;
        }
        return json[field].get<int>();
    }
}
