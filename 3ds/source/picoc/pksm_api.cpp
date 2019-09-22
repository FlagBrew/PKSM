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

#include "BankChoice.hpp"
#include "BoxChoice.hpp"
#include "FortyChoice.hpp"
#include "PB7.hpp"
#include "PK4.hpp"
#include "PK5.hpp"
#include "PK6.hpp"
#include "PK7.hpp"
#include "STDirectory.hpp"
#include "ThirtyChoice.hpp"
#include "banks.hpp"
#include "gui.hpp"
#include "loader.hpp"
#include "utils.hpp"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>

static void* strToRet(const std::string& str)
{
    char* ret = (char*)malloc(str.size() + 1);
    std::copy(str.begin(), str.end(), ret);
    ret[str.size()] = '\0';
    return (void*)ret;
}

static void* strToRet(const std::u16string& str)
{
    u16* ret = (u16*)malloc((str.size() + 1) * 2);
    std::copy(str.begin(), str.end(), ret);
    ret[str.size()] = u'\0';
    return (void*)ret;
}

extern "C" {
#include "pksm_api.h"

static void checkGen(struct ParseState* Parser, Generation gen)
{
    switch (gen)
    {
        case Generation::FOUR:
        case Generation::FIVE:
        case Generation::SIX:
        case Generation::SEVEN:
        case Generation::LGPE:
            break;
        default:
            ProgramFail(Parser, "Generation is not possible!");
            break;
    }
}

static struct Value* getNextVarArg(struct Value* arg)
{
    return (struct Value*)((char*)arg + MEM_ALIGN(sizeof(struct Value) + TypeStackSizeValue(arg)));
}

void gui_warn(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    Gui::warn((char*)Param[0]->Val->Pointer);
}

void gui_choice(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Integer = (int)Gui::showChoiceMessage((char*)Param[0]->Val->Pointer);
}

void gui_menu6x5(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* question            = (char*)Param[0]->Val->Pointer;
    int options               = Param[1]->Val->Integer;
    char** labels             = (char**)Param[2]->Val->Pointer;
    pkm* pokemon              = (pkm*)Param[3]->Val->Pointer;
    Generation gen            = Generation(Param[4]->Val->Integer);
    ThirtyChoice screen       = ThirtyChoice(question, labels, pokemon, options, gen);
    ReturnValue->Val->Integer = screen.run();
}

void gui_menu20x2(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* question            = (char*)Param[0]->Val->Pointer;
    int options               = Param[1]->Val->Integer;
    char** labels             = (char**)Param[2]->Val->Pointer;
    FortyChoice screen        = FortyChoice(question, labels, options);
    ReturnValue->Val->Integer = screen.run();
}

void sav_sbo(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    if (TitleLoader::save->generation() == Generation::FOUR)
    {
        ReturnValue->Val->Integer = ((Sav4*)TitleLoader::save.get())->getSBO();
    }
    else
    {
        ReturnValue->Val->Integer = 0;
    }
}

void sav_gbo(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    if (TitleLoader::save->generation() == Generation::FOUR)
    {
        ReturnValue->Val->Integer = ((Sav4*)TitleLoader::save.get())->getGBO();
    }
    else
    {
        ReturnValue->Val->Integer = 0;
    }
}

void sav_boxDecrypt(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    TitleLoader::save->cryptBoxData(true);
}

void sav_boxEncrypt(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    TitleLoader::save->cryptBoxData(false);
}

void gui_keyboard(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* out    = (char*)Param[0]->Val->Pointer;
    char* hint   = (char*)Param[1]->Val->Pointer;
    int numChars = Param[2]->Val->Integer;

    C3D_FrameEnd(0);

    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 1, numChars);
    swkbdSetHintText(&state, hint);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    swkbdInputText(&state, out, numChars * 3); // numChars is UTF-16 codepoints, each UTF-8 codepoint needs up to 3 bytes, so
    out[numChars * 3 - 1] = '\0';
}

void gui_numpad(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    unsigned int* out = (unsigned int*)Param[0]->Val->Pointer;
    std::string hint  = (char*)Param[1]->Val->Pointer;
    int numChars      = Param[2]->Val->Integer;

    char number[numChars + 1] = {0};

    C3D_FrameEnd(0);

    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NUMPAD, 2, numChars);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
    swkbdSetButton(&state, SWKBD_BUTTON_LEFT, "What?", false);
    SwkbdButton button;
    do
    {
        button = swkbdInputText(&state, number, sizeof(number));
        if (button != SWKBD_BUTTON_CONFIRM)
        {
            Gui::warn(hint);
            C3D_FrameEnd(0); // Just make sure
        }
    } while (button != SWKBD_BUTTON_CONFIRM);
    number[numChars] = '\0';
    *out             = std::atoi(number);
}

void current_directory(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    std::string fileName      = Parser->FileName;
    fileName                  = fileName.substr(0, fileName.rfind('/'));
    ReturnValue->Val->Pointer = strToRet(fileName);
}

void read_directory(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    std::string dir = (char*)Param[0]->Val->Pointer;
    STDirectory directory(dir);
    struct dirData
    {
        int amount;
        char** data;
    };
    dirData* ret = (dirData*)malloc(sizeof(dirData));
    if (directory.good())
    {
        ret->amount = directory.count();
        ret->data   = (char**)malloc(sizeof(char*) * directory.count());
        for (size_t i = 0; i < directory.count(); i++)
        {
            std::string item          = dir + "/" + directory.item(i);
            ret->data[i]              = (char*)malloc(sizeof(char) * (item.size() + 1));
            ret->data[i][item.size()] = '\0';
            strcpy(ret->data[i], item.data());
        }
    }
    else
    {
        ret->amount = 0;
        ret->data   = nullptr;
    }
    ReturnValue->Val->Pointer = ret;
}

