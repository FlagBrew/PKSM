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

#include "ScriptScreen.hpp"
#include "Directory.hpp"
#include "archive.hpp"
#include "loader.hpp"
#include "FSStream.hpp"
extern "C" {
    #include "picoc.h"
}
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

    // slight change to stripy top
    void menuTop()
    {
        for (int x = 0; x < 400; x += 7)
        {
            for (int y = 0; y < 240; y += 7)
            {
                Gui::sprite(ui_sheet_bg_stripe_top_idx, x, y);
            }
        }
        C2D_DrawRectSolid(0, 0, 0.5f, 400, 20, C2D_Color32(15, 22, 89, 255));
    }
}

ScriptScreen::ScriptScreen() : currDirString("romfs:" + getScriptDir(TitleLoader::save->version())),
                               currDir(currDirString), hid(8, 1), sdSearch(false), cScripts(false)
{
    if (!currDir.good())
    {
        std::string tmp = "/3ds/PKSM" + getScriptDir(TitleLoader::save->version());
        currDir = STDirectory(tmp);
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

void ScriptScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    menuTop();

    // Leaving space for the icon
    Gui::dynamicText(currDirString, 15, 2, FONT_SIZE_11, FONT_SIZE_11, COLOR_YELLOW, false);
    Gui::staticText(GFX_TOP, 224, i18n::localize("SCRIPTS_INST1"), FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);

    C2D_DrawRectSolid(0, 20 + hid.index() * 25, 0.5f, 400, 25, C2D_Color32(128, 128, 128, 255));
    C2D_DrawRectSolid(1, 21 + hid.index() * 25, 0.5f, 398, 23, COLOR_MASKBLACK);

    for (size_t i = hid.page() * hid.maxVisibleEntries(); i < (hid.page() + 1) * hid.maxVisibleEntries(); i++)
    {
        if (i >= currFiles.size())
        {
            break;
        }
        else
        {
            Gui::sprite(currFiles[i].second ? ui_sheet_icon_folder_idx : ui_sheet_icon_script_idx, 3, 23 + i % hid.maxVisibleEntries() * 25);
            Gui::dynamicText(currFiles[i].first, 30, 24 + (i % hid.maxVisibleEntries() * 25), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, false);
        }
    }

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(true);
    C2D_DrawRectSolid(20, 40, 0.5f, 280, 60, C2D_Color32(128, 128, 128, 255));
    C2D_DrawRectSolid(21, 41, 0.5f, 278, 58, COLOR_MASKBLACK);
    Gui::staticText(GFX_BOTTOM, 224, i18n::localize("SCRIPTS_INST2"), FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);

    Gui::dynamicText(currFiles[hid.fullIndex()].first, 30, 44, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, 260, false);
}

void ScriptScreen::update(touchPosition* touch)
{
    hid.update(currFiles.size());
    u32 down = hidKeysDown();
    if (down & KEY_B)
    {
        if (currDirString == (sdSearch ? "/3ds/PKSM" : "romfs:") + (cScripts ? std::string("/scripts/universal") : getScriptDir(TitleLoader::save->version())))
        {
            Gui::screenBack();
            return;
        }
        else
        {
            currDirString = currDirString.substr(0, currDirString.find_last_of('/'));
            currDir = STDirectory(currDirString);
            updateEntries();
        }
    }
    else if (down & KEY_A)
    {
        if (currFiles[hid.fullIndex()].second)
        {
            currDirString += '/' + currFiles[hid.fullIndex()].first;
            currDir = STDirectory(currDirString);
            updateEntries();
        }
        else
        {
            if (Gui::showChoiceMessage(i18n::localize("SCRIPTS_CONFIRM_USE"), '\'' + currFiles[hid.fullIndex()].first + '\''))
            {
                applyScript();
            }
        }
    }
    else if (down & KEY_X)
    {
        std::string dirString = (!sdSearch ? "/3ds/PKSM" : "romfs:") + (cScripts ? std::string("/scripts/universal") : getScriptDir(TitleLoader::save->version()));
        STDirectory dir = STDirectory(dirString);
        if (dir.good())
        {
            sdSearch = !sdSearch;
            currDirString = dirString;
            currDir = dir;
            updateEntries();
        }
        else
        {
            Gui::warn("\"" + dirString + "\"", i18n::localize("SCRIPTS_NOT_FOUND"));
        }
    }
    else if (down & KEY_Y)
    {
        std::string dirString = (sdSearch ? "/3ds/PKSM" : "romfs:") + (!cScripts ? std::string("/scripts/universal") : getScriptDir(TitleLoader::save->version()));
        STDirectory dir = STDirectory(dirString);
        if (dir.good())
        {
            cScripts = !cScripts;
            currDirString = dirString;
            currDir = dir;
            updateEntries();
        }
        else
        {
            Gui::warn("\"" + dirString + "\"", i18n::localize("SCRIPTS_NOT_FOUND"));
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
    for (size_t i = 0; i < currDir.count(); i++)
    {
        std::string item = currDir.item(i);
        if (item != "." && item != "..")
        {
            currFiles.push_back(std::make_pair(item, currDir.folder(i)));
        }
    }
    std::sort(currFiles.begin(), currFiles.end(), [this](std::pair<std::string, bool>& first, std::pair<std::string, bool>& second)
    {
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

static std::pair<u8*, size_t> scriptRead(std::string path)
{
    u8* data = nullptr;
    size_t size = 0;
    std::ifstream in(path, std::ios::binary | std::ios::ate);
    if (in.good())
    {
        size = in.tellg();
        in.seekg(0, std::ios::beg);
        data = new u8[size];
        in.read((char*)data, size);
    }
    else
    {
        Gui::error(i18n::localize("SCRIPTS_FAILED_OPEN"), errno);
    }
    in.close();

    return {data, size};
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

    if (!scriptData.first)
    {
        return;
    }

    for (size_t i = 0; i < std::min(MAGIC.size(), scriptData.second); i++)
    {
        if (scriptData.first[i] != MAGIC[i])
        {
            Gui::warn(i18n::localize("SCRIPTS_INVALID"));
            return;
        }
    }

    size_t index = MAGIC.size();
    while (index < scriptData.second)
    {
        u32 offset = *(u32*)(scriptData.first + index);
        u32 length = *(u32*)(scriptData.first + index + 4);
        u32 repeat = *(u32*)(scriptData.first + index + 8 + length);

        if (TitleLoader::save->generation() == Generation::FOUR)
        {
            u32 sbo = 0;
            u32 gbo = 0;
            switch (TitleLoader::save->version())
            {
                case 7:
                case 8:
                    sbo = ((SavHGSS*)TitleLoader::save.get())->getSBO();
                    gbo = ((SavHGSS*)TitleLoader::save.get())->getGBO();
                    break;
                case 10:
                case 11:
                    sbo = ((SavDP*)TitleLoader::save.get())->getSBO();
                    gbo = ((SavDP*)TitleLoader::save.get())->getGBO();
                    break;
                case 12:
                    sbo = ((SavPT*)TitleLoader::save.get())->getSBO();
                    gbo = ((SavPT*)TitleLoader::save.get())->getGBO();
                    break;
            }
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
            std::copy(scriptData.first + index + 8, scriptData.first + index + 8 + length, TitleLoader::save->data + offset + i * length);
        }

        index += 12 + length;
    }

    delete[] scriptData.first;
}

void ScriptScreen::parsePicoCScript(std::string& file)
{
    Picoc* picoc = picoC();
    PicocPlatformScanFile(picoc, file.c_str());
    char* args[3];
    std::string data = std::to_string((int)TitleLoader::save->data);
    args[0] = data.data();
    std::string length = std::to_string(TitleLoader::save->length);
    args[1] = length.data();
    char version = TitleLoader::save->version();
    args[2] = &version;
    PicocCallMain(picoc, 3, args);
    PicocCleanup(picoc);
}