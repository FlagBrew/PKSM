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

#include "InjectorScreen.hpp"
#include "gui.hpp"
#include "loader.hpp"

static constexpr std::string_view languages[] = {"JPN", "ENG", "FRE", "ITA", "GER", "UNUSED", "SPA", "KOR", "CHS", "CHT"};

bool InjectorScreen::setLanguage(Language language)
{
    if (isLangAvailable(language))
    {
        lang       = language;
        wondercard = MysteryGift::wondercard(ids[i18n::langString(lang)]);

        changeDate();
    }
    return false;
}

InjectorScreen::InjectorScreen(nlohmann::json ids)
    : hid(40, 8), ids(ids), emptySlot(TitleLoader::save->emptyGiftLocation()), gifts(TitleLoader::save->currentGifts())
{
    const std::string& langString = i18n::langString(Configuration::getInstance().language());
    if (ids.find(langString) != ids.end())
    {
        wondercard = MysteryGift::wondercard(ids[langString]);
        game       = MysteryGift::wondercardInfo(ids[langString]).game;
        lang       = Configuration::getInstance().language();
    }
    else
    {
        wondercard = MysteryGift::wondercard(*ids.begin());
        game       = MysteryGift::wondercardInfo(*ids.begin()).game;
        lang       = i18n::langFromString(ids.begin().key());
    }

    slot          = emptySlot + 1;
    int langIndex = 1;
    for (int y = 46; y < 70; y += 23)
    {
        for (int x = 121; x < 274; x += 38)
        {
            if (langIndex != (int)Language::UNUSED)
            {
                buttons.push_back(new Button(
                    x, y, 38, 23, [this, langIndex]() { return this->setLanguage((Language)langIndex); }, ui_sheet_res_null_idx, "", 0, 0));
            }
            langIndex++;
        }
    }
    buttons.push_back(new Button(235, 102, 38, 23,
        [this]() {
            overwriteCard = true;
            return false;
        },
        ui_sheet_res_null_idx, "", 0, 0));
    buttons.push_back(new Button(273, 102, 38, 23,
        [this]() {
            overwriteCard = false;
            return false;
        },
        ui_sheet_res_null_idx, "", 0, 0));
    buttons.push_back(new Button(235, 135, 38, 23,
        [this]() {
            adaptLanguage = true;
            return false;
        },
        ui_sheet_res_null_idx, "", 0, 0));
    buttons.push_back(new Button(273, 135, 38, 23,
        [this]() {
            adaptLanguage = false;
            return false;
        },
        ui_sheet_res_null_idx, "", 0, 0));
    buttons.push_back(new Button(255, 168, 38, 23,
        [this]() {
            choosingSlot = true;
            hid.select(slot - 1);
            return true;
        },
        ui_sheet_emulated_button_unselected_red_idx, "", 0.0f, 0));
    buttons.push_back(new Button(282, 212, 34, 28,
        []() {
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, 0));

    changeDate();
}

InjectorScreen::InjectorScreen(std::unique_ptr<WCX> wcx)
    : wondercard(std::move(wcx)), hid(40, 8), ids({}), emptySlot(TitleLoader::save->emptyGiftLocation()), gifts(TitleLoader::save->currentGifts())
{
    lang = Language::UNUSED;

    slot          = emptySlot + 1;
    int langIndex = 1;
    for (int y = 46; y < 70; y += 23)
    {
        for (int x = 121; x < 274; x += 38)
        {
            if (langIndex != (int)Language::UNUSED)
            {
                buttons.push_back(new Button(
                    x, y, 38, 23, [this, langIndex]() { return this->setLanguage((Language)langIndex); }, ui_sheet_res_null_idx, "", 0, 0));
            }
            langIndex++;
        }
    }
    buttons.push_back(new Button(235, 102, 38, 23,
        [this]() {
            overwriteCard = true;
            return false;
        },
        ui_sheet_res_null_idx, "", 0, 0));
    buttons.push_back(new Button(273, 102, 38, 23,
        [this]() {
            overwriteCard = false;
            return false;
        },
        ui_sheet_res_null_idx, "", 0, 0));
    buttons.push_back(new Button(235, 135, 38, 23,
        [this]() {
            adaptLanguage = true;
            return false;
        },
        ui_sheet_res_null_idx, "", 0, 0));
    buttons.push_back(new Button(273, 135, 38, 23,
        [this]() {
            adaptLanguage = false;
            return false;
        },
        ui_sheet_res_null_idx, "", 0, 0));
    buttons.push_back(new Button(255, 168, 38, 23,
        [this]() {
            if (TitleLoader::save->generation() == Generation::LGPE)
            {
                Gui::warn(i18n::localize("WC_LGPE"), i18n::localize("NOT_A_BUG"));
                return false;
            }
            else
            {
                choosingSlot = true;
                hid.select(slot - 1);
                return true;
            }
        },
        TitleLoader::save->generation() == Generation::LGPE ? ui_sheet_emulated_button_unavailable_red_idx
                                                            : ui_sheet_emulated_button_unselected_red_idx,
        "", 0.0f, 0));
    buttons.push_back(new Button(282, 212, 34, 28,
        []() {
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, 0));
}

void InjectorScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetBottom);
    Gui::sprite(ui_sheet_emulated_bg_bottom_red, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_bottom_red_idx, -3, 213);
    Gui::sprite(ui_sheet_stripe_wondercard_info_idx, 0, 14);
    Gui::dynamicText(i18n::localize("WC_SWITCH"), 9, 15, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);

    Gui::sprite(ui_sheet_point_big_idx, 15, 54);
    Gui::staticText(i18n::localize("LANGUAGES"), 26, 49, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::sprite(ui_sheet_point_big_idx, 15, 110);
    Gui::staticText(i18n::localize("OVERWRITE_WC"), 26, 105, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::sprite(ui_sheet_point_big_idx, 15, 143);
    Gui::staticText(i18n::localize("ADAPT_LANGUAGE"), 26, 138, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    Gui::sprite(ui_sheet_point_big_idx, 15, 176);
    Gui::staticText(i18n::localize("INJECT_TO_SLOT"), 26, 171, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

    int langIndex = 1;
    for (int y = 46; y < 70; y += 23)
    {
        for (int x = 121; x < 274; x += 38)
        {
            if (langIndex != (int)Language::UNUSED)
            {
                if (isLangAvailable((Language)langIndex))
                {
                    if (langIndex == (int)lang)
                    {
                        Gui::sprite(ui_sheet_emulated_button_selected_red_idx, x, y);
                        Gui::dynamicText(std::string(languages[langIndex - 1]), x + 38 / 2, y + 2, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE,
                            TextPosX::CENTER, TextPosY::TOP);
                    }
                    else
                    {
                        Gui::sprite(ui_sheet_emulated_button_unselected_red_idx, x, y);
                        Gui::dynamicText(std::string(languages[langIndex - 1]), x + 38 / 2, y + 2, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK,
                            TextPosX::CENTER, TextPosY::TOP);
                    }
                }
                else
                {
                    Gui::sprite(ui_sheet_emulated_button_unavailable_red_idx, x, y);
                    Gui::dynamicText(std::string(languages[langIndex - 1]), x + 38 / 2, y + 2, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK,
                        TextPosX::CENTER, TextPosY::TOP);
                }
            }
            langIndex++;
        }
    }

    for (Button* button : buttons)
    {
        button->draw();
    }

    for (int y = 103; y < 138; y += 33)
    {
        bool first = true;
        for (int x = 235; x < 274; x += 38)
        {
            const std::string& word = first ? i18n::localize("YES") : i18n::localize("NO");
            if (overwriteCard)
            {
                Gui::sprite(ui_sheet_emulated_button_selected_red_idx, x, y);
                Gui::dynamicText(word, x + 38 / 2, y + 2, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
            }
            else
            {
                Gui::sprite(ui_sheet_emulated_button_unselected_red_idx, x, y);
                Gui::dynamicText(word, x + 38 / 2, y + 2, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
            }
            first = false;
        }
    }
    if (overwriteCard)
    {
        Gui::sprite(ui_sheet_emulated_button_selected_red_idx, 235, 103);
        Gui::dynamicText(i18n::localize("YES"), 235 + 38 / 2, 106, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        Gui::sprite(ui_sheet_emulated_button_unselected_red_idx, 273, 103);
        Gui::dynamicText(i18n::localize("NO"), 273 + 38 / 2, 106, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    }
    else
    {
        Gui::sprite(ui_sheet_emulated_button_unselected_red_idx, 235, 103);
        Gui::dynamicText(i18n::localize("YES"), 235 + 38 / 2, 106, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        Gui::sprite(ui_sheet_emulated_button_selected_red_idx, 273, 103);
        Gui::dynamicText(i18n::localize("NO"), 273 + 38 / 2, 106, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    }

    if (adaptLanguage)
    {
        Gui::sprite(ui_sheet_emulated_button_selected_red_idx, 235, 136);
        Gui::dynamicText(i18n::localize("YES"), 235 + 38 / 2, 139, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        Gui::sprite(ui_sheet_emulated_button_unselected_red_idx, 273, 136);
        Gui::dynamicText(i18n::localize("NO"), 273 + 38 / 2, 139, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
    }
    else
    {
        Gui::sprite(ui_sheet_emulated_button_unselected_red_idx, 235, 136);
        Gui::dynamicText(i18n::localize("YES"), 235 + 38 / 2, 139, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);
        Gui::sprite(ui_sheet_emulated_button_selected_red_idx, 273, 136);
        Gui::dynamicText(i18n::localize("NO"), 273 + 38 / 2, 139, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    }

    Gui::dynamicText(std::to_string(slot), 255 + 38 / 2, 170, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

    Gui::dynamicText(i18n::localize("START_TO_INJECT"), 160, 221, FONT_SIZE_12, FONT_SIZE_12, COLOR_BLACK, TextPosX::CENTER, TextPosY::TOP);

    if (!choosingSlot)
    {
        C2D_SceneBegin(g_renderTargetTop);
        Gui::sprite(ui_sheet_emulated_bg_top_red, 0, 0);
        Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
        Gui::sprite(ui_sheet_bar_arc_top_red_idx, 0, 0);

        Gui::backgroundAnimatedTop();

        Gui::sprite(ui_sheet_textbox_event_name_idx, 0, 3);
        Gui::dynamicText(wondercard->title(), 25, 7, FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::ball(wondercard->ball(), 4, 6);
        if (wondercard->pokemon())
        {
            if (wondercard->species() == 490 && wondercard->egg())
            {
                Gui::pkm(490, -1, wondercard->generation(), wondercard->gender(), 276, 49, 2.0f, COLOR_GREY_BLEND, 1.0f);
                Gui::pkm(490, -1, wondercard->generation(), wondercard->gender(), 272, 44, 2.0f);
            }
            else
            {
                Gui::pkm(wondercard->species(), wondercard->alternativeForm(), wondercard->generation(), wondercard->gender(), 276, 49, 2.0f,
                    COLOR_GREY_BLEND, 1.0f);
                Gui::pkm(wondercard->species(), wondercard->alternativeForm(), wondercard->generation(), wondercard->gender(), 272, 44, 2.0f);
            }
        }
        else
        {
            Gui::sprite(ui_sheet_icon_item_idx, 301, 68);
        }

        for (int i = 0; i < 4; i++)
        {
            Gui::sprite(ui_sheet_stripe_info_row_idx, 0, 34 + 40 * i);
        }
        for (int i = 0; i < 7; i++)
        {
            Gui::sprite(ui_sheet_point_big_idx, 1, 40 + 20 * i);
        }
        Gui::staticText(i18n::localize("SPECIES"), 9, 35, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("LEVEL"), 9, 55, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("HELD_ITEM"), 9, 75, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("OT"), 9, 95, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("TID_SID"), 9, 115, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("GAME"), 9, 135, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::staticText(i18n::localize("DATE"), 9, 155, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        if (wondercard->pokemon())
        {
            Gui::dynamicText(i18n::species(Configuration::getInstance().language(), wondercard->species()), 87, 35, FONT_SIZE_14, FONT_SIZE_14,
                COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::dynamicText(std::to_string(wondercard->level()), 87, 55, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::dynamicText(i18n::item(Configuration::getInstance().language(), wondercard->heldItem()), 87, 75, FONT_SIZE_14, FONT_SIZE_14,
                COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            std::string text = wondercard->otName();
            u16 tid = wondercard->TID(), sid = wondercard->SID();
            if (text == i18n::localize("YOUR_OT_NAME") || text == "")
            {
                text = TitleLoader::save->otName();
                tid  = TitleLoader::save->TID();
                sid  = TitleLoader::save->SID();
            }
            Gui::dynamicText(text, 87, 95, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::dynamicText(StringUtils::format("%i/%i", tid, sid), 87, 115, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::dynamicText(game, 87, 135, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::dynamicText(StringUtils::format("%i/%i/%i", wondercard->day(), wondercard->month(), wondercard->year()), 87, 155, FONT_SIZE_14,
                FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            if (wondercard->generation() == Generation::SEVEN)
            {
                Gui::sprite(ui_sheet_point_big_idx, 1, 180);
                Gui::staticText(i18n::localize("ITEM"), 9, 175, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
                u16 additionalItem = ((WC7*)wondercard.get())->additionalItem();
                Gui::dynamicText(i18n::item(Configuration::getInstance().language(), additionalItem), 87, 175, FONT_SIZE_14, FONT_SIZE_14,
                    COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            }
        }
        else if (wondercard->item())
        {
            Gui::staticText(i18n::localize("NA"), 87, 35, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::staticText(i18n::localize("NA"), 87, 55, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            const std::string* itemString = &i18n::item(Configuration::getInstance().language(), wondercard->object());
            std::string numString         = "";
            if (wondercard->generation() == Generation::SIX)
            {
                numString = " x " + std::to_string(((WC6*)wondercard.get())->objectQuantity());
            }
            else if (wondercard->generation() == Generation::SEVEN)
            {
                itemString = &i18n::item(Configuration::getInstance().language(), ((WC7*)wondercard.get())->object(item));
                numString  = " x " + std::to_string(((WC7*)wondercard.get())->objectQuantity(item));
            }
            Gui::dynamicText(numString.empty() ? *itemString : *itemString + numString, 87, 75, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK,
                TextPosX::LEFT, TextPosY::TOP);
            Gui::staticText(i18n::localize("NA"), 87, 95, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::staticText(i18n::localize("NA"), 87, 115, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::dynamicText(game, 87, 135, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::staticText(i18n::localize("NA"), 87, 155, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        }
        else if (wondercard->BP())
        {
            Gui::staticText(i18n::localize("NA"), 87, 35, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::staticText(i18n::localize("NA"), 87, 55, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::staticText(i18n::localize("BP"), 87, 75, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::staticText(i18n::localize("NA"), 87, 95, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::staticText(i18n::localize("NA"), 87, 115, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::dynamicText(game, 87, 135, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::staticText(i18n::localize("NA"), 87, 155, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        }
        for (int i = 0; i < 3; i++)
        {
            Gui::sprite(ui_sheet_stripe_stats_move_row_idx, 232, 134 + 40 * i);
        }
        Gui::sprite(ui_sheet_point_big_idx, 237, 140);
        for (int i = 0; i < 4; i++)
        {
            Gui::sprite(ui_sheet_point_small_idx, 238, 161 + 20 * i);
        }
        Gui::staticText(i18n::localize("MOVES"), 251, 136, FONT_SIZE_14, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        for (int i = 0; i < 4; i++)
        {
            Gui::dynamicText(i18n::move(Configuration::getInstance().language(), wondercard->move(i)), 251, 156 + 20 * i, FONT_SIZE_14, FONT_SIZE_14,
                COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        }
    }
    else
    {
        C2D_DrawRectSolid(0, 0, 0.5, 320, 240, COLOR_MASKBLACK);
        Gui::dynamicText(i18n::localize("WC_CHANGE_SLOT"), 160, 100, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
        Gui::dynamicText(i18n::localize("WC_DUMP2"), 160, 128, FONT_SIZE_18, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

        C2D_SceneBegin(g_renderTargetTop);
        Gui::sprite(ui_sheet_part_mtx_5x8_idx, 0, 0);
        auto saveGeneration = TitleLoader::save->generation();
        for (size_t i = 0; i < 40; i++)
        {
            int x        = i % 8;
            int y        = i / 8;
            size_t fullI = i + hid.page() * 40;
            if (fullI >= TitleLoader::save->maxWondercards())
            {
                break;
            }
            if (hid.index() == i)
            {
                C2D_DrawRectSolid(x * 50, y * 48, 0.5f, 49, 47, C2D_Color32(15, 22, 89, 255));
            }
            if (fullI < gifts.size())
            {
                if (gifts[fullI].species > -1)
                {
                    Gui::pkm(gifts[fullI].species, gifts[fullI].form, saveGeneration, gifts[fullI].gender, x * 50 + 7, y * 48 + 2);
                }
                else
                {
                    Gui::sprite(ui_sheet_icon_item_idx, x * 50 + 20, y * 48 + 18);
                }

                Gui::dynamicText(
                    std::to_string(fullI + 1), x * 50 + 50 / 2, y * 48 + 36, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
            }
            else if (fullI == gifts.size())
            {
                Gui::dynamicText(
                    std::to_string(fullI + 1), x * 50 + 50 / 2, y * 48 + 36, FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
            }
            else
            {
                Gui::dynamicText(std::to_string(fullI + 1), x * 50 + 50 / 2, y * 48 + 36, FONT_SIZE_9, FONT_SIZE_9, COLOR_MASKBLACK, TextPosX::CENTER,
                    TextPosY::TOP);
            }
        }
    }
}

void InjectorScreen::update(touchPosition* touch)
{
    Screen::update();
    u32 downKeys = hidKeysDown();
    if (!choosingSlot)
    {
        for (Button* button : buttons)
        {
            if (button->update(touch))
            {
                return;
            }
        }
        if (downKeys & KEY_START)
        {
            int tmpSlot = emptySlot;
            if (overwriteCard)
            {
                tmpSlot = slot - 1;
            }
            TitleLoader::save->mysteryGift(*wondercard, tmpSlot);
            if (adaptLanguage)
            {
                TitleLoader::save->language((u8)lang);
            }
            Gui::screenBack();
            return;
        }
    }
    else
    {
        hid.update(std::min(TitleLoader::save->maxWondercards(), (size_t)emptySlot + 1));
        if (downKeys & KEY_A)
        {
            slot         = hid.fullIndex() + 1;
            choosingSlot = false;
        }
    }
    if (downKeys & KEY_B)
    {
        if (!choosingSlot)
        {
            Gui::screenBack();
            return;
        }
        else
        {
            choosingSlot = false;
        }
    }
    if (wondercard->generation() == Generation::SEVEN && wondercard->item() && ((WC7*)wondercard.get())->items() > 1)
    {
        if (downKeys & KEY_L)
        {
            if (item == 0)
            {
                item = ((WC7*)wondercard.get())->items();
            }
            item--;
        }
        else if (downKeys & KEY_R)
        {
            item++;
            if (item == ((WC7*)wondercard.get())->items())
            {
                item = 0;
            }
        }
    }
}

bool InjectorScreen::isLangAvailable(Language l) const
{
    return ids.find(i18n::langString(l)) != ids.end();
}

void InjectorScreen::changeDate()
{
    time_t current = time(NULL);
    int day        = Configuration::getInstance().day() ? Configuration::getInstance().day() : gmtime(&current)->tm_mday;
    int month      = Configuration::getInstance().month() ? Configuration::getInstance().month() : gmtime(&current)->tm_mon;
    int year       = Configuration::getInstance().year() ? Configuration::getInstance().year() : gmtime(&current)->tm_year;
    wondercard->day(day);
    wondercard->month(month);
    wondercard->year(year);
}
