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

#include "GpssScreen.hpp"
#include "banks.hpp"
#include "Button.hpp"
#include "CloudViewOverlay.hpp"
#include "Configuration.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "utils/DateTime.hpp"
#include "utils/format.hpp"
#include <cstdlib>
#include <format>
#include <sys/stat.h>

GpssScreen::GpssScreen(
    const std::string& instructions, std::shared_ptr<pksm::PKFilter> filter, int storageBox)
    : Screen(instructions),
      filter(filter ? filter : std::make_shared<pksm::PKFilter>()),
      storageBox(storageBox),
      websiteURL(Configuration::getInstance().apiUrl())
{
}

bool GpssScreen::leftOnPageError(std::optional<int> error)
{
    if (!error)
    {
        return false;
    }

    if (*error != 0)
    {
        Gui::warn(pksm::format(i18n::localize("GPSS_COMMUNICATION_ERROR"), *error));
    }
    else
    {
        Gui::warn(i18n::localize("OFFLINE_ERROR"));
    }
    Gui::screenBack();
    return true;
}

void GpssScreen::reportUploadStatus(long status)
{
    switch (status)
    {
        case 200:
        case 201:
            break;
        case 400:
            Gui::error(i18n::localize("SHARE_FAILED_CHECK"), status);
            break;
        case 401:
            Gui::warn(i18n::localize("GPSS_BANNED"));
            break;
        case 502:
            Gui::error(i18n::localize("HTTP_OFFLINE"), status);
            break;
        case 429:
        case 503:
            Gui::warn(i18n::localize("GPSS_TEMP_DISABLED") + '\n' + i18n::localize("PLEASE_WAIT"));
            break;
        default:
            Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status);
            break;
    }
}

bool GpssScreen::showViewer()
{
    if (cursorIndex == 0)
    {
        return false;
    }

    if (infoMon && infoMon->species() != pksm::Species::None)
    {
        justSwitched = true;
        addOverlay<CloudViewOverlay>(infoMon);
    }
    return true;
}

bool GpssScreen::dumpPkm()
{
    if (!cloudChosen && cursorIndex != 0)
    {
        auto dumpMon = Banks::bank->pkm(storageBox, cursorIndex - 1);
        if (dumpMon && dumpMon->species() != pksm::Species::None &&
            Gui::showChoiceMessage(i18n::localize("BANK_CONFIRM_DUMP")))
        {
            DateTime now     = DateTime::now();
            std::string path = std::format(
                "/3ds/PKSM/dumps/{0:d}-{1:d}-{2:d}", now.year(), now.month(), now.day());
            mkdir(path.c_str(), 777);
            path += std::format("/{0:d}-{1:d}-{2:d}", now.hour(), now.minute(), now.second());
            path += " - " + std::to_string(int(dumpMon->species())) + " - " + dumpMon->nickname() +
                    " - " + std::format("{:08X}", dumpMon->PID()) + dumpMon->extension().data();

            FILE* out = fopen(path.c_str(), "wb");
            if (out)
            {
                fwrite(dumpMon->rawData().data(), 1, dumpMon->getLength(), out);
                fclose(out);
            }
            else
            {
                Gui::error(i18n::localize("FAILED_OPEN_DUMP"), errno);
            }
            return true;
        }
    }
    return false;
}

bool GpssScreen::prevBox(bool forceBottom)
{
    if (cloudChosen && !forceBottom)
    {
        return prevBoxTop();
    }

    storageBox--;
    if (storageBox == -1)
    {
        storageBox = Banks::bank->boxes() - 1;
    }
    return false;
}

bool GpssScreen::nextBox(bool forceBottom)
{
    if (cloudChosen && !forceBottom)
    {
        return nextBoxTop();
    }

    storageBox++;
    if (storageBox == Banks::bank->boxes())
    {
        storageBox = 0;
    }
    return false;
}

bool GpssScreen::prevBoxTop()
{
    return leftOnPageError(paging().prevPage());
}

bool GpssScreen::nextBoxTop()
{
    return leftOnPageError(paging().nextPage());
}

bool GpssScreen::jumpBoxTopBy(int delta)
{
    return leftOnPageError(paging().jumpPage(paging().page() + delta));
}

bool GpssScreen::jumpBoxTop()
{
    if (paging().pages() <= 1)
    {
        cloudChosen = true;
        return false;
    }

    SwkbdState state;
    const std::string hint = i18n::localize("BOX_JUMP") + " 1-" + std::to_string(paging().pages());
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, std::to_string(paging().pages()).size());
    swkbdSetFeatures(&state, SWKBD_FIXED_WIDTH);
    swkbdSetHintText(&state, hint.c_str());
    swkbdSetValidation(&state, SWKBD_NOTEMPTY_NOTBLANK, 0, 0);

    char input[12]  = {0};
    SwkbdButton ret = swkbdInputText(&state, input, sizeof(input));
    if (ret != SWKBD_BUTTON_CONFIRM)
    {
        return false;
    }

    char* end = nullptr;
    long page = std::strtol(input, &end, 10);
    if (end == input || *end != '\0' || page <= 0)
    {
        return false;
    }

    if (leftOnPageError(paging().jumpPage((int)page)))
    {
        return true;
    }

    cloudChosen = true;
    return false;
}

bool GpssScreen::clickBottomIndex(int index)
{
    if (cursorIndex == index && !cloudChosen && cursorIndex != 0)
    {
        pickup();
    }
    else
    {
        cursorIndex = index;
        cloudChosen = false;
    }
    return false;
}
