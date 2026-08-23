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

#ifndef BOXVIEW_HPP
#define BOXVIEW_HPP

#include "pkx/PKX.hpp"
#include <array>
#include <memory>

// One box, materialized once instead of once per draw.
//
// Asking a save or a bank for a slot hands back a freshly allocated PKX holding its
// own copy of the data, so a draw loop that walks every slot of a box pays thirty
// allocations and thirty buffer copies per frame to redraw data that only changes
// when the user does something. A BoxView materializes a slot on first use and keeps
// it: invalidate() and a box change are the only two things that make what it hands
// out stale.
class BoxView
{
public:
    // No generation puts more than thirty Pokemon in a box.
    static constexpr int MAX_SLOTS = 30;

    virtual ~BoxView() = default;

    BoxView(const BoxView&)            = delete;
    BoxView& operator=(const BoxView&) = delete;

    [[nodiscard]] int box() const { return currentBox; }

    // Switching boxes drops what was materialized. Cheap to call every frame, which
    // is how a screen keeps the view in step with a box index something else moved.
    void box(int newBox)
    {
        if (newBox != currentBox)
        {
            currentBox = newBox;
            invalidate();
        }
    }

    // Anything that writes to the underlying storage has to say so here.
    void invalidate()
    {
        for (auto& pkm : materialized)
        {
            pkm = nullptr;
        }
    }

    // Slot must be < MAX_SLOTS. The reference is valid until the next invalidate(),
    // box change, or destruction.
    [[nodiscard]] const pksm::PKX& at(int slot)
    {
        if (!materialized[slot])
        {
            materialized[slot] = materialize(currentBox, slot);
        }
        return *materialized[slot];
    }

protected:
    BoxView() = default;

    [[nodiscard]] virtual std::unique_ptr<pksm::PKX> materialize(int box, int slot) const = 0;

private:
    std::array<std::unique_ptr<pksm::PKX>, MAX_SLOTS> materialized;
    // No box has been materialized yet, so the first box() call fills.
    int currentBox = -1;
};

// Refills the views a screen shows when the scope ends, if `needed`.
//
// A screen's update() leaves through a dozen early returns, and most of them are the
// ones that just wrote to a box. Refilling on scope exit catches every path without
// asking each write site to remember to.
class ScopedRefill
{
public:
    ScopedRefill(bool needed, BoxView& first, BoxView* second = nullptr)
        : first(&first), second(second), needed(needed)
    {
    }

    ScopedRefill(const ScopedRefill&)            = delete;
    ScopedRefill& operator=(const ScopedRefill&) = delete;

    ~ScopedRefill()
    {
        if (needed)
        {
            first->invalidate();
            if (second)
            {
                second->invalidate();
            }
        }
    }

private:
    BoxView* first;
    BoxView* second;
    bool needed;
};

// A box of the loaded save.
class SaveBoxView final : public BoxView
{
protected:
    [[nodiscard]] std::unique_ptr<pksm::PKX> materialize(int box, int slot) const override;
};

// A box of the open bank.
class BankBoxView final : public BoxView
{
protected:
    [[nodiscard]] std::unique_ptr<pksm::PKX> materialize(int box, int slot) const override;
};

// The party of the loaded save. Not a box, but drawn from the same loop and just as
// often, so it goes stale on the same terms. There is only one, so the box index is
// ignored.
class PartyView final : public BoxView
{
protected:
    [[nodiscard]] std::unique_ptr<pksm::PKX> materialize(int box, int slot) const override;
};

#endif
