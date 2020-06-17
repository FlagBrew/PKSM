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

#include "BankChoice.hpp"
#include "BoxChoice.hpp"
#include "Configuration.hpp"
#include "FortyChoice.hpp"
#include "PkmUtils.hpp"
#include "STDirectory.hpp"
#include "ThirtyChoice.hpp"
#include "banks.hpp"
#include "base64.hpp"
#include "fetch.hpp"
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "nlohmann/json.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK3.hpp"
#include "pkx/PK4.hpp"
#include "pkx/PK5.hpp"
#include "pkx/PK6.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PK8.hpp"
#include "sav/Sav3.hpp"
#include "sav/Sav4.hpp"
#include "sav/Sav8.hpp"
#include "utils/flagUtil.hpp"
#include "utils/genToPkx.hpp"
#include "utils/random.hpp"
#include "utils/utils.hpp"
#include "wcx/PGF.hpp"
#include "wcx/PGT.hpp"
#include "wcx/WB7.hpp"
#include "wcx/WC4.hpp"
#include "wcx/WC6.hpp"
#include "wcx/WC7.hpp"
#include "wcx/WC8.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>

#include "picoc.h"
#undef min

namespace
{
    void* strToRet(const std::string& str)
    {
        char* ret = (char*)malloc(str.size() + 1);
        if (ret)
        {
            std::copy(str.begin(), str.end(), ret);
            ret[str.size()] = '\0';
        }
        return (void*)ret;
    }

    void* strToRet(const std::u16string& str)
    {
        u16* ret = (u16*)malloc((str.size() + 1) * 2);
        if (ret)
        {
            std::copy(str.begin(), str.end(), ret);
            ret[str.size()] = u'\0';
        }
        return (void*)ret;
    }

    template <typename... Ts>
    [[noreturn]] void scriptFail(struct ParseState* Parser, const std::string& str, Ts... args) {
        ProgramFail(Parser, str.c_str(), args...);
        std::abort(); // Dummy call to suppress compiler warning: ProgramFail does not return
    }

    template <>
    [[noreturn]] void scriptFail<>(struct ParseState* Parser, const std::string& str) {
        ProgramFail(Parser, str.c_str());
        std::abort(); // Dummy call to suppress compiler warning: ProgramFail does not return
    }

    void checkGen(struct ParseState* Parser, pksm::Generation gen)
    {
        switch (gen)
        {
            case pksm::Generation::THREE:
            case pksm::Generation::FOUR:
            case pksm::Generation::FIVE:
            case pksm::Generation::SIX:
            case pksm::Generation::SEVEN:
            case pksm::Generation::LGPE:
            case pksm::Generation::EIGHT:
                return;
            case pksm::Generation::UNUSED:
                break;
        }
        scriptFail(Parser, "Generation is not possible!");
    }

    struct Value* getNextVarArg(struct Value* arg) { return (struct Value*)((char*)arg + MEM_ALIGN(sizeof(struct Value) + TypeStackSizeValue(arg))); }

    std::unique_ptr<pksm::PKX> getPokemon(u8* data, pksm::Generation gen, bool isParty)
    {
        if (gen == pksm::Generation::THREE)
        {
            auto ret = pksm::PKX::getPKM<pksm::Generation::THREE>(nullptr, isParty);
            std::copy(data, data + ret->getLength(), ret->rawData());
            return ret;
        }
        else
        {
            return pksm::PKX::getPKM(gen, data, isParty, true);
        }
    }
}