void delete_directory(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    struct dirData
    {
        int amount;
        char** data;
    };
    dirData* dir = (dirData*)Param[0]->Val->Pointer;
    if (dir)
    {
        for (int i = 0; i < dir->amount; i++)
        {
            free(dir->data[i]);
        }
        free(dir->data);
        free(dir);
    }
}

void sav_inject_pkx(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data          = (u8*)Param[0]->Val->Pointer;
    Generation gen    = Generation(Param[1]->Val->Integer);
    int box           = Param[2]->Val->Integer;
    int slot          = Param[3]->Val->Integer;
    bool doTradeEdits = Param[4]->Val->Integer;
    checkGen(Parser, gen);

    std::shared_ptr<PKX> pkm = nullptr;

    switch (gen)
    {
        case Generation::FOUR:
            pkm = std::make_shared<PK4>(data, false);
            break;
        case Generation::FIVE:
            pkm = std::make_shared<PK5>(data, false);
            break;
        case Generation::SIX:
            pkm = std::make_shared<PK6>(data, false);
            break;
        case Generation::SEVEN:
            pkm = std::make_shared<PK7>(data, false);
            break;
        case Generation::LGPE:
        default:
            pkm = std::make_shared<PB7>(data, false);
            break;
    }

    if (pkm)
    {
        if (TitleLoader::save->generation() == Generation::LGPE)
        {
            if (pkm->generation() == Generation::LGPE)
            {
                TitleLoader::save->pkm(pkm, box, slot, doTradeEdits);
                TitleLoader::save->dex(pkm);
            }
        }
        else
        {
            TitleLoader::save->transfer(pkm);
            bool moveBad = false;
            for (int i = 0; i < 4; i++)
            {
                if (pkm->move(i) > TitleLoader::save->maxMove())
                {
                    moveBad = true;
                    break;
                }
                if (pkm->relearnMove(i) > TitleLoader::save->maxMove())
                {
                    moveBad = true;
                    break;
                }
            }
            if (pkm->species() > TitleLoader::save->maxSpecies())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_SPECIES"));
                return;
            }
            else if (pkm->alternativeForm() > TitleLoader::save->formCount(pkm->species()))
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_FORM"));
                return;
            }
            else if (pkm->ability() > TitleLoader::save->maxAbility())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_ABILITY"));
                return;
            }
            else if (pkm->heldItem() > TitleLoader::save->maxItem())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_ITEM"));
                return;
            }
            else if (pkm->ball() > TitleLoader::save->maxBall())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_BALL"));
                return;
            }
            else if (moveBad)
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_MOVE"));
                return;
            }
            TitleLoader::save->pkm(pkm, box, slot, doTradeEdits);
            TitleLoader::save->dex(pkm);
        }
    }
}

void cfg_default_ot(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strToRet(Configuration::getInstance().defaultOT());
}

void cfg_default_tid(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->UnsignedShortInteger = Configuration::getInstance().defaultTID();
}

void cfg_default_sid(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->UnsignedShortInteger = Configuration::getInstance().defaultSID();
}

void cfg_default_day(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    int ret = Configuration::getInstance().day();
    if (ret == 0)
    {
        const time_t current      = time(NULL);
        ReturnValue->Val->Integer = gmtime(&current)->tm_mday;
    }
    else
    {
        ReturnValue->Val->Integer = ret;
    }
}

void cfg_default_month(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    int ret = Configuration::getInstance().month();
    if (ret == 0)
    {
        const time_t current      = time(NULL);
        ReturnValue->Val->Integer = gmtime(&current)->tm_mon;
    }
    else
    {
        ReturnValue->Val->Integer = ret;
    }
}

void cfg_default_year(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    int ret = Configuration::getInstance().year();
    if (ret == 0)
    {
        const time_t current      = time(NULL);
        ReturnValue->Val->Integer = gmtime(&current)->tm_year;
    }
    else
    {
        ReturnValue->Val->Integer = ret;
    }
}

void gui_boxes(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    int* fromStorage = (int*)Param[0]->Val->Pointer;
    int* box         = (int*)Param[1]->Val->Pointer;
    int* slot        = (int*)Param[2]->Val->Pointer;
    int doCrypt      = Param[3]->Val->Integer;

    BoxChoice screen = BoxChoice((bool)doCrypt);
    auto result      = screen.run();

    *fromStorage              = std::get<0>(result);
    *box                      = std::get<1>(result);
    *slot                     = std::get<2>(result) - 1;
    ReturnValue->Val->Integer = std::get<0>(result) == 0 && std::get<1>(result) == -1 && std::get<2>(result) == -1 ? -1 : 0;
}

void net_udp_receiver(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* buffer       = (char*)Param[0]->Val->Pointer;
    int size           = (int)Param[1]->Val->Integer;
    int* bytesReceived = (int*)Param[2]->Val->Pointer;

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int fd            = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        ReturnValue->Val->Integer = errno;
        return;
    }
    memset(&addr, 0, addrlen);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PKSM_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, (struct sockaddr*)&addr, addrlen) < 0)
    {
        ReturnValue->Val->Integer = errno;
        close(fd);
        return;
    }
    *bytesReceived = 0;
    while (*bytesReceived < size)
    {
        int n = recvfrom(fd, buffer + *bytesReceived, size, 0, (struct sockaddr*)&addr, &addrlen);
        *bytesReceived += n;
        if (n <= 0)
            break;
    }

    close(fd);
    ReturnValue->Val->Integer = 0;
}

