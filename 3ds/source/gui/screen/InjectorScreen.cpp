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

#include "InjectorScreen.hpp"
#include "Configuration.hpp"
#include "EnablableToggleButton.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "nlohmann/json.hpp"
#include "ToggleButton.hpp"
#include "wcx/WC6.hpp"
#include "wcx/WC7.hpp"
#include <format>

namespace
{
    constexpr std::string_view languages[] = {
        "JPN", "ENG", "FRE", "ITA", "GER", "UNUSED", "SPA", "KOR", "CHS", "CHT"};
}

bool InjectorScreen::setLanguage(pksm::Language language)
{
    if (isLangAvailable(language))
    {
        lang       = language;
        wondercard = MysteryGift::wondercard((*ids)[i18n::langString(lang)]);

        wondercard->date(Configuration::getInstance().date());
    }
    return false;
}

InjectorScreen::InjectorScreen(nlohmann::json myIds)
    : hid(40, 8), ids(std::make_unique<nlohmann::json>(myIds))
{
    size_t currentCards = TitleLoader::save->currentGiftAmount();
    for (size_t i = 0; i < currentCards; i++)
    {
        auto gift = TitleLoader::save->mysteryGift(i);
        if (gift->pokemon())
        {
            gifts.emplace_back(
                gift->title(), "", int(gift->species()), gift->alternativeForm(), gift->gender());
        }
        else
        {
            gifts.emplace_back(gift->title(), "", -1, -1, pksm::Gender::Genderless);
        }
    }
    emptySlot =
        currentCards == TitleLoader::save->maxWondercards() ? currentCards - 1 : currentCards;

    const std::string& langString = i18n::langString(Configuration::getInstance().language());
    if (ids->find(langString) != ids->end())
    {
        wondercard = MysteryGift::wondercard((*ids)[langString]);
        game       = MysteryGift::wondercardInfo((*ids)[langString]).game;
        lang       = Configuration::getInstance().language();
    }
    else
    {
        wondercard = MysteryGift::wondercard(*ids->begin());
        game       = MysteryGift::wondercardInfo(*ids->begin()).game;
        lang       = i18n::langFromString(ids->begin().key());
    }

    slot = emptySlot + 1;

    makeButtons();

    wondercard->date(Configuration::getInstance().date());
}

InjectorScreen::InjectorScreen(std::unique_ptr<pksm::WCX> wcx)
    : wondercard(std::move(wcx)), hid(40, 8)
{
    size_t currentCards = TitleLoader::save->currentGiftAmount();
    for (size_t i = 0; i < currentCards; i++)
    {
        auto gift = TitleLoader::save->mysteryGift(i);
        if (gift->pokemon())
        {
            gifts.emplace_back(
                gift->title(), "", int(gift->species()), gift->alternativeForm(), gift->gender());
        }
        else
        {
            gifts.emplace_back(gift->title(), "", -1, -1, pksm::Gender::Genderless);
        }
    }
    emptySlot =
        currentCards == TitleLoader::save->maxWondercards() ? currentCards - 1 : currentCards;

    lang = pksm::Language::UNUSED;

    slot = emptySlot + 1;

    makeButtons();
}