extern "C" {
#include "pksm_api.h"

void gui_warn(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    Gui::warn((char*)Param[0]->Val->Pointer);
}

void gui_choice(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Integer = (int)Gui::showChoiceMessage((char*)Param[0]->Val->Pointer);
}

void gui_splash(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    Gui::waitFrame((char*)Param[0]->Val->Pointer);
}

void gui_menu6x5(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* question            = (char*)Param[0]->Val->Pointer;
    int options               = Param[1]->Val->Integer;
    char** labels             = (char**)Param[2]->Val->Pointer;
    pkm* pokemon              = (pkm*)Param[3]->Val->Pointer;
    pksm::Generation gen      = pksm::Generation(Param[4]->Val->Integer);
    ThirtyChoice screen       = ThirtyChoice(question, labels, pokemon, options, gen);
    auto ret                  = Gui::runScreen(screen);
    ReturnValue->Val->Integer = ret;
}

void gui_menu20x2(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* question            = (char*)Param[0]->Val->Pointer;
    int options               = Param[1]->Val->Integer;
    char** labels             = (char**)Param[2]->Val->Pointer;
    FortyChoice screen        = FortyChoice(question, labels, options);
    auto ret                  = Gui::runScreen(screen);
    ReturnValue->Val->Integer = ret;
}

void sav_sbo(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    if (TitleLoader::save->generation() == pksm::Generation::FOUR)
    {
        ReturnValue->Val->Integer = ((pksm::Sav4*)TitleLoader::save.get())->getSBO();
    }
    else
    {
        ReturnValue->Val->Integer = 0;
    }
}

void sav_gbo(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    if (TitleLoader::save->generation() == pksm::Generation::FOUR)
    {
        ReturnValue->Val->Integer = ((pksm::Sav4*)TitleLoader::save.get())->getGBO();
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

    SwkbdState state;
    swkbdInit(&state, SWKBD_TYPE_NORMAL, 1, numChars - 1);
    swkbdSetHintText(&state, hint);
    swkbdSetValidation(&state, SWKBD_NOTBLANK_NOTEMPTY, SWKBD_FILTER_PROFANITY, 0);
    swkbdInputText(&state, out, numChars * 3); // numChars is UTF-16 codepoints, each UTF-8 codepoint needs up to 3 bytes, so
    out[numChars * 3 - 1] = '\0';
}

void gui_numpad(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    unsigned int* out = (unsigned int*)Param[0]->Val->Pointer;
    std::string hint  = (char*)Param[1]->Val->Pointer;
    int numChars      = Param[2]->Val->Integer;

    char number[numChars + 1] = {0};

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
        if (directory.count() > 0)
        {
            ret->data = (char**)malloc(sizeof(char*) * directory.count());
            for (size_t i = 0; i < directory.count(); i++)
            {
                ret->data[i] = (char*)strToRet(dir + '/' + directory.item(i));
            }
        }
        else
        {
            ret->data = nullptr;
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

void save_path(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int numArgs)
{
    auto savePath = TitleLoader::savePath();
    if (savePath.empty())
    {
        ReturnValue->Val->Pointer = nullptr;
    }
    else
    {
        ReturnValue->Val->Pointer = strToRet(TitleLoader::savePath());
    }
}

void sav_inject_pkx(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data             = (u8*)Param[0]->Val->Pointer;
    pksm::Generation gen = pksm::Generation(Param[1]->Val->Integer);
    int box              = Param[2]->Val->Integer;
    int slot             = Param[3]->Val->Integer;
    bool doTradeEdits    = Param[4]->Val->Integer;
    checkGen(Parser, gen);

    auto pkm = getPokemon(data, gen, false);

    if (pkm)
    {
        pkm = TitleLoader::save->transfer(*pkm);
        if (!pkm)
        {
            Gui::warn(fmt::format(i18n::localize("NO_TRANSFER_PATH_SINGLE"), (std::string)gen, (std::string)TitleLoader::save->generation()));
            return;
        }
        auto invalidReason = TitleLoader::save->invalidTransferReason(*pkm);
        if (invalidReason != pksm::Sav::BadTransferReason::OKAY)
        {
            Gui::warn(i18n::localize("NO_TRANSFER_PATH") + '\n' + i18n::badTransfer(Configuration::getInstance().language(), invalidReason));
        }
        else
        {
            pkm->refreshChecksum();
            TitleLoader::save->pkm(*pkm, box, slot, doTradeEdits);
            TitleLoader::save->dex(*pkm);
        }
    }
}

void cfg_default_ot(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    pksm::Generation gen = pksm::Generation(Param[0]->Val->Integer);

    checkGen(Parser, gen);

    ReturnValue->Val->Pointer = strToRet(PkmUtils::getDefault(gen)->otName());
}

void cfg_default_tid(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    pksm::Generation gen = pksm::Generation(Param[0]->Val->Integer);

    checkGen(Parser, gen);

    ReturnValue->Val->UnsignedShortInteger = PkmUtils::getDefault(gen)->TID();
}

void cfg_default_sid(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    pksm::Generation gen = pksm::Generation(Param[0]->Val->Integer);

    checkGen(Parser, gen);

    ReturnValue->Val->UnsignedShortInteger = PkmUtils::getDefault(gen)->SID();
}

void cfg_default_day(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Integer = Configuration::getInstance().date().day();
}

void cfg_default_month(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Integer = Configuration::getInstance().date().month();
}

void cfg_default_year(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Integer = Configuration::getInstance().date().year();
}

void gui_boxes(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    int* fromStorage = (int*)Param[0]->Val->Pointer;
    int* box         = (int*)Param[1]->Val->Pointer;
    int* slot        = (int*)Param[2]->Val->Pointer;
    int doCrypt      = Param[3]->Val->Integer;

    BoxChoice screen = BoxChoice((bool)doCrypt);
    auto result      = Gui::runScreen(screen);

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
    u8* data             = (u8*)Param[0]->Val->Pointer;
    pksm::Generation gen = pksm::Generation(Param[1]->Val->Integer);
    int box              = Param[2]->Val->Integer;
    int slot             = Param[3]->Val->Integer;

    checkGen(Parser, gen);

    auto pkm = getPokemon(data, gen, false);

    pkm->refreshChecksum();
    Banks::bank->pkm(*pkm, box, slot);
}

void bank_get_pkx(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    pksm::Generation* outGen = (pksm::Generation*)Param[0]->Val->Pointer;
    int box                  = Param[1]->Val->Integer;
    int slot                 = Param[2]->Val->Integer;

    if (box + slot / 30 >= Banks::bank->boxes() * 30)
    {
        scriptFail(Parser, "Invalid box, slot number: Max box is %i", Banks::bank->boxes() - 1);
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
    Gui::runScreen(screen);
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
    ReturnValue->Val->Pointer = (void*)i18n::species(Configuration::getInstance().language(), pksm::Species{u16(Param[0]->Val->Integer)}).c_str();
}

void i18n_form(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    ReturnValue->Val->Pointer = (void*)i18n::form(Configuration::getInstance().language(), pksm::GameVersion{u8(Param[0]->Val->Integer)},
        pksm::Species{u16(Param[1]->Val->Integer)}, u8(Param[2]->Val->Integer))
                                    .c_str();
}

void pkx_decrypt(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data             = (u8*)Param[0]->Val->Pointer;
    pksm::Generation gen = pksm::Generation(Param[1]->Val->Integer);
    bool isParty         = (bool)Param[2]->Val->Integer;

    checkGen(Parser, gen);

    // Will automatically decrypt data; explicitly meant to not use getPokemon
    std::unique_ptr<pksm::PKX> pkm = pksm::PKX::getPKM(gen, data, isParty, true);
}

void pkx_encrypt(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data             = (u8*)Param[0]->Val->Pointer;
    pksm::Generation gen = pksm::Generation(Param[1]->Val->Integer);
    bool isParty         = (bool)Param[2]->Val->Integer;

    checkGen(Parser, gen);

    std::unique_ptr<pksm::PKX> pkm = getPokemon(data, gen, isParty);
    pkm->encrypt();
    if (gen == pksm::Generation::THREE)
    {
        std::copy(pkm->rawData(), pkm->rawData() + pkm->getLength(), data);
    }
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
    u8* data             = (u8*)Param[0]->Val->Pointer;
    pksm::Generation gen = pksm::Generation(Param[1]->Val->Integer);
    int slot             = Param[2]->Val->Integer;
    checkGen(Parser, gen);

    auto pkm = getPokemon(data, gen, false);

    if (pkm)
    {
        pkm = TitleLoader::save->transfer(*pkm);
        if (!pkm)
        {
            Gui::warn(fmt::format(i18n::localize("NO_TRANSFER_PATH_SINGLE"), (std::string)gen, (std::string)TitleLoader::save->generation()));
            return;
        }
        auto invalidReason = TitleLoader::save->invalidTransferReason(*pkm);
        if (invalidReason != pksm::Sav::BadTransferReason::OKAY)
        {
            Gui::warn(i18n::localize("NO_TRANSFER_PATH") + '\n' + i18n::badTransfer(Configuration::getInstance().language(), invalidReason));
        }
        else
        {
            pkm->refreshChecksum();
            TitleLoader::save->pkm(*pkm, slot);
            TitleLoader::save->fixParty();
            TitleLoader::save->dex(*pkm);
        }
    }
}

void pkx_box_size(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    pksm::Generation gen = pksm::Generation(Param[0]->Val->Integer);
    checkGen(Parser, gen);

    switch (gen)
    {
        case pksm::Generation::THREE:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::THREE>::PKX::BOX_LENGTH;
            break;
        case pksm::Generation::FOUR:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::FOUR>::PKX::BOX_LENGTH;
            break;
        case pksm::Generation::FIVE:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::FIVE>::PKX::BOX_LENGTH;
            break;
        case pksm::Generation::SIX:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::SIX>::PKX::BOX_LENGTH;
            break;
        case pksm::Generation::SEVEN:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::SEVEN>::PKX::BOX_LENGTH;
            break;
        case pksm::Generation::LGPE:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::LGPE>::PKX::BOX_LENGTH;
            break;
        case pksm::Generation::EIGHT:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::EIGHT>::PKX::BOX_LENGTH;
            break;
        case pksm::Generation::UNUSED:
            break;
    }
}

void pkx_party_size(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    pksm::Generation gen = pksm::Generation(Param[0]->Val->Integer);
    checkGen(Parser, gen);

    switch (gen)
    {
        case pksm::Generation::THREE:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::THREE>::PKX::PARTY_LENGTH;
            break;
        case pksm::Generation::FOUR:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::FOUR>::PKX::PARTY_LENGTH;
            break;
        case pksm::Generation::FIVE:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::FIVE>::PKX::PARTY_LENGTH;
            break;
        case pksm::Generation::SIX:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::SIX>::PKX::PARTY_LENGTH;
            break;
        case pksm::Generation::SEVEN:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::SEVEN>::PKX::PARTY_LENGTH;
            break;
        case pksm::Generation::LGPE:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::LGPE>::PKX::PARTY_LENGTH;
            break;
        case pksm::Generation::EIGHT:
            ReturnValue->Val->Integer = pksm::GenToPkx<pksm::Generation::EIGHT>::PKX::PARTY_LENGTH;
            break;
        case pksm::Generation::UNUSED:
            break;
    }
}

void pkx_generate(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data    = (u8*)Param[0]->Val->Pointer;
    int species = Param[1]->Val->Integer;

    // is fine to not use getPokemon
    auto pkm  = pksm::PKX::getPKM(TitleLoader::save->generation(), data, false, true);
    auto orig = PkmUtils::getDefault(TitleLoader::save->generation());
    switch (TitleLoader::save->generation())
    {
        case pksm::Generation::THREE:
            std::copy(orig->rawData(), orig->rawData() + pksm::GenToPkx<pksm::Generation::THREE>::PKX::BOX_LENGTH, data);
            break;
        case pksm::Generation::FOUR:
            std::copy(orig->rawData(), orig->rawData() + pksm::GenToPkx<pksm::Generation::FOUR>::PKX::BOX_LENGTH, data);
            break;
        case pksm::Generation::FIVE:
            std::copy(orig->rawData(), orig->rawData() + pksm::GenToPkx<pksm::Generation::FIVE>::PKX::BOX_LENGTH, data);
            break;
        case pksm::Generation::SIX:
            std::copy(orig->rawData(), orig->rawData() + pksm::GenToPkx<pksm::Generation::SIX>::PKX::BOX_LENGTH, data);
            break;
        case pksm::Generation::SEVEN:
            std::copy(orig->rawData(), orig->rawData() + pksm::GenToPkx<pksm::Generation::SEVEN>::PKX::BOX_LENGTH, data);
            break;
        case pksm::Generation::LGPE:
            std::copy(orig->rawData(), orig->rawData() + pksm::GenToPkx<pksm::Generation::LGPE>::PKX::BOX_LENGTH, data);
            break;
        case pksm::Generation::EIGHT:
            std::copy(orig->rawData(), orig->rawData() + pksm::GenToPkx<pksm::Generation::EIGHT>::PKX::BOX_LENGTH, data);
            break;
        // Should never happen
        case pksm::Generation::UNUSED:
            break;
    }

    if (Configuration::getInstance().useSaveInfo())
    {
        pkm->TID(TitleLoader::save->TID());
        pkm->SID(TitleLoader::save->SID());
        pkm->otName(TitleLoader::save->otName());
        pkm->otGender(TitleLoader::save->gender());
        pkm->version(TitleLoader::save->version());
        switch (pkm->version())
        {
            case pksm::GameVersion::R:
            case pksm::GameVersion::S:
            case pksm::GameVersion::E:
                pkm->metLocation(0x0010); // Route 101, probably RSE
                break;
            case pksm::GameVersion::FR:
            case pksm::GameVersion::LG:
                pkm->metLocation(0x0065); // Route 1, probably FRLG
                break;
            case pksm::GameVersion::HG:
            case pksm::GameVersion::SS:
                pkm->metLocation(0x0095); // Route 1, HGSS
                break;
            case pksm::GameVersion::D:
            case pksm::GameVersion::P:
            case pksm::GameVersion::Pt:
                pkm->metLocation(0x0010); // Route 201, DPPt
                break;
            case pksm::GameVersion::B:
            case pksm::GameVersion::W:
            case pksm::GameVersion::B2:
            case pksm::GameVersion::W2:
                pkm->metLocation(0x000e); // Route 1, BWB2W2
                break;
            case pksm::GameVersion::X:
            case pksm::GameVersion::Y:
                pkm->metLocation(0x0008); // Route 1, XY
                break;
            case pksm::GameVersion::OR:
            case pksm::GameVersion::AS:
                pkm->metLocation(0x00cc); // Route 101, ORAS
                break;
            case pksm::GameVersion::SN:
            case pksm::GameVersion::MN:
            case pksm::GameVersion::US:
            case pksm::GameVersion::UM:
                pkm->metLocation(0x0006); // Route 1, SMUSUM
                break;
            case pksm::GameVersion::GP:
            case pksm::GameVersion::GE:
                pkm->metLocation(0x0003); // Route 1, LGPE
                break;
            case pksm::GameVersion::SW:
            case pksm::GameVersion::SH:
                pkm->metLocation(0x000C); // Route 1, SWSH
                break;
            default:
                break;
        }
    }

    // From SpeciesOverlay
    pkm->nickname(i18n::species(pkm->language(), pksm::Species{u16(species)}));
    pkm->species(pksm::Species{u16(species)});
    pkm->alternativeForm(0);
    pkm->setAbility(0);
    pkm->PID(pksm::PKX::getRandomPID(
        pkm->species(), pkm->gender(), pkm->version(), pkm->nature(), pkm->alternativeForm(), pkm->abilityNumber(), pkm->PID(), pkm->generation()));
    pkm->level(orig->level());
}

void sav_get_max(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    SAV_MAX_FIELD field = SAV_MAX_FIELD(Param[0]->Val->Integer);

    switch (field)
    {
        case MAX_SLOTS:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for MAX_SLOTS", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxSlot();
            break;
        case MAX_BOXES:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for MAX_BOXES", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxBoxes();
            break;
        case MAX_WONDER_CARDS:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for MAX_WONDER_CARDS", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->maxWondercards();
            break;
        case MAX_FORM:
            if (NumArgs != 2)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for MAX_FORM", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->formCount(pksm::Species{u16(getNextVarArg(Param[0])->Val->Integer)});
            break;
        case MAX_IN_POUCH:
            if (NumArgs != 2)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for MAX_IN_POUCH", NumArgs);
            }
            else
            {
                auto pouches           = TitleLoader::save->pouches();
                pksm::Sav::Pouch pouch = pksm::Sav::Pouch(getNextVarArg(Param[0])->Val->Integer);
                auto found             = std::find_if(
                    pouches.begin(), pouches.end(), [pouch](const std::pair<pksm::Sav::Pouch, int>& item) { return item.first == pouch; });
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
            scriptFail(Parser, "Field number %i is invalid", (int)field);
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
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_OT_NAME", NumArgs);
            }
            ReturnValue->Val->Pointer = strToRet(TitleLoader::save->otName());
            break;
        case SAV_TID:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_TID", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->TID();
            break;
        case SAV_SID:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_SID", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->SID();
            break;
        case SAV_GENDER:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_GENDER", NumArgs);
            }
            ReturnValue->Val->Integer = int(TitleLoader::save->gender());
            break;
        case SAV_COUNTRY:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_COUNTRY", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->country();
            break;
        case SAV_SUBREGION:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_SUBREGION", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->subRegion();
            break;
        case SAV_REGION:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_REGION", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->consoleRegion();
            break;
        case SAV_LANGUAGE:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_LANGUAGE", NumArgs);
            }
            ReturnValue->Val->Integer = u8(TitleLoader::save->language());
            break;
        case SAV_MONEY:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_MONEY", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->money();
            break;
        case SAV_BP:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_BP", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->BP();
            break;
        case SAV_HOURS:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_HOURS", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->playedHours();
            break;
        case SAV_MINUTES:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_MINUTES", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->playedMinutes();
            break;
        case SAV_SECONDS:
            if (NumArgs != 1)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_SECONDS", NumArgs);
            }
            ReturnValue->Val->Integer = TitleLoader::save->playedSeconds();
            break;
        case SAV_ITEM:
            if (NumArgs != 3)
            {
                scriptFail(Parser, "Incorrect number of args (%i) for SAV_ITEM", NumArgs);
            }
            else
            {
                struct Value* nextArg  = getNextVarArg(Param[0]);
                pksm::Sav::Pouch pouch = pksm::Sav::Pouch(nextArg->Val->Integer);
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
            scriptFail(Parser, "Field number %i is invalid", (int)field);
    }
}

void sav_check_value(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    SAV_VALUE_CHECK field = SAV_VALUE_CHECK(Param[0]->Val->Integer);
    int value             = Param[1]->Val->Integer;

    switch (field)
    {
        case SAV_VALUE_SPECIES:
            ReturnValue->Val->Integer = TitleLoader::save->availableSpecies().count(pksm::Species{u16(value)});
            break;
        case SAV_VALUE_MOVE:
            ReturnValue->Val->Integer = TitleLoader::save->availableMoves().count(value);
            break;
        case SAV_VALUE_ITEM:
            ReturnValue->Val->Integer = TitleLoader::save->availableItems().count(value);
            break;
        case SAV_VALUE_ABILITY:
            ReturnValue->Val->Integer = TitleLoader::save->availableAbilities().count(pksm::Ability{u16(value)});
            break;
        case SAV_VALUE_BALL:
            ReturnValue->Val->Integer = TitleLoader::save->availableBalls().count(pksm::Ball{u8(value)});
            break;
        default:
            scriptFail(Parser, "Field number %i is invalid", (int)field);
    }
}

void pkx_is_valid(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data             = (u8*)Param[0]->Val->Pointer;
    pksm::Generation gen = pksm::Generation(Param[1]->Val->Integer);
    checkGen(Parser, gen);

    auto pkm = getPokemon(data, gen, false);

    if (pkm->species() == pksm::Species::None || pkm->species() > pksm::PKX::PKSM_MAX_SPECIES)
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
    pksm::Generation gen  = pksm::Generation(Param[1]->Val->Integer);
    PKX_FIELD field       = PKX_FIELD(Param[2]->Val->Integer);
    struct Value* nextArg = getNextVarArg(Param[2]);
    checkGen(Parser, gen);

    // Slight overhead from constructing and deconstructing the unique_ptr, but avoids a logic repetition
    pksm::PKX* pkm = getPokemon(data, gen, false).release();

    switch (field)
    {
        case OT_NAME:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for OT_NAME", NumArgs);
            }
            pkm->otName((char*)nextArg->Val->Pointer);
            break;
        case TID:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for TID", NumArgs);
            }
            pkm->TID(nextArg->Val->Integer);
            break;
        case SID:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for SID", NumArgs);
            }
            pkm->SID(nextArg->Val->Integer);
            break;
        case SHINY:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for SHINY", NumArgs);
            }
            pkm->shiny((bool)nextArg->Val->Integer);
            break;
        case LANGUAGE:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for LANGUAGE", NumArgs);
            }
            pkm->language(getSafeLanguage(pkm->generation(), pksm::Language(nextArg->Val->Integer)));
            break;
        case MET_LOCATION:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_LOCATION", NumArgs);
            }
            pkm->metLocation(nextArg->Val->Integer);
            break;
        case MOVE:
            if (NumArgs != 5)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MOVE", NumArgs);
            }
            pkm->move(nextArg->Val->Integer, getNextVarArg(nextArg)->Val->Integer);
            break;
        case BALL:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for BALL", NumArgs);
            }
            pkm->ball(pksm::Ball{u8(nextArg->Val->Integer)});
            break;
        case LEVEL:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for LEVEL", NumArgs);
            }
            pkm->level(nextArg->Val->Integer);
            break;
        case GENDER:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for GENDER", NumArgs);
            }
            pkm->gender(pksm::Gender{u8(nextArg->Val->Integer)});
            break;
        case ABILITY:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for ABILITY", NumArgs);
            }
            pkm->ability(pksm::Ability{u8(nextArg->Val->Integer)});
            break;
        case IV_HP:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_HP", NumArgs);
            }
            pkm->iv(pksm::Stat::HP, nextArg->Val->Integer);
            break;
        case IV_ATK:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_ATK", NumArgs);
            }
            pkm->iv(pksm::Stat::ATK, nextArg->Val->Integer);
            break;
        case IV_DEF:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_DEF", NumArgs);
            }
            pkm->iv(pksm::Stat::DEF, nextArg->Val->Integer);
            break;
        case IV_SPATK:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_SPATK", NumArgs);
            }
            pkm->iv(pksm::Stat::SPATK, nextArg->Val->Integer);
            break;
        case IV_SPDEF:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_SPDEF", NumArgs);
            }
            pkm->iv(pksm::Stat::SPDEF, nextArg->Val->Integer);
            break;
        case IV_SPEED:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_SPEED", NumArgs);
            }
            pkm->iv(pksm::Stat::SPD, nextArg->Val->Integer);
            break;
        case NICKNAME:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for NICKNAME", NumArgs);
            }
            pkm->nickname((char*)nextArg->Val->Pointer);
            break;
        case ITEM:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for ITEM", NumArgs);
            }
            pkm->heldItem(nextArg->Val->Integer);
            break;
        case POKERUS:
            if (NumArgs != 5)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for POKERUS", NumArgs);
            }
            pkm->pkrsStrain(nextArg->Val->Integer);
            pkm->pkrsDays(getNextVarArg(nextArg)->Val->Integer);
            break;
        case EGG_DAY:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG_DAY", NumArgs);
            }
            {
                Date date = pkm->eggDate();
                date.day((u8)nextArg->Val->Integer);
                pkm->eggDate(date);
            }
            break;
        case EGG_MONTH:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG_MONTH", NumArgs);
            }
            {
                Date date = pkm->eggDate();
                date.month((u8)nextArg->Val->Integer);
                pkm->eggDate(date);
            }
            break;
        case EGG_YEAR:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG_YEAR", NumArgs);
            }
            {
                Date date = pkm->eggDate();
                date.year((u32)nextArg->Val->Integer);
                pkm->eggDate(date);
            }
            break;
        case MET_DAY:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_DAY", NumArgs);
            }
            {
                Date date = pkm->eggDate();
                date.day((u8)nextArg->Val->Integer);
                pkm->eggDate(date);
            }
            break;
        case MET_MONTH:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_MONTH", NumArgs);
            }
            {
                Date date = pkm->eggDate();
                date.month((u8)nextArg->Val->Integer);
                pkm->eggDate(date);
            }
            break;
        case MET_YEAR:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_YEAR", NumArgs);
            }
            {
                Date date = pkm->eggDate();
                date.year((u32)nextArg->Val->Integer);
                pkm->eggDate(date);
            }
            break;
        case FORM:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for FORM", NumArgs);
            }
            pkm->alternativeForm(nextArg->Val->Integer);
            break;
        case EV_HP:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_HP", NumArgs);
            }
            pkm->ev(pksm::Stat::HP, nextArg->Val->Integer);
            break;
        case EV_ATK:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_ATK", NumArgs);
            }
            pkm->ev(pksm::Stat::ATK, nextArg->Val->Integer);
            break;
        case EV_DEF:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_DEF", NumArgs);
            }
            pkm->ev(pksm::Stat::DEF, nextArg->Val->Integer);
            break;
        case EV_SPATK:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_SPATK", NumArgs);
            }
            pkm->ev(pksm::Stat::SPATK, nextArg->Val->Integer);
            break;
        case EV_SPDEF:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_SPDEF", NumArgs);
            }
            pkm->ev(pksm::Stat::SPDEF, nextArg->Val->Integer);
            break;
        case EV_SPEED:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_SPEED", NumArgs);
            }
            pkm->ev(pksm::Stat::SPD, nextArg->Val->Integer);
            break;
        case SPECIES:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for SPECIES", NumArgs);
            }
            pkm->species(pksm::Species{u16(nextArg->Val->Integer)});
            break;
        case PID:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for PID", NumArgs);
            }
            pkm->PID(nextArg->Val->Integer);
            break;
        case NATURE:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for NATURE", NumArgs);
            }
            pkm->nature(pksm::Nature{u8(nextArg->Val->Integer)});
            break;
        case FATEFUL:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for FATEFUL", NumArgs);
            }
            pkm->fatefulEncounter((bool)nextArg->Val->Integer);
            break;
        case PP:
            if (NumArgs != 5)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for PP", NumArgs);
            }
            pkm->PP(nextArg->Val->Integer, getNextVarArg(nextArg)->Val->Integer);
            break;
        case PP_UPS:
            if (NumArgs != 5)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for PP_UPS", NumArgs);
            }
            pkm->PPUp(nextArg->Val->Integer, getNextVarArg(nextArg)->Val->Integer);
            break;
        case EGG:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG", NumArgs);
            }
            pkm->egg((bool)nextArg->Val->Integer);
            break;
        case NICKNAMED:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for NICKNAMED", NumArgs);
            }
            pkm->nicknamed((bool)nextArg->Val->Integer);
            break;
        case EGG_LOCATION:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG_LOCATION", NumArgs);
            }
            pkm->eggLocation(nextArg->Val->Integer);
            break;
        case MET_LEVEL:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_LEVEL", NumArgs);
            }
            pkm->metLevel(nextArg->Val->Integer);
            break;
        case OT_GENDER:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for OT_GENDER", NumArgs);
            }
            pkm->otGender(pksm::Gender{u8(nextArg->Val->Integer)});
            break;
        case ORIGINAL_GAME:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for ORIGINAL_GAME", NumArgs);
            }
            pkm->version(pksm::GameVersion(nextArg->Val->Integer));
            break;
        default:
            delete pkm;
            scriptFail(Parser, "Field number %i is invalid", (int)field);
    }
    if (gen == pksm::Generation::THREE)
    {
        std::copy(pkm->rawData(), pkm->rawData() + pkm->getLength(), data);
    }
    delete pkm;
}