void net_tcp_receiver(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* buffer       = (char*)Param[0]->Val->Pointer;
    int size           = (int)Param[1]->Val->Integer;
    int* bytesReceived = (int*)Param[2]->Val->Pointer;

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int fd            = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (fd < 0)
    {
        ReturnValue->Val->Integer = errno;
        return;
    }
    memset(&addr, 0, addrlen);
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(PKSM_PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, (struct sockaddr*)&addr, addrlen) < 0)
    {
        ReturnValue->Val->Integer = errno;
        close(fd);
        return;
    }
    if (listen(fd, 5) < 0)
    {
        ReturnValue->Val->Integer = errno;
        close(fd);
        return;
    }
    int fdconn;
    if ((fdconn = accept(fd, (struct sockaddr*)&addr, &addrlen)) < 0)
    {
        ReturnValue->Val->Integer = errno;
        close(fd);
        return;
    }
    *bytesReceived = 0;
    while (*bytesReceived < size)
    {
        int n = recv(fdconn, buffer + *bytesReceived, size, 0);
        *bytesReceived += n;
        if (n <= 0)
            break;
    }

    close(fdconn);
    close(fd);
    ReturnValue->Val->Integer = 0;
}

void net_tcp_sender(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* ip     = (char*)Param[0]->Val->Pointer;
    int port     = (int)Param[1]->Val->Integer;
    char* buffer = (char*)Param[2]->Val->Pointer;
    int size     = (int)Param[3]->Val->Integer;

    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    int fd            = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        ReturnValue->Val->Integer = errno;
        return;
    }
    memset(&addr, 0, addrlen);
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    inet_pton(AF_INET, ip, &addr.sin_addr);
    if (connect(fd, (struct sockaddr*)&addr, addrlen) < 0)
    {
        ReturnValue->Val->Integer = errno;
        close(fd);
        return;
    }

    int total = 0;
    int chunk = 1024;
    int n;
    while (total < size)
    {
        size_t tosend = size - total > chunk ? chunk : size - total;
        n             = send(fd, buffer + total, tosend, 0);
        if (n == -1)
        {
            break;
        }
        total += n;
    }

    close(fd);
    ReturnValue->Val->Integer = total == size ? 0 : errno;
}

void bank_inject_pkx(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data       = (u8*)Param[0]->Val->Pointer;
    Generation gen = Generation(Param[1]->Val->Integer);
    int box        = Param[2]->Val->Integer;
    int slot       = Param[3]->Val->Integer;

    checkGen(Parser, gen);

    std::shared_ptr<PKX> pkm = nullptr;

    switch (gen)
    {
        case Generation::FOUR:
            pkm = std::make_shared<PK4>(data, false, false, true);
            break;
        case Generation::FIVE:
            pkm = std::make_shared<PK5>(data, false, false, true);
            break;
        case Generation::SIX:
            pkm = std::make_shared<PK6>(data, false, false, true);
            break;
        case Generation::SEVEN:
            pkm = std::make_shared<PK7>(data, false, false, true);
            break;
        case Generation::LGPE:
        default:
            pkm = std::make_shared<PB7>(data, false, true);
            break;
    }

    Banks::bank->pkm(pkm, box, slot);
}

void bank_get_pkx(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    Generation* outGen = (Generation*)Param[0]->Val->Pointer;
    int box            = Param[1]->Val->Integer;
    int slot           = Param[2]->Val->Integer;

    if (box + slot / 30 >= Banks::bank->boxes() * 30)
    {
        ProgramFail(Parser, "Invalid box, slot number: Max box is %i", Banks::bank->boxes() - 1);
    }
    else
    {
        auto pkm = Banks::bank->pkm(box, slot);
        *outGen  = pkm->generation();

        u8* out = (u8*)malloc(pkm->getLength());
        std::copy(pkm->rawData(), pkm->rawData() + pkm->getLength(), out);
        ReturnValue->Val->Pointer = (void*)out;
    }
}

void bank_get_size(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Integer = Banks::bank->boxes();
}

void bank_select(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    BankChoice screen;
    screen.run();
}

void net_ip(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char hostbuffer[256];
    if (gethostname(hostbuffer, sizeof(hostbuffer)) == -1)
    {
        ReturnValue->Val->Pointer = (void*)"";
    }
    struct hostent* host_entry = gethostbyname(hostbuffer);
    if (host_entry == NULL)
    {
        ReturnValue->Val->Pointer = (void*)"";
    }
    ReturnValue->Val->Pointer = (void*)inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0]));
}

void sav_get_pkx(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data = (u8*)Param[0]->Val->Pointer;
    int box  = Param[1]->Val->Integer;
    int slot = Param[2]->Val->Integer;

    auto pkm = TitleLoader::save->pkm(box, slot);
    memcpy(data, pkm.get()->rawData(), pkm.get()->getLength());
}

void party_get_pkx(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data = (u8*)Param[0]->Val->Pointer;
    int slot = Param[1]->Val->Integer;

    auto pkm = TitleLoader::save->pkm(slot);
    memcpy(data, pkm.get()->rawData(), pkm.get()->getLength());
}

void i18n_species(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Pointer = (void*)i18n::species(Configuration::getInstance().language(), Param[0]->Val->Integer).c_str();
}

void pkx_decrypt(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data       = (u8*)Param[0]->Val->Pointer;
    Generation gen = Generation(Param[1]->Val->Integer);

    checkGen(Parser, gen);

    std::shared_ptr<PKX> pkm = nullptr;

    switch (gen)
    {
        case Generation::FOUR:
            pkm = std::make_shared<PK4>(data, true, false, true);
            break;
        case Generation::FIVE:
            pkm = std::make_shared<PK5>(data, true, false, true);
            break;
        case Generation::SIX:
            pkm = std::make_shared<PK6>(data, true, false, true);
            break;
        case Generation::SEVEN:
            pkm = std::make_shared<PK7>(data, true, false, true);
            break;
        case Generation::LGPE:
        default:
            pkm = std::make_shared<PB7>(data, true, true);
            break;
    }
}

