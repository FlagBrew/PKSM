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

#include "BagScreen.hpp"
#include "AccelButton.hpp"
#include "BagItemOverlay.hpp"
#include "ClickButton.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "sav/Item.hpp"
#include "sav/Sav1.hpp"
#include "sav/Sav2.hpp"
#include "sav/Sav3.hpp"

BagScreen::BagScreen()
    : Screen(i18n::localize("A_ITEM_EDIT") + '\n' + i18n::localize("L_POUCH") + '\n' +
             i18n::localize("R_ITEM") + '\n' + i18n::localize("B_BACK")),
      limits(TitleLoader::save->pouches()),
      allowedItems(TitleLoader::save->generation() == pksm::Generation::THREE
                       ? ((pksm::Sav3*)TitleLoader::save.get())->validItems3()
                       : (TitleLoader::save->generation() == pksm::Generation::TWO
                                 ? ((pksm::Sav2*)TitleLoader::save.get())->validItems2()
                                 : (TitleLoader::save->generation() == pksm::Generation::ONE
                                           ? ((pksm::Sav1*)TitleLoader::save.get())->validItems1()
                                           : TitleLoader::save->validItems())))
{
    currentPouch = (int)limits[0].first;
    for (size_t i = 0; i < limits.size(); i++)
    {
        buttons.push_back(std::make_unique<Button>(
            3, i * 30 + 1, 100, 30, [this, i]() { return switchPouch(i); },
            ui_sheet_emulated_button_pouch_idx,
            i18n::pouch(Configuration::getInstance().language(), limits[i].first), FONT_SIZE_12,
            COLOR_BLACK));
    }
    buttons.push_back(std::make_unique<AccelButton>(
        117, -15, 198, 30, [this]() { return clickIndex(-1); }, ui_sheet_res_null_idx, "",
        FONT_SIZE_12, COLOR_BLACK, 10, 5));
    for (size_t i = 0; i < std::min(itemsForPouch(limits[0].first).size(), (size_t)7); i++)
    {
        buttons.push_back(std::make_unique<ClickButton>(
            117, 15 + i * 30, 131, 30, [this, i]() { return clickIndex(i); }, ui_sheet_res_null_idx,
            "", FONT_SIZE_12, COLOR_BLACK));
    }
    buttons.push_back(std::make_unique<AccelButton>(
        117, 225, 198, 30, [this]() { return clickIndex(7); }, ui_sheet_res_null_idx, "",
        FONT_SIZE_12, COLOR_BLACK, 10, 5));
    for (int i = 0; i < 7; i++)
    {
        amountButtons.push_back(std::make_unique<AccelButton>(
            249, 23 + i * 30, 13, 13,
            [this, i]()
            {
                editCount(false, i);
                selectingPouch = false;
                selectedItem   = i;
                return false;
            },
            ui_sheet_emulated_button_minus_small_black_idx, "", 0.0f, COLOR_BLACK));
        amountButtons.push_back(std::make_unique<ClickButton>(
            262, 23 + i * 30, 37, 13,
            [this, i]()
            {
                setCount(i);
                selectingPouch = false;
                selectedItem   = i;
                return false;
            },
            ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
        amountButtons.push_back(std::make_unique<AccelButton>(
            299, 23 + i * 30, 13, 13,
            [this, i]()
            {
                editCount(true, i);
                selectingPouch = false;
                selectedItem   = i;
                return false;
            },
            ui_sheet_emulated_button_plus_small_black_idx, "", 0.0f, COLOR_BLACK));
    }

    updateFirstEmpty();
}

void BagScreen::drawTop() const
{
    Gui::sprite(ui_sheet_emulated_bg_top_yellow_idx, 0, 0);
    Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
    Gui::backgroundAnimatedTop();
    Gui::sprite(ui_sheet_bar_arc_top_red_idx, 0, 0);
}

void BagScreen::drawBottom() const
{
    Gui::sprite(ui_sheet_emulated_bg_bottom_yellow_idx, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::drawSolidRect(0, 0, 110, 240, COLOR_DARKYELLOW);

    for (const auto& button : buttons)
    {
        button->draw();
    }

    if (firstItem != 0)
    {
        Gui::sprite(ui_sheet_button_editor_idx, 131, -15);
    }
    if (firstItem + 7 < firstEmpty)
    {
        Gui::sprite(ui_sheet_button_editor_idx, 131, 225);
    }

    for (int i = firstItem > 0 ? -1 : 0;
         i <= std::min(std::min(firstEmpty - firstItem, 7), limits[currentPouch].second); i++)
    {
        auto item = TitleLoader::save->item(limits[currentPouch].first, firstItem + i);
        Gui::sprite(ui_sheet_emulated_button_item_idx, 117, 15 + 30 * i);
        u16 id;
        if (item->generation() == pksm::Generation::ONE)
        {
            id = ((pksm::Item1*)item.get())->id1();
        }
        else if (item->generation() == pksm::Generation::TWO)
        {
            id = ((pksm::Item2*)item.get())->id2();
        }
        else if (item->generation() == pksm::Generation::THREE)
        {
            id = ((pksm::Item3*)item.get())->id3();
        }
        else
        {
            id = item->id();
        }
        const std::string& text =
            item->generation() == pksm::Generation::THREE
                ? i18n::item3(Configuration::getInstance().language(), id)
                : (item->generation() == pksm::Generation::TWO
                          ? i18n::item2(Configuration::getInstance().language(), id)
                          : (item->generation() == pksm::Generation::ONE
                                    ? i18n::item1(Configuration::getInstance().language(), id)
                                    : i18n::item(Configuration::getInstance().language(), id)));
        Gui::text(text, 117 + 131 / 2, 30 + 30 * i, FONT_SIZE_12,
            canEdit(limits[currentPouch].first, *item) ? COLOR_BLACK : COLOR_GREY, TextPosX::CENTER,
            TextPosY::CENTER);
        if (id > 0)
        {
            Gui::text(std::to_string((int)item->count()), 262 + 37 / 2, 30 + 30 * i, FONT_SIZE_12,
                canEdit(limits[currentPouch].first, *item) ? COLOR_BLACK : COLOR_GREY,
                TextPosX::CENTER, TextPosY::CENTER);
        }
    }

    u8 mod    = 0;
    auto item = TitleLoader::save->item(limits[currentPouch].first, firstEmpty);
    u16 id    = item->generation() == pksm::Generation::THREE
                  ? ((pksm::Item3*)item.get())->id3()
                  : (item->generation() == pksm::Generation::TWO
                            ? ((pksm::Item2*)item.get())->id2()
                            : (item->generation() == pksm::Generation::ONE
                                      ? ((pksm::Item1*)item.get())->id1()
                                      : item->id()));
    if (id > 0)
    {
        mod = 1;
    }
    for (int i = 0;
         i < std::min(std::min(firstEmpty - firstItem + mod, 7), limits[currentPouch].second); i++)
    {
        if (canEdit(limits[currentPouch].first,
                *TitleLoader::save->item(limits[currentPouch].first, firstItem + i)))
        {
            amountButtons[i * 3]->draw();
            amountButtons[i * 3 + 2]->draw();
        }
    }

    int xMod = Gui::pointerBob();

    if (!selectingPouch)
    {
        Gui::sprite(ui_sheet_pointer_horizontal_idx, 100 + xMod, 20 + selectedItem * 30);
    }
    Gui::sprite(
        ui_sheet_pointer_horizontal_idx, -12 + (selectingPouch ? xMod : 0), 6 + currentPouch * 30);
}

void BagScreen::update(touchPosition* touch)
{
    if (justSwitched)
    {
        if (keysHeld() & KEY_TOUCH)
        {
            return;
        }
        else
        {
            justSwitched = false;
        }
    }
    u32 downKeys   = hidKeysDown();
    u32 repeatKeys = hidKeysDownRepeat();
    if (downKeys & KEY_B)
    {
        if (!selectingPouch)
        {
            selectingPouch = true;
        }
        else
        {
            Gui::screenBack();
            return;
        }
    }
    if (downKeys & KEY_R)
    {
        selectingPouch = false;
    }
    if (downKeys & KEY_L)
    {
        selectingPouch = true;
    }
    if (!selectingPouch)
    {
        if (repeatKeys & KEY_DOWN)
        {
            if (selectedItem + firstItem < firstEmpty)
            {
                if (selectedItem == 6)
                {
                    if (firstItem + 6 < firstEmpty)
                    {
                        firstItem++;
                    }
                }
                else
                {
                    selectedItem++;
                }
            }
            else
            {
                selectedItem = 0;
                firstItem    = 0;
            }
        }
        else if (repeatKeys & KEY_UP)
        {
            if (firstItem > 0 && selectedItem == 0)
            {
                firstItem--;
            }
            else if (selectedItem > 0)
            {
                selectedItem--;
            }
            else
            {
                firstItem = std::max(firstEmpty - 6, 0);
                for (int i = firstItem + 6; i > firstItem; i--)
                {
                    if (i == firstEmpty)
                    {
                        selectedItem = i - firstItem;
                    }
                }
            }
        }
        else if (repeatKeys & KEY_LEFT)
        {
            editCount(false, selectedItem);
        }
        else if (repeatKeys & KEY_RIGHT)
        {
            editCount(true, selectedItem);
        }

        if (downKeys & KEY_A)
        {
            editItem();
        }
    }
    else
    {
        if (repeatKeys & KEY_DOWN)
        {
            currentPouch = (currentPouch + 1) % limits.size();
            switchPouch(currentPouch);
        }
        else if (repeatKeys & KEY_UP)
        {
            int newPouch = currentPouch - 1;
            if (newPouch == -1)
            {
                newPouch = limits.size() - 1;
            }
            switchPouch(newPouch);
        }

        if (downKeys & KEY_A)
        {
            selectingPouch = false;
        }
    }
    for (auto& button : buttons)
    {
        button->update(touch);
    }

    u8 mod    = 0;
    auto item = TitleLoader::save->item(limits[currentPouch].first, firstEmpty);
    u16 id    = item->generation() == pksm::Generation::THREE
                  ? ((pksm::Item3*)item.get())->id3()
                  : (item->generation() == pksm::Generation::TWO
                            ? ((pksm::Item2*)item.get())->id2()
                            : (item->generation() == pksm::Generation::ONE
                                      ? ((pksm::Item1*)item.get())->id1()
                                      : item->id()));
    if (id > 0)
    {
        mod = 1;
    }
    for (int i = 0;
         i < std::min(std::min(firstEmpty - firstItem + mod, 7), limits[currentPouch].second); i++)
    {
        amountButtons[i * 3]->update(touch);
        amountButtons[i * 3 + 1]->update(touch);
        amountButtons[i * 3 + 2]->update(touch);
    }
}

bool BagScreen::clickIndex(int i)
{
    if (i == -1)
    {
        if (firstItem > 0)
        {
            firstItem--;
            selectedItem = 0;
        }
        else
        {
            firstItem    = std::max(0, firstEmpty - 6);
            selectedItem = 6;
        }
    }
    else if (i == 7)
    {
        if (firstItem + 6 < firstEmpty)
        {
            firstItem++;
            selectedItem = 6;
        }
        else
        {
            firstItem    = 0;
            selectedItem = 0;
        }
    }
    else if (selectedItem == i)
    {
        editItem();
    }
    else
    {
        if (i <= firstEmpty - firstItem)
        {
            selectedItem = i;
        }
    }
    selectingPouch = false;
    return false;
}

bool BagScreen::switchPouch(int newPouch)
{
    currentPouch = newPouch;

    updateFirstEmpty();

    firstItem    = 0;
    selectedItem = 0;
    return false;
}

void BagScreen::editItem()
{
    int limit = itemsForPouch(limits[currentPouch].first).size() + 1; // Add one for None
    std::vector<std::pair<const std::string*, int>> items(limit);
    items[0] = std::make_pair(&i18n::item(Configuration::getInstance().language(), 0), 0);
    auto currentItem =
        TitleLoader::save->item(limits[currentPouch].first, firstItem + selectedItem);

    std::pair<const std::string*, int> currentItemPair;
    if (currentItem->generation() == pksm::Generation::ONE)
    {
        currentItemPair = std::make_pair(&i18n::item1(Configuration::getInstance().language(),
                                             ((pksm::Item1*)currentItem.get())->id1()),
            ((pksm::Item1*)currentItem.get())->id1());
    }
    else if (currentItem->generation() == pksm::Generation::TWO)
    {
        currentItemPair = std::make_pair(&i18n::item2(Configuration::getInstance().language(),
                                             ((pksm::Item2*)currentItem.get())->id2()),
            ((pksm::Item2*)currentItem.get())->id2());
    }
    else if (currentItem->generation() == pksm::Generation::THREE)
    {
        currentItemPair = std::make_pair(&i18n::item3(Configuration::getInstance().language(),
                                             ((pksm::Item3*)currentItem.get())->id3()),
            ((pksm::Item3*)currentItem.get())->id3());
    }
    else
    {
        currentItemPair =
            std::make_pair(&i18n::item(Configuration::getInstance().language(), currentItem->id()),
                currentItem->id());
    }

    if (!canEdit(limits[currentPouch].first, *currentItem))
    {
        return;
    }

    for (int i = 1; i < limit; i++)
    {
        int itemId = itemsForPouch(limits[currentPouch].first)[i - 1];
        if (TitleLoader::save->generation() == pksm::Generation::ONE)
        {
            items[i] = std::make_pair(
                &i18n::item1(Configuration::getInstance().language(), itemId), itemId);
        }
        else if (TitleLoader::save->generation() == pksm::Generation::TWO)
        {
            items[i] = std::make_pair(
                &i18n::item2(Configuration::getInstance().language(), itemId), itemId);
        }
        else if (TitleLoader::save->generation() == pksm::Generation::THREE)
        {
            items[i] = std::make_pair(
                &i18n::item3(Configuration::getInstance().language(), itemId), itemId);
        }
        else
        {
            items[i] = std::make_pair(
                &i18n::item(Configuration::getInstance().language(), itemId), itemId);
        }
    }
    std::sort(items.begin() + 1, items.end(),
        [](std::pair<const std::string*, int> p1, std::pair<const std::string*, int> p2)
        { return (*p1.first) < (*p2.first); });

    size_t currItemIndex = 0;

    for (size_t i = 0; i < items.size(); i++)
    {
        if (items[i] == currentItemPair)
        {
            currItemIndex = i;
            break;
        }
    }

    addOverlay<BagItemOverlay>(
        items, currItemIndex, limits[currentPouch], firstItem + selectedItem, firstEmpty);
}

void BagScreen::editCount(bool up, int selected)
{
    auto item = TitleLoader::save->item(limits[currentPouch].first, firstItem + selected);

    if (!canEdit(limits[currentPouch].first, *item))
    {
        return;
    }

    u16 id = item->generation() == pksm::Generation::THREE
               ? ((pksm::Item3*)item.get())->id3()
               : (item->generation() == pksm::Generation::TWO
                         ? ((pksm::Item2*)item.get())->id2()
                         : (item->generation() == pksm::Generation::ONE
                                   ? ((pksm::Item1*)item.get())->id1()
                                   : item->id()));
    if (id > 0)
    {
        if (up)
        {
            if (item->count() < item->maxCount())
            {
                item->count(item->count() + 1);
            }
            else
            {
                item->count(1);
            }
        }
        else
        {
            if (item->count() > 1)
            {
                item->count(item->count() - 1);
            }
            else
            {
                item->count(item->maxCount());
            }
        }
        TitleLoader::save->item(*item, limits[currentPouch].first, firstItem + selected);
    }
}

bool BagScreen::canEdit(pksm::Sav::Pouch pouch, const pksm::Item& item) const
{
    if (pouch == pksm::Sav::Pouch::KeyItem)
    {
        return false;
    }
    if (TitleLoader::save->generation() == pksm::Generation::LGPE)
    {
        if (std::find(lgpeKeyItems.begin(), lgpeKeyItems.end(), item.id()) != lgpeKeyItems.end())
        {
            return false;
        }
        return true;
    }
    else if (TitleLoader::save->generation() == pksm::Generation::ONE)
    {
        if (std::find(rgbyKeyItems.begin(), rgbyKeyItems.end(),
                static_cast<const pksm::Item1&>(item).id1()) != rgbyKeyItems.end())
        {
            return false;
        }
        return true;
    }
    return true;
}

void BagScreen::setCount(int selected)
{
    auto item = TitleLoader::save->item(limits[currentPouch].first, firstItem + selected);

    if (!canEdit(limits[currentPouch].first, *item))
    {
        return;
    }

    u16 id = item->generation() == pksm::Generation::THREE
               ? ((pksm::Item3*)item.get())->id3()
               : (item->generation() == pksm::Generation::TWO
                         ? ((pksm::Item2*)item.get())->id2()
                         : (item->generation() == pksm::Generation::ONE
                                   ? ((pksm::Item1*)item.get())->id1()
                                   : item->id()));
    if (id > 0)
    {
        SwkbdState state;
        swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, item->maxCount() < 10000 ? 4 : 5);
        swkbdSetHintText(&state, i18n::localize("ITEMS").c_str());
        swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
        char input[6]   = {0};
        SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
        input[5]        = '\0';
        if (ret == SWKBD_BUTTON_CONFIRM)
        {
            int newCount = std::atoi(input);
            item->count(std::min((int)item->maxCount(), newCount));
            TitleLoader::save->item(*item, limits[currentPouch].first, firstItem + selected);
        }
    }
}

void BagScreen::updateFirstEmpty()
{
    firstEmpty = limits[currentPouch].second - 1;
    for (int i = 0; i < limits[currentPouch].second; i++)
    {
        auto item = TitleLoader::save->item(limits[currentPouch].first, i);
        u16 id    = item->generation() == pksm::Generation::THREE
                      ? ((pksm::Item3*)item.get())->id3()
                      : (item->generation() == pksm::Generation::TWO
                                ? ((pksm::Item2*)item.get())->id2()
                                : (item->generation() == pksm::Generation::ONE
                                          ? ((pksm::Item1*)item.get())->id1()
                                          : item->id()));
        if (id == 0)
        {
            firstEmpty = i;
            break;
        }
    }
}

std::span<const int> BagScreen::itemsForPouch(pksm::Sav::Pouch pouch) const
{
    const auto found = std::find_if(allowedItems.begin(), allowedItems.end(),
        [s = pouch](const auto& i) { return i.first == s; });

    if (found->second.size() > 1000 || found->second.size() == 0)
    {
        // Should a match fail to be found, we don't want the app freaking out, so return an empty
        // array.
        return {};
    }

    return found->second;
}