void pkx_get_value(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data              = (u8*)Param[0]->Val->Pointer;
    pksm::Generation gen  = pksm::Generation(Param[1]->Val->Integer);
    PKX_FIELD field       = PKX_FIELD(Param[2]->Val->Integer);
    struct Value* nextArg = getNextVarArg(Param[2]);
    checkGen(Parser, gen);

    pksm::PKX* pkm = getPokemon(data, gen, false).release();

    switch (field)
    {
        case OT_NAME:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for OT_NAME", NumArgs);
            }
            ReturnValue->Val->Pointer = strToRet(pkm->otName());
            break;
        case TID:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for TID", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->TID();
            break;
        case SID:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for SID", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->SID();
            break;
        case SHINY:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for SHINY", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->shiny();
            break;
        case LANGUAGE:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for LANGUAGE", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = u8(pkm->language());
            break;
        case MET_LOCATION:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_LOCATION", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->metLocation();
            break;
        case MOVE:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MOVE", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->move(nextArg->Val->Integer);
            break;
        case BALL:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for BALL", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = u8(pkm->ball());
            break;
        case LEVEL:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for LEVEL", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->level();
            break;
        case GENDER:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for GENDER", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = u8(pkm->gender());
            break;
        case ABILITY:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for ABILITY", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = u16(pkm->ability());
            break;
        case IV_HP:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_HP", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(pksm::Stat::HP);
            break;
        case IV_ATK:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_ATK", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(pksm::Stat::ATK);
            break;
        case IV_DEF:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_DEF", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(pksm::Stat::DEF);
            break;
        case IV_SPATK:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_SPATK", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(pksm::Stat::SPATK);
            break;
        case IV_SPDEF:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_SPDEF", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(pksm::Stat::SPDEF);
            break;
        case IV_SPEED:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for IV_SPEED", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->iv(pksm::Stat::SPD);
            break;
        case NICKNAME:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for NICKNAME", NumArgs);
            }
            ReturnValue->Val->Pointer = strToRet(pkm->nickname());
            break;
        case ITEM:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for ITEM", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->heldItem();
            break;
        case POKERUS:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for POKERUS", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->pkrs();
            break;
        case EGG_DAY:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG_DAY", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->eggDate().day();
            break;
        case EGG_MONTH:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG_MONTH", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->eggDate().month();
            break;
        case EGG_YEAR:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG_YEAR", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->eggDate().year();
            break;
        case MET_DAY:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_DAY", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->metDate().day();
            break;
        case MET_MONTH:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_MONTH", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->metDate().month();
            break;
        case MET_YEAR:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_YEAR", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->metDate().year();
            break;
        case FORM:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for FORM", NumArgs);
            }
            ReturnValue->Val->UnsignedInteger = pkm->alternativeForm();
            break;
        case EV_HP:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_HP", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(pksm::Stat::HP);
            break;
        case EV_ATK:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_ATK", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(pksm::Stat::ATK);
            break;
        case EV_DEF:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_DEF", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(pksm::Stat::DEF);
            break;
        case EV_SPATK:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_SPATK", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(pksm::Stat::SPATK);
            break;
        case EV_SPDEF:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_SPDEF", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(pksm::Stat::SPDEF);
            break;
        case EV_SPEED:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EV_SPEED", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->ev(pksm::Stat::SPD);
            break;
        case SPECIES:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for SPECIES", NumArgs);
            }
            ReturnValue->Val->Integer = u16(pkm->species());
            break;
        case PID:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for PID", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->PID();
            break;
        case NATURE:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for NATURE", NumArgs);
            }
            ReturnValue->Val->Integer = u8(pkm->nature());
            break;
        case FATEFUL:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for FATEFUL", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->fatefulEncounter();
            break;
        case PP:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for PP", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->PP(nextArg->Val->Integer);
            break;
        case PP_UPS:
            if (NumArgs != 4)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for PP_UPS", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->PPUp(nextArg->Val->Integer);
            break;
        case EGG:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->egg();
            break;
        case NICKNAMED:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for NICKNAMED", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->nicknamed();
            break;
        case EGG_LOCATION:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for EGG_LOCATION", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->eggLocation();
            break;
        case MET_LEVEL:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for MET_LEVEL", NumArgs);
            }
            ReturnValue->Val->Integer = pkm->metLevel();
            break;
        case OT_GENDER:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for OT_GENDER", NumArgs);
            }
            ReturnValue->Val->Integer = u8(pkm->otGender());
            break;
        case ORIGINAL_GAME:
            if (NumArgs != 3)
            {
                delete pkm;
                scriptFail(Parser, "Incorrect number of args (%i) for ORIGINAL_GAME", NumArgs);
            }
            ReturnValue->Val->Integer = u8(pkm->version());
            break;
        default:
            delete pkm;
            scriptFail(Parser, "Field number %i is invalid", (int)field);
    }
    delete pkm;
}