void pkx_encrypt(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data       = (u8*)Param[0]->Val->Pointer;
    Generation gen = Generation(Param[1]->Val->Integer);

    checkGen(Parser, gen);

    std::shared_ptr<PKX> pkm = nullptr;

    switch (gen)
    {
        case Generation::FOUR:
            pkm = std::make_shared<PK4>(data, false, false, true);
            break;
        case Generation::FIVE:
            pkm = std::make_shared<PK5>(data, false, false, true);
            break;
        case Generation::SIX:
            pkm = std::make_shared<PK6>(data, false, false, true);
            break;
        case Generation::SEVEN:
            pkm = std::make_shared<PK7>(data, false, false, true);
            break;
        case Generation::LGPE:
        default:
            pkm = std::make_shared<PB7>(data, false, true);
            break;
    }

    pkm->encrypt();
}

void pksm_utf8_to_utf16(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strToRet(StringUtils::UTF8toUTF16((char*)Param[0]->Val->Pointer));
}

void pksm_utf16_to_utf8(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Pointer = strToRet(StringUtils::UTF16toUTF8((char16_t*)Param[0]->Val->Pointer));
}

void party_inject_pkx(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data       = (u8*)Param[0]->Val->Pointer;
    Generation gen = Generation(Param[1]->Val->Integer);
    int slot       = Param[2]->Val->Integer;
    checkGen(Parser, gen);

    std::shared_ptr<PKX> pkm = nullptr;

    switch (gen)
    {
        case Generation::FOUR:
            pkm = std::make_shared<PK4>(data, false);
            break;
        case Generation::FIVE:
            pkm = std::make_shared<PK5>(data, false);
            break;
        case Generation::SIX:
            pkm = std::make_shared<PK6>(data, false);
            break;
        case Generation::SEVEN:
            pkm = std::make_shared<PK7>(data, false);
            break;
        case Generation::LGPE:
        default:
            pkm = std::make_shared<PB7>(data, false);
            break;
    }

    if (pkm)
    {
        if (TitleLoader::save->generation() == Generation::LGPE)
        {
            if (pkm->generation() == Generation::LGPE)
            {
                TitleLoader::save->pkm(pkm, slot);
                TitleLoader::save->dex(pkm);
            }
        }
        else
        {
            TitleLoader::save->transfer(pkm);
            bool moveBad = false;
            for (int i = 0; i < 4; i++)
            {
                if (pkm->move(i) > TitleLoader::save->maxMove())
                {
                    moveBad = true;
                    break;
                }
                if (pkm->relearnMove(i) > TitleLoader::save->maxMove())
                {
                    moveBad = true;
                    break;
                }
            }
            if (pkm->species() > TitleLoader::save->maxSpecies())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_SPECIES"));
                return;
            }
            else if (pkm->alternativeForm() > TitleLoader::save->formCount(pkm->species()))
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_FORM"));
                return;
            }
            else if (pkm->ability() > TitleLoader::save->maxAbility())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_ABILITY"));
                return;
            }
            else if (pkm->heldItem() > TitleLoader::save->maxItem())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_ITEM"));
                return;
            }
            else if (pkm->ball() > TitleLoader::save->maxBall())
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_BALL"));
                return;
            }
            else if (moveBad)
            {
                Gui::warn(i18n::localize("STORAGE_BAD_TRANFER") + '\n' + i18n::localize("STORAGE_BAD_MOVE"));
                return;
            }
            TitleLoader::save->pkm(pkm, slot);
            TitleLoader::save->dex(pkm);
        }
    }
}

void pkx_box_size(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    Generation gen = Generation(Param[0]->Val->Integer);
    checkGen(Parser, gen);

    switch (gen)
    {
        case Generation::FOUR:
        case Generation::FIVE:
            ReturnValue->Val->Integer = 136;
            break;
        case Generation::SIX:
        case Generation::SEVEN:
            ReturnValue->Val->Integer = 232;
            break;
        case Generation::LGPE:
        default:
            ReturnValue->Val->Integer = 260;
            break;
    }
}

void pkx_party_size(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    Generation gen = Generation(Param[0]->Val->Integer);
    checkGen(Parser, gen);

    switch (gen)
    {
        case Generation::FOUR:
            ReturnValue->Val->Integer = 236;
            break;
        case Generation::FIVE:
            ReturnValue->Val->Integer = 220;
            break;
        case Generation::SIX:
        case Generation::SEVEN:
        case Generation::LGPE:
        default:
            ReturnValue->Val->Integer = 260;
            break;
    }
}

