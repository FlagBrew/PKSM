/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2026 Bernardo Giordano, Admiral Fish, piepie62
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

#include "BoxView.hpp"
#include "Bank.hpp"
#include "banks.hpp"
#include "loader.hpp"
#include "sav/Sav.hpp"

std::unique_ptr<pksm::PKX> SaveBoxView::materialize(int box, int slot) const
{
    return TitleLoader::save->pkm(u8(box), u8(slot));
}

std::unique_ptr<pksm::PKX> BankBoxView::materialize(int box, int slot) const
{
    return Banks::bank->pkm(box, slot);
}

std::unique_ptr<pksm::PKX> PartyView::materialize(int, int slot) const
{
    return TitleLoader::save->pkm(u8(slot));
}