void InjectorScreen::makeButtons()
{
    int langIndex = 1;
    for (int y = 46; y < 70; y += 23)
    {
        for (int x = 121; x < 274; x += 38)
        {
            pksm::Language langVal = pksm::Language(langIndex);
            if (langVal != pksm::Language::UNUSED)
            {
                langButtons.push_back(std::make_unique<EnablableToggleButton>(
                    x, y, 38, 23, [this, langVal]() { return this->setLanguage(langVal); },
                    [this, langVal]() {
                        return this->lang == pksm::Language::UNUSED ||
                               !this->isLangAvailable(langVal);
                    },
                    ui_sheet_emulated_button_selected_red_idx,
                    std::string(languages[langIndex - 1]), FONT_SIZE_14, COLOR_WHITE,
                    ui_sheet_emulated_button_unselected_red_idx,
                    std::string(languages[langIndex - 1]), FONT_SIZE_14, COLOR_BLACK,
                    ui_sheet_emulated_button_unavailable_red_idx,
                    std::string(languages[langIndex - 1]), FONT_SIZE_14, COLOR_BLACK, &langButtons,
                    false));

                if (langVal == lang)
                {
                    langButtons.back()->setState(true);
                }
                else
                {
                    langButtons.back()->setState(false);
                }
            }
            langIndex++;
        }
    }

    overwriteButtons.push_back(std::make_unique<ToggleButton>(
        235, 102, 38, 23,
        [this]()
        {
            overwriteCard = true;
            return false;
        },
        ui_sheet_emulated_button_selected_red_idx, i18n::localize("YES"), FONT_SIZE_14, COLOR_WHITE,
        ui_sheet_emulated_button_unselected_red_idx, i18n::localize("YES"), FONT_SIZE_14,
        COLOR_BLACK, &overwriteButtons, false));
    overwriteButtons.push_back(std::make_unique<ToggleButton>(
        273, 102, 38, 23,
        [this]()
        {
            overwriteCard = false;
            return false;
        },
        ui_sheet_emulated_button_selected_red_idx, i18n::localize("NO"), FONT_SIZE_14, COLOR_WHITE,
        ui_sheet_emulated_button_unselected_red_idx, i18n::localize("NO"), FONT_SIZE_14,
        COLOR_BLACK, &overwriteButtons, false));

    overwriteButtons.back()->setState(true);

    adaptButtons.push_back(std::make_unique<ToggleButton>(
        235, 135, 38, 23,
        [this]()
        {
            adaptLanguage = true;
            return false;
        },
        ui_sheet_emulated_button_selected_red_idx, i18n::localize("YES"), FONT_SIZE_14, COLOR_WHITE,
        ui_sheet_emulated_button_unselected_red_idx, i18n::localize("YES"), FONT_SIZE_14,
        COLOR_BLACK, &adaptButtons, false));
    adaptButtons.push_back(std::make_unique<ToggleButton>(
        273, 135, 38, 23,
        [this]()
        {
            adaptLanguage = false;
            return false;
        },
        ui_sheet_emulated_button_selected_red_idx, i18n::localize("NO"), FONT_SIZE_14, COLOR_WHITE,
        ui_sheet_emulated_button_unselected_red_idx, i18n::localize("NO"), FONT_SIZE_14,
        COLOR_BLACK, &adaptButtons, false));

    adaptButtons.back()->setState(true);

    miscButtons.push_back(std::make_unique<Button>(
        255, 168, 38, 23,
        [this]()
        {
            if (TitleLoader::save->generation() >= pksm::Generation::LGPE)
            {
                Gui::warn(i18n::localize("WC_LGPE") + '\n' + i18n::localize("NOT_A_BUG"));
                return false;
            }
            else
            {
                choosingSlot = true;
                hid.select(slot - 1);
                return true;
            }
        },
        TitleLoader::save->generation() == pksm::Generation::LGPE
            ? ui_sheet_emulated_button_unavailable_red_idx
            : ui_sheet_emulated_button_unselected_red_idx,
        "", 0.0f, COLOR_BLACK));
    miscButtons.push_back(std::make_unique<Button>(
        282, 212, 34, 28,
        []()
        {
            Gui::screenBack();
            return true;
        },
        ui_sheet_button_back_idx, "", 0.0f, COLOR_BLACK));
}

InjectorScreen::~InjectorScreen() {}

void InjectorScreen::drawBottom() const
{
    Gui::sprite(ui_sheet_emulated_bg_bottom_red, 0, 0);
    Gui::sprite(ui_sheet_bg_style_bottom_idx, 0, 0);
    Gui::sprite(ui_sheet_bar_bottom_red_idx, -3, 213);
    Gui::sprite(ui_sheet_stripe_wondercard_info_idx, 0, 14);
    Gui::text(i18n::localize("WC_SWITCH"), 9, 15, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
        TextPosY::TOP);

    Gui::sprite(ui_sheet_point_big_idx, 15, 54);
    Gui::text(i18n::localize("LANGUAGES"), 26, 49, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT,
        TextPosY::TOP);
    Gui::sprite(ui_sheet_point_big_idx, 15, 110);
    Gui::text(i18n::localize("OVERWRITE_WC"), 26, 105, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT,
        TextPosY::TOP);
    Gui::sprite(ui_sheet_point_big_idx, 15, 143);
    Gui::text(i18n::localize("ADAPT_LANGUAGE"), 26, 138, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT,
        TextPosY::TOP);
    Gui::sprite(ui_sheet_point_big_idx, 15, 176);
    Gui::text(i18n::localize("INJECT_TO_SLOT"), 26, 171, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT,
        TextPosY::TOP);

    for (const auto& langButton : langButtons)
    {
        langButton->draw();
    }

    for (const auto& overwriteButton : overwriteButtons)
    {
        overwriteButton->draw();
    }

    for (const auto& adaptButton : adaptButtons)
    {
        adaptButton->draw();
    }

    for (const auto& button : miscButtons)
    {
        button->draw();
    }

    Gui::text(std::to_string(slot), 255 + 38 / 2, 170, FONT_SIZE_14, COLOR_BLACK, TextPosX::CENTER,
        TextPosY::TOP);

    Gui::text(i18n::localize("START_TO_INJECT"), 160, 221, FONT_SIZE_12, COLOR_BLACK,
        TextPosX::CENTER, TextPosY::TOP);

    if (choosingSlot)
    {
        Gui::drawSolidRect(0, 0, 320, 240, COLOR_MASKBLACK);
        Gui::text(i18n::localize("WC_CHANGE_SLOT"), 160, 100, FONT_SIZE_18, COLOR_WHITE,
            TextPosX::CENTER, TextPosY::TOP);
        Gui::text(i18n::localize("WC_DUMP2"), 160, 128, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::TOP);
    }
}