void sav_inject_wcx(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8* data             = (u8*)Param[0]->Val->Pointer;
    pksm::Generation gen = pksm::Generation(Param[1]->Val->Integer);
    int slot             = Param[2]->Val->Integer;
    bool alternateFormat = (bool)(Param[3]->Val->Integer);
    checkGen(Parser, gen);

    std::unique_ptr<pksm::WCX> wcx = nullptr;

    switch (gen)
    {
        case pksm::Generation::FOUR:
            if (alternateFormat)
            {
                wcx = std::make_unique<pksm::WC4>(data);
            }
            else
            {
                wcx = std::make_unique<pksm::PGT>(data);
            }
            break;
        case pksm::Generation::FIVE:
            wcx = std::make_unique<pksm::PGF>(data);
            break;
        case pksm::Generation::SIX:
            wcx = std::make_unique<pksm::WC6>(data, alternateFormat);
            break;
        case pksm::Generation::SEVEN:
            wcx = std::make_unique<pksm::WC7>(data, alternateFormat);
            break;
        case pksm::Generation::LGPE:
            wcx = std::make_unique<pksm::WB7>(data, alternateFormat);
            break;
        case pksm::Generation::EIGHT:
            wcx = std::make_unique<pksm::WC8>(data);
            break;
        case pksm::Generation::UNUSED:
        case pksm::Generation::THREE:
        case pksm::Generation::ONE:
        case pksm::Generation::TWO:
            return;
    }

    if (wcx && gen == TitleLoader::save->generation())
    {
        TitleLoader::save->mysteryGift(*wcx, slot);
    }
}