void pkx_generate(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data                 = (u8*)Param[0]->Val->Pointer;
    int species              = Param[1]->Val->Integer;
    std::unique_ptr<PKX> pkm = nullptr;
    switch (TitleLoader::save->generation())
    {
        case Generation::FOUR:
            std::fill_n(data, 136, 0);
            pkm = std::make_unique<PK4>(data, false, false, true);
            break;
        case Generation::FIVE:
            std::fill_n(data, 136, 0);
            pkm = std::make_unique<PK5>(data, false, false, true);
            break;
        case Generation::SIX:
            std::fill_n(data, 232, 0);
            pkm = std::make_unique<PK6>(data, false, false, true);
            break;
        case Generation::SEVEN:
            std::fill_n(data, 232, 0);
            pkm = std::make_unique<PK7>(data, false, false, true);
            break;
        case Generation::LGPE:
        default:
            pkm = std::make_unique<PB7>(data, false, true);
            std::fill_n(data, 260, 0);
            break;
    }

    // From EditorScreen
    if (Configuration::getInstance().useSaveInfo())
    {
        pkm->TID(TitleLoader::save->TID());
        pkm->SID(TitleLoader::save->SID());
        pkm->otName(TitleLoader::save->otName());
    }
    else
    {
        pkm->TID(Configuration::getInstance().defaultTID());
        pkm->SID(Configuration::getInstance().defaultSID());
        pkm->otName(Configuration::getInstance().defaultOT());
    }
    pkm->ball(4);
    pkm->encryptionConstant((((u32)randomNumbers()) % 0xFFFFFFFF) + 1);
    pkm->version(TitleLoader::save->version());
    switch (pkm->version())
    {
        case 7:
        case 8:
            pkm->metLocation(0x0095); // Route 1, HGSS
            break;
        case 10:
        case 11:
        case 12:
            pkm->metLocation(0x0010); // Route 201, DPPt
            break;
        case 20:
        case 21:
        case 22:
        case 23:
            pkm->metLocation(0x000e); // Route 1, BWB2W2
            break;
        case 24:
        case 25:
            pkm->metLocation(0x0008); // Route 1, XY
            break;
        case 26:
        case 27:
            pkm->metLocation(0x00cc); // Route 101, ORAS
            break;
        case 30:
        case 31:
        case 32:
        case 33:
            pkm->metLocation(0x0006); // Route 1, SMUSUM
            break;
        case 42:
        case 43:
            pkm->metLocation(0x0003); // Route 1, LGPE
            break;
    }
    pkm->fixMoves();
    // pkm->PID((u32)randomNumbers());
    pkm->language(Configuration::getInstance().language());
    const time_t current = time(NULL);
    pkm->metDay(Configuration::getInstance().day() ? Configuration::getInstance().day() : gmtime(&current)->tm_mday);
    pkm->metMonth(Configuration::getInstance().month() ? Configuration::getInstance().month() : gmtime(&current)->tm_mon);
    pkm->metYear(Configuration::getInstance().year() ? Configuration::getInstance().year() - 2000 : gmtime(&current)->tm_year - 2000);
    pkm->metLevel(1);
    if (pkm->generation() == Generation::SIX)
    {
        ((PK6*)pkm.get())->consoleRegion(Configuration::getInstance().nationality());
        ((PK6*)pkm.get())->geoCountry(0, Configuration::getInstance().defaultCountry());
        ((PK6*)pkm.get())->geoRegion(0, Configuration::getInstance().defaultRegion());
        ((PK6*)pkm.get())->country(Configuration::getInstance().defaultCountry());
        ((PK6*)pkm.get())->region(Configuration::getInstance().defaultRegion());
    }
    else if (pkm->generation() == Generation::SEVEN)
    {
        ((PK7*)pkm.get())->consoleRegion(Configuration::getInstance().nationality());
        ((PK7*)pkm.get())->geoCountry(0, Configuration::getInstance().defaultCountry());
        ((PK7*)pkm.get())->geoRegion(0, Configuration::getInstance().defaultRegion());
        ((PK7*)pkm.get())->country(Configuration::getInstance().defaultCountry());
        ((PK7*)pkm.get())->region(Configuration::getInstance().defaultRegion());
    }

    // From SpeciesOverlay
    pkm->nickname(i18n::species(Configuration::getInstance().language(), species));
    pkm->species((u16)species);
    pkm->alternativeForm(0);
    pkm->setAbility(0);
    pkm->PID(PKX::getRandomPID(
        pkm->species(), pkm->gender(), pkm->version(), pkm->nature(), pkm->alternativeForm(), pkm->abilityNumber(), pkm->PID(), pkm->generation()));
}

void sav_get_max(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    SAV_MAX_FIELD field = SAV_MAX_FIELD(Param[0]->Val->Integer);

    switch (field)
    {
        case MAX_SLOTS:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_SLOTS", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxSlot();
            break;
        case MAX_BOXES:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_BOXES", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxBoxes();
            break;
        case MAX_WONDER_CARDS:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_WONDER_CARDS", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxWondercards();
            break;
        case MAX_SPECIES:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_SPECIES", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxSpecies();
            break;
        case MAX_MOVE:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_MOVE", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxMove();
            break;
        case MAX_ITEM:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_ITEM", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxItem();
            break;
        case MAX_ABILITY:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_ABILITY", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxAbility();
            break;
        case MAX_BALL:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_BALL", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxBall();
            break;
        case MAX_FORM:
            if (NumArgs != 2)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_FORM", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->formCount(getNextVarArg(Param[0])->Val->Integer);
            break;
        case MAX_IN_POUCH:
            if (NumArgs != 2)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for MAX_IN_POUCH", NumArgs);
            }
            {
                auto pouches = TitleLoader::save->pouches();
                Pouch pouch  = Pouch(getNextVarArg(Param[0])->Val->Integer);
                auto found = std::find_if(pouches.begin(), pouches.end(), [pouch](const std::pair<Pouch, int>& item) { return item.first == pouch; });
                if (found != pouches.end())
                {
                    ReturnValue->Val->Integer = found->second;
                }
                else
                {
                    ReturnValue->Val->Integer = 0;
                }
            }
            break;
        default:
            ProgramFail(Parser, "Field number %i is invalid", (int)field);
            break;
    }
}