void InjectorScreen::drawTop() const
{
    if (!choosingSlot)
    {
        Gui::sprite(ui_sheet_emulated_bg_top_red, 0, 0);
        Gui::sprite(ui_sheet_bg_style_top_idx, 0, 0);
        Gui::sprite(ui_sheet_bar_arc_top_red_idx, 0, 0);

        Gui::backgroundAnimatedTop();

        Gui::sprite(ui_sheet_textbox_event_name_idx, 0, 3);
        Gui::text(
            wondercard->title(), 25, 7, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::ball(wondercard->ball(), 4, 6);
        if (wondercard->pokemon())
        {
            if (wondercard->species() == pksm::Species::Manaphy && wondercard->egg())
            {
                Gui::manaphyEgg(272, 44, 1.5f, COLOR_GREY_BLEND, 1.5f);
                Gui::manaphyEgg(268, 40, 1.5f);

                Gui::pkm(pksm::Species::Manaphy, -1, wondercard->generation(), wondercard->gender(),
                    276, 49, 2.0f, COLOR_GREY_BLEND, 1.0f);
                Gui::pkm(pksm::Species::Manaphy, -1, wondercard->generation(), wondercard->gender(),
                    272, 44, 2.0f);
            }
            else
            {
                if (wondercard->egg())
                {
                    Gui::egg(272, 44, 1.5f, COLOR_GREY_BLEND, 1.5f);
                    Gui::egg(268, 40, 1.5f);
                }

                if (wondercard->shiny())
                {
                    Gui::shinyStar(272, 44, 1.5f, COLOR_GREY_BLEND, 1.5f);
                    Gui::shinyStar(268, 40, 1.5f);
                }

                Gui::pkm(wondercard->species(), wondercard->alternativeForm(),
                    wondercard->generation(), wondercard->gender(), 276, 49, 2.0f, COLOR_GREY_BLEND,
                    1.0f);
                Gui::pkm(wondercard->species(), wondercard->alternativeForm(),
                    wondercard->generation(), wondercard->gender(), 272, 44, 2.0f);
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
        Gui::text(i18n::localize("SPECIES"), 9, 35, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        Gui::text(i18n::localize("LEVEL"), 9, 55, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        Gui::text(i18n::localize("HELD_ITEM"), 9, 75, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        Gui::text(
            i18n::localize("OT"), 9, 95, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(i18n::localize("TID_SID"), 9, 115, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        Gui::text(i18n::localize("GAME"), 9, 135, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        Gui::text(i18n::localize("DATE"), 9, 155, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        if (wondercard->pokemon())
        {
            Gui::text(wondercard->species().localize(Configuration::getInstance().language()), 87,
                35, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::text(std::to_string(wondercard->level()), 87, 55, FONT_SIZE_14, COLOR_BLACK,
                TextPosX::LEFT, TextPosY::TOP);
            Gui::text(i18n::item(Configuration::getInstance().language(), wondercard->heldItem()),
                87, 75, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            std::string text = wondercard->otName();
            u32 tid = wondercard->formatTID(), sid = wondercard->formatSID();
            if (text.empty())
            {
                text = TitleLoader::save->otName();
                tid  = TitleLoader::save->displayTID();
                sid  = TitleLoader::save->displaySID();
            }
            Gui::text(text, 87, 95, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::text(std::format("{:d}/{:d}", tid, sid), 87, 115, FONT_SIZE_14, COLOR_BLACK,
                TextPosX::LEFT, TextPosY::TOP);
            Gui::text(game, 87, 135, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Date date = wondercard->date();
            Gui::text(std::format("{:d}/{:d}/{:d}", date.day(), date.month(), date.year()), 87, 155,
                FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            if (wondercard->generation() == pksm::Generation::SEVEN)
            {
                Gui::sprite(ui_sheet_point_big_idx, 1, 180);
                Gui::text(i18n::localize("ITEM"), 9, 175, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                    TextPosY::TOP);
                u16 additionalItem = ((pksm::WC7*)wondercard.get())->additionalItem();
                Gui::text(i18n::item(Configuration::getInstance().language(), additionalItem), 87,
                    175, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            }
        }
        else if (wondercard->item())
        {
            Gui::text(i18n::localize("NA"), 87, 35, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            Gui::text(i18n::localize("NA"), 87, 55, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            const std::string* itemString =
                &i18n::item(Configuration::getInstance().language(), wondercard->object());
            std::string numString = "";
            if (wondercard->generation() == pksm::Generation::SIX)
            {
                numString =
                    " x " + std::to_string(((pksm::WC6*)wondercard.get())->objectQuantity());
            }
            else if (wondercard->generation() == pksm::Generation::SEVEN)
            {
                itemString = &i18n::item(Configuration::getInstance().language(),
                    ((pksm::WC7*)wondercard.get())->object(item));
                numString =
                    " x " + std::to_string(((pksm::WC7*)wondercard.get())->objectQuantity(item));
            }
            Gui::text(numString.empty() ? *itemString : *itemString + numString, 87, 75,
                FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::text(i18n::localize("NA"), 87, 95, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            Gui::text(i18n::localize("NA"), 87, 115, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            Gui::text(game, 87, 135, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::text(i18n::localize("NA"), 87, 155, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
        }
        else if (wondercard->BP())
        {
            Gui::text(i18n::localize("NA"), 87, 35, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            Gui::text(i18n::localize("NA"), 87, 55, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            Gui::text(i18n::localize("BP"), 87, 75, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            Gui::text(i18n::localize("NA"), 87, 95, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            Gui::text(i18n::localize("NA"), 87, 115, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
            Gui::text(game, 87, 135, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
            Gui::text(i18n::localize("NA"), 87, 155, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
                TextPosY::TOP);
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
        Gui::text(i18n::localize("MOVES"), 251, 136, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT,
            TextPosY::TOP);
        for (int i = 0; i < 4; i++)
        {
            Gui::text(i18n::move(Configuration::getInstance().language(), wondercard->move(i)), 251,
                156 + 20 * i, FONT_SIZE_14, COLOR_BLACK, TextPosX::LEFT, TextPosY::TOP);
        }
    }
    else
    {
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
                Gui::drawSolidRect(x * 50, y * 48, 49, 47, PKSM_Color(15, 22, 89, 255));
            }
            if (fullI < gifts.size())
            {
                if (gifts[fullI].species > -1)
                {
                    Gui::pkm(pksm::Species{u16(gifts[fullI].species)}, gifts[fullI].form,
                        saveGeneration, gifts[fullI].gender, x * 50 + 7, y * 48 + 2);
                }
                else
                {
                    Gui::sprite(ui_sheet_icon_item_idx, x * 50 + 20, y * 48 + 18);
                }

                Gui::text(std::to_string(fullI + 1), x * 50 + 50 / 2, y * 48 + 36, FONT_SIZE_9,
                    COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
            }
            else if (fullI == gifts.size())
            {
                Gui::text(std::to_string(fullI + 1), x * 50 + 50 / 2, y * 48 + 36, FONT_SIZE_9,
                    COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
            }
            else
            {
                Gui::text(std::to_string(fullI + 1), x * 50 + 50 / 2, y * 48 + 36, FONT_SIZE_9,
                    COLOR_MASKBLACK, TextPosX::CENTER, TextPosY::TOP);
            }
        }
    }
}

void InjectorScreen::update(touchPosition* touch)
{
    u32 downKeys = hidKeysDown();
    if (!choosingSlot)
    {
        for (auto& button : miscButtons)
        {
            if (button->update(touch))
            {
                return;
            }
        }
        for (auto& button : langButtons)
        {
            if (button->update(touch))
            {
                return;
            }
        }
        for (auto& button : adaptButtons)
        {
            if (button->update(touch))
            {
                return;
            }
        }
        for (auto& button : overwriteButtons)
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
            if (adaptLanguage && getSafeLanguage(TitleLoader::save->generation(), lang) == lang)
            {
                TitleLoader::save->language(getSafeLanguage(TitleLoader::save->generation(), lang));
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
    if (wondercard->generation() == pksm::Generation::SEVEN && wondercard->item() &&
        ((pksm::WC7*)wondercard.get())->items() > 1)
    {
        if (downKeys & KEY_L)
        {
            if (item == 0)
            {
                item = ((pksm::WC7*)wondercard.get())->items();
            }
            item--;
        }
        else if (downKeys & KEY_R)
        {
            item++;
            if (item == ((pksm::WC7*)wondercard.get())->items())
            {
                item = 0;
            }
        }
    }
}

bool InjectorScreen::isLangAvailable(pksm::Language l) const
{
    return ids && ids->find(i18n::langString(l)) != ids->end();
}
