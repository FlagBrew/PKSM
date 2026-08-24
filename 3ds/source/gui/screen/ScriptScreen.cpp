/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2025 Bernardo Giordano, Admiral Fish, piepie62
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

#include "ScriptScreen.hpp"
#include "Archive.hpp"
#include "banks.hpp"
#include "Configuration.hpp"
#include "Directory.hpp"
#include "endian.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "sav/Sav.hpp"
#include "sav/Sav4.hpp"
#include "ScreenStack.hpp"
#include "ScrollingTextScreen.hpp"
#include <array>

#include "picoc.h"
#undef min // Get rid of picoc's min function

#include <algorithm>

namespace
{
    constexpr std::string_view MAGIC = "PKSMSCRIPT";

    std::string getScriptDir(pksm::GameVersion version)
    {
        switch (version)
        {
            case pksm::GameVersion::RD:
            case pksm::GameVersion::GN:
            case pksm::GameVersion::BU:
            case pksm::GameVersion::YW:
                return "/scripts/rgby";
            // i am sorry in advance to anyone who makes Gen II scripts for having to write any
            // directly save-interacting code one, three, or five times
            case pksm::GameVersion::GD:
            case pksm::GameVersion::SV:
                return "/scripts/gs";
            case pksm::GameVersion::C:
                return "/scripts/c";
            case pksm::GameVersion::R:
            case pksm::GameVersion::S:
                return "/scripts/rs";
            case pksm::GameVersion::E:
                return "/scripts/e";
            case pksm::GameVersion::FR:
            case pksm::GameVersion::LG:
                return "/scripts/frlg";
            case pksm::GameVersion::HG:
            case pksm::GameVersion::SS:
                return "/scripts/hgss";
            case pksm::GameVersion::D:
            case pksm::GameVersion::P:
                return "/scripts/dp";
            case pksm::GameVersion::Pt:
                return "/scripts/pt";
            case pksm::GameVersion::B:
            case pksm::GameVersion::W:
                return "/scripts/bw";
            case pksm::GameVersion::B2:
            case pksm::GameVersion::W2:
                return "/scripts/b2w2";
            case pksm::GameVersion::X:
            case pksm::GameVersion::Y:
                return "/scripts/xy";
            case pksm::GameVersion::OR:
            case pksm::GameVersion::AS:
                return "/scripts/oras";
            case pksm::GameVersion::SN:
            case pksm::GameVersion::MN:
                return "/scripts/sm";
            case pksm::GameVersion::US:
            case pksm::GameVersion::UM:
                return "/scripts/usum";
            case pksm::GameVersion::GP:
            case pksm::GameVersion::GE:
                return "/scripts/lgpe";
            case pksm::GameVersion::SW:
            case pksm::GameVersion::SH:
                return "/scripts/swsh";
            default:
                return "/scripts/" + std::to_string((int)version);
        }
    }

    Picoc* picoC()
    {
        static Picoc picoc;
        PicocInitialize(&picoc, PICOC_STACKSIZE);
        return &picoc;
    }

    std::vector<u8> scriptRead(const std::string& path)
    {
        std::vector<u8> ret;
        FILE* in = fopen(path.c_str(), "rb");
        fseek(in, 0, SEEK_END);
        if (!ferror(in))
        {
            size_t size = ftell(in);
            fseek(in, 0, SEEK_SET);
            ret = std::vector<u8>(size);
            fread(ret.data(), 1, size, in);
        }
        else
        {
            Gui::error(i18n::localize("SCRIPTS_FAILED_OPEN"), errno);
        }
        fclose(in);

        return ret;
    }
}

ScriptScreen::ScriptScreen()
    : browser("romfs:" + getScriptDir(TitleLoader::save->version())),
      hid(8, 1),
      sdSearch(false),
      cScripts(false)
{
    if (!browser.good() && browser.reroot("/3ds/PKSM" + getScriptDir(TitleLoader::save->version())))
    {
        sdSearch = true;
    }
}

