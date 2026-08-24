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

#ifndef LISTPICKEROVERLAY_HPP
#define LISTPICKEROVERLAY_HPP

#include "SearchableOverlay.hpp"

// A searchable picker drawn as two columns of ten text rows on the top screen.
// Subclasses provide the text of each entry; everything else about browsing is
// inherited from SearchableOverlay.
class ListPickerOverlay : public SearchableOverlay<HidDirection::VERTICAL, HidDirection::HORIZONTAL>
{
public:
    ListPickerOverlay(ReplaceableScreen& screen, const std::string& instructionText,
        const std::string& searchHint)
        : SearchableOverlay(screen, instructionText, searchHint, 20, 2)
    {
    }

    void drawTop() const override;

protected:
    // Text of the entry at index, which counts from the start of the filtered entries.
    virtual std::string entryLine(size_t index) const = 0;
};

#endif
