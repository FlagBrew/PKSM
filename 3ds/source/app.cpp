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

#include "app.hpp"
#include "appIcon.hpp"
#include "Archive.hpp"
#include "banks.hpp"
#include "Button.hpp"
#include "Configuration.hpp"
#include "fetch.hpp"
#include "format.h"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "io.hpp"
#include "loader.hpp"
#include "nlohmann/json.hpp"
#include "PkmUtils.hpp"
#include "random.hpp"
#include "revision.h"
#include "thread.hpp"
#include "TitleLoadScreen.hpp"
#include "utils/crypto.hpp"
#include "website.h"
#include <3ds.h>
#include <array>
#include <atomic>
#include <malloc.h>
#include <stdio.h>
#include <sys/stat.h>

namespace
{
    u32 old_time_limit;
    Handle hbldrHandle;
    std::atomic_flag moveIcon     = ATOMIC_FLAG_INIT;
    std::atomic_flag doCartScan   = ATOMIC_FLAG_INIT;
    std::atomic_flag continueI18N = ATOMIC_FLAG_INIT;

    struct asset
    {
        std::string url;
        std::string path;
        decltype(pksm::crypto::sha256({})) hash;
    };

    asset assets[2] = {
        {"https://raw.githubusercontent.com/piepie62/PKResources/master/pkm_spritesheet.t3x",
         "/3ds/PKSM/assets/pkm_spritesheet.t3x",   {0xa5, 0x0e, 0x59, 0x75, 0x00, 0xf0, 0xe1, 0x6a, 0x6e, 0xe9, 0xd4, 0x5b, 0xb3, 0x3b,
                0x9c, 0x08, 0xe8, 0x69, 0xc0, 0x1d, 0x10, 0x53, 0x3f, 0xe0, 0xbe, 0x7e, 0x2c, 0xa4,
                0xe7, 0x6d, 0xcc, 0x48}  },
        {"https://raw.githubusercontent.com/piepie62/PKResources/master/types_spritesheet.t3x",
         "/3ds/PKSM/assets/types_spritesheet.t3x", {0xea, 0x7f, 0x92, 0x86, 0x0a, 0x9b, 0x4d, 0x50, 0x3a, 0x0c, 0x2a, 0x6e, 0x48, 0x60,
                0xfb, 0x93, 0x1f, 0xd3, 0xd7, 0x7d, 0x6a, 0xbb, 0x1d, 0xdb, 0xac, 0x59, 0xeb, 0xf1,
                0x66, 0x34, 0xa4, 0x91}}
    };

    bool matchSha256HashFromFile(
        const std::string& path, const decltype(pksm::crypto::sha256({}))& sha)
    {
        bool match = false;
        auto in    = Archive::sd().file(path, FS_OPEN_READ);
        if (in)
        {
            size_t size = in->size();
            char* data  = new char[size];
            in->read(data, size);
            auto hash = pksm::crypto::sha256({(u8*)data, size});
            delete[] data;
            match = sha == hash;
            in->close();
        }
        return match;
    }

    bool assetsMatch(void)
    {
        for (auto& item : assets)
        {
            if (!io::exists(item.path))
            {
                return false;
            }
            if (!matchSha256HashFromFile(item.path, item.hash))
            {
                return false;
            }
        }
        return true;
    }

    Result downloadAdditionalAssets(void)
    {
        Result res = 0;

        for (auto item : assets)
        {
            bool downloadAsset = true;
            if (io::exists(item.path))
            {
                if (matchSha256HashFromFile(item.path, item.hash))
                {
                    downloadAsset = false;
                }
                else
                {
                    std::remove(item.path.c_str());
                }
            }
            if (downloadAsset)
            {
#if !CITRA_DEBUG
                u32 status;
                ACU_GetWifiStatus(&status);
                if (status == 0)
                {
                    return -1;
                }
#endif
                Result res1 = Fetch::download(item.url, item.path);
                if (R_FAILED(res1))
                {
                    return res1;
                }
            }
        }
        return res;
    }