void sav_wcx_free_slot(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    int ret = TitleLoader::save->currentGiftAmount();
    if ((size_t)ret == TitleLoader::save->maxWondercards())
    {
        ret--;
    }
    ReturnValue->Val->Integer = ret;
}

void pksm_base64_decode(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8** out     = (u8**)Param[0]->Val->Pointer;
    int* outSize = (int*)Param[1]->Val->Pointer;
    char* in     = (char*)Param[2]->Val->Pointer;
    int inSize   = Param[3]->Val->Integer;

    auto data = base64_decode(in, inSize);

    *outSize = data.size();
    *out     = (u8*)malloc(data.size());
    if (*out)
    {
        std::copy(data.begin(), data.end(), *out);
    }
}

void pksm_base64_encode(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char** out   = (char**)Param[0]->Val->Pointer;
    int* outSize = (int*)Param[1]->Val->Pointer;
    u8* in       = (u8*)Param[2]->Val->Pointer;
    int inSize   = Param[3]->Val->Integer;

    auto data = base64_encode(in, inSize);

    *outSize = data.size();
    *out     = (char*)strToRet(data);
}

void fetch_web_content(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char** out   = (char**)Param[0]->Val->Pointer;
    int* outSize = (int*)Param[1]->Val->Pointer;
    char* url    = (char*)Param[2]->Val->Pointer;

    std::string outData;
    auto fetch = Fetch::init(url, url[4] == 's', &outData, nullptr, "");
    auto ret   = Fetch::perform(fetch);
    if (ret.index() == 0)
    {
        ReturnValue->Val->Integer = -(int)std::get<0>(ret);
        *out                      = nullptr;
        *outSize                  = 0;
        return;
    }
    else
    {
        if (std::get<1>(ret) == CURLE_OK)
        {
            fetch->getinfo(CURLINFO_RESPONSE_CODE, &ReturnValue->Val->LongInteger);
            *out     = (char*)strToRet(outData);
            *outSize = outData.size();
            return;
        }
        else
        {
            ReturnValue->Val->Integer = -((int)std::get<1>(ret) + 100);
            *out                      = nullptr;
            *outSize                  = 0;
            return;
        }
    }
}

