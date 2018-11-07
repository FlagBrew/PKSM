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

#include "InjectSelectorScreen.hpp"
#include "InjectorScreen.hpp"
#include "loader.hpp"

InjectSelectorScreen::InjectSelectorScreen() : hid(10, 2)
{
    MysteryGift::init(TitleLoader::save->generation());
    wondercards = MysteryGift::wondercards();

    if (TitleLoader::save->generation() == 5)
    {
        if (TitleLoader::save->version() == 22 || TitleLoader::save->version() == 23)
        {
            ((SavB2W2*)TitleLoader::save.get())->cryptMysteryGiftData();
        }
        else
        {
            ((SavBW*)TitleLoader::save.get())->cryptMysteryGiftData();
        }
    }

    buttons.push_back(new Button(160 - 70/2, 207 - 23, 70, 23, [this](){ return this->doQR(); }, ui_sheet_emulated_button_qr_idx, "", FONT_SIZE_14, COLOR_WHITE));
}

InjectSelectorScreen::~InjectSelectorScreen()
{
    MysteryGift::exit();
    if (TitleLoader::save->generation() == 5)
    {
        if (TitleLoader::save->version() == 22 || TitleLoader::save->version() == 23)
        {
            ((SavB2W2*)TitleLoader::save.get())->cryptMysteryGiftData();
        }
        else
        {
            ((SavBW*)TitleLoader::save.get())->cryptMysteryGiftData();
        }
    }
    for (auto button : buttons)
    {
        delete button;
    }
}

void InjectSelectorScreen::update(touchPosition* touch)
{
    Screen::update();
    hid.update(wondercards.size());
    u32 downKeys = hidKeysDown();
    u32 heldKeys = hidKeysHeld();
    if (downKeys & KEY_B)
    {
        Gui::screenBack();
        return;
    }
    if (heldKeys & KEY_L && heldKeys & KEY_R)
    {
        doQR();
        return;
    }
    if (downKeys & KEY_A)
    {
        Gui::setScreen(std::make_unique<InjectorScreen>(wondercards[hid.fullIndex()]));
        return;
    }

    for (auto button : buttons)
    {
        if (button->update(touch))
        {
            return;
        }
    }
}

void InjectSelectorScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::backgroundTop(true);

    Gui::dynamicText(GFX_TOP, 4, "Event Database", FONT_SIZE_14, FONT_SIZE_14, C2D_Color32(140, 158, 255, 255));

    for (size_t i = 0; i < 10; i++)
    {
        if (i % 2 == 0)
        {
            int x = 20;
            int y = 41 + (i / 2) * 37;
            if (i == 0)
            {
                Gui::sprite(i == hid.index() ? ui_sheet_eventmenu_bar_selected_idx : ui_sheet_eventmenu_bar_unselected_idx, x, y);
            }
            else if (i == 8)
            {
                Gui::sprite(i == hid.index() ? ui_sheet_emulated_eventmenu_bar_selected_flipped_vertical_idx : ui_sheet_emulated_eventmenu_bar_unselected_flipped_vertical_idx, x, y);
            }
            else
            {
                C2D_DrawRectSolid(x, y, 0.5f, 178, 34, i == hid.index() ? C2D_Color32(0x3D, 0x5A, 0xFE, 0xFF) : C2D_Color32(0x8C, 0x9E, 0xFF, 0xFF));
            }
        }
        else
        {
            int x = 201;
            int y = 41 + (i / 2) * 37;
            if (i == 1)
            {
                Gui::sprite(i == hid.index() ? ui_sheet_emulated_eventmenu_bar_selected_flipped_horizontal_idx : ui_sheet_emulated_eventmenu_bar_unselected_flipped_horizontal_idx, x, y);
            }
            else if (i == 9)
            {
                Gui::sprite(i == hid.index() ? ui_sheet_emulated_eventmenu_bar_selected_flipped_both_idx : ui_sheet_emulated_eventmenu_bar_unselected_flipped_both_idx, x, y);
            }
            else
            {
                C2D_DrawRectSolid(x, y, 0.5f, 178, 34, i == hid.index() ? C2D_Color32(0x3D, 0x5A, 0xFE, 0xFF) : C2D_Color32(0x8C, 0x9E, 0xFF, 0xFF));
            }
        }
    }

    for (size_t i = hid.page() * 10; i < (size_t) hid.page() * 10 + 10; i++)
    {
        if (i >= wondercards.size())
        {
            break;
        }
        else
        {
            MysteryGift::giftData data;
            std::string lang = i18n::langString(Configuration::getInstance().language());
            if (wondercards[i].find(lang) != wondercards[i].end())
            {
                data = MysteryGift::wondercardInfo(wondercards[i][lang]);
            }
            else
            {
                data = MysteryGift::wondercardInfo(*wondercards[i].begin());
            }
            int x = i % 2 == 0 ? 21 : 201;
            int y = 43 + ((i % 10) / 2) * 37;
            if (data.species == -1)
            {
                Gui::sprite(ui_sheet_icon_item_idx, x + 12, y + 9);
            }
            else
            {
                Gui::pkm(data.species, data.form, TitleLoader::save->generation(), x, y);
            }
            std::string text;
            if (data.name.size() > 30)
            {
                size_t chop = 26;
                // Get rid of ending codepoint segments
                while (data.name[chop - 1] & 0x80 && !(data.name[chop - 1] & 0x40))
                {
                    chop--;
                }
                // Get rid of the first codepoint segment
                if (data.name[chop - 1] & 0x80 && data.name[chop - 1] & 0x40)
                {
                    chop--;
                }
                text = data.name.substr(0, chop) + "...";
            }
            else
            {
                text = data.name;
            }
            Gui::dynamicText(text, x + 34, y + 10, FONT_SIZE_11, FONT_SIZE_11, i == hid.fullIndex() ? C2D_Color32(232, 234, 246, 255) : C2D_Color32(26, 35, 126, 255), 138, true);
        }
    }

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(true);
    Gui::dynamicText(GFX_BOTTOM, 224, "Press \uE000 to continue or \uE001 to return.", FONT_SIZE_11, FONT_SIZE_11, C2D_Color32(197, 202, 233, 255));

    Gui::sprite(ui_sheet_eventmenu_page_indicator_idx, 65, 13);

    Gui::staticText("\uE004", 75, 17, FONT_SIZE_18, FONT_SIZE_18, C2D_Color32(197, 202, 233, 255), false);
    Gui::staticText("\uE005", 228, 17, FONT_SIZE_18, FONT_SIZE_18, C2D_Color32(197, 202, 233, 255), false);
    Gui::dynamicText(92, 20, 136, StringUtils::format("%d/%d", hid.page() + 1, wondercards.size() % 10 == 0 ? wondercards.size() / 10 : wondercards.size() / 10 + 1), FONT_SIZE_12, FONT_SIZE_12, C2D_Color32(197, 202, 233, 255));

    for (auto button : buttons)
    {
        button->draw();
    }

    Gui::staticText(GFX_BOTTOM, 207 - 21, "\uE004+\uE005 \uE01E", FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
}

bool InjectSelectorScreen::doQR()
{
    u8* data = nullptr;
    QRMode initMode = QRMode(TitleLoader::save->generation());

    QRScanner::init(initMode, data);

    if (data != nullptr)
    {
        std::unique_ptr<WCX> wcx = nullptr;

        switch (TitleLoader::save->generation())
        {
            case 4:
                wcx = std::make_unique<PGT>(data);
                break;
            case 5:
                wcx = std::make_unique<PGF>(data);
                break;
            case 6:
                wcx = std::make_unique<WC6>(data);
                break;
            case 7:
                wcx = std::make_unique<WC7>(data);
                break;
        }

        if (wcx)
        {
            Gui::setScreen(std::make_unique<InjectorScreen>(std::move(wcx)));

            delete data;
            return true;
        }

        delete data;
    }
    return false;
}