    Result consoleDisplayError(const std::string& message, Result res)
    {
        moveIcon.clear();
        consoleInit(GFX_TOP, nullptr);
        fmt::print(FMT_STRING("\x1b[2;16H\x1b[34mPKSM v{:d}.{:d}.{:d}-{:s}\x1b[0m"), VERSION_MAJOR,
            VERSION_MINOR, VERSION_MICRO, GIT_REV);
        fmt::print(FMT_STRING("\x1b[5;1HError during startup: \x1b[31m0x{:08X}\x1b[0m"), (u32)res);
        fmt::print(FMT_STRING("\x1b[8;1HDescription: \x1b[33m{:s}\x1b[0m"), message);
        fmt::print(FMT_STRING("\x1b[29;16HPress START to exit."));
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
        while (aptMainLoop() && !(hidKeysDown() & KEY_START))
        {
            hidScanInput();
        }
        return res;
    }

    Result HBLDR_SetTarget(const char* path)
    {
        u32 pathLen = strlen(path) + 1;
        u32* cmdbuf = getThreadCommandBuffer();

        cmdbuf[0] = IPC_MakeHeader(2, 0, 2); // 0x20002
        cmdbuf[1] = IPC_Desc_StaticBuffer(pathLen, 0);
        cmdbuf[2] = (u32)path;

        Result rc = svcSendSyncRequest(hbldrHandle);
        if (R_SUCCEEDED(rc))
        {
            rc = cmdbuf[1];
        }
        return rc;
    }

    void backupExtData()
    {
        Archive::copyDir(Archive::data(), u"/", Archive::sd(), u"/3ds/PKSM/extDataBackup");
    }

    void backupBanks()
    {
        Archive::copyDir(Archive::sd(), u"/3ds/PKSM/banks", Archive::sd(), u"/3ds/PKSM/banksBkp");
    }