// struct JSON* json_new();
void json_new(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* ret = new nlohmann::json;
    // explicitly set it to invalid
    *ret                      = nlohmann::json::parse("{", nullptr, false);
    ReturnValue->Val->Pointer = (void*)ret;
}

// void json_parse(struct JSON* out, const char* data);
void json_parse(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* out = (nlohmann::json*)Param[0]->Val->Pointer;
    *out                = nlohmann::json::parse((char*)Param[1]->Val->Pointer, nullptr, false);
}

// void json_delete(struct JSON* freed);
void json_delete(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    delete ((nlohmann::json*)Param[0]->Val->Pointer);
}

// int json_is_valid(struct JSON* check);
void json_is_valid(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* check     = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = check->is_discarded() ? 0 : 1;
}

// int json_is_int(struct JSON* check);
void json_is_int(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* check     = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = check->is_number_integer() ? 1 : 0;
}

// int json_is_bool(struct JSON* check);
void json_is_bool(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* check     = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = check->is_boolean() ? 1 : 0;
}

// int json_is_string(struct JSON* check);
void json_is_string(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* check     = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = check->is_string() ? 1 : 0;
}

// int json_is_array(struct JSON* check);
void json_is_array(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* check     = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = check->is_array() ? 1 : 0;
}

// int json_is_object(struct JSON* check);
void json_is_object(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* check     = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = check->is_object() ? 1 : 0;
}