void sav_get_value(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    SAV_FIELD field = SAV_FIELD(Param[0]->Val->Integer);

    switch (field)
    {
        case SAV_OT_NAME:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_OT_NAME", NumArgs);
            }
            ReturnValue->Val->Pointer = strToRet(TitleLoader::save->otName());
            break;
        case SAV_TID:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_TID", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->TID();
            break;
        case SAV_SID:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_SID", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->SID();
            break;
        case SAV_GENDER:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_GENDER", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->gender();
            break;
        case SAV_COUNTRY:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_COUNTRY", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->country();
            break;
        case SAV_SUBREGION:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_SUBREGION", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->subRegion();
            break;
        case SAV_REGION:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_REGION", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->consoleRegion();
            break;
        case SAV_LANGUAGE:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_LANGUAGE", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->language();
            break;
        case SAV_MONEY:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_MONEY", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->money();
            break;
        case SAV_BP:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_BP", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->BP();
            break;
        case SAV_HOURS:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_HOURS", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->playedHours();
            break;
        case SAV_MINUTES:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_MINUTES", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->playedMinutes();
            break;
        case SAV_SECONDS:
            if (NumArgs != 1)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_SECONDS", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->playedSeconds();
            break;
        case SAV_ITEM:
            if (NumArgs != 3)
            {
                ProgramFail(Parser, "Incorrect number of args (%i) for SAV_ITEM", NumArgs);
            }
            {
                struct Value* nextArg = getNextVarArg(Param[0]);
                Pouch pouch           = Pouch(nextArg->Val->Integer);
                if (auto item = TitleLoader::save->item(pouch, getNextVarArg(nextArg)->Val->Integer))
                {
                    ReturnValue->Val->Integer = item->id();
                }
                else
                {
                    ReturnValue->Val->Integer = 0;
                }
            }
            break;
        default:
            ProgramFail(Parser, "Field number %i is invalid", (int)field);
            break;
    }
}

void pkx_is_valid(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data       = (u8*)Param[0]->Val->Pointer;
    Generation gen = Generation(Param[1]->Val->Integer);
    checkGen(Parser, gen);

    std::unique_ptr<PKX> pkm = nullptr;
    switch (gen)
    {
        case Generation::FOUR:
            pkm = std::make_unique<PK4>(data, false, false, true);
            break;
        case Generation::FIVE:
            pkm = std::make_unique<PK5>(data, false, false, true);
            break;
        case Generation::SIX:
            pkm = std::make_unique<PK6>(data, false, false, true);
            break;
        case Generation::SEVEN:
            pkm = std::make_unique<PK7>(data, false, false, true);
            break;
        case Generation::LGPE:
        default:
            pkm = std::make_unique<PB7>(data, false, true);
            break;
    }

    if (pkm->species() == 0)
    {
        ReturnValue->Val->Integer = 0;
    }
    else
    {
        ReturnValue->Val->Integer = 1;
    }
}

void pkx_set_value(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data              = (u8*)Param[0]->Val->Pointer;
    Generation gen        = Generation(Param[1]->Val->Integer);
    PKX_FIELD field       = PKX_FIELD(Param[2]->Val->Integer);
    struct Value* nextArg = getNextVarArg(Param[2]);
    checkGen(Parser, gen);

    PKX* pkm = nullptr;
    switch (gen)
    {
        case Generation::FOUR:
            pkm = new PK4(data, false, false, true);
            break;
        case Generation::FIVE:
            pkm = new PK5(data, false, false, true);
            break;
        case Generation::SIX:
            pkm = new PK6(data, false, false, true);
            break;
        case Generation::SEVEN:
            pkm = new PK7(data, false, false, true);
            break;
        case Generation::LGPE:
        default:
            pkm = new PB7(data, false, true);
            break;
    }

    switch (field)
    {
        case OT_NAME:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for OT_NAME", NumArgs);
            }
            pkm->otName((char*)nextArg->Val->Pointer);
            break;
        case TID:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for TID", NumArgs);
            }
            pkm->TID(nextArg->Val->Integer);
            break;
        case SID:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for SID", NumArgs);
            }
            pkm->SID(nextArg->Val->Integer);
            break;
        case SHINY:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for SHINY", NumArgs);
            }
            pkm->shiny((bool)nextArg->Val->Integer);
            break;
        case LANGUAGE:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for LANGUAGE", NumArgs);
            }
            pkm->language(Language(nextArg->Val->Integer));
            break;
        case MET_LOCATION:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_LOCATION", NumArgs);
            }
            pkm->metLocation(nextArg->Val->Integer);
            break;
        case MOVE:
            if (NumArgs != 5)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MOVE", NumArgs);
            }
            pkm->move(nextArg->Val->Integer, getNextVarArg(nextArg)->Val->Integer);
            break;
        case BALL:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for BALL", NumArgs);
            }
            pkm->ball(nextArg->Val->Integer);
            break;
        case LEVEL:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for LEVEL", NumArgs);
            }
            pkm->level(nextArg->Val->Integer);
            break;
        case GENDER:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for GENDER", NumArgs);
            }
            pkm->gender(nextArg->Val->Integer);
            break;
        case ABILITY:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for ABILITY", NumArgs);
            }
            pkm->ability(nextArg->Val->Integer);
            break;
        case IV_HP:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_HP", NumArgs);
            }
            pkm->iv(0, nextArg->Val->Integer);
            break;
        case IV_ATK:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_ATK", NumArgs);
            }
            pkm->iv(1, nextArg->Val->Integer);
            break;
        case IV_DEF:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_DEF", NumArgs);
            }
            pkm->iv(2, nextArg->Val->Integer);
            break;
        case IV_SPATK:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_SPATK", NumArgs);
            }
            pkm->iv(4, nextArg->Val->Integer);
            break;
        case IV_SPDEF:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_SPDEF", NumArgs);
            }
            pkm->iv(5, nextArg->Val->Integer);
            break;
        case IV_SPEED:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_SPEED", NumArgs);
            }
            pkm->iv(3, nextArg->Val->Integer);
            break;
        case NICKNAME:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for NICKNAME", NumArgs);
            }
            pkm->nickname((char*)nextArg->Val->Pointer);
            break;
        case ITEM:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for ITEM", NumArgs);
            }
            pkm->heldItem(nextArg->Val->Integer);
            break;
        case POKERUS:
            if (NumArgs != 5)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for POKERUS", NumArgs);
            }
            pkm->pkrsStrain(nextArg->Val->Integer);
            pkm->pkrsDays(getNextVarArg(nextArg)->Val->Integer);
            break;
        case EGG_DAY:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG_DAY", NumArgs);
            }
            pkm->eggDay(nextArg->Val->Integer);
            break;
        case EGG_MONTH:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG_MONTH", NumArgs);
            }
            pkm->eggMonth(nextArg->Val->Integer);
            break;
        case EGG_YEAR:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG_YEAR", NumArgs);
            }
            pkm->eggYear(nextArg->Val->Integer > 2000 ? nextArg->Val->Integer - 2000 : nextArg->Val->Integer);
            break;
        case MET_DAY:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_DAY", NumArgs);
            }
            pkm->metDay(nextArg->Val->Integer);
            break;
        case MET_MONTH:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_MONTH", NumArgs);
            }
            pkm->metMonth(nextArg->Val->Integer);
            break;
        case MET_YEAR:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_YEAR", NumArgs);
            }
            pkm->metYear(nextArg->Val->Integer > 2000 ? nextArg->Val->Integer - 2000 : nextArg->Val->Integer);
            break;
        case FORM:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for FORM", NumArgs);
            }
            pkm->alternativeForm(nextArg->Val->Integer);
            break;
        case EV_HP:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_HP", NumArgs);
            }
            pkm->ev(0, nextArg->Val->Integer);
            break;
        case EV_ATK:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_ATK", NumArgs);
            }
            pkm->ev(1, nextArg->Val->Integer);
            break;
        case EV_DEF:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_DEF", NumArgs);
            }
            pkm->ev(2, nextArg->Val->Integer);
            break;
        case EV_SPATK:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_SPATK", NumArgs);
            }
            pkm->ev(4, nextArg->Val->Integer);
            break;
        case EV_SPDEF:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_SPDEF", NumArgs);
            }
            pkm->ev(5, nextArg->Val->Integer);
            break;
        case EV_SPEED:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_SPEED", NumArgs);
            }
            pkm->ev(3, nextArg->Val->Integer);
            break;
        case SPECIES:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for SPECIES", NumArgs);
            }
            pkm->species(nextArg->Val->Integer);
            break;
        case PID:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for PID", NumArgs);
            }
            pkm->PID(nextArg->Val->Integer);
            break;
        case NATURE:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for NATURE", NumArgs);
            }
            pkm->nature(nextArg->Val->Integer);
            break;
        case FATEFUL:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for FATEFUL", NumArgs);
            }
            pkm->fatefulEncounter((bool)nextArg->Val->Integer);
            break;
        case PP:
            if (NumArgs != 5)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for PP", NumArgs);
            }
            pkm->PP(nextArg->Val->Integer, getNextVarArg(nextArg)->Val->Integer);
            break;
        case PP_UPS:
            if (NumArgs != 5)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for PP_UPS", NumArgs);
            }
            pkm->PPUp(nextArg->Val->Integer, getNextVarArg(nextArg)->Val->Integer);
            break;
        case EGG:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG", NumArgs);
            }
            pkm->egg((bool)nextArg->Val->Integer);
            break;
        case NICKNAMED:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for NICKNAMED", NumArgs);
            }
            pkm->nicknamed((bool)nextArg->Val->Integer);
            break;
        case EGG_LOCATION:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG_LOCATION", NumArgs);
            }
            pkm->eggLocation(nextArg->Val->Integer);
            break;
        case MET_LEVEL:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_LEVEL", NumArgs);
            }
            pkm->metLevel(nextArg->Val->Integer);
            break;
        case OT_GENDER:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for OT_GENDER", NumArgs);
            }
            pkm->otGender(nextArg->Val->Integer);
            break;
        case ORIGINAL_GAME:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for ORIGINAL_GAME", NumArgs);
            }
            pkm->version(nextArg->Val->Integer);
            break;
        default:
            delete pkm;
            ProgramFail(Parser, "Field number %i is invalid", (int)field);
            break;
    }
    delete pkm;
}

