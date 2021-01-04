/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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
#include "Button.hpp"
#include "Configuration.hpp"
#include "InjectorScreen.hpp"
#include "QRScanner.hpp"
#include "ToggleButton.hpp"
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "mysterygift.hpp"
#include "nlohmann/json.hpp"
#include "wcx/PGF.hpp"
#include "wcx/PGT.hpp"
#include "wcx/WC6.hpp"
#include "wcx/WC7.hpp"
#include <sys/stat.h>

namespace
{
    constexpr std::string_view langs[] = {
        "JPN", "ENG", "FRE", "ITA", "GER", "SPA", "KOR", "CHS", "CHT"};
}

InjectSelectorScreen::InjectSelectorScreen()
    : Screen(i18n::localize("A_SELECT") + '\n' + i18n::localize("L_PAGE_PREV") + '\n' +
             i18n::localize("R_PAGE_NEXT") + '\n' + i18n::localize("B_BACK")),
      hid(10, 2),
      dumpHid(40, 8)
{
    MysteryGift::init(TitleLoader::save->generation());
    wondercards = MysteryGift::wondercards();

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

    // QR
    instructions.addCircle(false, 160, 195, 11, COLOR_GREY);
    instructions.addLine(false, 160, 177, 160, 206, 4, COLOR_GREY);
    instructions.addBox(
        false, 160 - 100 / 2, 177 - 23, 100, 23, COLOR_GREY, i18n::localize("QR_SCANNER"));
    buttons.push_back(std::make_unique<Button>(
        160 - 70 / 2, 207 - 23, 70, 23, [this]() { return this->doQR(); },
        ui_sheet_emulated_button_qr_idx, "", FONT_SIZE_14, COLOR_WHITE));
    // Filter
    for (int i = 0; i < 9; i++)
    {
        langFilters.push_back(std::make_unique<ToggleButton>(
            268, 3 + i * 24, 38, 23,
            [this, i]() {
                hid.select(0);
                return this->toggleFilter(std::string(langs[i]));
            },
            ui_sheet_emulated_button_selected_blue_idx, std::string(langs[i]), FONT_SIZE_14,
            COLOR_WHITE, ui_sheet_emulated_button_unselected_blue_idx, std::nullopt, std::nullopt,
            COLOR_BLACK, &langFilters, true));
        langFilters.back()->setState(false);
    }
}

InjectSelectorScreen::~InjectSelectorScreen()
{
    MysteryGift::exit();
}