// int json_get_int(struct JSON* get);
void json_get_int(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* get       = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = get->get<int>();
}

// int json_get_bool(struct JSON* get);
void json_get_bool(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* get       = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = get->get<bool>() ? 1 : 0;
}

// const char* json_get_string(struct JSON* get);
void json_get_string(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* get       = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Pointer = strToRet(get->get_ref<std::string&>());
}

// int json_array_size(struct JSON* get);
void json_array_size(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* get       = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = get->size();
}

// struct JSON* json_array_element(struct JSON* get, int index);
void json_array_element(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* get       = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Pointer = &(*get)[Param[1]->Val->Integer];
}

// int json_object_contains(struct JSON* get, char* elemName);
void json_object_contains(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* get       = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Integer = get->contains((char*)Param[1]->Val->Pointer);
}

// struct JSON* json_object_element(struct JSON* get, const char* elemName);
void json_object_element(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    nlohmann::json* get       = (nlohmann::json*)Param[0]->Val->Pointer;
    ReturnValue->Val->Pointer = &(*get)[(char*)Param[1]->Val->Pointer];
}

// void sav_get_data(char* dataOut, unsigned int size, int off1, int off2);
void sav_get_data(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* data = (char*)Param[0]->Val->Pointer;
    u32 size   = Param[1]->Val->UnsignedInteger;
    s32 off1   = Param[2]->Val->Integer;
    s32 off2   = Param[3]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        std::copy(block->decryptedData() + off2, block->decryptedData() + off2 + size, data);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        scriptFail(Parser, "G3 editing API is not quite decided yet");
    }
    else
    {
        std::copy(TitleLoader::save->rawData().get() + off1 + off2, TitleLoader::save->rawData().get() + off1 + off2 + size, data);
    }
}
// void sav_set_data(char* data, unsigned int size, int off1, int off2);
void sav_set_data(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* data = (char*)Param[0]->Val->Pointer;
    u32 size   = Param[1]->Val->UnsignedInteger;
    s32 off1   = Param[2]->Val->Integer;
    s32 off2   = Param[3]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        std::copy(data, data + size, block->decryptedData() + off2);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        scriptFail(Parser, "G3 editing API is not quite decided yet");
    }
    else
    {
        std::copy(data, data + size, TitleLoader::save->rawData().get() + off1 + off2);
    }
}

// int sav_get_bit(int off1, int off2, int bit);
void sav_get_bit(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    s32 off1 = Param[0]->Val->Integer;
    s32 off2 = Param[1]->Val->Integer;
    s32 bit  = Param[2]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block                = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        ReturnValue->Val->Integer = pksm::FlagUtil::getFlag(block->decryptedData(), off2, bit);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        if (off1 != -1)
        {
            auto block                = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            ReturnValue->Val->Integer = pksm::FlagUtil::getFlag(block, off2, bit);
        }
        else
        {
            ReturnValue->Val->Integer = pksm::FlagUtil::getFlag(TitleLoader::save->rawData().get(), off2, bit);
        }
    }
    else
    {
        ReturnValue->Val->Integer = pksm::FlagUtil::getFlag(TitleLoader::save->rawData().get(), off1 + off2, bit);
    }
}
// void sav_set_bit(int bitVal, int off1, int off2, int bit);
void sav_set_bit(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    s32 data = Param[0]->Val->Integer;
    s32 off1 = Param[1]->Val->Integer;
    s32 off2 = Param[2]->Val->Integer;
    s32 bit  = Param[3]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        pksm::FlagUtil::setFlag(block->decryptedData(), off2, bit, data);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        if (off1 != -1)
        {
            auto block = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            pksm::FlagUtil::setFlag(block, off2, bit, data);
        }
        else
        {
            pksm::FlagUtil::setFlag(TitleLoader::save->rawData().get(), off2, bit, data);
        }
    }
    else
    {
        pksm::FlagUtil::setFlag(TitleLoader::save->rawData().get(), off1 + off2, bit, data);
    }
}

// char sav_get_byte(int off1, int off2);
void sav_get_byte(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    s32 off1 = Param[0]->Val->Integer;
    s32 off2 = Param[1]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block                          = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        ReturnValue->Val->UnsignedCharacter = block->decryptedData()[off2];
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        if (off1 != -1)
        {
            auto block                          = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            ReturnValue->Val->UnsignedCharacter = block[off2];
        }
        else
        {
            ReturnValue->Val->UnsignedCharacter = TitleLoader::save->rawData().get()[off2];
        }
    }
    else
    {
        ReturnValue->Val->UnsignedCharacter = TitleLoader::save->rawData()[off1 + off2];
    }
}
// void sav_set_byte(char data, int off1, int off2);
void sav_set_byte(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u8 data  = Param[0]->Val->UnsignedCharacter;
    s32 off1 = Param[1]->Val->Integer;
    s32 off2 = Param[2]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block                   = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        block->decryptedData()[off2] = data;
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        if (off1 != -1)
        {
            auto block  = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            block[off2] = data;
        }
        else
        {
            TitleLoader::save->rawData().get()[off2] = data;
        }
    }
    else
    {
        TitleLoader::save->rawData()[off1 + off2] = data;
    }
}