void pkx_get_value(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data              = (u8*)Param[0]->Val->Pointer;
    Generation gen        = Generation(Param[1]->Val->Integer);
    PKX_FIELD field       = PKX_FIELD(Param[2]->Val->Integer);
    struct Value* nextArg = getNextVarArg(Param[2]);
    checkGen(Parser, gen);

    PKX* pkm = nullptr;
    switch (gen)
    {
        case Generation::FOUR:
            pkm = new PK4(data, false, false, true);
            break;
        case Generation::FIVE:
            pkm = new PK5(data, false, false, true);
            break;
        case Generation::SIX:
            pkm = new PK6(data, false, false, true);
            break;
        case Generation::SEVEN:
            pkm = new PK7(data, false, false, true);
            break;
        case Generation::LGPE:
        default:
            pkm = new PB7(data, false, true);
            break;
    }

    switch (field)
    {
        case OT_NAME:
        {
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for OT_NAME", NumArgs);
            }
            std::string name = pkm->otName();
            char* ret        = (char*)malloc(name.size() + 1);
            std::copy(name.begin(), name.end(), ret);
            ret[name.size()]                  = '\0';
            ReturnValue->Val->UnsignedInteger = (u32)ret;
        }
        break;
        case TID:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for TID", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->TID();
            break;
        case SID:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for SID", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->SID();
            break;
        case SHINY:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for SHINY", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->shiny();
            break;
        case LANGUAGE:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for LANGUAGE", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->language();
            break;
        case MET_LOCATION:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_LOCATION", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->metLocation();
            break;
        case MOVE:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MOVE", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->move(nextArg->Val->Integer);
            break;
        case BALL:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for BALL", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->ball();
            break;
        case LEVEL:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for LEVEL", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->level();
            break;
        case GENDER:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for GENDER", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->gender();
            break;
        case ABILITY:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for ABILITY", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->ability();
            break;
        case IV_HP:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_HP", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(0);
            break;
        case IV_ATK:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_ATK", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(1);
            break;
        case IV_DEF:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_DEF", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(2);
            break;
        case IV_SPATK:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_SPATK", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(4);
            break;
        case IV_SPDEF:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_SPDEF", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(5);
            break;
        case IV_SPEED:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for IV_SPEED", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(3);
            break;
        case NICKNAME:
        {
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for NICKNAME", NumArgs);
            }
            std::string nick = pkm->nickname();
            char* ret        = (char*)malloc(nick.size() + 1);
            std::copy(nick.begin(), nick.end(), ret);
            ret[nick.size()]                  = '\0';
            ReturnValue->Val->UnsignedInteger = (u32)ret;
        }
        break;
        case ITEM:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for ITEM", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->heldItem();
            break;
        case POKERUS:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for POKERUS", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->pkrs();
            break;
        case EGG_DAY:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG_DAY", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->eggDay();
            break;
        case EGG_MONTH:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG_MONTH", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->eggMonth();
            break;
        case EGG_YEAR:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG_YEAR", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->eggYear();
            break;
        case MET_DAY:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_DAY", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->metDay();
            break;
        case MET_MONTH:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_MONTH", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->metMonth();
            break;
        case MET_YEAR:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_YEAR", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->metYear();
            break;
        case FORM:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for FORM", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->alternativeForm();
            break;
        case EV_HP:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_HP", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(0);
            break;
        case EV_ATK:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_ATK", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(1);
            break;
        case EV_DEF:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_DEF", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(2);
            break;
        case EV_SPATK:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_SPATK", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(4);
            break;
        case EV_SPDEF:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_SPDEF", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(5);
            break;
        case EV_SPEED:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EV_SPEED", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(3);
            break;
        case SPECIES:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for SPECIES", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->species();
            break;
        case PID:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for PID", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->PID();
            break;
        case NATURE:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for NATURE", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->nature();
            break;
        case FATEFUL:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for FATEFUL", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->fatefulEncounter();
            break;
        case PP:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for PP", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->PP(nextArg->Val->Integer);
            break;
        case PP_UPS:
            if (NumArgs != 4)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for PP_UPS", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->PPUp(nextArg->Val->Integer);
            break;
        case EGG:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->egg();
            break;
        case NICKNAMED:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for NICKNAMED", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->nicknamed();
            break;
        case EGG_LOCATION:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for EGG_LOCATION", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->eggLocation();
            break;
        case MET_LEVEL:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for MET_LEVEL", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->metLevel();
            break;
        case OT_GENDER:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for OT_GENDER", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->otGender();
            break;
        case ORIGINAL_GAME:
            if (NumArgs != 3)
            {
                delete pkm;
                ProgramFail(Parser, "Incorrect number of args (%i) for ORIGINAL_GAME", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->version();
            break;
        default:
            delete pkm;
            ProgramFail(Parser, "Field number %i is invalid", (int)field);
            break;
    }
    delete pkm;
}