void InjectSelectorScreen::update(touchPosition* touch)
{
    u32 downKeys = hidKeysDown();
    u32 heldKeys = hidKeysHeld();
    if (updateGifts)
    {
        size_t currentCards = TitleLoader::save->currentGiftAmount();
        for (size_t i = 0; i < currentCards; i++)
        {
            auto gift = TitleLoader::save->mysteryGift(i);
            if (gift->pokemon())
            {
                gifts.emplace_back(gift->title(), "", int(gift->species()), gift->alternativeForm(),
                    gift->gender());
            }
            else
            {
                gifts.emplace_back(gift->title(), "", -1, -1, pksm::Gender::Genderless);
            }
        }
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
            bool allReleased = true;
            for (const auto& card : wondercards[hid.fullIndex()])
            {
                MysteryGift::giftData info = MysteryGift::wondercardInfo(card.get<int>());
                if (!info.released)
                {
                    allReleased = false;
                    break;
                }
            }
            if (allReleased ||
                Gui::showChoiceMessage(
                    "Not all of these wonder card(s) are released.\nContinue to injection screen?"))
            {
                Gui::setScreen(std::make_unique<InjectorScreen>(wondercards[hid.fullIndex()]));
                updateGifts = true;
                return;
            }
        }
        if (downKeys & KEY_X)
        {
            if (TitleLoader::save->generation() == pksm::Generation::LGPE)
            {
                Gui::warn(i18n::localize("WC_LGPE") + '\n' + i18n::localize("NOT_A_BUG"));
            }
            else
            {
                dump = true;
            }
            return;
        }

        for (auto& button : buttons)
        {
            if (button->update(touch))
            {
                return;
            }
        }

        for (auto& button : langFilters)
        {
            if (button->update(touch))
            {
                return;
            }
        }

        for (auto& button : typeFilters)
        {
            if (button->update(touch))
            {
                return;
            }
        }
    }
    else
    {
        dumpHid.update(std::max(TitleLoader::save->currentGiftAmount(), 1));
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

void InjectSelectorScreen::drawBottom() const
{
    Gui::backgroundBottom(true);
    Gui::text(i18n::localize("WC_INST1"), 160, 222, FONT_SIZE_11, PKSM_Color(197, 202, 233, 255),
        TextPosX::CENTER, TextPosY::TOP);

    Gui::sprite(ui_sheet_eventmenu_page_indicator_idx, 65, 13);

    Gui::text("\uE004", 75, 17, FONT_SIZE_18, PKSM_Color(197, 202, 233, 255), TextPosX::LEFT,
        TextPosY::TOP);
    Gui::text("\uE005", 228, 17, FONT_SIZE_18, PKSM_Color(197, 202, 233, 255), TextPosX::LEFT,
        TextPosY::TOP);
    Gui::text(
        fmt::format(FMT_STRING("{:d}/{:d}"), hid.page() + 1,
            wondercards.size() % 10 == 0 ? wondercards.size() / 10 : wondercards.size() / 10 + 1),
        160, 20, FONT_SIZE_12, PKSM_Color(197, 202, 233, 255), TextPosX::CENTER, TextPosY::TOP);

    for (const auto& button : buttons)
    {
        button->draw();
    }

    for (const auto& button : langFilters)
    {
        button->draw();
    }

    for (const auto& button : typeFilters)
    {
        button->draw();
    }

    Gui::text("\uE004+\uE005 \uE01E", 160, 207 - 21, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP);

    if (dump)
    {
        Gui::drawSolidRect(0, 0, 320, 240, COLOR_MASKBLACK);
        Gui::text(i18n::localize("WC_DUMP1"), 160, 107, FONT_SIZE_18, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::TOP);
    }
}

void InjectSelectorScreen::drawTop() const
{
    if (!dump)
    {
        Gui::backgroundTop(true);

        Gui::text(i18n::localize("EVENT_DATABASE"), 200, 4, FONT_SIZE_14,
            PKSM_Color(140, 158, 255, 255), TextPosX::CENTER, TextPosY::TOP);

        for (size_t i = 0; i < 10; i++)
        {
            if (i % 2 == 0)
            {
                int x = 20;
                int y = 41 + (i / 2) * 37;
                if (i == 0)
                {
                    Gui::sprite(i == hid.index() ? ui_sheet_eventmenu_bar_selected_idx
                                                 : ui_sheet_eventmenu_bar_unselected_idx,
                        x, y);
                }
                else if (i == 8)
                {
                    Gui::sprite(
                        i == hid.index()
                            ? ui_sheet_emulated_eventmenu_bar_selected_flipped_vertical_idx
                            : ui_sheet_emulated_eventmenu_bar_unselected_flipped_vertical_idx,
                        x, y);
                }
                else
                {
                    Gui::drawSolidRect(x, y, 178, 34,
                        i == hid.index() ? PKSM_Color(0x3D, 0x5A, 0xFE, 0xFF)
                                         : PKSM_Color(0x8C, 0x9E, 0xFF, 0xFF));
                }
            }
            else
            {
                int x = 201;
                int y = 41 + (i / 2) * 37;
                if (i == 1)
                {
                    Gui::sprite(
                        i == hid.index()
                            ? ui_sheet_emulated_eventmenu_bar_selected_flipped_horizontal_idx
                            : ui_sheet_emulated_eventmenu_bar_unselected_flipped_horizontal_idx,
                        x, y);
                }
                else if (i == 9)
                {
                    Gui::sprite(i == hid.index()
                                    ? ui_sheet_emulated_eventmenu_bar_selected_flipped_both_idx
                                    : ui_sheet_emulated_eventmenu_bar_unselected_flipped_both_idx,
                        x, y);
                }
                else
                {
                    Gui::drawSolidRect(x, y, 178, 34,
                        i == hid.index() ? PKSM_Color(0x3D, 0x5A, 0xFE, 0xFF)
                                         : PKSM_Color(0x8C, 0x9E, 0xFF, 0xFF));
                }
            }
        }

        for (size_t i = hid.page() * 10; i < (size_t)hid.page() * 10 + 10; i++)
        {
            if (i >= wondercards.size())
            {
                break;
            }
            else
            {
                MysteryGift::giftData data;
                const std::string& lang = i18n::langString(Configuration::getInstance().language());
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
                    Gui::pkm(pksm::Species{u16(data.species)}, data.form,
                        TitleLoader::save->generation(), data.gender, x, y);
                }
                PKSM_Color color = i == hid.fullIndex() ? PKSM_Color(232, 234, 246, 255)
                                                        : PKSM_Color(26, 35, 126, 255);
                TextWidthAction action = i == hid.fullIndex() ? TextWidthAction::SQUISH_OR_SCROLL
                                                              : TextWidthAction::SQUISH_OR_SLICE;
                Gui::text(data.name, x + 103, y + 14, FONT_SIZE_11, color, TextPosX::CENTER,
                    TextPosY::CENTER, action, 138.0f);
            }
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
            size_t fullI = i + dumpHid.page() * 40;
            if (fullI >= TitleLoader::save->maxWondercards())
            {
                break;
            }
            if (dumpHid.index() == i)
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

                Gui::text(std::to_string(fullI + 1), x * 50 + 25, y * 48 + 36, FONT_SIZE_9,
                    COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
            }
            else
            {
                Gui::text(std::to_string(fullI + 1), x * 50 + 25, y * 48 + 36, FONT_SIZE_9,
                    COLOR_MASKBLACK, TextPosX::CENTER, TextPosY::TOP);
            }
        }
    }
}

