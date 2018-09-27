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

static const char* MAGIC = "PKSMSCRIPT";

namespace
{
    std::string getScriptDir(int version)
    {
        switch (version)
        {
            case 7:
            case 8:
                return "/3ds/PKSM/scripts/hgss";
            case 10:
            case 11:
                return "/3ds/PKSM/scripts/dp";
            case 12:
                return "/3ds/PKSM/scripts/pt";
            case 20:
            case 21:
                return "/3ds/PKSM/scripts/bw";
            case 22:
            case 23:
                return "/3ds/PKSM/scripts/b2w2";
            case 24:
            case 25:
                return "/3ds/PKSM/scripts/xy";
            case 26:
            case 27:
                return "/3ds/PKSM/scripts/oras";
            case 30:
            case 31:
                return "/3ds/PKSM/scripts/sm";
            case 32:
            case 33:
                return "/3ds/PKSM/scripts/usum";
            default:
                return "";
        }
    }
}

ScriptScreen::ScriptScreen() : currDirString(getScriptDir(TitleLoader::save->version())), origDirLength(currDirString.size()),
                               currDir(Archive::sd(), StringUtils::UTF8toUTF16(currDirString)), hid(8, 1)
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

    Gui::dynamicText(currFiles[hid.fullIndex()].first, 30, 44, FONT_SIZE_11, FONT_SIZE_11, COLOR_WHITE);
}

void ScriptScreen::update(touchPosition* touch)
{
    hid.update(currFiles.size());
    u32 down = hidKeysDown();
    if (down & KEY_B)
    {
        if (currDirString.size() == origDirLength)
        {
            Gui::screenBack();
            return;
        }
        else
        {
            currDirString = currDirString.substr(0, currDirString.find_last_of('/'));
            currDir = Directory(Archive::sd(), StringUtils::UTF8toUTF16(currDirString));
            updateEntries();
        }
    }
    else if (down & KEY_A)
    {
        if (currFiles[hid.fullIndex()].second)
        {
            currDirString += '/' + currFiles[hid.fullIndex()].first;
            currDir = Directory(Archive::sd(), StringUtils::UTF8toUTF16(currDirString));
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
}

void ScriptScreen::updateEntries()
{
    hid.select(0);
    currFiles.clear();
    for (size_t i = 0; i < currDir.count(); i++)
    {
        currFiles.push_back(std::make_pair(StringUtils::UTF16toUTF8(currDir.item(i)), currDir.folder(i)));
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

void ScriptScreen::applyScript()
{
    FSStream in(Archive::sd(), StringUtils::UTF8toUTF16(currDirString + '/' + currFiles[hid.fullIndex()].first), FS_OPEN_READ);
    if (in.good())
    {
        size_t size = in.size();
        u8* data = new u8[size];
        in.read(data, size);
        in.close();

        for (size_t i = 0; i < strlen(MAGIC); i++)
        {
            if (data[i] != MAGIC[i])
            {
                Gui::warn("Not a valid script!");
                return;
            }
        }

        size_t index = strlen(MAGIC);
        while (index < size)
        {
            u32 offset = *(u32*)(data + index);
            u32 length = *(u32*)(data + index + 4);
            u32 repeat = *(u32*)(data + index + 8 + length);

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
                std::copy(data + index + 8, data + index + 8 + length, TitleLoader::save->data + offset + i * length);
            }

            index += 12 + length;
        }

        delete[] data;
    }
    else
    {
        Gui::warn("Could not open script file!");
    }
}