const std::string& ScriptScreen::emptyLabel() const
{
    return i18n::localize(browser.good() ? "EMPTY" : "FOLDER_DOESNT_EXIST");
}

void ScriptScreen::drawTop() const
{
    const auto& entries = browser.entries();

    // slight change to stripy top
    Gui::drawSolidRect(0, 0, 400, 240, PKSM_Color(26, 35, 126, 255));
    for (int x = -240; x < 400; x += 7)
    {
        Gui::drawLine(x, 0, x + 240, 240, 2, COLOR_LINEBLUE);
    }
    Gui::drawSolidRect(0, 0, 400, 25, PKSM_Color(15, 22, 89, 255));

    // Leaving space for the icon
    Gui::text(browser.path(), 15, 2, FONT_SIZE_11, COLOR_YELLOW, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("SCRIPTS_INST1"), 200, 224, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP, TextWidthAction::SQUISH, 398);

    Gui::drawSolidRect(0, 20 + hid.index() * 25, 400, 25, PKSM_Color(128, 128, 128, 255));
    Gui::drawSolidRect(1, 21 + hid.index() * 25, 398, 23, COLOR_MASKBLACK);

    if (entries.empty())
    {
        Gui::text(emptyLabel(), 30, 24, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        return;
    }

    for (size_t i = hid.page() * hid.maxVisibleEntries();
         i < (hid.page() + 1) * hid.maxVisibleEntries(); i++)
    {
        if (i >= entries.size())
        {
            break;
        }
        else
        {
            Gui::sprite(entries[i].directory ? ui_sheet_icon_folder_idx : ui_sheet_icon_script_idx,
                3, 23 + i % hid.maxVisibleEntries() * 25);
            Gui::text(entries[i].name, 30, 24 + (i % hid.maxVisibleEntries() * 25), FONT_SIZE_11,
                COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        }
    }
}

void ScriptScreen::drawBottom() const
{
    Gui::backgroundBottom(true);
    Gui::drawSolidRect(20, 40, 280, 60, PKSM_Color(128, 128, 128, 255));
    Gui::drawSolidRect(21, 41, 278, 58, COLOR_MASKBLACK);
    Gui::text(i18n::localize("SCRIPTS_INST2"), 160, 224, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER,
        TextPosY::TOP, TextWidthAction::SQUISH, 318);

    const auto& entries = browser.entries();
    Gui::text(entries.empty() ? emptyLabel() : entries[hid.fullIndex()].name, 30, 44, FONT_SIZE_11,
        COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, TextWidthAction::SCROLL, 260.0f);
}

void ScriptScreen::update(touchPosition* touch)
{
    hid.update(browser.entries().size());
    u32 down = hidKeysDown();
    if (down & KEY_B)
    {
        if (browser.atRoot())
        {
            ScreenStack::requestPop();
            return;
        }
        else if (browser.leave())
        {
            hid.select(0);
        }
    }
    else if (down & KEY_A)
    {
        const auto& entries = browser.entries();
        if (!entries.empty())
        {
            const size_t index = hid.fullIndex();
            if (entries[index].directory)
            {
                if (browser.enter(index))
                {
                    hid.select(0);
                }
            }
            else
            {
                if (Gui::showChoiceMessage(i18n::localize("SCRIPTS_CONFIRM_USE") + "\n" +
                                           ('\'' + entries[index].name + '\'')))
                {
                    applyScript();
                }
            }
        }
    }
    else if (down & KEY_X)
    {
        std::string dirString = (!sdSearch ? "/3ds/PKSM" : "romfs:") +
                                (cScripts ? std::string("/scripts/universal")
                                          : getScriptDir(TitleLoader::save->version()));
        if (browser.reroot(dirString))
        {
            sdSearch = !sdSearch;
            hid.select(0);
        }
        else
        {
            Gui::warn(("\"" + dirString + "\"") + '\n' + i18n::localize("SCRIPTS_NOT_FOUND"));
        }
    }
    else if (down & KEY_Y)
    {
        std::string dirString = (sdSearch ? "/3ds/PKSM" : "romfs:") +
                                (!cScripts ? std::string("/scripts/universal")
                                           : getScriptDir(TitleLoader::save->version()));
        if (browser.reroot(dirString))
        {
            cScripts = !cScripts;
            hid.select(0);
        }
        else
        {
            Gui::warn(("\"" + dirString + "\"") + '\n' + i18n::localize("SCRIPTS_NOT_FOUND"));
        }
    }
}

void ScriptScreen::applyScript()
{
    std::string scriptFile = browser.pathOf(hid.fullIndex());
    if (scriptFile.rfind(".c") == scriptFile.size() - 2)
    {
        parsePicoCScript(scriptFile);
        return;
    }
    auto scriptData = scriptRead(scriptFile);

    if (scriptData.empty())
    {
        return;
    }

    for (size_t i = 0; i < std::min(MAGIC.size(), scriptData.size()); i++)
    {
        if (scriptData[i] != MAGIC[i])
        {
            Gui::warn(i18n::localize("SCRIPTS_INVALID"));
            return;
        }
    }

    size_t index = MAGIC.size();
    while (index < scriptData.size())
    {
        u32 offset = LittleEndian::convertTo<u32>(scriptData.data() + index);
        u32 length = LittleEndian::convertTo<u32>(scriptData.data() + index + 4);
        u32 repeat = LittleEndian::convertTo<u32>(scriptData.data() + index + 8 + length);

        if (TitleLoader::save->generation() == pksm::Generation::FOUR)
        {
            u32 sbo = ((pksm::Sav4*)TitleLoader::save.get())->getSBO();
            u32 gbo = ((pksm::Sav4*)TitleLoader::save.get())->getGBO();
            if (TitleLoader::save->boxOffset(0, 0) - sbo <= offset &&
                TitleLoader::save->boxOffset(TitleLoader::save->maxBoxes(), 0) - sbo >= offset)
            {
                offset += sbo;
            }
            else
            {
                offset += gbo;
            }
        }

        for (size_t i = 0; i < repeat; i++)
        {
            std::copy(scriptData.data() + index + 8, scriptData.data() + index + 8 + length,
                &TitleLoader::save->rawData()[offset + i * length]);
        }

        index += 12 + length;
    }
}

void ScriptScreen::parsePicoCScript(std::string& file)
{
    // The loops used in PicoC make this basically a necessity
    aptSetHomeAllowed(false);
    // setup for printing errors
    static char error[4096];
    std::fill_n(error, sizeof(error), '\0');
    // Save stdout state
    int stdout_save = dup(STDOUT_FILENO);
    // Set stdout to buffer to error
    setvbuf(stdout, error, _IOFBF, 4096);

    Picoc* picoc = picoC();
    if (!PicocPlatformSetExitPoint(picoc))
    {
        static constexpr int NUM_ARGS = 1;
        PicocPlatformScanFile(picoc, file.c_str());
        char* args[NUM_ARGS];
        char version = (char)TitleLoader::save->version();
        args[0]      = &version;
        PicocCallMain(picoc, NUM_ARGS, args);
    }

    // Restore stdout state
    dup2(stdout_save, STDOUT_FILENO);

    if (picoc->PicocExitValue != 0)
    {
        std::string show = error;
        if (!show.empty())
        {
            Gui::warn(i18n::localize("SCRIPTS_EXECUTION_ERROR") + '\n' + file);
            show += "\nExit code: " + std::to_string(picoc->PicocExitValue);
            ScreenStack::push(std::make_unique<ScrollingTextScreen>(show, std::nullopt));
        }
    }

    if (Banks::bank->hasChanged())
    {
        Banks::bank->save();
    }
    TitleLoader::save->cryptBoxData(false);
    PicocCleanup(picoc);
    // And here we'll clean up
    aptSetHomeAllowed(true);
}
