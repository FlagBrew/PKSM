/*
 *   This file is part of PKSM
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

#include "Configuration.hpp"
#include "DateTime.hpp"
#include "MainMenu.hpp"
#include "Sav.hpp"
#include "TitleLoadScreen.hpp"
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include <arpa/inet.h>
#include <unistd.h>

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
    if (!Gui::showChoiceMessage(i18n::localize("WIRELESS_WARNING") + '\n' + fmt::format(i18n::localize("WIRELESS_IP"), getHostId())))
    {
        return false;
    }

    int fd;
    struct sockaddr_in servaddr;
    if ((fd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_CREATE_FAIL"), errno);
        return false;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(PKSM_PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_BIND_FAIL"), errno);
        close(fd);
        return false;
    }

    if (listen(fd, 5) < 0)
    {
        Gui::error(i18n::localize("SOCKET_LISTEN_FAIL"), errno);
        close(fd);
        return false;
    }

    int fdconn;
    int addrlen = sizeof(servaddr);
    if ((fdconn = accept(fd, (struct sockaddr*)&servaddr, (socklen_t*)&addrlen)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_ACCEPT_FAIL"), errno);
        close(fd);
        return false;
    }

    lastIPAddr = servaddr.sin_addr;

    size_t size                = 0x180B19;
    std::shared_ptr<u8[]> data = std::shared_ptr<u8[]>(new u8[size]);

    size_t total = 0;
    size_t chunk = 1024;
    int n;
    while (total < size)
    {
        size_t torecv = size - total > chunk ? chunk : size - total;
        n             = recv(fdconn, &data[total], torecv, 0);
        total += n;
        if (n <= 0)
        {
            break;
        }
        fmt::print(stderr, "Recv {:d} bytes, {:d} still missing\n", total, size - total);
    }

    close(fdconn);
    close(fd);

    if (n == 0 || total == size)
    {
        if (TitleLoader::load(data, total))
        {
            saveFromBridge = true;
            Gui::setScreen(std::make_unique<MainMenu>());
        }
    }
    else
    {
        Gui::error(i18n::localize("DATA_RECEIVE_FAIL"), errno);
    }

    return true;
}

bool sendSaveToBridge(void)
{
    bool result = false;
    // send via TCP
    int fd;
    struct sockaddr_in servaddr;
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_CREATE_FAIL"), errno);
        return result;
    }
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(PKSM_PORT);
    servaddr.sin_addr   = lastIPAddr;

    if (connect(fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
    {
        Gui::error(i18n::localize("SOCKET_CONNECTION_FAIL"), errno);
        close(fd);
        return result;
    }

    size_t size  = TitleLoader::save->getLength();
    size_t total = 0;
    size_t chunk = 1024;
    int n;
    while (total < size)
    {
        size_t tosend = size - total > chunk ? chunk : size - total;
        n             = send(fd, TitleLoader::save->rawData().get() + total, tosend, 0);
        if (n == -1)
        {
            break;
        }
        total += n;
        fmt::print(stderr, "Recv {:d} bytes, {:d} still missing\n", total, size - total);
    }
    if (total == size)
    {
        // Gui::createInfo("Success!", "Data sent back correctly.");
        result = true;
    }
    else
    {
        Gui::error(i18n::localize("DATA_SEND_FAIL"), errno);
    }

    close(fd);
    saveFromBridge = false;
    return result;
}

void backupBridgeChanges()
{
    DateTime now     = DateTime::now();
    std::string path = fmt::format(FMT_STRING("/3ds/PKSM/backups/bridge/{0:d}-{1:d}-{2:d}_{3:d}-{4:d}-{5:d}.bak"), now.year(), now.month(), now.day(),
        now.hour(), now.minute(), now.second());
    FILE* out        = fopen(path.c_str(), "wb");
    if (out)
    {
        fwrite(TitleLoader::save->rawData().get(), 1, TitleLoader::save->getLength(), out);
        fclose(out);
    }
}
