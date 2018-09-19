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

#ifndef MOVESELECTIONSCREEN_HPP
#define MOVESELECTIONSCREEN_HPP

#include "SelectionScreen.hpp"
#include "Hid.hpp"
#include "Configuration.hpp"
#include "PK7.hpp"
#include "PK6.hpp"

class MoveSelectionScreen : public SelectionScreen
{
public:
    MoveSelectionScreen(std::shared_ptr<PKX> pkm, int moveIndex) : SelectionScreen(pkm), moveIndex(moveIndex), hid(40, 2)
    {
        hid.update(i18n::moves());
        if (moveIndex < 4)
        {
            hid.select((u16) i18n::sortedMoveIndex(Configuration::getInstance().language(), i18n::move(Configuration::getInstance().language(), pkm->move(moveIndex))));
        }
        else
        {
            if (pkm->gen6())
            {
                PK6* pk6 = ((PK6*)pkm.get());
                hid.select((u16) i18n::sortedMoveIndex(Configuration::getInstance().language(), i18n::move(Configuration::getInstance().language(), pk6->relearnMove(moveIndex - 4))));
            }
            else if (pkm->gen7())
            {
                PK7* pk7 = ((PK7*)pkm.get());
                hid.select((u16) i18n::sortedMoveIndex(Configuration::getInstance().language(), i18n::move(Configuration::getInstance().language(), pk7->relearnMove(moveIndex - 4))));
            }
        }
    }
    ~MoveSelectionScreen()
    {
        pkm->fixMoves();
    }
    void draw() const override;
    void update(touchPosition* touch) override;
private:
    int moveIndex;
    Hid hid;
};

#endif