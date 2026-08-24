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

#include "MoveOverlay.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "pkx/PKX.hpp"
#include "sav/Sav.hpp"
#include "utils.hpp"
#include <set>

namespace
{
    int index(std::vector<std::pair<pksm::Move, std::string>>& search, const std::string& v)
    {
        if (v == search[0].second || v == "")
        {
            return 0;
        }
        int index = -1, min = 0, mid = 0, max = search.size();
        while (min <= max)
        {
            mid = min + (max - min) / 2;
            if (search[mid].second == v)
            {
                index = mid;
                break;
            }
            if (search[mid].second < v)
            {
                min = mid + 1;
            }
            else
            {
                max = mid - 1;
            }
        }
        return index >= 0 ? index : 0;
    }
}

MoveOverlay::MoveOverlay(ReplaceableScreen& screen, pksm::IPKFilterable& object, int moveIndex)
    : ListPickerOverlay(screen, i18n::localize("A_SELECT") + '\n' + i18n::localize("B_BACK"),
          i18n::localize("MOVE")),
      object(object),
      moveIndex(moveIndex)
{
    const std::vector<std::string>& rawMoves =
        i18n::rawMoves(Configuration::getInstance().language());
    pksm::Generation gen = !object.isFilter() ? object.generation() : pksm::Generation::EIGHT;
    const std::set<pksm::Move> availableMoves =
        TitleLoader::save
            ? TitleLoader::save->availableMoves()
            : pksm::VersionTables::availableMoves(pksm::GameVersion::oldestVersion(gen));
    moves.reserve(availableMoves.size());
    for (auto i = availableMoves.begin(); i != availableMoves.end(); i++)
    {
        if (*i >= pksm::Move::BreakneckBlitzA && *i <= pksm::Move::TwinkleTackleB)
        {
            continue;
        }
        moves.emplace_back(*i, rawMoves[u16(*i)]);
    }
    std::sort(moves.begin(), moves.end(),
        [](const std::pair<pksm::Move, std::string>& pair1,
            const std::pair<pksm::Move, std::string>& pair2)
        {
            if (pair1.first == pksm::Move::None)
            {
                return pair2.first != pksm::Move::None;
            }
            if (pair2.first == pksm::Move::None)
            {
                return false;
            }
            return pair1.second < pair2.second;
        });
    validMoves = moves;

    hid.update(moves.size());
    if (moveIndex < 4)
    {
        hid.select((u16)index(
            moves, i18n::move(Configuration::getInstance().language(), object.move(moveIndex))));
    }
    else
    {
        hid.select((u16)index(moves, i18n::move(Configuration::getInstance().language(),
                                         object.relearnMove(moveIndex - 4))));
    }
}

std::string MoveOverlay::entryLine(size_t index) const
{
    return std::to_string(u16(moves[index].first)) + " - " + moves[index].second;
}

void MoveOverlay::filter(const std::string& search)
{
    if (search.empty())
    {
        moves = validMoves;
        return;
    }
    moves.clear();
    moves.emplace_back(validMoves[0]);
    for (size_t i = 1; i < validMoves.size(); i++)
    {
        std::string itemName = validMoves[i].second.substr(0, search.size());
        StringUtils::toLower(itemName);
        if (itemName == search)
        {
            moves.emplace_back(validMoves[i]);
        }
    }
}

bool MoveOverlay::commit()
{
    if (moveIndex < 4)
    {
        object.move(moveIndex, moves[hid.fullIndex()].first);
    }
    else
    {
        object.relearnMove(moveIndex - 4, moves[hid.fullIndex()].first);
    }
    if (!object.isFilter())
    {
        static_cast<pksm::PKX&>(object).fixMoves();
        static_cast<pksm::PKX&>(object).healPP();
    }
    return true;
}

void MoveOverlay::discard()
{
    if (!object.isFilter())
    {
        static_cast<pksm::PKX&>(object).fixMoves();
    }
}
