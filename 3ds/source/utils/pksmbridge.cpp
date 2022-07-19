/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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

#include "Configuration.hpp"
#include "DateTime.hpp"
#include "MainMenu.hpp"
#include "Sav.hpp"
#include "TitleLoadScreen.hpp"
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "pksmbridge_api.h"
#include "pksmbridge_tcp.h"
#include "utils/crypto.hpp"
#include <arpa/inet.h>

namespace
{
    bool saveFromBridge = false;
    struct in_addr lastIPAddr;

    char* getHostId()
    {
        static sockaddr_in addr;
        addr.sin_addr.s_addr = gethostid();
        return inet_ntoa(addr.sin_addr);
    }

    bool verifyPKSMBridgeFileSHA256Checksum(struct pksmBridgeFile file)
    {
        uint32_t expectedSHA256ChecksumSize = 32;
        if (file.checksumSize != expectedSHA256ChecksumSize)
        {
            return false;
        }
        std::array<u8, 32> checksum = pksm::crypto::sha256(file.contents, file.size);
        int result                  = memcmp(checksum.data(), file.checksum, file.checksumSize);
        return (result == 0) ? true : false;
    }
}

bool isLoadedSaveFromBridge(void)
{
    return saveFromBridge;
}
void setLoadedSaveFromBridge(bool v)
{
    saveFromBridge = false;
}

bool receiveSaveFromBridge(void)
{
    if (!Gui::showChoiceMessage(i18n::localize("WIRELESS_WARNING") + '\n' +
                                fmt::format(i18n::localize("WIRELESS_IP"), getHostId())))
    {
        return false;
    };
    uint8_t* file = nullptr;
    uint32_t fileSize;
    enum pksmBridgeError error = receiveFileOverPKSMBridgeViaTCP(
        PKSM_PORT, &lastIPAddr, &file, &fileSize, &verifyPKSMBridgeFileSHA256Checksum);

    switch (error)
    {
        case PKSM_BRIDGE_ERROR_NONE:
            break;
        case PKSM_BRIDGE_ERROR_UNSUPPORTED_PROTCOL_VERSION:
            Gui::error(i18n::localize("BRIDGE_ERROR_UNSUPPORTED_PROTOCOL_VERISON"), errno);
            return false;
        case PKSM_BRIDGE_ERROR_CONNECTION_ERROR:
            Gui::error(i18n::localize("SOCKET_CONNECTION_FAIL"), errno);
            return false;
        case PKSM_BRIDGE_DATA_READ_FAILURE:
            Gui::error(i18n::localize("DATA_RECEIVE_FAIL"), errno);
            return false;
        case PKSM_BRIDGE_DATA_WRITE_FAILURE:
            Gui::error(i18n::localize("DATA_SEND_FAIL"), errno);
            return false;
        case PKSM_BRIDGE_DATA_FILE_CORRUPTED:
            Gui::error(i18n::localize("BRIDGE_ERROR_FILE_DATA_CORRUPTED"), errno);
            return false;
        case PKSM_BRIDGE_ERROR_UNEXPECTED_MESSAGE:
            Gui::error(i18n::localize("BRIDGE_ERROR_UNEXPECTED_MESSAGE"), errno);
            return false;
        default:
            Gui::error(i18n::localize("BRIDGE_ERROR_UNHANDLED"), errno);
            return false;
    }

    std::shared_ptr<u8[]> data = std::shared_ptr<u8[]>(file, free);
    if (!TitleLoader::load(data, fileSize))
    {
        Gui::error(i18n::localize("BRIDGE_ERROR_FILE_DATA_CORRUPTED"), 2);
        return false;
    }

    saveFromBridge = true;
    Gui::setScreen(std::make_unique<MainMenu>());
    return true;
}

bool sendSaveToBridge(void)
{
    struct pksmBridgeFile file;
    file.size                   = TitleLoader::save->getLength();
    file.contents               = TitleLoader::save->rawData().get();
    std::array<u8, 32> checksum = pksm::crypto::sha256(file.contents, file.size);
    file.checksum               = checksum.data();
    file.checksumSize           = checksum.size();
    enum pksmBridgeError error  = sendFileOverPKSMBridgeViaTCP(PKSM_PORT, lastIPAddr, file);
    switch (error)
    {
        case PKSM_BRIDGE_ERROR_NONE:
            return true;
        case PKSM_BRIDGE_ERROR_UNSUPPORTED_PROTCOL_VERSION:
            Gui::error(i18n::localize("BRIDGE_ERROR_UNSUPPORTED_PROTOCOL_VERISON"), errno);
            return false;
        case PKSM_BRIDGE_ERROR_CONNECTION_ERROR:
            Gui::error(i18n::localize("SOCKET_CONNECTION_FAIL"), errno);
            return false;
        case PKSM_BRIDGE_DATA_READ_FAILURE:
            Gui::error(i18n::localize("DATA_RECEIVE_FAIL"), errno);
            return false;
        case PKSM_BRIDGE_DATA_WRITE_FAILURE:
            Gui::error(i18n::localize("DATA_SEND_FAIL"), errno);
            return false;
        case PKSM_BRIDGE_DATA_FILE_CORRUPTED:
            Gui::error(i18n::localize("BRIDGE_ERROR_FILE_DATA_CORRUPTED"), errno);
            return false;
        case PKSM_BRIDGE_ERROR_UNEXPECTED_MESSAGE:
            Gui::error(i18n::localize("BRIDGE_ERROR_UNEXPECTED_MESSAGE"), errno);
            return false;
        default:
            Gui::error(i18n::localize("BRIDGE_ERROR_UNHANDLED"), errno);
            return false;
    }
}

void backupBridgeChanges()
{
    DateTime now = DateTime::now();
    std::string path =
        fmt::format(FMT_STRING("/3ds/PKSM/backups/bridge/{0:d}-{1:d}-{2:d}_{3:d}-{4:d}-{5:d}.bak"),
            now.year(), now.month(), now.day(), now.hour(), now.minute(), now.second());
    FILE* out = fopen(path.c_str(), "wb");
    if (out)
    {
        fwrite(TitleLoader::save->rawData().get(), 1, TitleLoader::save->getLength(), out);
        fclose(out);
    }
}
