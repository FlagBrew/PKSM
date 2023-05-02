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

#ifndef PRINTERATOR_HPP
#define PRINTERATOR_HPP

#include <cstdio>
#include <iterator>

class Printerator
{
private:
    FILE* output;

public:
    constexpr Printerator() noexcept : Printerator(stdout) {}

    constexpr explicit Printerator(FILE* o) noexcept : output(o) {}

    using difference_type   = std::ptrdiff_t;
    using value_type        = void;
    using pointer           = void;
    using reference         = void;
    using iterator_category = std::output_iterator_tag;
    using iterator_concept  = std::output_iterator_tag;

    constexpr Printerator& operator*() noexcept { return *this; }

    constexpr const Printerator& operator*() const noexcept { return *this; }

    constexpr bool operator==(const Printerator&) const noexcept  = default;

    constexpr Printerator& operator=(int c) noexcept
    {
        std::fputc(c, output);
        return *this;
    }

    constexpr Printerator& operator++() noexcept { return *this; }

    constexpr Printerator& operator++(int) noexcept { return *this; }
};

#endif