// short sav_get_short(int off1, int off2);
void sav_get_short(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    s32 off1 = Param[0]->Val->Integer;
    s32 off2 = Param[1]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block                             = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        ReturnValue->Val->UnsignedShortInteger = LittleEndian::convertTo<u16>(block->decryptedData() + off2);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        if (off1 != -1)
        {
            auto block                             = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            ReturnValue->Val->UnsignedShortInteger = LittleEndian::convertTo<u16>(block + off2);
        }
        else
        {
            ReturnValue->Val->UnsignedShortInteger = LittleEndian::convertTo<u16>(TitleLoader::save->rawData().get() + off2);
        }
    }
    else
    {
        ReturnValue->Val->UnsignedShortInteger = LittleEndian::convertTo<u16>(TitleLoader::save->rawData().get() + off1 + off2);
    }
}
// void sav_set_short(short data, int off1, int off2);
void sav_set_short(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u16 data = Param[0]->Val->UnsignedShortInteger;
    s32 off1 = Param[1]->Val->Integer;
    s32 off2 = Param[2]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        LittleEndian::convertFrom<u16>(block->decryptedData() + off2, data);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        if (off1 != -1)
        {
            auto block = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            LittleEndian::convertFrom<u16>(block + off2, data);
        }
        else
        {
            LittleEndian::convertFrom<u16>(TitleLoader::save->rawData().get() + off2, data);
        }
    }
    else
    {
        LittleEndian::convertFrom<u16>(TitleLoader::save->rawData().get() + off1 + off2, data);
    }
}

// int sav_get_int(int off1, int off2);
void sav_get_int(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    s32 off1 = Param[0]->Val->Integer;
    s32 off2 = Param[1]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block                        = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        ReturnValue->Val->UnsignedInteger = LittleEndian::convertTo<u32>(block->decryptedData() + off2);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        if (off1 != -1)
        {
            auto block                        = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            ReturnValue->Val->UnsignedInteger = LittleEndian::convertTo<u32>(block + off2);
        }
        else
        {
            ReturnValue->Val->UnsignedInteger = LittleEndian::convertTo<u32>(TitleLoader::save->rawData().get() + off2);
        }
    }
    else
    {
        ReturnValue->Val->UnsignedInteger = LittleEndian::convertTo<u32>(TitleLoader::save->rawData().get() + off1 + off2);
    }
}
// void sav_set_int(int data, int off1, int off2);
void sav_set_int(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    u32 data = Param[0]->Val->UnsignedInteger;
    s32 off1 = Param[1]->Val->Integer;
    s32 off2 = Param[2]->Val->Integer;
    if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        LittleEndian::convertFrom<u32>(block->decryptedData() + off2, data);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        if (off1 != -1)
        {
            auto block = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            LittleEndian::convertFrom<u32>(block + off2, data);
        }
        else
        {
            LittleEndian::convertFrom<u32>(TitleLoader::save->rawData().get() + off2, data);
        }
    }
    else
    {
        LittleEndian::convertFrom<u32>(TitleLoader::save->rawData().get() + off1 + off2, data);
    }
}

// void sav_set_string(char* string, int off1, int off2, unsigned int codepoints);
void sav_set_string(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    char* string   = (char*)Param[0]->Val->Pointer;
    s32 off1       = Param[1]->Val->Integer;
    s32 off2       = Param[2]->Val->Integer;
    u32 codepoints = Param[3]->Val->UnsignedInteger; // Includes null terminator
    if (TitleLoader::save->generation() == pksm::Generation::FOUR)
    {
        StringUtils::setString4(TitleLoader::save->rawData().get(), string, off1 + off2, codepoints);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        if (off1 != -1)
        {
            u8* data = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            StringUtils::setString3(data, string, off2, codepoints, TitleLoader::save->language() == pksm::Language::JPN);
        }
        else
        {
            StringUtils::setString3(
                TitleLoader::save->rawData().get(), string, off2, codepoints, TitleLoader::save->language() == pksm::Language::JPN);
        }
    }
    else if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        StringUtils::setString(block->decryptedData(), string, off2, codepoints, u'\0');
    }
    else
    {
        StringUtils::setString(TitleLoader::save->rawData().get(), string, off1 + off2, codepoints,
            TitleLoader::save->generation() == pksm::Generation::FIVE ? u'\uFFFF' : u'\0');
    }
}
// char* sav_get_string(int off1, int off2, unsigned int codepoints);
void sav_get_string(struct ParseState* Parser, struct Value* ReturnValue, struct Value** Param, int NumArgs)
{
    s32 off1       = Param[0]->Val->Integer;
    s32 off2       = Param[1]->Val->Integer;
    u32 codepoints = Param[1]->Val->UnsignedInteger; // Includes null terminator

    if (TitleLoader::save->generation() == pksm::Generation::FOUR)
    {
        std::string data          = StringUtils::getString4(TitleLoader::save->rawData().get(), off1 + off2, codepoints);
        ReturnValue->Val->Pointer = strToRet(data);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::THREE)
    {
        std::string ret;
        if (off1 != -1)
        {
            u8* data = ((pksm::Sav3*)TitleLoader::save.get())->getBlock(off1);
            ret      = StringUtils::getString3(data, off2, codepoints, TitleLoader::save->language() == pksm::Language::JPN);
        }
        else
        {
            ret = StringUtils::getString3(TitleLoader::save->rawData().get(), off2, codepoints, TitleLoader::save->language() == pksm::Language::JPN);
        }
        ReturnValue->Val->Pointer = strToRet(ret);
    }
    else if (TitleLoader::save->generation() == pksm::Generation::EIGHT)
    {
        auto block                = ((pksm::Sav8*)TitleLoader::save.get())->getBlock(off1);
        std::string data          = StringUtils::getString(block->decryptedData(), off2, codepoints, u'\0');
        ReturnValue->Val->Pointer = strToRet(data);
    }
    else
    {
        std::string data          = StringUtils::getString(TitleLoader::save->rawData().get(), off1 + off2, codepoints,
            TitleLoader::save->generation() == pksm::Generation::FIVE ? u'\uFFFF' : u'\0');
        ReturnValue->Val->Pointer = strToRet(data);
    }
}
}
