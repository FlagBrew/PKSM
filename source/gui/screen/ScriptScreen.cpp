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
            default:
                return "";
        }
    }

    Picoc* picoC()
    {
        static Picoc* picoc = new Picoc;
        PicocInitialise(picoc, PICOC_STACKSIZE);
        return picoc;
    }
}

ScriptScreen::ScriptScreen() : currDirString("romfs:" + getScriptDir(TitleLoader::save->version())),
                               currDir(currDirString), hid(8, 1), sdSearch(false)
{
    updateEntries();
}

void ScriptScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::backgroundTop(true);

    // Leaving space for the icon
    Gui::dynamicText(currDirString, 30, 2, FONT_SIZE_11, FONT_SIZE_11, COLOR_YELLOW, false);
    Gui::staticText(GFX_TOP, 224, "Press \uE000 to execute script or enter directory", FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);

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
            // Leaving space for the icon
            Gui::dynamicText(currFiles[i].first, 30, 24 + (i % hid.maxVisibleEntries() * 25), FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE, false);
        }
    }

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(true);
    C2D_DrawRectSolid(20, 40, 0.5f, 280, 60, C2D_Color32(128, 128, 128, 255));
    C2D_DrawRectSolid(21, 41, 0.5f, 278, 58, COLOR_MASKBLACK);
    Gui::staticText(GFX_BOTTOM, 224, "Press \uE002 to switch between built-in scripts and ones on the SD card", FONT_SIZE_9, FONT_SIZE_9, COLOR_WHITE);

    Gui::dynamicText(currFiles[hid.fullIndex()].first, 30, 44, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
}

void ScriptScreen::update(touchPosition* touch)
{
    hid.update(currFiles.size());
    u32 down = hidKeysDown();
    if (down & KEY_B)
    {
        if (currDirString == (sdSearch ? "/3ds/PKSM" : "romfs:") + getScriptDir(TitleLoader::save->version()))
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
            if (Gui::showChoiceMessage("Do you want to use the following script?", '\'' + currFiles[hid.fullIndex()].first + '\''))
            {
                applyScript();
            }
        }
    }
    else if (down & KEY_X)
    {
        sdSearch = !sdSearch;
        currDirString = (sdSearch ? "/3ds/PKSM" : "romfs:") + getScriptDir(TitleLoader::save->version());
        currDir = STDirectory(currDirString);
        updateEntries();
    }
}

void ScriptScreen::updateEntries()
{
    hid.select(0);
    currFiles.clear();
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
    if (path.find("romfs") == 0)
    {
        std::ifstream in(path, std::ios::binary);
        if (in.good())
        {
            in.seekg(0, std::ios::end);
            size = in.tellg();
            in.seekg(0, std::ios::beg);
            data = new u8[size];
            in.read((char*)data, size);
        }
        else
        {
            Gui::warn("Could not open script file!");
        }
        in.close();
    }
    else
    {
        FSStream in(Archive::sd(), StringUtils::UTF8toUTF16(path), FS_OPEN_READ);
        if (in.good())
        {
            size = in.size();
            u8* data = new u8[size];
            in.read(data, size);
            in.close();
        }
        else
        {
            Gui::warn("Could not open script file!");
            in.close();
        }
    }

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

    for (size_t i = 0; i < MAGIC.size(); i++)
    {
        if (scriptData.first[i] != MAGIC[i])
        {
            Gui::warn("Not a valid script!");
            return;
        }
    }

    size_t index = MAGIC.size();
    while (index < scriptData.second)
    {
        u32 offset = *(u32*)(scriptData.first + index);
        u32 length = *(u32*)(scriptData.first + index + 4);
        u32 repeat = *(u32*)(scriptData.first + index + 8 + length);

        if (TitleLoader::save->generation() == 4)
        {
            u32 sbo = 0;
            u32 gbo = 0;
            switch (TitleLoader::save->version())
            {
                case 7:
                case 8:
                    sbo = ((SavHGSS*)TitleLoader::save.get())->sbo;
                    gbo = ((SavHGSS*)TitleLoader::save.get())->gbo;
                    break;
                case 10:
                case 11:
                    sbo = ((SavDP*)TitleLoader::save.get())->sbo;
                    gbo = ((SavDP*)TitleLoader::save.get())->gbo;
                    break;
                case 12:
                    sbo = ((SavPT*)TitleLoader::save.get())->sbo;
                    gbo = ((SavPT*)TitleLoader::save.get())->gbo;
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
    PicocCallMain(picoc, 0, NULL);
    PicocCleanup(picoc);
}