bool InjectSelectorScreen::doQR()
{
    std::unique_ptr<pksm::WCX> wcx;
    switch (TitleLoader::save->generation())
    {
        case pksm::Generation::FOUR:
            wcx = QRScanner<pksm::WC4>::scan();
            break;
        case pksm::Generation::FIVE:
            wcx = QRScanner<pksm::PGF>::scan();
            break;
        case pksm::Generation::SIX:
            wcx = QRScanner<pksm::WC6>::scan();
            break;
        case pksm::Generation::SEVEN:
            wcx = QRScanner<pksm::WC7>::scan();
            break;
        case pksm::Generation::EIGHT:
            wcx = QRScanner<pksm::WC8>::scan();
            break;
        default:
            return false;
    }

    if (wcx)
    {
        Gui::setScreen(std::make_unique<InjectorScreen>(std::move(wcx)));
        updateGifts = true;
        return true;
    }
    return false;
}

void InjectSelectorScreen::dumpCard(void) const
{
    auto wc          = TitleLoader::save->mysteryGift(dumpHid.fullIndex());
    DateTime now     = DateTime::now();
    std::string path = fmt::format(
        FMT_STRING("/3ds/PKSM/dumps/{0:d}-{1:d}-{2:d}"), now.year(), now.month(), now.day());
    mkdir(path.c_str(), 777);
    path += fmt::format(FMT_STRING("/{0:d}-{1:d}-{2:d} - {3:d} - {4:s}{5:s}"), now.hour(),
        now.minute(), now.second(), wc->ID(), wc->title(), wc->extension().data());
    FILE* out = fopen(path.c_str(), "wb");
    if (out)
    {
        fwrite(wc->rawData(), 1, wc->size(), out);
        fclose(out);
    }
    else
    {
        Gui::error(i18n::localize("FAILED_OPEN_DUMP"), errno);
    }
}

bool InjectSelectorScreen::toggleFilter(const std::string& lang)
{
    if (langFilter != lang)
    {
        wondercards = MysteryGift::wondercards();
        for (size_t i = wondercards.size(); i > 0; i--)
        {
            if (!wondercards[i - 1].contains(lang))
            {
                wondercards.erase(wondercards.begin() + i - 1);
            }
        }
        langFilter = lang;
    }
    else
    {
        wondercards = MysteryGift::wondercards();
        langFilter  = "";
    }
    return false;
}

bool InjectSelectorScreen::toggleFilter(u8 type)
{
    // Stubbed for now
    return false;
}