void sav_set_string(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* string   = (char*)Param[0]->Val->Pointer;
    u32 offset     = Param[1]->Val->UnsignedInteger;
    u32 codepoints = Param[2]->Val->UnsignedInteger; // Includes null terminator
    if (TitleLoader::save->generation() != Generation::FOUR)
    {
        std::u16string write = StringUtils::UTF8toUTF16(string);
        while (write.size() >= codepoints)
        {
            write.pop_back();
        }
        for (size_t i = 0; i < write.size(); i++) // Not sure whether this includes terminator, so let's make sure
        {
            *((u16*)(TitleLoader::save->rawData() + offset) + i) = write[i];
        }
        if (TitleLoader::save->generation() == Generation::FIVE)
        {
            *((u16*)(TitleLoader::save->rawData() + offset) + write.size()) = 0xFFFF;
        }
        else
        {
            *((u16*)(TitleLoader::save->rawData() + offset) + write.size()) = 0;
        }
        for (size_t i = write.size() + 1; i < codepoints; i++)
        {
            *((u16*)(TitleLoader::save->rawData() + offset) + i) = 0;
        }
    }
    else
    {
        auto write = StringUtils::stringToG4(string);
        while (write.size() > codepoints) // Remove non-terminator codepoints
        {
            write.erase(write.end() - 1);
        }
        for (size_t i = 0; i < write.size(); i++) // Definitely includes the terminator
        {
            *((u16*)(TitleLoader::save->rawData() + offset) + i) = write[i];
        }
        for (size_t i = write.size() + 1; i < codepoints; i++)
        {
            *((u16*)(TitleLoader::save->rawData() + offset) + i) = 0;
        }
    }
}

void sav_get_string(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u32 offset     = Param[0]->Val->UnsignedInteger;
    u32 codepoints = Param[1]->Val->UnsignedInteger; // Includes null terminator

    if (TitleLoader::save->generation() == Generation::FOUR)
    {
        std::string data = StringUtils::getString4(TitleLoader::save->rawData(), offset, codepoints);
        char* ret        = (char*)malloc(data.size() + 1);
        std::copy(data.begin(), data.end(), ret);
        ret[data.size()]                  = '\0';
        ReturnValue->Val->UnsignedInteger = (u32)ret;
    }
    else if (TitleLoader::save->generation() == Generation::FIVE)
    {
        std::string data = StringUtils::getString(TitleLoader::save->rawData(), offset, codepoints, u'\uFFFF');
        char* ret        = (char*)malloc(data.size() + 1);
        std::copy(data.begin(), data.end(), ret);
        ret[data.size()]                  = '\0';
        ReturnValue->Val->UnsignedInteger = (u32)ret;
    }
    else
    {
        std::string data = StringUtils::getString(TitleLoader::save->rawData(), offset, codepoints);
        char* ret        = (char*)malloc(data.size() + 1);
        std::copy(data.begin(), data.end(), ret);
        ret[data.size()]                  = '\0';
        ReturnValue->Val->UnsignedInteger = (u32)ret;
    }
}
}
