/*
 *   This file is part of PKExtraSavesSubScreen::Group::SM
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

#include "ExtraSavesSubScreen.hpp"
#include "Configuration.hpp"
#include "FileChooseOverlay.hpp"
#include "GameVersion.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "Species.hpp"
#include "Title.hpp"

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

    // R, S, E, FR, LG, X, Y, OR, AS, S, M, US, UM, R, G, B, Y, G, S, C, LGP, LGE, Sw, Sh
    std::array<std::string, 24> ctrIds = {"", "", "", "", "", "0x0055D", "0x0055E", "0x011C4",
        "0x011C5", "0x01648", "0x0175E", "0x01B50", "0x01B51", "0x01710", "0x0170D", "0x01711",
        "0x01712", "0x01726", "0x01727", "0x01728", "", "", "", ""};

    // RGB will play by different rules, because of course they will. BDSP will be a breath of fresh
    // air.
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
            case ExtraSavesSubScreen::Group::RS:
                return ctrIds[0];
            case ExtraSavesSubScreen::Group::E:
                return ctrIds[2];
            case ExtraSavesSubScreen::Group::FRLG:
                return ctrIds[3];
            case ExtraSavesSubScreen::Group::XY:
                return ctrIds[5];
            case ExtraSavesSubScreen::Group::ORAS:
                return ctrIds[7];
            case ExtraSavesSubScreen::Group::SM:
                return ctrIds[9];
            case ExtraSavesSubScreen::Group::USUM:
                return ctrIds[11];
            case ExtraSavesSubScreen::Group::Y:
                return ctrIds[16];
            case ExtraSavesSubScreen::Group::GS:
                return ctrIds[17];
            case ExtraSavesSubScreen::Group::C:
                return ctrIds[19];
            case ExtraSavesSubScreen::Group::LGPE:
                return ctrIds[20];
            case ExtraSavesSubScreen::Group::SwSh:
                return ctrIds[22];
            case ExtraSavesSubScreen::Group::RGB:
                // handled differently
                break;
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
            case ExtraSavesSubScreen::Group::RS:
                return ctrIds[1];
            case ExtraSavesSubScreen::Group::E:
                return ctrIds[2];
            case ExtraSavesSubScreen::Group::FRLG:
                return ctrIds[4];
            case ExtraSavesSubScreen::Group::XY:
                return ctrIds[6];
            case ExtraSavesSubScreen::Group::ORAS:
                return ctrIds[8];
            case ExtraSavesSubScreen::Group::SM:
                return ctrIds[10];
            case ExtraSavesSubScreen::Group::USUM:
                return ctrIds[12];
            case ExtraSavesSubScreen::Group::Y:
                return ctrIds[16];
            case ExtraSavesSubScreen::Group::GS:
                return ctrIds[18];
            case ExtraSavesSubScreen::Group::C:
                return ctrIds[19];
            case ExtraSavesSubScreen::Group::LGPE:
                return ctrIds[21];
            case ExtraSavesSubScreen::Group::SwSh:
                return ctrIds[23];
            case ExtraSavesSubScreen::Group::RGB:
                // handled differently
                break;
        }
        return "";
    }

    constexpr std::tuple<const char*, pksm::Species, int> groupToLabel1(
        ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return {"Pt", pksm::Species::Giratina, 1};
            case ExtraSavesSubScreen::Group::DP:
                return {"D", pksm::Species::Dialga, 0};
            case ExtraSavesSubScreen::Group::HGSS:
                return {"HG", pksm::Species::HoOh, 0};
            case ExtraSavesSubScreen::Group::BW:
                return {"B", pksm::Species::Reshiram, 0};
            case ExtraSavesSubScreen::Group::B2W2:
                return {"B2", pksm::Species::Kyurem, 2};
            case ExtraSavesSubScreen::Group::RS:
                return {"R", pksm::Species::Groudon, 0};
            case ExtraSavesSubScreen::Group::E:
                return {"E", pksm::Species::Rayquaza, 0};
            case ExtraSavesSubScreen::Group::FRLG:
                return {"FR", pksm::Species::Charizard, 0};
            case ExtraSavesSubScreen::Group::XY:
                return {"X", pksm::Species::Xerneas, 0};
            case ExtraSavesSubScreen::Group::ORAS:
                return {"OR", pksm::Species::Groudon, 1};
            case ExtraSavesSubScreen::Group::SM:
                return {"S", pksm::Species::Solgaleo, 0};
            case ExtraSavesSubScreen::Group::USUM:
                return {"US", pksm::Species::Necrozma, 1};
            case ExtraSavesSubScreen::Group::Y:
                return {"Y", pksm::Species::Pikachu, 0};
            case ExtraSavesSubScreen::Group::GS:
                return {"G", pksm::Species::HoOh, 0};
            case ExtraSavesSubScreen::Group::C:
                return {"C", pksm::Species::Suicune, 0};
            case ExtraSavesSubScreen::Group::LGPE:
                return {"LGP", pksm::Species::Pikachu, 0};
            case ExtraSavesSubScreen::Group::SwSh:
                return {"Sw", pksm::Species::Zacian, 1};
            case ExtraSavesSubScreen::Group::RGB:
                // handled differently
                break;
        }
        return {"", pksm::Species::None, 0};
    }

    constexpr std::tuple<const char*, pksm::Species, int> groupToLabel2(
        ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return {"Pt", pksm::Species::Giratina, 1};
            case ExtraSavesSubScreen::Group::DP:
                return {"P", pksm::Species::Palkia, 0};
            case ExtraSavesSubScreen::Group::HGSS:
                return {"SS", pksm::Species::Lugia, 0};
            case ExtraSavesSubScreen::Group::BW:
                return {"W", pksm::Species::Zekrom, 0};
            case ExtraSavesSubScreen::Group::B2W2:
                return {"W2", pksm::Species::Kyurem, 1};
            case ExtraSavesSubScreen::Group::RS:
                return {"S", pksm::Species::Kyogre, 0};
            case ExtraSavesSubScreen::Group::E:
                return {"E", pksm::Species::Rayquaza, 0};
            case ExtraSavesSubScreen::Group::FRLG:
                return {"LG", pksm::Species::Venusaur, 0};
            case ExtraSavesSubScreen::Group::XY:
                return {"Y", pksm::Species::Yveltal, 0};
            case ExtraSavesSubScreen::Group::ORAS:
                return {"AS", pksm::Species::Kyogre, 1};
            case ExtraSavesSubScreen::Group::SM:
                return {"M", pksm::Species::Lunala, 0};
            case ExtraSavesSubScreen::Group::USUM:
                return {"UM", pksm::Species::Necrozma, 2};
            case ExtraSavesSubScreen::Group::Y:
                return {"Y", pksm::Species::Pikachu, 0};
            case ExtraSavesSubScreen::Group::GS:
                return {"S", pksm::Species::Lugia, 0};
            case ExtraSavesSubScreen::Group::C:
                return {"C", pksm::Species::Suicune, 0};
            case ExtraSavesSubScreen::Group::LGPE:
                return {"LGE", pksm::Species::Eevee, 0};
            case ExtraSavesSubScreen::Group::SwSh:
                return {"Sh", pksm::Species::Zamazenta, 1};
            case ExtraSavesSubScreen::Group::RGB:
                // handled differently
                break;
        }
        return {"", pksm::Species::None, 0};
    }

    constexpr pksm::GameVersion groupToGameId1(ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return pksm::GameVersion::Pt;
            case ExtraSavesSubScreen::Group::DP:
                return pksm::GameVersion::D;
            case ExtraSavesSubScreen::Group::HGSS:
                return pksm::GameVersion::HG;
            case ExtraSavesSubScreen::Group::BW:
                return pksm::GameVersion::B;
            case ExtraSavesSubScreen::Group::B2W2:
                return pksm::GameVersion::B2;
            case ExtraSavesSubScreen::Group::RS:
                return pksm::GameVersion::R;
            case ExtraSavesSubScreen::Group::E:
                return pksm::GameVersion::E;
            case ExtraSavesSubScreen::Group::FRLG:
                return pksm::GameVersion::FR;
            case ExtraSavesSubScreen::Group::XY:
                return pksm::GameVersion::X;
            case ExtraSavesSubScreen::Group::ORAS:
                return pksm::GameVersion::OR;
            case ExtraSavesSubScreen::Group::SM:
                return pksm::GameVersion::SN;
            case ExtraSavesSubScreen::Group::USUM:
                return pksm::GameVersion::US;
            case ExtraSavesSubScreen::Group::Y:
                return pksm::GameVersion::YW;
            case ExtraSavesSubScreen::Group::GS:
                return pksm::GameVersion::GD;
            case ExtraSavesSubScreen::Group::C:
                return pksm::GameVersion::C;
            case ExtraSavesSubScreen::Group::LGPE:
                return pksm::GameVersion::GP;
            case ExtraSavesSubScreen::Group::SwSh:
                return pksm::GameVersion::SW;
            case ExtraSavesSubScreen::Group::RGB:
                // handled differently
                break;
        }
        return pksm::GameVersion::INVALID;
    }

    constexpr pksm::GameVersion groupToGameId2(ExtraSavesSubScreen::Group g)
    {
        switch (g)
        {
            case ExtraSavesSubScreen::Group::Pt:
                return pksm::GameVersion::Pt;
            case ExtraSavesSubScreen::Group::DP:
                return pksm::GameVersion::P;
            case ExtraSavesSubScreen::Group::HGSS:
                return pksm::GameVersion::SS;
            case ExtraSavesSubScreen::Group::BW:
                return pksm::GameVersion::W;
            case ExtraSavesSubScreen::Group::B2W2:
                return pksm::GameVersion::W2;
            case ExtraSavesSubScreen::Group::RS:
                return pksm::GameVersion::S;
            case ExtraSavesSubScreen::Group::E:
                return pksm::GameVersion::E;
            case ExtraSavesSubScreen::Group::FRLG:
                return pksm::GameVersion::LG;
            case ExtraSavesSubScreen::Group::XY:
                return pksm::GameVersion::Y;
            case ExtraSavesSubScreen::Group::ORAS:
                return pksm::GameVersion::AS;
            case ExtraSavesSubScreen::Group::SM:
                return pksm::GameVersion::MN;
            case ExtraSavesSubScreen::Group::USUM:
                return pksm::GameVersion::UM;
            case ExtraSavesSubScreen::Group::Y:
                return pksm::GameVersion::YW;
            case ExtraSavesSubScreen::Group::GS:
                return pksm::GameVersion::SV;
            case ExtraSavesSubScreen::Group::C:
                return pksm::GameVersion::C;
            case ExtraSavesSubScreen::Group::LGPE:
                return pksm::GameVersion::GE;
            case ExtraSavesSubScreen::Group::SwSh:
                return pksm::GameVersion::SH;
            case ExtraSavesSubScreen::Group::RGB:
                // handled differently
                break;
        }
        return pksm::GameVersion::INVALID;
    }

    void drawIcon(std::tuple<const char*, pksm::Species, int> label, int x, int y)
    {
        Gui::drawSolidRect(x, y, 48, 48, COLOR_HIGHBLUE);
        Gui::pkm(std::get<1>(label), std::get<2>(label), pksm::Generation::SEVEN,
            pksm::Gender::Genderless, x + 8, y);
        Gui::text(std::get<0>(label), x + 24, y + 30, FONT_SIZE_11, COLOR_WHITE, TextPosX::CENTER,
            TextPosY::TOP);
    }
}

ExtraSavesSubScreen::ExtraSavesSubScreen(Group g)
    : Screen(i18n::localize("A_ADD_SAVE") + '\n' + i18n::localize("X_DELETE_SAVE") + '\n' +
             i18n::localize("B_BACK")),
      group(g)
{
    for (size_t i = 0; i < 24; i++)
    {
        static constexpr pksm::GameVersion versions[24] = {pksm::GameVersion::R,
            pksm::GameVersion::S, pksm::GameVersion::E, pksm::GameVersion::FR,
            pksm::GameVersion::LG, pksm::GameVersion::X, pksm::GameVersion::Y,
            pksm::GameVersion::OR, pksm::GameVersion::AS, pksm::GameVersion::SN,
            pksm::GameVersion::MN, pksm::GameVersion::US, pksm::GameVersion::UM,
            pksm::GameVersion::RD, pksm::GameVersion::GN, pksm::GameVersion::BU,
            pksm::GameVersion::YW, pksm::GameVersion::GD, pksm::GameVersion::SV,
            pksm::GameVersion::C, pksm::GameVersion::GP, pksm::GameVersion::GE,
            pksm::GameVersion::SW, pksm::GameVersion::SH};
        std::string id = Configuration::getInstance().titleId(versions[i]);
        u64 tid        = strtoull(id.c_str(), nullptr, 16);
        ctrIds[i]      = Title::tidToCheckpointPrefix(tid);
    }
    updateSaves();
}

void ExtraSavesSubScreen::updateSaves()
{
    if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP ||
        group == ExtraSavesSubScreen::Group::HGSS || group == ExtraSavesSubScreen::Group::BW ||
        group == ExtraSavesSubScreen::Group::B2W2)
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
            for (const auto& postfix : dsPostfixes)
            {
                auto pFixSaves =
                    Configuration::getInstance().extraSaves(std::string(dsIds[0]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(std::string(dsIds[0]) + postfix, std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::DP:
            for (const auto& postfix : dsPostfixes)
            {
                auto pFixSaves = Configuration::getInstance().extraSaves(
                    std::string(selectedGame ? dsIds[2] : dsIds[1]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(
                        std::string(selectedGame ? dsIds[2] : dsIds[1]) + postfix,
                        std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::HGSS:
            for (const auto& postfix : dsPostfixes)
            {
                auto pFixSaves = Configuration::getInstance().extraSaves(
                    std::string(selectedGame ? dsIds[4] : dsIds[3]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(
                        std::string(selectedGame ? dsIds[4] : dsIds[3]) + postfix,
                        std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::BW:
            for (const auto& postfix : dsPostfixes)
            {
                auto pFixSaves = Configuration::getInstance().extraSaves(
                    std::string(selectedGame ? dsIds[6] : dsIds[5]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(
                        std::string(selectedGame ? dsIds[6] : dsIds[5]) + postfix,
                        std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::B2W2:
            for (const auto& postfix : dsPostfixes)
            {
                auto pFixSaves = Configuration::getInstance().extraSaves(
                    std::string(selectedGame ? dsIds[8] : dsIds[7]) + postfix);
                if (!pFixSaves.empty())
                {
                    numSaves += pFixSaves.size();
                    dsCurrentSaves.emplace(
                        std::string(selectedGame ? dsIds[8] : dsIds[7]) + postfix,
                        std::move(pFixSaves));
                }
            }
            break;
        case ExtraSavesSubScreen::Group::RGB:
            currentSaves = Configuration::getInstance().extraSaves(std::string(
                selectedGame ? ((selectedGame == 1) ? ctrIds[14] : ctrIds[15]) : ctrIds[13]));
            numSaves     = currentSaves.size();
            break;
        default:
            currentSaves = Configuration::getInstance().extraSaves(
                std::string(selectedGame ? groupToId2(group) : groupToId1(group)));
            numSaves = currentSaves.size();
            break;
    }
}

void ExtraSavesSubScreen::drawTop() const
{
    Gui::backgroundTop(true);
    if (group == ExtraSavesSubScreen::Group::RGB)
    {
        std::tuple<const char*, pksm::Species, int> label = {"R", pksm::Species::Charmander, 0};
        drawIcon(label, 80, 96);
        if (!selectedGame)
        {
            Gui::drawSelector(79, 95);
        }
        label = {"G", pksm::Species::Bulbasaur, 0};
        drawIcon(label, 176, 96);
        if (selectedGame == 1)
        {
            Gui::drawSelector(175, 95);
        }
        label = {"B", pksm::Species::Squirtle, 0};
        drawIcon(label, 272, 96);
        if (selectedGame == 2)
        {
            Gui::drawSelector(271, 95);
        }
    }
    else if (group != ExtraSavesSubScreen::Group::Pt && group != ExtraSavesSubScreen::Group::E &&
             group != ExtraSavesSubScreen::Group::C && group != ExtraSavesSubScreen::Group::Y)
    {
        auto label = groupToLabel1(group);
        drawIcon(label, 128, 96);
        if (!selectedGame)
        {
            Gui::drawSelector(127, 95);
        }
        label = groupToLabel2(group);
        drawIcon(label, 224, 96);
        if (selectedGame)
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
    Gui::text(i18n::localize("EXTRA_SAVES"), 200, 12, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::CENTER);
}

void ExtraSavesSubScreen::update(touchPosition* touch)
{
    if (updateConfig)
    {
        if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP ||
            group == ExtraSavesSubScreen::Group::HGSS || group == ExtraSavesSubScreen::Group::BW ||
            group == ExtraSavesSubScreen::Group::B2W2)
        {
            if (!addString.empty())
            {
                dsCurrentSaves.begin()->second.emplace_back(std::move(addString));
            }
            for (const auto& saves : dsCurrentSaves)
            {
                Configuration::getInstance().extraSaves(saves.first, saves.second);
            }
        }
        else if (group == ExtraSavesSubScreen::Group::RGB)
        {
            if (!addString.empty())
            {
                currentSaves.emplace_back(std::move(addString));
            }
            Configuration::getInstance().extraSaves(
                selectedGame ? ((selectedGame == 1) ? ctrIds[14] : ctrIds[15]) : ctrIds[13],
                currentSaves);
        }
        else
        {
            if (!addString.empty())
            {
                currentSaves.emplace_back(std::move(addString));
            }
            Configuration::getInstance().extraSaves(
                selectedGame ? groupToId2(group) : groupToId1(group), currentSaves);
        }
        updateConfig = false;
        updateSaves();
        addString.clear();
    }
    u32 down = hidKeysDown();
    if (down & KEY_LEFT || down & KEY_RIGHT)
    {
        if (group == ExtraSavesSubScreen::Group::RGB)
        {
            if (down & KEY_LEFT)
            {
                selectedGame--;
                if (selectedGame < 0)
                {
                    selectedGame = 2;
                }
            }
            else
            {
                selectedGame++;
                if (selectedGame > 2)
                {
                    selectedGame = 0;
                }
            }
        }
        else
        {
            selectedGame = !selectedGame;
        }
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
        if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP ||
            group == ExtraSavesSubScreen::Group::HGSS || group == ExtraSavesSubScreen::Group::BW ||
            group == ExtraSavesSubScreen::Group::B2W2)
        {
            if (dsCurrentSaves.empty())
            {
                dsCurrentSaves[selectedGame ? groupToId2(group) : groupToId1(group)] = {};
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
        const std::string* entry = nullptr;
        if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP ||
            group == ExtraSavesSubScreen::Group::HGSS || group == ExtraSavesSubScreen::Group::BW ||
            group == ExtraSavesSubScreen::Group::B2W2)
        {
            int i = 0;
            for (const auto& idGroup : dsCurrentSaves)
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
        if (Gui::showChoiceMessage(
                i18n::localize("DELETE_EXTRASAVE_ENTRY") + '\n' + ("\'" + *entry + "\'")))
        {
            if (group == ExtraSavesSubScreen::Group::Pt ||
                group == ExtraSavesSubScreen::Group::DP ||
                group == ExtraSavesSubScreen::Group::HGSS ||
                group == ExtraSavesSubScreen::Group::BW ||
                group == ExtraSavesSubScreen::Group::B2W2)
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

    if (group == ExtraSavesSubScreen::Group::RGB)
    {
        Gui::text(
            i18n::game(Configuration::getInstance().language(),
                selectedGame ? (selectedGame == 1 ? pksm::GameVersion::GN : pksm::GameVersion::BU)
                             : pksm::GameVersion::RD),
            27, 26, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }
    else
    {
        Gui::text(selectedGame
                      ? i18n::game(Configuration::getInstance().language(), groupToGameId2(group))
                      : i18n::game(Configuration::getInstance().language(), groupToGameId1(group)),
            27, 26, FONT_SIZE_14, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
    }

    if (selectedSave > -1)
    {
        Gui::drawSolidRect(24, 96 + 17 * selectedSave, 174, 16, PKSM_Color(0x0f, 0x16, 0x59, 255));
    }

    if (group == ExtraSavesSubScreen::Group::Pt || group == ExtraSavesSubScreen::Group::DP ||
        group == ExtraSavesSubScreen::Group::HGSS || group == ExtraSavesSubScreen::Group::BW ||
        group == ExtraSavesSubScreen::Group::B2W2)
    {
        int y    = 97;
        size_t i = 0;
        size_t j = 0;
        for (const auto& idSaves : dsCurrentSaves)
        {
            if (i + idSaves.second.size() >= (size_t)firstSave)
            {
                for (; j < 6 && j < idSaves.second.size(); j++)
                {
                    if ((int)j == selectedSave)
                    {
                        Gui::text(idSaves.second[i - firstSave + j], 29, y, FONT_SIZE_11,
                            COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SCROLL,
                            169);
                    }
                    else
                    {
                        Gui::text(idSaves.second[i - firstSave + j], 29, y, FONT_SIZE_11,
                            COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SLICE,
                            169);
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
                    Gui::text(currentSaves[i], 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT,
                        TextPosY::TOP, TextWidthAction::SCROLL, 169);
                }
                else
                {
                    Gui::text(currentSaves[i], 29, y, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT,
                        TextPosY::TOP, TextWidthAction::SLICE, 169);
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

    Gui::text(i18n::localize("A_ADD_SAVE"), 248, 120, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::CENTER, TextWidthAction::WRAP, 94);
    Gui::text(i18n::localize("X_DELETE_SAVE"), 248, 172, FONT_SIZE_14, COLOR_WHITE,
        TextPosX::CENTER, TextPosY::CENTER, TextWidthAction::WRAP, 94);

    Gui::text(i18n::localize("EXTRASAVES_CONFIGURE_INSTRUCTIONS"), 160, 223, FONT_SIZE_11,
        COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    Gui::drawSolidRect(245, 23, 48, 48, COLOR_BLACK);
    Gui::drawSolidRect(243, 21, 52, 52, PKSM_Color(15, 22, 89, 255));
    if (group == ExtraSavesSubScreen::Group::RGB)
    {
        std::tuple<const char*, pksm::Species, int> rLabel = {"R", pksm::Species::Charmander, 0};
        std::tuple<const char*, pksm::Species, int> gLabel = {"G", pksm::Species::Bulbasaur, 0};
        std::tuple<const char*, pksm::Species, int> bLabel = {"B", pksm::Species::Squirtle, 0};

        drawIcon(selectedGame ? ((selectedGame == 1) ? gLabel : bLabel) : rLabel, 245, 23);
    }
    else
    {
        drawIcon(selectedGame ? groupToLabel2(group) : groupToLabel1(group), 245, 23);
    }
}
