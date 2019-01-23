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

#include "InjectSelectorScreen.hpp"
#include "InjectorScreen.hpp"
#include "loader.hpp"
#include "FSStream.hpp"

InjectSelectorScreen::InjectSelectorScreen() : hid(10, 2), dumpHid(40, 8)
{
    MysteryGift::init(TitleLoader::save->generation());
    wondercards = MysteryGift::wondercards();

    if (TitleLoader::save->generation() == Generation::FIVE)
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

    gifts = TitleLoader::save->currentGifts();

    buttons.push_back(new Button(160 - 70/2, 207 - 23, 70, 23, [this](){ return this->doQR(); }, ui_sheet_emulated_button_qr_idx, "", FONT_SIZE_14, COLOR_WHITE));
}

InjectSelectorScreen::~InjectSelectorScreen()
{
    MysteryGift::exit();
    if (TitleLoader::save->generation() == Generation::FIVE)
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
    u32 downKeys = hidKeysDown();
    u32 heldKeys = hidKeysHeld();
    if (updateGifts)
    {
        gifts = TitleLoader::save->currentGifts();
    }
    if (!dump)
    {
        hid.update(wondercards.size());
        if (downKeys & KEY_B)
        {
            Gui::screenBack();
            return;
        }
        if ((heldKeys & KEY_L && downKeys & KEY_R) || (downKeys & KEY_L && heldKeys & KEY_R))
        {
            doQR();
            return;
        }
        if (downKeys & KEY_A)
        {
            Gui::setScreen(std::make_unique<InjectorScreen>(wondercards[hid.fullIndex()]));
            updateGifts = true;
            return;
        }
        if (downKeys & KEY_X)
        {
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                Gui::warn(i18n::localize("WC_LGPE"), i18n::localize("NOT_A_BUG"));
            }
            else
            {
                dump = true;
            }
            return;
        }

        if (downKeys & KEY_TOUCH)
        {
            for (auto button : buttons)
            {
                if (button->update(touch))
                {
                    return;
                }
            }
        }
    }
    else
    {
        dumpHid.update(std::max(TitleLoader::save->emptyGiftLocation(), 1));
        if (downKeys & KEY_A)
        {
            dumpCard();
            dump = false;
        }
        else if (downKeys & KEY_B)
        {
            dump = false;
        }
    }
}

void InjectSelectorScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(true);
    Gui::dynamicText(GFX_BOTTOM, 222, i18n::localize("WC_INST1"), FONT_SIZE_11, FONT_SIZE_11, C2D_Color32(197, 202, 233, 255));

    Gui::sprite(ui_sheet_eventmenu_page_indicator_idx, 65, 13);

    Gui::staticText("\uE004", 75, 17, FONT_SIZE_18, FONT_SIZE_18, C2D_Color32(197, 202, 233, 255), false);
    Gui::staticText("\uE005", 228, 17, FONT_SIZE_18, FONT_SIZE_18, C2D_Color32(197, 202, 233, 255), false);
    Gui::dynamicText(92, 20, 136, StringUtils::format("%d/%d", hid.page() + 1, wondercards.size() % 10 == 0 ? wondercards.size() / 10 : wondercards.size() / 10 + 1), FONT_SIZE_12, FONT_SIZE_12, C2D_Color32(197, 202, 233, 255));

    for (auto button : buttons)
    {
        button->draw();
    }

    Gui::staticText(GFX_BOTTOM, 207 - 21, "\uE004+\uE005 \uE01E", FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);

    if (!dump)
    {
        C2D_SceneBegin(g_renderTargetTop);
        Gui::backgroundTop(true);

        Gui::dynamicText(GFX_TOP, 4, i18n::localize("EVENT_DATABASE"), FONT_SIZE_14, FONT_SIZE_14, C2D_Color32(140, 158, 255, 255));

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
                Gui::dynamicText(text, x + 34, y + 12, FONT_SIZE_11, FONT_SIZE_11, i == hid.fullIndex() ? C2D_Color32(232, 234, 246, 255) : C2D_Color32(26, 35, 126, 255), 138, true);
            }
        }
    }
    else
    {
        C2D_DrawRectSolid(0, 0, 0.5, 320, 240, COLOR_MASKBLACK);
        Gui::dynamicText(GFX_BOTTOM, 107, i18n::localize("WC_DUMP1"), FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE);

        C2D_SceneBegin(g_renderTargetTop);
        Gui::sprite(ui_sheet_part_mtx_5x8_idx, 0, 0);
        auto saveGeneration = TitleLoader::save->generation();
        for (size_t i = 0; i < 40; i++)
        {
            int x = i % 8;
            int y = i / 8;
            size_t fullI = i + dumpHid.page() * 40;
            if (fullI >= TitleLoader::save->maxWondercards())
            {
                break;
            }
            if (dumpHid.index() == i)
            {
                C2D_DrawRectSolid(x * 50, y * 48, 0.5f, 49, 47, C2D_Color32(15, 22, 89, 255));
            }
            if (fullI < gifts.size())
            {
                if (gifts[fullI].species > -1)
                {
                    Gui::pkm(gifts[fullI].species, gifts[fullI].form, saveGeneration, x * 50 + 7, y * 48 + 2);
                }
                else
                {
                    Gui::sprite(ui_sheet_icon_item_idx, x * 50 + 20, y * 48 + 18);
                }

                Gui::dynamicText(x * 50, y * 48 + 36, 50, std::to_string(fullI + 1), FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);
            }
            else
            {
                Gui::dynamicText(x * 50, y * 48 + 36, 50, std::to_string(fullI + 1), FONT_SIZE_9, FONT_SIZE_9, COLOR_MASKBLACK);
            }
        }
    }
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
            case Generation::FOUR:
                wcx = std::make_unique<PGT>(data);
                break;
            case Generation::FIVE:
                wcx = std::make_unique<PGF>(data);
                break;
            case Generation::SIX:
                wcx = std::make_unique<WC6>(data);
                break;
            case Generation::SEVEN:
                wcx = std::make_unique<WC7>(data);
                break;
            default:
                break;
        }

        if (wcx)
        {
            Gui::setScreen(std::make_unique<InjectorScreen>(std::move(wcx)));
            updateGifts = true;

            delete[] data;
            return true;
        }

        delete data;
    }
    return false;
}

void InjectSelectorScreen::dumpCard(void) const
{
    auto wc = TitleLoader::save->mysteryGift(dumpHid.fullIndex());
    char stringDate[11] = {0};
    char stringTime[10] = {0};
    time_t unixTime = time(NULL);
    struct tm* timeStruct = gmtime((const time_t *)&unixTime);
    std::strftime(stringDate, 10,"%Y-%m-%d", timeStruct);
    std::strftime(stringTime, 9,"/%H-%M-%S", timeStruct);
    std::string path = std::string("/3ds/PKSM/dumps/") + stringDate;
    mkdir(path.c_str(), 777);
    path += stringTime;
    path += " - " + std::to_string(wc->ID()) + " - " + wc->title();
    switch (wc->generation())
    {
        case Generation::FOUR:
            path += ".pgt";
            break;
        case Generation::FIVE:
            path += ".pgf";
            break;
        case Generation::SIX:
            path += ".wc6";
            break;
        case Generation::SEVEN:
            path += ".wc7";
            break;
        case Generation::LGPE:
            path += ".wb7";
            break;
        case Generation::UNUSED:
            Gui::warn(i18n::localize("THE_FUCK"), i18n::localize("REPORT_THIS"));
            return;
    }
    FSStream out(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_CREATE | FS_OPEN_WRITE, wc->size());
    if (out.good())
    {
        out.write(wc->rawData(), wc->size());
    }
    else
    {
        Gui::error(i18n::localize("FAILED_OPEN_DUMP"), out.result());
    }
    out.close();
}