    bool update(std::string execPath)
    {
        u32 status;
        ACU_GetWifiStatus(&status);
        if (status == 0)
        {
            return false;
        }
        execPath        = execPath.substr(execPath.find(':') + 1);
        std::string url = "", path = "", retString = "";
        const std::string patronCode = Configuration::getInstance().patronCode();
        if (Configuration::getInstance().alphaChannel() && !patronCode.empty())
        {
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, ("patreon: " + patronCode).c_str());
            if (auto fetch = Fetch::init(
                    WEBSITE_URL "api/v2/patreon/update-check/PKSM", true, &retString, headers, ""))
            {
                moveIcon.clear();
                Gui::waitFrame(i18n::localize("UPDATE_CHECKING"));
                auto res = Fetch::perform(fetch);
                if (res.index() == 1)
                {
                    if (std::get<1>(res) != CURLE_OK)
                    {
                        Gui::error(i18n::localize("CURL_ERROR"), std::get<1>(res) + 100);
                    }
                    else
                    {
                        long status_code;
                        fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
                        switch (status_code)
                        {
                            case 200:
                            {
                                nlohmann::json retJson =
                                    nlohmann::json::parse(retString, nullptr, false);
                                if (const std::string hash =
                                        (retJson.is_object() && retJson.contains("hash"))
                                            ? retJson["hash"].get<std::string>()
                                            : GIT_REV;
                                    hash.substr(0, 8) != GIT_REV)
                                {
                                    url = WEBSITE_URL "api/v2/patreon/update/" + patronCode +
                                          "/PKSM/" + hash + "/";
                                    if (execPath.empty())
                                    {
                                        url  += "cia";
                                        path = "/3ds/PKSM/PKSM.cia";
                                    }
                                    else
                                    {
                                        url  += "3dsx";
                                        path = execPath + ".new";
                                    }
                                }
                                break;
                            }
                            case 204:
                                Gui::warn(i18n::localize("UPDATE_MISSING"));
                                break;
                            case 401:
                                Gui::warn(i18n::localize("NOT_PATRON") + '\n' +
                                          i18n::localize("INCIDENT_LOGGED"));
                                break;
                            case 502:
                                Gui::error(i18n::localize("HTTP_OFFLINE"), status_code);
                                break;
                            default:
                                Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                                break;
                        }
                    }
                }
            }
            curl_slist_free_all(headers);
        }
        else if (auto fetch =
                     Fetch::init("https://api.github.com/repos/FlagBrew/PKSM/releases/latest", true,
                         &retString, nullptr, ""))
        {
            moveIcon.clear();
            Gui::waitFrame(i18n::localize("UPDATE_CHECKING"));
            auto res = Fetch::perform(fetch);
            if (res.index() == 1)
            {
                if (std::get<1>(res) != CURLE_OK)
                {
                    Gui::error(i18n::localize("CURL_ERROR"), std::get<1>(res) + 100);
                }
                else
                {
                    long status_code;
                    fetch->getinfo(CURLINFO_RESPONSE_CODE, &status_code);
                    switch (status_code)
                    {
                        case 200:
                        {
                            nlohmann::json retJson =
                                nlohmann::json::parse(retString, nullptr, false);
                            if (retJson.is_discarded() || !retJson.contains("tag_name") ||
                                !retJson["tag_name"].is_string())
                            {
                                Gui::warn(i18n::localize("UPDATE_CHECK_ERROR_BAD_JSON_1") + '\n' +
                                          i18n::localize("UPDATE_CHECK_ERROR_BAD_JSON_2"));
                            }
                            else
                            {
                                std::string newVersion = retJson["tag_name"].get<std::string>();
                                size_t pos             = 0;
                                size_t pos2            = 0;
                                int newMajor           = std::stoi(newVersion, &pos);
                                int newMinor = std::stoi(newVersion.substr(pos + 1), &pos2);
                                int newMicro = std::stoi(newVersion.substr(pos + pos2 + 2));

                                if (newMajor > VERSION_MAJOR ||
                                    (newMajor == VERSION_MAJOR && newMinor > VERSION_MINOR) ||
                                    (newMajor == VERSION_MAJOR && newMinor == VERSION_MINOR &&
                                        newMicro > VERSION_MICRO))
                                {
                                    url = "https://github.com/FlagBrew/PKSM/releases/download/" +
                                          newVersion + "/PKSM";
                                    if (execPath != "")
                                    {
                                        url  += ".3dsx";
                                        path = execPath + ".new";
                                    }
                                    else
                                    {
                                        url  += ".cia";
                                        path = "/3ds/PKSM/PKSM.cia";
                                    }
                                }
                            }
                            break;
                        }
                        case 502:
                            Gui::error(i18n::localize("HTTP_OFFLINE"), status_code);
                            break;
                        default:
                            Gui::error(i18n::localize("HTTP_UNKNOWN_ERROR"), status_code);
                            break;
                    }
                }
            }
        }
        if (!url.empty())
        {
            Gui::waitFrame(i18n::localize("UPDATE_FOUND_BACKUP"));
            backupExtData();
            backupBanks();
            Gui::waitFrame(i18n::localize("UPDATE_FOUND_DOWNLOAD"));
            std::string fileName = path.substr(path.find_last_of('/') + 1);
            Result res           = Fetch::download(
                url, path, "",
                [](void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal,
                    curl_off_t ulnow)
                {
                    Gui::showDownloadProgress(*(std::string*)clientp, dlnow / 1024, dltotal / 1024);
                    return 0;
                },
                &fileName);
            if (R_FAILED(res))
            {
                Gui::error(i18n::localize("UPDATE_FOUND_BUT_FAILED_DOWNLOAD"), res);
                Archive::sd().deleteFile(path);
                return false;
            }

            Gui::waitFrame(i18n::localize("UPDATE_INSTALLING"));
            if (execPath != "")
            {
                // Stop using the 3DSX
                romfsExit();
                if (R_FAILED(Archive::moveFile(Archive::sd(), path, Archive::sd(), execPath)))
                {
                    // RUN, THE INSTALL FAILED
                    romfsInit();
                    Archive::sd().deleteFile(path);
                    return false;
                }
                else
                {
                    // No need to reinit ROMFS, as we're definitely about to reboot
                    // And if we don't reboot, then catastrophic errors are likely. Honestly,
                    // probably a good thing
                    return true;
                }
            }
            else
            {
                // Adapted from https://github.com/joel16/3DShell/blob/master/source/cia.c
                AM_TitleEntry title;
                Handle dstHandle;
                auto ciaFile = Archive::sd().file(path, FS_OPEN_READ);
                if (ciaFile)
                {
                    if (R_FAILED(res = AM_GetCiaFileInfo(
                                     MEDIATYPE_SD, &title, std::get<0>(ciaFile->getRawHandle()))))
                    {
                        Gui::error(i18n::localize("BAD_CIA_FILE"), res);
                        ciaFile->close();
                        return false;
                    }

                    if (R_FAILED(res = AM_StartCiaInstall(MEDIATYPE_SD, &dstHandle)))
                    {
                        Gui::error(i18n::localize("CIA_INSTALL_START_FAIL"), res);
                        ciaFile->close();
                        return false;
                    }

                    size_t bufSize = 0x10000;
                    u8* buf        = (u8*)memalign(0x1000, bufSize);
                    u8 backupBuf[0x1000];
                    if (!buf)
                    {
                        buf     = backupBuf;
                        bufSize = sizeof(backupBuf);
                    }
                    u32 bytesWritten, bytesRead;
                    u64 offset          = 0;
                    bool ciaInstallGood = true;
                    do
                    {
                        memset(buf, 0, bufSize);

                        bytesRead = ciaFile->read(buf, bufSize);
                        if (R_FAILED(ciaFile->result()))
                        {
                            Gui::error(i18n::localize("CIA_UPDATE_READ_FAIL"), ciaFile->result());
                            ciaFile->close();
                            FSFILE_Close(dstHandle);
                            return false;
                        }

                        if (R_FAILED(res = FSFILE_Write(dstHandle, &bytesWritten, offset, buf,
                                         bytesRead, FS_WRITE_FLUSH)))
                        {
                            Gui::error(i18n::localize("CIA_UPDATE_WRITE_FAIL"), res);
                            ciaFile->close();
                            FSFILE_Close(dstHandle);
                            return false;
                        }

                        if (bytesWritten != bytesRead)
                        {
                            ciaInstallGood = false;
                        }

                        offset += bytesRead;
                    }
                    while (offset < ciaFile->size() && ciaInstallGood);

                    if (buf != backupBuf)
                    {
                        free(buf);
                    }

                    if (!ciaInstallGood)
                    {
                        AM_CancelCIAInstall(dstHandle);
                        ciaFile->close();
                        Gui::warn("Bytes written doesn't match bytes read:\n" +
                                  std::to_string(bytesWritten) + " vs " +
                                  std::to_string(bytesRead));
                        return false;
                    }

                    if (R_FAILED(res = AM_FinishCiaInstall(dstHandle)))
                    {
                        Gui::error(i18n::localize("CIA_INSTALL_FINISH_FAIL"), res);
                        ciaFile->close();
                        return false;
                    }

                    ciaFile->close();

                    Archive::sd().deleteFile(path);

                    return true;
                }
            }
        }
        return false;
    }

    void cartScan(void*)
    {
#if !CITRA_DEBUG
        bool oldCardIn;
        FSUSER_CardSlotIsInserted(&oldCardIn);

        while (doCartScan.test_and_set())
        {
            bool cardIn = false;

            FSUSER_CardSlotIsInserted(&cardIn);
            if (cardIn != oldCardIn)
            {
                bool power;
                FSUSER_CardSlotGetCardIFPowerStatus(&power);
                if (cardIn)
                {
                    if (!power)
                    {
                        FSUSER_CardSlotPowerOn(&power);
                    }
                    while (!power && doCartScan.test_and_set())
                    {
                        FSUSER_CardSlotGetCardIFPowerStatus(&power);
                    }
                    svcSleepThread(500'000'000);
                    for (size_t i = 0; i < 10; i++)
                    {
                        if ((oldCardIn = TitleLoader::scanCard()))
                        {
                            break;
                        }
                    }
                }
                else
                {
                    FSUSER_CardSlotPowerOff(&power);
                    TitleLoader::scanCard();
                    oldCardIn = false;
                }
            }
        }
#endif
    }

    void iconThread(void*)
    {
        int x = 176, y = 96;
        u16 w, h;
        bool up   = pksm::randomNumber(0, 1) ? true : false;
        bool left = pksm::randomNumber(0, 1) ? true : false;
        u8 yMag   = pksm::randomNumber(0, 1) + 1;
        u8 xMag   = pksm::randomNumber(0, 1) + 1;
        while (moveIcon.test_and_set())
        {
            int xOff = 0;
            std::fill_n(gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &w, &h), 240 * 400 * 3, 0);
            for (const auto& line : bootSplash)
            {
                std::copy(line.begin(), line.end(),
                    gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &w, &h) + (x + xOff++) * 3 * 240 + y * 3);
            }

            if (up)
            {
                y -= yMag;
            }
            else
            {
                y += yMag;
            }
            if (y >= 240 - 48 || y <= 0)
            {
                yMag = pksm::randomNumber(0, 1) + 1;
                up   = !up;
            }

            if (left)
            {
                x -= xMag;
            }
            else
            {
                x += xMag;
            }
            if (x >= 400 - 48 || x <= 0)
            {
                xMag = pksm::randomNumber(0, 1) + 1;
                left = !left;
            }

            gfxFlushBuffers();
            gfxSwapBuffersGpu();
            gspWaitForVBlank();
        }
    }

    void i18nThread(void*)
    {
        static constexpr pksm::Language languages[] = {pksm::Language::JPN, pksm::Language::ENG,
            pksm::Language::FRE, pksm::Language::ITA, pksm::Language::GER, pksm::Language::SPA,
            pksm::Language::KOR, pksm::Language::CHS, pksm::Language::CHT, pksm::Language::NL,
            pksm::Language::PT, pksm::Language::RU, pksm::Language::RO};
        for (const auto& i : languages)
        {
            if (continueI18N.test_and_set())
            {
                i18n::init(i);
            }
            else
            {
                return;
            }
        }
    }

    Result rebootToPKSM(const std::string& execPath)
    {
        Result res = -1;
        if (execPath.empty())
        {
            aptSetChainloaderToSelf();
            res = 0;
        }
        else
        {
#if !CITRA_DEBUG
            std::string path = execPath.substr(execPath.find('/'));
            res              = HBLDR_SetTarget(path.c_str());
#endif
        }
        if (R_FAILED(res))
        {
            Gui::warn(
                i18n::localize("UPDATE_SUCCESS_1") + '\n' + i18n::localize("UPDATE_SUCCESS_2"));
        }
        return -1;
    }

    // Also checks modified time. If the checksum file is newer than the file, recalculate and write
    // checksum If checksum file doesn't exist, calculate and write checksum
    decltype(pksm::crypto::sha256({})) readGiftChecksum(const std::string& fileName)
    {
        struct
        {
            bool exists;
            u64 mtime;
        } fileInfo, checksumFileInfo;
        struct stat mystat;

        const std::string path         = "/3ds/PKSM/mysterygift/" + fileName;
        const std::string checksumPath = path + ".sha";
        const std::string romfsPath    = "romfs:/mg/" + fileName;
        decltype(pksm::crypto::sha256({})) ret;

        fileInfo.exists = (stat(path.c_str(), &mystat) == 0);
        archive_getmtime(path.c_str(), &fileInfo.mtime);
        checksumFileInfo.exists = (stat(checksumPath.c_str(), &mystat) == 0);
        archive_getmtime(checksumPath.c_str(), &checksumFileInfo.mtime);
        stat(romfsPath.c_str(), &mystat);

        if (mystat.st_mtim.tv_sec > (s64)fileInfo.mtime)
        {
            Archive::sd().deleteFile(path);
            Archive::sd().deleteFile(checksumPath);

            fileInfo.exists         = false;
            checksumFileInfo.exists = false;
        }

        // Either both exist and file was modified before checksum file, or checksum file exists and
        // file doesn't
        if (checksumFileInfo.exists &&
            (!fileInfo.exists || (fileInfo.exists && fileInfo.mtime <= checksumFileInfo.mtime)))
        {
            FILE* correctSum = fopen(checksumPath.c_str(), "rb");
            if (correctSum)
            {
                fread(ret.data(), 1, 32, correctSum);
                fclose(correctSum);
                return ret;
            }
        }

        FILE* file = fopen(romfsPath.c_str(), "rb");
        if (file)
        {
            fseek(file, 0, SEEK_END);
            size_t size = ftell(file);
            rewind(file);
            std::unique_ptr<u8[]> data = std::unique_ptr<u8[]>(new u8[size]);
            fread(data.get(), 1, size, file);
            fclose(file);

            ret = pksm::crypto::sha256({data.get(), size});

            file = fopen(checksumPath.c_str(), "wb");
            if (file)
            {
                fwrite(ret.data(), 1, ret.size(), file);
                fclose(file);
            }
        }

        return ret;
    }

    void updateGifts(void)
    {
#if !CITRA_DEBUG
        u32 status;
        ACU_GetWifiStatus(&status);
        if (status == 0)
        {
            return;
        }
#endif
        struct giftCurlData
        {
            giftCurlData(FILE* file, const std::string& fileName)
                : fileName(fileName), file(file), response(0), prevDownload(0), addedToTotal(false)
            {
            }

            std::string fileName;
            FILE* file;
            pksm::crypto::SHA256 shaContext;
            long response;
            u32 prevDownload;
            bool addedToTotal;
        };

        curl_write_callback giftWriteFunc = [](char* data, size_t size, size_t nitems, void* out)
        {
            giftCurlData* writeMe = (giftCurlData*)out;

            if (data)
            {
                writeMe->shaContext.update({(u8*)data, size * nitems});

                return fwrite(data, size, nitems, writeMe->file);
            }
            else
            {
                return size * nitems;
            }
        };

        static constexpr std::array<pksm::Generation, 4> mgGens = {pksm::Generation::FOUR,
            pksm::Generation::FIVE, pksm::Generation::SIX, pksm::Generation::SEVEN};

        std::atomic<size_t> filesDone = 0;
        size_t filesToDownload        = 0;
        std::vector<giftCurlData> curlVars;
        curlVars.reserve(mgGens.size() * 2);

        Gui::waitFrame(i18n::localize("MYSTERY_GIFT_CHECK"));

        for (const auto& gen : mgGens)
        {
            for (const std::string& fileName :
                {"sheet" + (std::string)gen + ".json.bz2", "data" + (std::string)gen + ".bin.bz2"})
            {
                decltype(pksm::crypto::sha256({})) checksum = readGiftChecksum(fileName);

                std::vector<u8> recvChecksum;
                if (auto fetch = Fetch::init(
                        WEBSITE_URL "api/v2/files/download/mystery-gift/" + fileName + ".sha", true,
                        nullptr, nullptr, ""))
                {
                    fetch->setopt(
                        CURLOPT_WRITEFUNCTION, (curl_write_callback)[](char* buffer, size_t size,
                                                   size_t items, void* userThing) {
                            std::vector<u8>* recv = (std::vector<u8>*)userThing;
                            recv->insert(recv->end(), (u8*)buffer, (u8*)buffer + size * items);
                            return size * items;
                        });
                    fetch->setopt(CURLOPT_WRITEDATA, &recvChecksum);

                    Fetch::perform(fetch);

                    long response;

                    fetch->getinfo(CURLINFO_RESPONSE_CODE, &response);
                    if (response == 200)
                    {
                        if (memcmp(recvChecksum.data(), checksum.data(),
                                std::min(checksum.size(), recvChecksum.size())))
                        {
                            if (fetch = Fetch::init(
                                    WEBSITE_URL "api/v2/files/download/mystery-gift/" + fileName,
                                    true, nullptr, nullptr, ""))
                            {
                                std::string outPath = "/3ds/PKSM/mysterygift/" + fileName;
                                FILE* outFile       = fopen(outPath.c_str(), "wb");

                                if (outFile)
                                {
                                    curlVars.emplace_back(outFile, outPath);
                                    fetch->setopt(CURLOPT_WRITEFUNCTION, giftWriteFunc);
                                    fetch->setopt(CURLOPT_WRITEDATA, &curlVars.back());

                                    if (Fetch::performAsync(fetch,
                                            [progress = curlVars.end() - 1, &filesDone](
                                                CURLcode code, std::shared_ptr<Fetch> fetch)
                                            {
                                                filesDone++;
                                                fclose(progress->file);
                                                fetch->getinfo(
                                                    CURLINFO_RESPONSE_CODE, &progress->response);
                                            }) != CURLM_OK)
                                    {
                                        filesDone++;
                                        fclose(outFile);
                                    }
                                    else
                                    {
                                        filesToDownload++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        while (filesDone != filesToDownload)
        {
            Gui::waitFrame(fmt::format(
                i18n::localize("MYSTERY_GIFT_DOWNLOAD"), (size_t)filesDone, filesToDownload));
            svcSleepThread(50'000'000);
        }

        for (auto& info : curlVars)
        {
            std::string shaFile = info.fileName + ".sha";
            if (info.response != 200)
            {
                remove(info.fileName.c_str());
                remove(shaFile.c_str());
            }
            else
            {
                decltype(pksm::crypto::sha256({})) checksum = info.shaContext.finish();

                FILE* f = fopen(shaFile.c_str(), "wb");
                if (f)
                {
                    fwrite(checksum.data(), 1, checksum.size(), f);
                    fclose(f);
                }
            }
        }
    }
}

Result App::init(const std::string& execPath)
{
    Result res;

    hidInit();
    gfxInitDefault();
    Threads::init(1);

    moveIcon.test_and_set();
    Threads::create(iconThread);

#if !CITRA_DEBUG
    if (R_FAILED(res = svcConnectToPort(&hbldrHandle, "hb:ldr")))
    {
        return consoleDisplayError(
            "Rosalina sysmodule has not been found.\n\nMake sure you're running latest Luma3DS.",
            res);
    }
#endif
    APT_GetAppCpuTimeLimit(&old_time_limit);
    APT_SetAppCpuTimeLimit(30);

    if (R_FAILED(res = cfguInit()))
    {
        return consoleDisplayError("cfguInit failed.", res);
    }
    if (R_FAILED(res = romfsInit()))
    {
        return consoleDisplayError("romfsInit failed.", res);
    }
    if (R_FAILED(res = Archive::init(execPath)))
    {
        return consoleDisplayError("Archive::init failed.", res);
    }
    if (R_FAILED(res = pxiDevInit()))
    {
        return consoleDisplayError("pxiDevInit failed.", res);
    }
    if (R_FAILED(res = amInit()))
    {
        return consoleDisplayError("amInit failed.", res);
    }
    if (R_FAILED(res = acInit()))
    {
        return consoleDisplayError("acInit failed.", res);
    }

    u32* socketBuffer = (u32*)memalign(SOC_ALIGN, SOC_BUFFERSIZE);
    if (socketBuffer == NULL)
    {
        return consoleDisplayError("Failed to create socket buffer.", -1);
    }
    if (socInit(socketBuffer, SOC_BUFFERSIZE))
    {
        return consoleDisplayError("socInit failed.", -1);
    }

    if (CURLcode code = curl_global_init(CURL_GLOBAL_NOTHING))
    {
        return consoleDisplayError("cURL init falied", (Result)code);
    }

    if (R_FAILED(Fetch::initMulti()))
    {
        return consoleDisplayError("Initializing network connection failed.", -1);
    }

    if (R_FAILED(res = downloadAdditionalAssets()))
    {
        return consoleDisplayError("Additional assets download failed.\n\nAlways make sure you're "
                                   "connected to the internet and on the lastest version.",
            res);
    }
    if (R_FAILED(res = Gui::init()))
    {
        return consoleDisplayError("Gui::init failed.", res);
    }

    i18n::addCallbacks(i18n::initGui, i18n::exitGui);
    moveIcon.clear();
    i18n::init(Configuration::getInstance().language());

    PkmUtils::initDefaults();

    if (!assetsMatch())
    {
        Gui::warn("Additional assets are not correct.\nPress A to start PKSM update...");
        if (!update(execPath))
        {
            Gui::warn("PKSM update failed.\nTry downloading assets manually before restarting.");
            return -1;
        }
        else
        {
            return rebootToPKSM(execPath);
        }
    }

    if (Configuration::getInstance().autoUpdate() && update(execPath))
    {
        return rebootToPKSM(execPath);
    }

    if (Configuration::getInstance().autoUpdate())
    {
        updateGifts();
    }

    if (R_FAILED(res = Banks::init()))
    {
        return consoleDisplayError("Banks::init failed.", res);
    }

    TitleLoader::init();

    Threads::executeTask([](void*) { TitleLoader::scanTitles(); }, nullptr);
    TitleLoader::scanSaves();

    doCartScan.test_and_set();
    Threads::create(cartScan, nullptr);

    continueI18N.test_and_set();
    Threads::executeTask(i18nThread, nullptr);

    Gui::setScreen(std::make_unique<TitleLoadScreen>());
    // uncomment when needing to debug with GDB
    // consoleDebugInit(debugDevice_SVC);

    return 0;
}

Result App::exit(void)
{
    moveIcon.clear();
    continueI18N.clear();
    svcCloseHandle(hbldrHandle);
    TitleLoader::exit();
    Gui::exit();
    Fetch::exitMulti();
    curl_global_cleanup();
    socExit();
    acExit();
    doCartScan.clear();
    Threads::exit();
    i18n::exit();
    amExit();
    pxiDevExit();
    Archive::exit();
    romfsExit();
    cfguExit();

    if (old_time_limit != UINT32_MAX)
    {
        APT_SetAppCpuTimeLimit(old_time_limit);
    }

    gfxExit();
    hidExit();

    return 0;
}

void App::end() {}
