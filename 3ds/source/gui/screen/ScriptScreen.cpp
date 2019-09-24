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

#include "ScriptScreen.hpp"
#include "Configuration.hpp"
#include "Directory.hpp"
#include "FSStream.hpp"
#include "ScrollingTextScreen.hpp"
#include "archive.hpp"
#include "banks.hpp"
#include "loader.hpp"
#include "picoc.h"
#undef min // Get rid of picoc's min function

static constexpr std::string_view MAGIC = "PKSMSCRIPT";

namespace
{
    std::string getScriptDir(int version)
    {
        switch (version)
        {
            case 7:
            case 8:
                return "/scripts/hgss";
            case 10:
            case 11:
                return "/scripts/dp";
            case 12:
                return "/scripts/pt";
            case 20:
            case 21:
                return "/scripts/bw";
            case 22:
            case 23:
                return "/scripts/b2w2";
            case 24:
            case 25:
                return "/scripts/xy";
            case 26:
            case 27:
                return "/scripts/oras";
            case 30:
            case 31:
                return "/scripts/sm";
            case 32:
            case 33:
                return "/scripts/usum";
            case 42:
            case 43:
                return "/scripts/lgpe";
            default:
                return "/scripts/" + std::to_string(version);
        }
    }

    Picoc* picoC()
    {
        static Picoc picoc;
        PicocInitialise(&picoc, PICOC_STACKSIZE);
        return &picoc;
    }
}

ScriptScreen::ScriptScreen()
    : currDirString("romfs:" + getScriptDir(TitleLoader::save->version())), currDir(currDirString), hid(8, 1), sdSearch(false), cScripts(false)
{
    if (!currDir.good())
    {
        std::string tmp = "/3ds/PKSM" + getScriptDir(TitleLoader::save->version());
        currDir         = STDirectory(tmp);
        if (!currDir.good())
        {
            currDir = STDirectory(currDirString);
        }
        else
        {
            currDirString = tmp;
        }
    }
    updateEntries();
}

