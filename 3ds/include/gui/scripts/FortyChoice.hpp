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

#ifndef FORTYCHOICE_HPP
#define FORTYCHOICE_HPP

#include "HidVertical.hpp"
#include "RunnableScreen.hpp"
#include <string>
#include <vector>

class FortyChoice : public RunnableScreen<size_t>
{
public:
    FortyChoice(char* question, char** text, int items) : RunnableScreen(0), question(question), hid(40, 2), items(items)
    {
        for (int i = 0; i < items; i++)
        {
            labels.emplace_back(text[i]);
        }
    }
    void drawTop() const override;
    void drawBottom() const override;
    void update(touchPosition* touch) override;

private:
    std::string question;
    HidVertical hid;
    std::vector<std::string> labels;
    const int items;
};

#endif
