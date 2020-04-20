/*
 *   This file is part of PKExtraSavesSubScreen::Group::SM
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

#include "ExtraSavesSubScreen.hpp"
#include "Configuration.hpp"
#include "FileChooseOverlay.hpp"
#include "GameVersion.hpp"
#include "Species.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"

namespace
{
    constexpr std::string_view dsIds[] = {
        "CPU", // Platinum
        "ADA", // Diamond
        "APA", // Pearl
        "IPK", // HeartGold
        "IPG", // SoulSilver
        "IRB", // Black
        "IRA", // White
        "IRE", // Black 2
        "IRD"  // White 2
    };

    constexpr char dsPostfixes[] = {'E', 'S', 'K', 'J', 'I', 'D', 'F', 'O'};

    constexpr std::string_view ctrIds[] = {"0x0055D", "0x0055E", "0x011C4", "0x011C5", "0x01648", "0x0175E", "0x01B50", "0x01B51"};

    std::string groupToId1(ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return std::string(dsIds[0]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::DP:
                return std::string(dsIds[1]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::HGSS:
                return std::string(dsIds[3]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::BW:
                return std::string(dsIds[5]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::B2W2:
                return std::string(dsIds[7]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::XY:
                return std::string(ctrIds[0]);
            case ExtraSavesSubScreen::Group::ORAS:
                return std::string(ctrIds[2]);
            case ExtraSavesSubScreen::Group::SM:
                return std::string(ctrIds[4]);
            case ExtraSavesSubScreen::Group::USUM:
                return std::string(ctrIds[6]);
        }
        return "";
    }

    std::string groupToId2(ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return std::string(dsIds[0]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::DP:
                return std::string(dsIds[2]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::HGSS:
                return std::string(dsIds[4]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::BW:
                return std::string(dsIds[6]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::B2W2:
                return std::string(dsIds[8]) + dsPostfixes[0];
            case ExtraSavesSubScreen::Group::XY:
                return std::string(ctrIds[1]);
            case ExtraSavesSubScreen::Group::ORAS:
                return std::string(ctrIds[3]);
            case ExtraSavesSubScreen::Group::SM:
                return std::string(ctrIds[5]);
            case ExtraSavesSubScreen::Group::USUM:
                return std::string(ctrIds[7]);
        }
        return "";
    }

    constexpr std::tuple<const char*, Species, int> groupToLabel1(ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return {"Pt", Species::Giratina, 1};
            case ExtraSavesSubScreen::Group::DP:
                return {"D", Species::Dialga, 0};
            case ExtraSavesSubScreen::Group::HGSS:
                return {"HG", Species::HoOh, 0};
            case ExtraSavesSubScreen::Group::BW:
                return {"B", Species::Reshiram, 0};
            case ExtraSavesSubScreen::Group::B2W2:
                return {"B2", Species::Kyurem, 2};
            case ExtraSavesSubScreen::Group::XY:
                return {"X", Species::Xerneas, 0};
            case ExtraSavesSubScreen::Group::ORAS:
                return {"OR", Species::Groudon, 1};
            case ExtraSavesSubScreen::Group::SM:
                return {"S", Species::Solgaleo, 0};
            case ExtraSavesSubScreen::Group::USUM:
                return {"US", Species::Necrozma, 1};
        }
        return {"", Species::None, 0};
    }

    constexpr std::tuple<const char*, Species, int> groupToLabel2(ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return {"Pt", Species::Giratina, 1};
            case ExtraSavesSubScreen::Group::DP:
                return {"P", Species::Palkia, 0};
            case ExtraSavesSubScreen::Group::HGSS:
                return {"SS", Species::Lugia, 0};
            case ExtraSavesSubScreen::Group::BW:
                return {"W", Species::Zekrom, 0};
            case ExtraSavesSubScreen::Group::B2W2:
                return {"W2", Species::Kyurem, 1};
            case ExtraSavesSubScreen::Group::XY:
                return {"Y", Species::Yveltal, 0};
            case ExtraSavesSubScreen::Group::ORAS:
                return {"AS", Species::Kyogre, 1};
            case ExtraSavesSubScreen::Group::SM:
                return {"M", Species::Lunala, 0};
            case ExtraSavesSubScreen::Group::USUM:
                return {"UM", Species::Necrozma, 2};
        }
        return {"", Species::None, 0};
    }

    constexpr GameVersion groupToGameId1(ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return GameVersion::Pt;
            case ExtraSavesSubScreen::Group::DP:
                return GameVersion::D;
            case ExtraSavesSubScreen::Group::HGSS:
                return GameVersion::HG;
            case ExtraSavesSubScreen::Group::BW:
                return GameVersion::B;
            case ExtraSavesSubScreen::Group::B2W2:
                return GameVersion::B2;
            case ExtraSavesSubScreen::Group::XY:
                return GameVersion::X;
            case ExtraSavesSubScreen::Group::ORAS:
                return GameVersion::OR;
            case ExtraSavesSubScreen::Group::SM:
                return GameVersion::SN;
            case ExtraSavesSubScreen::Group::USUM:
                return GameVersion::US;
        }
        return GameVersion::INVALID;
    }

    constexpr GameVersion groupToGameId2(ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return GameVersion::Pt;
            case ExtraSavesSubScreen::Group::DP:
                return GameVersion::P;
            case ExtraSavesSubScreen::Group::HGSS:
                return GameVersion::SS;
            case ExtraSavesSubScreen::Group::BW:
                return GameVersion::W;
            case ExtraSavesSubScreen::Group::B2W2:
                return GameVersion::W2;
            case ExtraSavesSubScreen::Group::XY:
                return GameVersion::Y;
            case ExtraSavesSubScreen::Group::ORAS:
                return GameVersion::AS;
            case ExtraSavesSubScreen::Group::SM:
                return GameVersion::MN;
            case ExtraSavesSubScreen::Group::USUM:
                return GameVersion::UM;
        }
        return GameVersion::INVALID;
    }

    void drawIcon(std::tuple<const char*, Species, int> label, int x, int y)
    {
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(std::get<1>(label), std::get<2>(label), Generation::SEVEN, Gender::Genderless, x + 8, y);
        Gui::text(std::get<0>(label), x + 24, y + 30, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    }
}

ExtraSavesSubScreen::ExtraSavesSubScreen(Group g)
    : Screen(i18n::localize("A_ADD_SAVE") + '\n' + i18n::localize("X_DELETE_SAVE") + '\n' + i18n::localize("B_BACK")), group(g)
{
    updateSaves();
}

void ExtraSavesSubScreen::updateSaves()
{
    if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP || group == ExtraSavesSubScreen::Group::HGSS ||
        group == ExtraSavesSubScreen::Group::BW || group == ExtraSavesSubScreen::Group::B2W2)
    {
        dsCurrentSaves.clear();
        numSaves     = 0;
        selectedSave = -1;
        firstSave    = 0;
    }
    else
    {
        currentSaves.clear();
        numSaves     = 0;
        selectedSave = -1;
        firstSave    = 0;
    }
    switch (group)
    {
        case ExtraSavesSubScreen::Group::Pt:
            for (auto& postfix : dsPostfixes)
            {
                auto pFixSaves = Configuration::getInstance().extraSaves(std::string(dsIds[0]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(std::string(dsIds[0]) + postfix, std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::DP:
            for (auto& postfix : dsPostfixes)
            {
                auto pFixSaves = Configuration::getInstance().extraSaves(std::string(secondSelected ? dsIds[2] : dsIds[1]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(std::string(secondSelected ? dsIds[2] : dsIds[1]) + postfix, std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::HGSS:
            for (auto& postfix : dsPostfixes)
            {
                auto pFixSaves = Configuration::getInstance().extraSaves(std::string(secondSelected ? dsIds[4] : dsIds[3]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(std::string(secondSelected ? dsIds[4] : dsIds[3]) + postfix, std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::BW:
            for (auto& postfix : dsPostfixes)
            {
                auto pFixSaves = Configuration::getInstance().extraSaves(std::string(secondSelected ? dsIds[6] : dsIds[5]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(std::string(secondSelected ? dsIds[6] : dsIds[5]) + postfix, std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::B2W2:
            for (auto& postfix : dsPostfixes)
            {
                auto pFixSaves = Configuration::getInstance().extraSaves(std::string(secondSelected ? dsIds[8] : dsIds[7]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(std::string(secondSelected ? dsIds[8] : dsIds[7]) + postfix, std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::XY:
            currentSaves = Configuration::getInstance().extraSaves(std::string(secondSelected ? ctrIds[1] : ctrIds[0]));
            numSaves     = currentSaves.size();
            break;
        case ExtraSavesSubScreen::Group::ORAS:
            currentSaves = Configuration::getInstance().extraSaves(std::string(secondSelected ? ctrIds[3] : ctrIds[2]));
            numSaves     = currentSaves.size();
            break;
        case ExtraSavesSubScreen::Group::SM:
            currentSaves = Configuration::getInstance().extraSaves(std::string(secondSelected ? ctrIds[5] : ctrIds[4]));
            numSaves     = currentSaves.size();
            break;
        case ExtraSavesSubScreen::Group::USUM:
            currentSaves = Configuration::getInstance().extraSaves(std::string(secondSelected ? ctrIds[7] : ctrIds[6]));
            numSaves     = currentSaves.size();
            break;
        default:
            break;
    }
}

void ExtraSavesSubScreen::drawTop() const
{
    Gui::backgroundTop(true);
    if (group != ExtraSavesSubScreen::Group::Pt)
    {
        auto label = groupToLabel1(group);
        drawIcon(label, 128, 96);
        if (!secondSelected)
        {
            Gui::drawSelector(127, 95);
        }
        label = groupToLabel2(group);
        drawIcon(label, 224, 96);
        if (secondSelected)
        {
            Gui::drawSelector(223, 95);
        }
    }
    else
    {
        int x      = 176;
        auto label = groupToLabel1(group);
        drawIcon(label, 176, 96);
        Gui::drawSelector(x - 1, 95);
    }
    Gui::text(i18n::localize("EXTRA_SAVES"), 200, 12, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER);
}

void ExtraSavesSubScreen::update(touchPosition* touch)
{
    if (updateConfig)
    {
        if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP || group == ExtraSavesSubScreen::Group::HGSS ||
            group == ExtraSavesSubScreen::Group::BW || group == ExtraSavesSubScreen::Group::B2W2)
        {
            if (!addString.empty())
            {
                dsCurrentSaves.begin()->second.emplace_back(std::move(addString));
            }
            for (auto& saves : dsCurrentSaves)
            {
                Configuration::getInstance().extraSaves(saves.first, saves.second);
            }
        }
        else
        {
            if (!addString.empty())
            {
                currentSaves.emplace_back(std::move(addString));
            }
            Configuration::getInstance().extraSaves(secondSelected ? groupToId2(group) : groupToId1(group), currentSaves);
        }
        updateConfig = false;
        updateSaves();
        addString.clear();
    }
    u32 down = hidKeysDown();
    if (down & KEY_LEFT || down & KEY_RIGHT)
    {
        secondSelected = !secondSelected;
        updateSaves();
    }

    if (down & KEY_DOWN)
    {
        if (selectedSave == 4)
        {
            if (firstSave + 5 < numSaves - 1)
            {
                firstSave++;
            }
            else
            {
                selectedSave++;
            }
        }
        else
        {
            if (firstSave + selectedSave < numSaves - 1)
            {
                selectedSave++;
            }
        }
    }
    else if (down & KEY_UP)
    {
        if (selectedSave == 1)
        {
            if (firstSave > 0)
            {
                firstSave--;
            }
            else
            {
                selectedSave--;
            }
        }
        else if (selectedSave != 0 && selectedSave != -1)
        {
            selectedSave--;
        }
    }

    if (numSaves == 0)
    {
        selectedSave = -1;
        firstSave    = 0;
    }
    else if (selectedSave == -1)
    {
        selectedSave = 0;
    }

    if (down & KEY_A)
    {
        if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP || group == ExtraSavesSubScreen::Group::HGSS ||
            group == ExtraSavesSubScreen::Group::BW || group == ExtraSavesSubScreen::Group::B2W2)
        {
            if (dsCurrentSaves.empty())
            {
                dsCurrentSaves[secondSelected ? groupToId2(group) : groupToId1(group)] = {};
            }
            addOverlay<FileChooseOverlay>(addString);
        }
        else
        {
            addOverlay<FileChooseOverlay>(addString);
        }
        updateConfig = true;
    }
    else if (down & KEY_B)
    {
        Gui::screenBack();
        return;
    }
    else if (down & KEY_X && selectedSave != -1)
    {
        std::string* entry = nullptr;
        if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP || group == ExtraSavesSubScreen::Group::HGSS ||
            group == ExtraSavesSubScreen::Group::BW || group == ExtraSavesSubScreen::Group::B2W2)
        {
            int i = 0;
            for (auto& idGroup : dsCurrentSaves)
            {
                if (i + (int)idGroup.second.size() > selectedSave + firstSave)
                {
                    entry = &idGroup.second[selectedSave + firstSave - i];
                }
            }
        }
        else
        {
            entry = &currentSaves[selectedSave + firstSave];
        }
        if (Gui::showChoiceMessage(i18n::localize("DELETE_EXTRASAVE_ENTRY") + '\n' + ("\'" + *entry + "\'")))
        {
            if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP || group == ExtraSavesSubScreen::Group::HGSS ||
                group == ExtraSavesSubScreen::Group::BW || group == ExtraSavesSubScreen::Group::B2W2)
            {
                int i = 0;
                for (auto& idGroup : dsCurrentSaves)
                {
                    if (i + (int)idGroup.second.size() > selectedSave + firstSave)
                    {
                        idGroup.second.erase(idGroup.second.begin() + selectedSave + firstSave - i);
                        break;
                    }
                }
            }
            else
            {
                currentSaves.erase(currentSaves.begin() + selectedSave + firstSave);
            }
            updateConfig = true;
        }
    }
}

void ExtraSavesSubScreen::drawBottom() const
{
    Gui::backgroundBottom(true);
    Gui::sprite(ui_sheet_gameselector_savebox_idx, 22, 94);

    Gui::text(secondSelected ? i18n::game(Configuration::getInstance().language(), groupToGameId2(group))
                             : i18n::game(Configuration::getInstance().language(), groupToGameId1(group)),
        27, 26, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);

    if (selectedSave > -1)
    {
        Gui::drawSolidRect(24, 96 + 17 * selectedSave, 174, 16, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP || group == ExtraSavesSubScreen::Group::HGSS ||
        group == ExtraSavesSubScreen::Group::BW || group == ExtraSavesSubScreen::Group::B2W2)
    {
        int y    = 97;
        size_t i = 0;
        size_t j = 0;
        for (auto& idSaves : dsCurrentSaves)
        {
            if (i + idSaves.second.size() >= (size_t)firstSave)
            {
                for (; j < 6 && j < idSaves.second.size(); j++)
                {
                    if ((int)j == selectedSave)
                    {
                        Gui::text(idSaves.second[i - firstSave + j], 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
                            TextWidthAction::SCROLL, 169);
                    }
                    else
                    {
                        Gui::text(idSaves.second[i - firstSave + j], 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP,
                            TextWidthAction::SLICE, 169);
                    }
                    y += 17;
                }
            }
            if (j == 6)
            {
                break;
            }
            i += idSaves.second.size();
        }
    }
    else
    {
        int y = 97;
        for (int i = firstSave; i < firstSave + 6; i++)
        {
            if (i < (int)currentSaves.size())
            {
                if (i - firstSave == selectedSave)
                {
                    Gui::text(currentSaves[i], 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SCROLL, 169);
                }
                else
                {
                    Gui::text(currentSaves[i], 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SLICE, 169);
                }
            }
            else
            {
                break;
            }
            y += 17;
        }
    }

    if (selectedSave > 0 && firstSave > 0)
    {
        Gui::drawSolidRect(191, 102, 4, 5, PKSM_Color(0x0f, 0x16, 0x59, 255));
        Gui::drawSolidTriangle(189, 102, 197, 102, 193, 97, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    if (selectedSave < 5 && firstSave + 5 < numSaves - 1)
    {
        Gui::drawSolidRect(191, 186, 4, 5, PKSM_Color(0x0f, 0x16, 0x59, 255));
        Gui::drawSolidTriangle(189, 191, 197, 191, 193, 196, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    Gui::text(i18n::localize("A_ADD_SAVE"), 248, 120, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER, TextWidthAction::WRAP, 94);
    Gui::text(i18n::localize("X_DELETE_SAVE"), 248, 172, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::CENTER, TextWidthAction::WRAP, 94);

    Gui::text(i18n::localize("EXTRASAVES_CONFIGURE_INSTRUCTIONS"), 160, 223, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    Gui::drawSolidRect(245, 23, 48, 48, COLOR_BLACK);
    Gui::drawSolidRect(243, 21, 52, 52, PKSM_Color(15, 22, 89, 255));
    drawIcon(secondSelected ? groupToLabel2(group) : groupToLabel1(group), 245, 23);
}