void ScriptScreen::drawTop() const
{
    // slight change to stripy top
    Gui::drawSolidRect(0, 0, 400, 240, PKSM_Color(26, 35, 126, 255));
    for (int x = -240; x < 400; x += 7)
    {
        Gui::drawLine(x, 0, x + 240, 240, 2, COLOR_LINEBLUE);
    }
    Gui::drawSolidRect(0, 0, 400, 25, PKSM_Color(15, 22, 89, 255));

    // Leaving space for the icon
    Gui::text(currDirString, 15, 2, FONT_SIZE_11, COLOR_YELLOW, TextPosX::LEFT, TextPosY::TOP);
    Gui::text(i18n::localize("SCRIPTS_INST1"), 200, 224, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    Gui::drawSolidRect(0, 20 + hid.index() * 25, 400, 25, PKSM_Color(128, 128, 128, 255));
    Gui::drawSolidRect(1, 21 + hid.index() * 25, 398, 23, COLOR_MASKBLACK);

    for (size_t i = hid.page() * hid.maxVisibleEntries(); i < (hid.page() + 1) * hid.maxVisibleEntries(); i++)
    {
        if (i >= currFiles.size())
        {
            break;
        }
        else
        {
            Gui::sprite(currFiles[i].second ? ui_sheet_icon_folder_idx : ui_sheet_icon_script_idx, 3, 23 + i % hid.maxVisibleEntries() * 25);
            Gui::text(currFiles[i].first, 30, 24 + (i % hid.maxVisibleEntries() * 25), FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP);
        }
    }
}

void ScriptScreen::drawBottom() const
{
    Gui::backgroundBottom(true);
    Gui::drawSolidRect(20, 40, 280, 60, PKSM_Color(128, 128, 128, 255));
    Gui::drawSolidRect(21, 41, 278, 58, COLOR_MASKBLACK);
    Gui::text(i18n::localize("SCRIPTS_INST2"), 160, 224, FONT_SIZE_9, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    Gui::text(currFiles[hid.fullIndex()].first, 30, 44, FONT_SIZE_11, COLOR_WHITE, TextPosX::LEFT, TextPosY::TOP, 260.0f);
}

void ScriptScreen::update(touchPosition* touch)
{
    hid.update(currFiles.size());
    u32 down = hidKeysDown();
    if (down & KEY_B)
    {
        if (currDirString ==
            (sdSearch ? "/3ds/PKSM" : "romfs:") + (cScripts ? std::string("/scripts/universal") : getScriptDir(TitleLoader::save->version())))
        {
            Gui::screenBack();
            return;
        }
        else
        {
            currDirString = currDirString.substr(0, currDirString.find_last_of('/'));
            currDir       = STDirectory(currDirString);
            updateEntries();
        }
    }
    else if (down & KEY_A)
    {
        if (currDir.good() && currDir.count() > 0)
        {
            if (currFiles[hid.fullIndex()].second)
            {
                currDirString += '/' + currFiles[hid.fullIndex()].first;
                currDir = STDirectory(currDirString);
                updateEntries();
            }
            else
            {
                if (Gui::showChoiceMessage(i18n::localize("SCRIPTS_CONFIRM_USE") + "\n" + ('\'' + currFiles[hid.fullIndex()].first + '\'')))
                {
                    applyScript();
                }
            }
        }
    }
    else if (down & KEY_X)
    {
        std::string dirString =
            (!sdSearch ? "/3ds/PKSM" : "romfs:") + (cScripts ? std::string("/scripts/universal") : getScriptDir(TitleLoader::save->version()));
        STDirectory dir = STDirectory(dirString);
        if (dir.good())
        {
            sdSearch      = !sdSearch;
            currDirString = dirString;
            currDir       = dir;
            updateEntries();
        }
        else
        {
            Gui::warn(("\"" + dirString + "\"") + '\n' + i18n::localize("SCRIPTS_NOT_FOUND"));
        }
    }
    else if (down & KEY_Y)
    {
        std::string dirString =
            (sdSearch ? "/3ds/PKSM" : "romfs:") + (!cScripts ? std::string("/scripts/universal") : getScriptDir(TitleLoader::save->version()));
        STDirectory dir = STDirectory(dirString);
        if (dir.good())
        {
            cScripts      = !cScripts;
            currDirString = dirString;
            currDir       = dir;
            updateEntries();
        }
        else
        {
            Gui::warn(("\"" + dirString + "\"") + '\n' + i18n::localize("SCRIPTS_NOT_FOUND"));
        }
    }
}

void ScriptScreen::updateEntries()
{
    hid.select(0);
    currFiles.clear();
    if (!currDir.good())
    {
        currFiles.push_back({i18n::localize("FOLDER_DOESNT_EXIST"), false});
        return;
    }
    if (currDir.count() == 0)
    {
        currFiles.push_back({i18n::localize("EMPTY"), false});
        return;
    }
    for (size_t i = 0; i < currDir.count(); i++)
    {
        std::string item = currDir.item(i);
        if (item != "." && item != "..")
        {
            currFiles.push_back(std::make_pair(item, currDir.folder(i)));
        }
    }
    std::sort(currFiles.begin(), currFiles.end(), [this](std::pair<std::string, bool>& first, std::pair<std::string, bool>& second) {
        if ((first.second && second.second) || (!first.second && !second.second))
        {
            return first.first < second.first;
        }
        else if (first.second)
        {
            return true;
        }
        else
        {
            return false;
        }
    });
}

static std::vector<u8> scriptRead(const std::string& path)
{
    std::vector<u8> ret;
    size_t size = 0;
    FILE* in    = fopen(path.c_str(), "rb");
    fseek(in, 0, SEEK_END);
    if (!ferror(in))
    {
        size = ftell(in);
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

void ScriptScreen::applyScript()
{
    std::string scriptFile = currDirString + '/' + currFiles[hid.fullIndex()].first;
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
        u32 offset = *(u32*)(scriptData.data() + index);
        u32 length = *(u32*)(scriptData.data() + index + 4);
        u32 repeat = *(u32*)(scriptData.data() + index + 8 + length);

        if (TitleLoader::save->generation() == Generation::FOUR)
        {
            u32 sbo = ((Sav4*)TitleLoader::save.get())->getSBO();
            u32 gbo = ((Sav4*)TitleLoader::save.get())->getGBO();
            if (TitleLoader::save->boxOffset(0, 0) - sbo <= offset && TitleLoader::save->boxOffset(TitleLoader::save->boxes, 0) - sbo >= offset)
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
            std::copy(scriptData.data() + index + 8, scriptData.data() + index + 8 + length, TitleLoader::save->rawData() + offset + i * length);
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
    std::fill_n(error, 4096, '\0');
    // Save stdout state
    int stdout_save = dup(STDOUT_FILENO);
    // Set stdout to buffer to error
    setvbuf(stdout, error, _IOFBF, 4096);

    Picoc* picoc = picoC();
    if (!PicocPlatformSetExitPoint(picoc))
    {
        PicocPlatformScanFile(picoc, file.c_str());
        char* args[3];
        std::string data   = std::to_string((int)TitleLoader::save->rawData());
        args[0]            = data.data();
        std::string length = std::to_string(TitleLoader::save->getLength());
        args[1]            = length.data();
        char version       = TitleLoader::save->version();
        args[2]            = &version;
        PicocCallMain(picoc, 3, args);
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
            Gui::setScreen(std::make_unique<ScrollingTextScreen>(error, nullptr));
        }
    }

    if (Banks::bank->hasChanged())
    {
        Banks::bank->save();
    }
    PicocCleanup(picoc);
    // And here we'll clean up
    aptSetHomeAllowed(true);
}
