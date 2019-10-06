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

#include "TitleLoadScreen.hpp"

bool TitleLoadScreen::loadSave() const
{
    bool status = false;
    if (selectedSave + firstSave == -1)
    {
        status = TitleLoader::load(titleFromIndex(selectedTitle));
    }
    else
    {
        status = TitleLoader::load(titleFromIndex(selectedTitle), availableCheckpointSaves[selectedSave + firstSave]);
    }
    if (status)
    {
        Gui::setScreen(std::make_unique<MainMenu>());
    }

    return status;
}

TitleLoadScreen::TitleLoadScreen()
    : Screen(
          i18n::localize("A_SELECT") + '\n' + i18n::localize("X_SETTINGS") + '\n' + i18n::localize("Y_ABSENT") + '\n' + i18n::localize("START_EXIT"))
{
    oldLang = Configuration::getInstance().language();
    buttons.push_back(std::make_unique<Button>(200, 147, 96, 51, &receiveSaveFromBridge, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
    buttons.push_back(std::make_unique<AccelButton>(
        24, 96, 175, 16, [this]() { return this->setSelectedSave(0); }, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK, 10, 10));
    for (int i = 1; i < 5; i++)
    {
        buttons.push_back(std::make_unique<ClickButton>(
            24, 96 + 17 * i, 175, 16, [this, i]() { return this->setSelectedSave(i); }, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
    }
    buttons.push_back(std::make_unique<AccelButton>(
        24, 181, 175, 16, [this]() { return this->setSelectedSave(5); }, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK, 10, 10));
    buttons.push_back(std::make_unique<Button>(200, 95, 96, 51, [this]() { return this->loadSave(); }, ui_sheet_res_null_idx, "", 0.0f, COLOR_BLACK));
}

void TitleLoadScreen::makeInstructions()
{
    instructions = Instructions(
        i18n::localize("A_SELECT") + '\n' + i18n::localize("X_SETTINGS") + '\n' + i18n::localize("Y_ABSENT") + '\n' + i18n::localize("START_EXIT"));
}

void TitleLoadScreen::drawTop() const
{
    Gui::drawSolidRect(0, 0, 400.0f, 240.0f, PKSM_Color(15, 22, 89, 255));

    Gui::sprite(ui_sheet_emulated_gameselector_bg_idx, 4, 29);
    Gui::sprite(ui_sheet_gameselector_cart_idx, 35, 93);

    if (auto title = TitleLoader::cardTitle)
    {
        if (title->icon().subtex->width == 48)
        {
            Gui::drawImageAt(title->icon(), 40, 98, NULL, 1.0f, 1.0f);
        }
        else
        {
            Gui::drawImageAt(title->icon(), 48, 106, NULL, 1.0f, 1.0f);
        }
        if (titleFromIndex(selectedTitle) == title)
        {
            Gui::drawSelector(39, 97);
        }
    }

    for (size_t i = 0; i < TitleLoader::nandTitles.size(); i++)
    {
        int y = TitleLoader::nandTitles.size() > 4 ? (i / 4) * 60 + 68 : 98;
        int x = 150 + (4 - (TitleLoader::nandTitles.size() % 4 == 0 ? 4 : TitleLoader::nandTitles.size() % 4)) * 30 + (i > 3 ? i - 4 : i) * 60;

        if (TitleLoader::nandTitles.size() > 4 && i < 4)
        {
            x = 150 + (i > 3 ? i - 4 : i) * 60;
        }

        Gui::drawImageAt(TitleLoader::nandTitles[i]->icon(), x, y, NULL, 1.0f, 1.0f);
        if (titleFromIndex(selectedTitle) == TitleLoader::nandTitles[i])
        {
            Gui::drawSelector(x - 1, y - 1);
        }
    }

    Gui::text(i18n::localize("LOADER_INSTRUCTIONS_TOP_ABSENT"), 200, 8, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP,
        TextWidthAction::SQUISH, 398);
    Gui::text(i18n::localize("LOADER_GAME_CARD"), 4 + 120 / 2, 197, FONT_SIZE_14, PKSM_Color(15, 22, 89, 255), TextPosX::CENTER, TextPosY::TOP);
    Gui::text(
        i18n::localize("LOADER_INSTALLED_GAMES"), 128 + 268 / 2, 197, FONT_SIZE_14, PKSM_Color(15, 22, 89, 255), TextPosX::CENTER, TextPosY::TOP);
}

void TitleLoadScreen::drawBottom() const
{
    Gui::backgroundBottom(true);
    Gui::sprite(ui_sheet_gameselector_savebox_idx, 22, 94);

    auto text      = Gui::parseText(i18n::localize("LOADER_ID"), FONT_SIZE_11, 0.0f);
    int nextIdPart = 27 + text->maxWidth(FONT_SIZE_11);
    Gui::text(text, 27, 46, FONT_SIZE_11, FONT_SIZE_11, COLOR_LIGHTBLUE, TextPosX::LEFT, TextPosY::TOP);

    text              = Gui::parseText(i18n::localize("LOADER_MEDIA_TYPE"), FONT_SIZE_11, 0.0f);
    int nextMediaPart = 27 + text->maxWidth(FONT_SIZE_11);
    Gui::text(text, 27, 58, FONT_SIZE_11, FONT_SIZE_11, COLOR_LIGHTBLUE, TextPosX::LEFT, TextPosY::TOP);

    if (auto title = titleFromIndex(selectedTitle))
    {
        Gui::drawSolidRect(243, 21, 52, 52, PKSM_Color(15, 22, 89, 255));
        if (title->icon().subtex->width == 48)
        {
            Gui::drawImageAt(title->icon(), 245, 23, NULL, 1.0f, 1.0f);
        }
        else
        {
            Gui::drawImageAt(title->icon(), 253, 31, NULL, 1.0f, 1.0f);
        }
        Gui::text(title->name(), 27, 26, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        Gui::text(StringUtils::format("%08X", title->lowId()), nextIdPart, 46, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

        Gui::text(selectedTitle == -1 ? i18n::localize("LOADER_CARTRIDGE") : i18n::localize("LOADER_SD"), nextMediaPart, 58, FONT_SIZE_11,
            COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }
    else
    {
        Gui::text(i18n::localize("NONE"), 27, 26, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }

    if (selectedSave > -1)
    {
        Gui::drawSolidRect(24, 96 + 17 * selectedSave, 174, 16, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    int y = 97;
    for (int i = firstSave; i < firstSave + 6; i++)
    {
        if (i == -1)
        {
            if (i - firstSave == selectedSave)
            {
                Gui::text(i18n::localize("LOADER_GAME_SAVE"), 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
                    TextWidthAction::SCROLL, 169.0f);
            }
            else
            {
                Gui::text(i18n::localize("LOADER_GAME_SAVE"), 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SLICE,
                    169.0f);
            }
        }
        else if (i < (int)availableCheckpointSaves.size())
        {
            std::string save = availableCheckpointSaves[i].substr(0, availableCheckpointSaves[i].find_last_of('/'));
            save             = save.substr(save.find_last_of('/') + 1);
            if (i - firstSave == selectedSave)
            {
                Gui::text(save, 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SCROLL, 169.0f);
            }
            else
            {
                Gui::text(save, 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SLICE, 169.0f);
            }
        }
        else
        {
            break;
        }
        y += 17;
    }

    if (selectedSave > 0 && firstSave > -1)
    {
        Gui::drawSolidRect(191, 102, 4, 5, PKSM_Color(0x0f, 0x16, 0x59, 255));
        Gui::drawSolidTriangle(189, 102, 197, 102, 193, 97, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    if (selectedSave < 5 && (size_t)firstSave + 5 < availableCheckpointSaves.size() - 1)
    {
        Gui::drawSolidRect(191, 186, 4, 5, PKSM_Color(0x0f, 0x16, 0x59, 255));
        Gui::drawSolidTriangle(189, 191, 197, 191, 193, 196, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    Gui::text(
        i18n::localize("LOADER_LOAD"), 200 + 96 / 2, 120, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER, TextWidthAction::WRAP, 94);
    Gui::text(i18n::localize("LOADER_WIRELESS"), 200 + 96 / 2, 172, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER,
        TextWidthAction::WRAP, 94);

    Gui::text(i18n::localize("LOADER_INSTRUCTIONS_BOTTOM"), 160, 223, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP,
        TextWidthAction::SQUISH, 318);
}

void TitleLoadScreen::update(touchPosition* touch)
{
    if (oldLang != Configuration::getInstance().language())
    {
        oldLang = Configuration::getInstance().language();
        makeInstructions();
    }
    u32 buttonsDown = hidKeysDown();
    if (TitleLoader::cardWasUpdated())
    {
        selectedGame  = false;
        selectedSave  = -1;
        firstSave     = -1;
        selectedTitle = -2;
    }
    if (selectedTitle == -2)
    {
        if (TitleLoader::cardTitle == nullptr && !TitleLoader::nandTitles.empty())
        {
            selectedTitle = 0;
        }
        else if (TitleLoader::cardTitle != nullptr)
        {
            selectedTitle = -1;
        }
        else
        {
            if (buttonsDown & KEY_Y)
            {
                Gui::screenBack();
                Gui::setScreen(std::make_unique<SaveLoadScreen>());
            }
            else if (buttonsDown & KEY_X)
            {
                Gui::setScreen(std::make_unique<ConfigScreen>());
            }
            buttons[0]->update(touch);
            return;
        }
    }

    if (selectedGame)
    {
        if (buttonsDown & KEY_B)
        {
            selectedGame = false;
            selectedSave = -1;
            firstSave    = -1;
        }
        if (buttonsDown & KEY_A)
        {
            loadSave();
            return;
        }
        if (buttonsDown & KEY_DOWN)
        {
            if (selectedSave == 4)
            {
                if (firstSave + 5 < (int)availableCheckpointSaves.size() - 1)
                {
                    firstSave++;
                }
                else if (firstSave + selectedSave < (int)availableCheckpointSaves.size() - 1)
                {
                    selectedSave++;
                }
            }
            else
            {
                if (firstSave + selectedSave < (int)availableCheckpointSaves.size() - 1)
                {
                    selectedSave++;
                }
            }
        }
        if (buttonsDown & KEY_UP)
        {
            if (selectedSave == 1)
            {
                if (firstSave > -1)
                {
                    firstSave--;
                }
                else
                {
                    selectedSave--;
                }
            }
            else if (selectedSave != 0)
            {
                selectedSave--;
            }
        }
        for (auto& button : buttons)
        {
            button->update(touch);
        }
    }
    else
    {
        if (buttonsDown & KEY_DOWN)
        {
            if (titleFromIndex(selectedTitle) == TitleLoader::cardTitle)
            {
                if (TitleLoader::nandTitles.size() > 4)
                {
                    selectedTitle = 4;
                }
                else if (!TitleLoader::nandTitles.empty())
                {
                    selectedTitle = 0;
                }
            }
            else
            {
                if (TitleLoader::nandTitles.size() > 4)
                {
                    if (selectedTitle < 4)
                    {
                        selectedTitle =
                            selectedTitle + 4 > (int)TitleLoader::nandTitles.size() - 1 ? TitleLoader::nandTitles.size() - 1 : selectedTitle + 4;
                    }
                    else
                    {
                        selectedTitle -= 4;
                    }
                }
                else if (TitleLoader::nandTitles.size() < 5)
                {
                    if (selectedTitle < (int)TitleLoader::nandTitles.size() - 2)
                    {
                        selectedTitle++;
                    }
                    else
                    {
                        if (TitleLoader::cardTitle)
                        {
                            selectedTitle = -1;
                        }
                        else
                        {
                            selectedTitle = 0;
                        }
                    }
                }
            }
        }
        else if (buttonsDown & KEY_UP)
        {
            if (titleFromIndex(selectedTitle) == TitleLoader::cardTitle)
            {
                if (TitleLoader::nandTitles.size() > 4)
                {
                    selectedTitle = 0;
                }
                else
                {
                    selectedTitle = TitleLoader::nandTitles.size() - 1;
                }
            }
            else
            {
                if (TitleLoader::nandTitles.size() > 4)
                {
                    if (selectedTitle > 3)
                    {
                        selectedTitle = selectedTitle - 4;
                    }
                    else
                    {
                        selectedTitle =
                            selectedTitle + 4 > (int)TitleLoader::nandTitles.size() - 1 ? TitleLoader::nandTitles.size() - 1 : selectedTitle + 4;
                    }
                }
                else if (TitleLoader::nandTitles.size() < 5 && selectedTitle > -2)
                {
                    selectedTitle--;
                    if (selectedTitle == -1 && !TitleLoader::cardTitle)
                    {
                        selectedTitle = TitleLoader::nandTitles.size() - 1;
                    }
                }
            }
        }
        else if (buttonsDown & KEY_RIGHT)
        {
            if (selectedTitle == (int)TitleLoader::nandTitles.size() - 1 || selectedTitle == 3)
            {
                if (TitleLoader::cardTitle)
                {
                    selectedTitle = -1;
                }
                else
                {
                    if (TitleLoader::nandTitles.size() > 4 && selectedTitle > 3)
                    {
                        if (selectedTitle > 3)
                        {
                            selectedTitle = 4;
                        }
                        else if (selectedTitle == 3)
                        {
                            selectedTitle = 0;
                        }
                    }
                    else
                    {
                        selectedTitle = 0;
                    }
                }
            }
            else
            {
                selectedTitle++;
            }
        }
        else if (buttonsDown & KEY_LEFT)
        {
            if (selectedTitle == -1)
            {
                selectedTitle = TitleLoader::nandTitles.size() < 4 ? TitleLoader::nandTitles.size() - 1 : 3;
            }
            else if (selectedTitle == 4)
            {
                if (TitleLoader::cardTitle)
                {
                    selectedTitle = -1;
                }
                else
                {
                    selectedTitle = (int)TitleLoader::nandTitles.size() - 1;
                }
            }
            else if (selectedTitle == 0)
            {
                if (TitleLoader::cardTitle)
                {
                    selectedTitle = -1;
                }
                else
                {
                    selectedTitle = TitleLoader::nandTitles.size() > 4 ? 3 : (int)TitleLoader::nandTitles.size() - 1;
                }
            }
            else
            {
                selectedTitle--;
            }
        }
        else if (buttonsDown & KEY_Y)
        {
            Gui::screenBack();
            Gui::setScreen(std::make_unique<SaveLoadScreen>());
            return;
        }
        if (buttonsDown & KEY_A)
        {
            selectedGame = true;
            selectedSave = 0;
        }
        if (buttons[0]->update(touch))
        {
            return;
        }
    }
    if (auto title = titleFromIndex(selectedTitle))
    {
        availableCheckpointSaves = TitleLoader::sdSaves[title->checkpointPrefix()];
    }
    else
    {
        availableCheckpointSaves = {};
    }

    if (buttonsDown & KEY_X)
    {
        Gui::setScreen(std::make_unique<ConfigScreen>());
    }
}

bool TitleLoadScreen::setSelectedSave(int i)
{
    if (i == 5)
    {
        if (firstSave + 5 < (int)availableCheckpointSaves.size() - 1)
        {
            firstSave++;
            selectedSave = 4;
        }
        else if (firstSave + 5 < (int)availableCheckpointSaves.size())
        {
            selectedSave = 5;
        }
    }
    else if (i == 0 && firstSave != -1)
    {
        firstSave--;
        selectedSave = 1;
    }
    else if (firstSave + i < (int)availableCheckpointSaves.size())
    {
        selectedSave = i;
    }
    return false;
}
