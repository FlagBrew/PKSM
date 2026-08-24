/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2026 Bernardo Giordano, Admiral Fish, piepie62
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

#include "WirelessTransfer.hpp"
#include "Configuration.hpp"
#include "DateTime.hpp"
#include "gui.hpp"
#include "i18n_ext.hpp"
#include "io.hpp"
#include "loader.hpp"
#include "MainMenu.hpp"
#include "nlohmann/json.hpp"
#include "revision.h"
#include "Sav.hpp"
#include "ScreenStack.hpp"
#include "server.hpp"
#include "Title.hpp"
#include "TransferProtocol.hpp"
#include "utils/format.hpp"
#include "utils/logging.hpp"
#include <3ds.h>
#include <algorithm>
#include <arpa/inet.h>
#include <array>
#include <atomic>
#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <format>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <poll.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

namespace
{
    constexpr u16 TRANSFER_PORT              = 8000;
    constexpr size_t MAX_TRANSFER_SIZE       = 32 * 1024 * 1024;
    constexpr size_t MULTIPART_HEAD_SIZE     = 64 * 1024;
    constexpr size_t NETWORK_CHUNK_SIZE      = 32 * 1024;
    constexpr int NETWORK_TIMEOUT_MS         = 15000;
    constexpr int MAX_AUTHENTICATION_TRIES   = 5;
    constexpr const char* TEMPORARY_UPLOAD   = "/3ds/PKSM/transfer_upload.tmp";
    constexpr const char* TRANSFER_DIRECTORY = "/3ds/PKSM/transfers";
    constexpr const char* WIRELESS_BACKUPS   = "/3ds/PKSM/backups/wireless";

    constexpr std::array<std::string_view, 9> dsIds = {
        "ADA", // Diamond
        "APA", // Pearl
        "CPU", // Platinum
        "IPK", // HeartGold
        "IPG", // SoulSilver
        "IRB", // Black
        "IRA", // White
        "IRE", // Black 2
        "IRD"  // White 2
    };
    constexpr std::array<pksm::GameVersion, 9> dsVersions = {pksm::GameVersion::D,
        pksm::GameVersion::P, pksm::GameVersion::Pt, pksm::GameVersion::HG, pksm::GameVersion::SS,
        pksm::GameVersion::B, pksm::GameVersion::W, pksm::GameVersion::B2, pksm::GameVersion::W2};

    enum class ReceiveState
    {
        Idle,
        Waiting,
        Complete,
        Failed
    };

    struct Metadata
    {
        std::string titleId;
        std::string titleName;
        std::string dataType = "save";
        std::string backupName;
        std::string fileName = "main";
    };

    std::mutex transferMutex;
    std::atomic<ReceiveState> receiveState  = ReceiveState::Idle;
    std::atomic<int> authenticationFailures = 0;
    std::string receiveToken;
    std::string receiveError;
    std::string lastPeerAddress;
    std::string lastTarget;
    Metadata receivedMetadata;
    std::shared_ptr<pksm::Sav> pendingSave;
    bool loadedFromTransfer = false;
    // Where the save loaded over the air lives on the SD card, so edits keep landing there.
    std::string persistedPath;

    std::string jsonString(const nlohmann::json& object, const char* key, std::string fallback = "")
    {
        auto value = object.find(key);
        return value != object.end() && value->is_string() ? value->get<std::string>()
                                                           : std::move(fallback);
    }

    Server::HttpResponse jsonError(int status, const std::string& message)
    {
        nlohmann::json response;
        response["ok"]    = false;
        response["error"] = message;
        return {status, "application/json", response.dump()};
    }

    void setReceiveError(const std::string& error)
    {
        std::lock_guard<std::mutex> lock(transferMutex);
        receiveError = error;
    }

    std::string boundaryFromContentType(const std::string& contentType)
    {
        size_t position = contentType.find("boundary=");
        if (position == std::string::npos)
        {
            return "";
        }
        std::string boundary = contentType.substr(position + 9);
        size_t separator     = boundary.find(';');
        if (separator != std::string::npos)
        {
            boundary.resize(separator);
        }
        while (!boundary.empty() && boundary.back() == ' ')
        {
            boundary.pop_back();
        }
        if (boundary.size() >= 2 && boundary.front() == '"' && boundary.back() == '"')
        {
            boundary = boundary.substr(1, boundary.size() - 2);
        }
        return boundary;
    }

    struct FileByteReader : TransferProtocol::ByteReader
    {
        FILE* input = nullptr;

        FileByteReader(const std::string& path, uint64_t offset)
        {
            input = fopen(path.c_str(), "rb");
            if (input && fseek(input, (long)offset, SEEK_SET) != 0)
            {
                fclose(input);
                input = nullptr;
            }
        }

        ~FileByteReader() override
        {
            if (input)
            {
                fclose(input);
            }
        }

        bool good() const { return input != nullptr; }

        size_t read(void* output, size_t size) override
        {
            return input ? fread(output, 1, size, input) : 0;
        }
    };

    struct SaveExtractSink : TransferProtocol::ExtractSink
    {
        std::shared_ptr<u8[]> currentData;
        size_t currentSize     = 0;
        size_t currentPosition = 0;
        std::string currentName;
        int currentPriority = 0;

        std::shared_ptr<pksm::Sav> save;
        std::string saveName;
        int savePriority = 0;

        bool directory(const std::string&) override { return true; }

        bool beginFile(const std::string& relativePath, uint32_t size) override
        {
            currentData.reset();
            currentSize     = size;
            currentPosition = 0;
            currentName     = relativePath;
            size_t slash    = relativePath.find_last_of('/');
            std::string base =
                slash == std::string::npos ? relativePath : relativePath.substr(slash + 1);
            currentPriority = base == "main" ? 2 : 1;
            if (size > 0 && size <= MAX_TRANSFER_SIZE && currentPriority >= savePriority)
            {
                currentData = std::shared_ptr<u8[]>(new u8[size]);
            }
            return true;
        }

        bool writeFile(const void* data, size_t size) override
        {
            if (!currentData)
            {
                return true;
            }
            if (currentPosition + size > currentSize)
            {
                return false;
            }
            std::memcpy(currentData.get() + currentPosition, data, size);
            currentPosition += size;
            return true;
        }

        void endFile() override
        {
            if (currentData && currentPosition == currentSize)
            {
                std::unique_ptr<pksm::Sav> parsed = pksm::Sav::getSave(currentData, currentSize);
                if (parsed && currentPriority > savePriority)
                {
                    save         = std::move(parsed);
                    saveName     = currentName;
                    savePriority = currentPriority;
                }
            }
            currentData.reset();
        }
    };

    bool readRawSave(const std::string& path, uint64_t offset, uint64_t size,
        std::shared_ptr<pksm::Sav>& outSave)
    {
        if (size == 0 || size > MAX_TRANSFER_SIZE)
        {
            return false;
        }
        FileByteReader input(path, offset);
        if (!input.good())
        {
            return false;
        }
        std::shared_ptr<u8[]> data(new u8[(size_t)size]);
        size_t total = 0;
        while (total < size)
        {
            size_t read =
                input.read(data.get() + total, std::min(NETWORK_CHUNK_SIZE, (size_t)size - total));
            if (read == 0)
            {
                return false;
            }
            total += read;
        }
        std::unique_ptr<pksm::Sav> parsed = pksm::Sav::getSave(data, (size_t)size);
        if (!parsed)
        {
            return false;
        }
        outSave = std::move(parsed);
        return true;
    }

    Server::HttpResponse handleInfo(const std::string&, const std::string&)
    {
        nlohmann::json info;
        info["device"] = "PKSM (3DS)";
        info["version"] =
            std::format("{:d}.{:d}.{:d}", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
        info["maxUploadBytes"] = MAX_TRANSFER_SIZE;
        info["freeSpaceBytes"] = 0;
        return {200, "application/json", info.dump()};
    }

    Server::HttpResponse handleUpload(const Server::UploadRequest& request)
    {
        std::string expectedToken;
        {
            std::lock_guard<std::mutex> lock(transferMutex);
            expectedToken = receiveToken;
        }
        std::string token = TransferProtocol::headerValue(request.headers, "X-CP-Token");
        if (!TransferProtocol::constantTimeEquals(token, expectedToken))
        {
            int failures = authenticationFailures.fetch_add(1) + 1;
            if (failures >= MAX_AUTHENTICATION_TRIES)
            {
                setReceiveError("Too many invalid PIN attempts.");
                receiveState.store(ReceiveState::Failed);
            }
            return jsonError(403, "Invalid token");
        }

        std::string contentType = TransferProtocol::headerValue(request.headers, "Content-Type");
        std::string boundary    = boundaryFromContentType(contentType);
        if (boundary.empty())
        {
            setReceiveError("The sender omitted the multipart boundary.");
            return jsonError(400, "Missing boundary");
        }

        FileByteReader headReader(request.bodyPath, 0);
        if (!headReader.good())
        {
            setReceiveError("The received upload could not be opened.");
            return jsonError(500, "Failed to open upload");
        }
        size_t headSize = std::min((uint64_t)MULTIPART_HEAD_SIZE, request.bodyLength);
        std::string head(headSize, '\0');
        if (headSize > 0 && headReader.read(head.data(), headSize) != headSize)
        {
            setReceiveError("The received upload could not be read.");
            return jsonError(400, "Bad upload");
        }

        std::string metadataJson;
        std::string parseError;
        uint64_t fileOffset = 0;
        uint64_t fileLength = 0;
        if (!TransferProtocol::parseMultipart(head, request.bodyLength, boundary, metadataJson,
                fileOffset, fileLength, parseError))
        {
            setReceiveError(parseError);
            return jsonError(400, "Bad upload");
        }

        nlohmann::json metadata = nlohmann::json::parse(metadataJson, nullptr, false);
        if (metadata.is_discarded() || !metadata.is_object())
        {
            setReceiveError("The sender supplied invalid metadata.");
            return jsonError(400, "Invalid meta");
        }

        std::shared_ptr<pksm::Sav> save;
        std::string receivedFileName = jsonString(metadata, "fileName", "main");
        bool isZip = metadata.contains("isZip") && metadata["isZip"].is_boolean() &&
                     metadata["isZip"].get<bool>();
        if (isZip)
        {
            FileByteReader input(request.bodyPath, fileOffset);
            SaveExtractSink output;
            std::string extractError;
            if (!input.good() ||
                !TransferProtocol::extractZip(input, fileLength, output, {}, {}, extractError) ||
                !output.save)
            {
                setReceiveError(extractError.empty() ? "No supported save was found in the package."
                                                     : extractError);
                return jsonError(400, "No supported save in package");
            }
            save             = std::move(output.save);
            receivedFileName = std::move(output.saveName);
        }
        else if (!readRawSave(request.bodyPath, fileOffset, fileLength, save))
        {
            setReceiveError("The received file is not a supported save.");
            return jsonError(400, "Unsupported save");
        }

        Metadata parsedMetadata;
        parsedMetadata.titleId    = jsonString(metadata, "titleId");
        parsedMetadata.titleName  = jsonString(metadata, "titleName", "Unknown");
        parsedMetadata.dataType   = jsonString(metadata, "dataType", "save");
        parsedMetadata.backupName = jsonString(metadata, "backupName");
        parsedMetadata.fileName   = std::move(receivedFileName);

        {
            std::lock_guard<std::mutex> lock(transferMutex);
            pendingSave      = std::move(save);
            receivedMetadata = std::move(parsedMetadata);
            lastPeerAddress  = request.peerAddress;
            receiveError.clear();
        }
        receiveState.store(ReceiveState::Complete);

        nlohmann::json response;
        response["ok"]        = true;
        response["savedPath"] = "PKSM editor";
        return {200, "application/json", response.dump()};
    }

    int generatePin()
    {
        u32 random     = 0;
        bool generated = false;
        if (R_SUCCEEDED(psInit()))
        {
            generated = R_SUCCEEDED(PS_GenerateRandomBytes(&random, sizeof(random)));
            psExit();
        }
        if (!generated)
        {
            random = (u32)(svcGetSystemTick() ^ osGetTime());
        }
        return 1000 + (int)(random % 9000);
    }

    struct Target
    {
        std::string address;
        u16 port;
    };

    bool parseTarget(const std::string& text, Target& target)
    {
        size_t colon        = text.find_last_of(':');
        std::string address = colon == std::string::npos ? text : text.substr(0, colon);
        int port            = TRANSFER_PORT;
        if (colon != std::string::npos)
        {
            std::string portText = text.substr(colon + 1);
            char* end            = nullptr;
            long parsed          = strtol(portText.c_str(), &end, 10);
            if (end == portText.c_str() || *end != '\0' || parsed <= 0 || parsed > 65535)
            {
                return false;
            }
            port = (int)parsed;
        }

        in_addr binary{};
        if (address.empty() || inet_pton(AF_INET, address.c_str(), &binary) != 1)
        {
            return false;
        }
        target = {std::move(address), (u16)port};
        return true;
    }

    bool promptTarget(Target& target)
    {
        std::string initial;
        {
            std::lock_guard<std::mutex> lock(transferMutex);
            initial = !lastTarget.empty()
                        ? lastTarget
                        : (lastPeerAddress.empty()
                                  ? ""
                                  : lastPeerAddress + ":" + std::to_string(TRANSFER_PORT));
        }

        SwkbdState keyboard;
        swkbdInit(&keyboard, SWKBD_TYPE_NORMAL, 2, 63);
        swkbdSetHintText(&keyboard, i18n::localize("WIRELESS_ADDRESS_HINT").c_str());
        swkbdSetValidation(&keyboard, SWKBD_NOTBLANK_NOTEMPTY, 0, 0);
        if (!initial.empty())
        {
            swkbdSetInitialText(&keyboard, initial.c_str());
        }
        char input[64] = {0};
        if (swkbdInputText(&keyboard, input, sizeof(input)) != SWKBD_BUTTON_CONFIRM ||
            !parseTarget(input, target))
        {
            return false;
        }
        {
            std::lock_guard<std::mutex> lock(transferMutex);
            lastTarget = input;
        }
        return true;
    }

    bool promptPin(std::string& pin)
    {
        SwkbdState keyboard;
        swkbdInit(&keyboard, SWKBD_TYPE_NUMPAD, 2, 4);
        swkbdSetFeatures(&keyboard, SWKBD_FIXED_WIDTH);
        swkbdSetHintText(&keyboard, i18n::localize("WIRELESS_PIN_HINT").c_str());
        swkbdSetValidation(&keyboard, SWKBD_FIXEDLEN, 0, 0);
        char input[5] = {0};
        if (swkbdInputText(&keyboard, input, sizeof(input)) != SWKBD_BUTTON_CONFIRM)
        {
            return false;
        }
        pin = input;
        return pin.size() == 4 && std::all_of(pin.begin(), pin.end(),
                                      [](unsigned char value) { return std::isdigit(value); });
    }

    int pollSocket(int socket, short events)
    {
        pollfd descriptor{socket, events, 0};
        return poll(&descriptor, 1, NETWORK_TIMEOUT_MS);
    }

    bool sendAll(int socket, const void* data, size_t size)
    {
        const u8* current = static_cast<const u8*>(data);
        size_t sent       = 0;
        while (sent < size)
        {
            if (pollSocket(socket, POLLOUT) <= 0)
            {
                return false;
            }
            int result = send(socket, current + sent, size - sent, 0);
            if (result <= 0)
            {
                return false;
            }
            sent += (size_t)result;
        }
        return true;
    }

    std::string safeFileName(const std::string& path)
    {
        size_t slash     = path.find_last_of("/\\");
        std::string base = slash == std::string::npos ? path : path.substr(slash + 1);
        std::string result;
        result.reserve(base.size());
        for (unsigned char value : base)
        {
            if (value >= 0x20 && value != 0x7F && value != ':' && value != '*' && value != '?' &&
                value != '"' && value != '<' && value != '>' && value != '|')
            {
                result.push_back((char)value);
            }
        }
        return result.empty() ? "main" : result;
    }

    char regionPostfix(pksm::Language language)
    {
        switch (language)
        {
            case pksm::Language::JPN:
                return 'J';
            case pksm::Language::FRE:
                return 'F';
            case pksm::Language::ITA:
                return 'I';
            case pksm::Language::GER:
                return 'D';
            case pksm::Language::SPA:
                return 'S';
            case pksm::Language::KOR:
                return 'K';
            default:
                return 'E';
        }
    }

    // The key the save lists are grouped by: a DS game code plus its region letter for Gen 4 and
    // Gen 5, the Checkpoint prefix of the owning title for everything else.
    std::string saveIdentifier(const pksm::Sav& save, const Metadata& metadata)
    {
        auto dsGame = std::find(dsVersions.begin(), dsVersions.end(), save.version());
        if (dsGame != dsVersions.end())
        {
            // A Gen 4 save cannot tell Diamond from Pearl on its own, so prefer the game code the
            // sender supplied whenever it names a game of the same generation.
            const std::string& sent = metadata.titleId;
            if (sent.size() == 3 || sent.size() == 4)
            {
                auto found =
                    std::find(dsIds.begin(), dsIds.end(), std::string_view(sent).substr(0, 3));
                if (found != dsIds.end() &&
                    pksm::Generation(dsVersions[std::distance(dsIds.begin(), found)]) ==
                        save.generation())
                {
                    return sent.size() == 4 ? sent : sent + regionPostfix(save.language());
                }
            }
            return std::string(dsIds[std::distance(dsVersions.begin(), dsGame)]) +
                   regionPostfix(save.language());
        }

        std::string title = Configuration::getInstance().titleId(save.version());
        if (title.empty())
        {
            return "";
        }
        return Title::tidToCheckpointPrefix<std::string>(strtoull(title.c_str(), nullptr, 16));
    }

    // Received saves get one directory per transfer, mirroring the layout the loader screens
    // already read: they label every save with the directory that holds it.
    std::string transferDirectory(const std::string& id, const Metadata& metadata)
    {
        mkdir(TRANSFER_DIRECTORY, 777);
        std::string path = std::string(TRANSFER_DIRECTORY) + '/' + id;
        mkdir(path.c_str(), 777);

        std::string label = metadata.backupName.empty() ? "" : safeFileName(metadata.backupName);
        if (label.empty() || label == "." || label == "..")
        {
            DateTime now = DateTime::now();
            label        = std::format("{:04d}-{:02d}-{:02d}_{:02d}-{:02d}-{:02d}", now.year(),
                       now.month(), now.day(), now.hour(), now.minute(), now.second());
        }
        path += '/' + label;

        std::string unique = path;
        for (int attempt = 2; io::exists(unique) && attempt < 100; attempt++)
        {
            unique = std::format("{}_{:d}", path, attempt);
        }
        if (mkdir(unique.c_str(), 777) != 0 && !io::exists(unique))
        {
            return "";
        }
        return unique;
    }

    // The caller owns the editing state: the save must be finished editing before its bytes are
    // worth writing anywhere.
    bool writeSave(const std::string& path, const pksm::Sav& save)
    {
        FILE* output = fopen(path.c_str(), "wb");
        if (!output)
        {
            Logging::error("WirelessTransfer - Failed to open {} for writing: {}", path, errno);
            return false;
        }
        // A save that arrived wrapped in emulator or dumper data is written back whole, so
        // the file stays loadable by whatever produced it.
        const size_t length = save.getEntireLengthIncludingFooter();
        size_t written      = fwrite(save.rawData().get(), 1, length, output);
        fclose(output);
        if (written != length)
        {
            Logging::error("WirelessTransfer - Wrote {} of {} bytes to {}", written, length, path);
            remove(path.c_str());
            return false;
        }
        return true;
    }

    // Extra saves are what makes a save visible on the loader screens no matter which title it
    // belongs to, and the config is only flushed by the settings screen, so flush it here too.
    void registerSave(const std::string& id, const std::string& path)
    {
        auto configured = Configuration::getInstance().extraSaves(id);
        if (std::find(configured.begin(), configured.end(), path) == configured.end())
        {
            configured.emplace_back(path);
            Configuration::getInstance().extraSaves(id, configured);
            Configuration::getInstance().save();
        }

        auto saves    = TitleLoader::sdSaves.lock();
        auto& forming = saves.get()[id];
        if (std::find(forming.begin(), forming.end(), path) == forming.end())
        {
            forming.emplace_back(path);
            TitleLoader::sdSavesVersion.fetch_add(1, std::memory_order_relaxed);
        }
    }

    // Keeps the received save on the SD card and hands it to the loader as a file save, so the
    // console can open it again later instead of it living only in memory.
    bool persistReceivedSave(const std::shared_ptr<pksm::Sav>& save, const Metadata& metadata)
    {
        std::string id = saveIdentifier(*save, metadata);
        if (id.empty())
        {
            Logging::warning("WirelessTransfer - No known title for the received save");
            return false;
        }
        std::string directory = transferDirectory(id, metadata);
        if (directory.empty())
        {
            Logging::error("WirelessTransfer - Failed to create a directory under {}/{}",
                TRANSFER_DIRECTORY, id);
            return false;
        }

        std::string path = directory + '/' + safeFileName(metadata.fileName);
        save->finishEditing();
        bool written = writeSave(path, *save);
        save->beginEditing();
        if (!written || !TitleLoader::load(nullptr, path))
        {
            remove(path.c_str());
            return false;
        }

        // After the load: an autoBackup run looks the path up in the save list, and a save that
        // arrived seconds ago does not need a backup of itself.
        registerSave(id, path);
        persistedPath = path;
        Logging::info("WirelessTransfer - Received save stored at {} for ID: {}", path, id);
        return true;
    }
}

bool WirelessTransfer::hasLoadedSave()
{
    return loadedFromTransfer;
}

void WirelessTransfer::clearLoadedSave()
{
    loadedFromTransfer = false;
    persistedPath.clear();
}

bool WirelessTransfer::receiveSave()
{
    if (!Server::isRunning())
    {
        Gui::error(i18n::localize("SOCKET_LISTEN_FAIL"), (Result)-1);
        return false;
    }

    std::string token   = std::format("{:04d}", generatePin());
    std::string address = Server::getAddress();
    std::string information =
        i18n::localize("WIRELESS_WARNING") + '\n' +
        pksm::format(i18n::localize("WIRELESS_IP"), address + ":" + std::to_string(TRANSFER_PORT)) +
        "\nPIN: " + token;
    if (!Gui::showChoiceMessage(information))
    {
        return false;
    }

    {
        std::lock_guard<std::mutex> lock(transferMutex);
        receiveToken = std::move(token);
        receiveError.clear();
        pendingSave.reset();
    }
    authenticationFailures.store(0);
    receiveState.store(ReceiveState::Waiting);
    remove(TEMPORARY_UPLOAD);
    Server::registerHandler("/transfer/info", handleInfo);
    Server::registerUploadHandler("/transfer/upload", TEMPORARY_UPLOAD, handleUpload);

    while (aptMainLoop() && receiveState.load() == ReceiveState::Waiting)
    {
        std::string message = information;
        {
            std::lock_guard<std::mutex> lock(transferMutex);
            if (!receiveError.empty())
            {
                message += '\n' + receiveError;
            }
        }
        Gui::waitFrame(message);
        hidScanInput();
    }

    Server::unregisterHandler("/transfer/info");
    Server::unregisterHandler("/transfer/upload");

    if (receiveState.load() != ReceiveState::Complete)
    {
        std::string error;
        {
            std::lock_guard<std::mutex> lock(transferMutex);
            error = receiveError;
        }
        receiveState.store(ReceiveState::Idle);
        if (!error.empty())
        {
            Gui::warn(error);
        }
        return false;
    }

    std::shared_ptr<pksm::Sav> received;
    Metadata metadata;
    {
        std::lock_guard<std::mutex> lock(transferMutex);
        received = std::move(pendingSave);
        metadata = receivedMetadata;
    }
    receiveState.store(ReceiveState::Idle);

    persistedPath.clear();
    if (!persistReceivedSave(received, metadata))
    {
        // The save is still worth editing even when the SD card refuses it; it just cannot be
        // opened again from the console afterwards.
        TitleLoader::save = std::move(received);
    }
    loadedFromTransfer = true;
    ScreenStack::push(std::make_unique<MainMenu>());
    return true;
}

bool WirelessTransfer::sendSave()
{
    Target target;
    std::string token;
    if (!promptTarget(target) || !promptPin(token))
    {
        return false;
    }

    Metadata metadata;
    {
        std::lock_guard<std::mutex> lock(transferMutex);
        metadata = receivedMetadata;
    }
    if (metadata.titleName.empty())
    {
        metadata.titleName = "Unknown";
    }
    if (metadata.backupName.empty())
    {
        DateTime now        = DateTime::now();
        metadata.backupName = std::format("PKSM_{:04d}{:02d}{:02d}-{:02d}{:02d}{:02d}", now.year(),
            now.month(), now.day(), now.hour(), now.minute(), now.second());
    }
    metadata.fileName = safeFileName(metadata.fileName);

    size_t payloadSize = TitleLoader::save->getEntireLengthIncludingFooter();
    nlohmann::json metadataJson;
    metadataJson["titleId"]        = metadata.titleId;
    metadataJson["titleName"]      = metadata.titleName;
    metadataJson["dataType"]       = metadata.dataType;
    metadataJson["backupName"]     = metadata.backupName;
    metadataJson["isZip"]          = false;
    metadataJson["fileBytesTotal"] = payloadSize;
    metadataJson["fileName"]       = metadata.fileName;
    DateTime timestamp             = DateTime::now();
    metadataJson["timestamp"]      = std::format("{:04d}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}",
             timestamp.year(), timestamp.month(), timestamp.day(), timestamp.hour(), timestamp.minute(),
             timestamp.second());

    std::string boundary     = std::format("----pksm-boundary-{}", osGetTime());
    std::string partMetadata = "--" + boundary +
                               "\r\nContent-Disposition: form-data; name=\"meta\"\r\n"
                               "Content-Type: application/json\r\n\r\n" +
                               metadataJson.dump() + "\r\n";
    std::string partFile =
        "--" + boundary + "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"" +
        metadata.fileName + "\"\r\nContent-Type: application/octet-stream\r\n\r\n";
    std::string partEnd  = "\r\n--" + boundary + "--\r\n";
    size_t contentLength = partMetadata.size() + partFile.size() + payloadSize + partEnd.size();

    int socketDescriptor = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socketDescriptor < 0)
    {
        Gui::error(i18n::localize("SOCKET_CREATE_FAIL"), errno);
        return false;
    }

    struct SocketGuard
    {
        int socket;

        ~SocketGuard() { close(socket); }
    } socketGuard{socketDescriptor};

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_port   = htons(target.port);
    inet_pton(AF_INET, target.address.c_str(), &address.sin_addr);
    Gui::waitFrame(i18n::localize("WIRELESS_SENDING"));
    if (connect(socketDescriptor, (sockaddr*)&address, sizeof(address)) != 0)
    {
        Gui::error(i18n::localize("SOCKET_CONNECTION_FAIL"), errno);
        return false;
    }
    fcntl(socketDescriptor, F_SETFL, fcntl(socketDescriptor, F_GETFL, 0) | O_NONBLOCK);

    std::string header =
        std::format("POST /transfer/upload HTTP/1.1\r\nHost: {}:{}\r\nConnection: close\r\n",
            target.address, target.port);
    header += "X-CP-Token: " + token + "\r\n";
    header += "Content-Type: multipart/form-data; boundary=" + boundary + "\r\n";
    header += "Content-Length: " + std::to_string(contentLength) + "\r\n\r\n";

    bool sent = sendAll(socketDescriptor, header.data(), header.size()) &&
                sendAll(socketDescriptor, partMetadata.data(), partMetadata.size()) &&
                sendAll(socketDescriptor, partFile.data(), partFile.size()) &&
                sendAll(socketDescriptor, TitleLoader::save->rawData().get(), payloadSize) &&
                sendAll(socketDescriptor, partEnd.data(), partEnd.size());
    if (!sent)
    {
        Gui::error(i18n::localize("DATA_SEND_FAIL"), errno);
        return false;
    }

    std::string response;
    char responseBuffer[1024];
    constexpr size_t MAX_RESPONSE_SIZE = 64 * 1024;
    while (response.size() < MAX_RESPONSE_SIZE && pollSocket(socketDescriptor, POLLIN) > 0)
    {
        size_t remaining = MAX_RESPONSE_SIZE - response.size();
        int received =
            recv(socketDescriptor, responseBuffer, std::min(sizeof(responseBuffer), remaining), 0);
        if (received <= 0)
        {
            break;
        }
        response.append(responseBuffer, (size_t)received);
    }

    bool success = response.starts_with("HTTP/1.1 200") || response.starts_with("HTTP/1.0 200");
    if (!success)
    {
        std::string detail;
        size_t body = response.find("\r\n\r\n");
        if (body != std::string::npos)
        {
            nlohmann::json parsed =
                nlohmann::json::parse(response.substr(body + 4), nullptr, false);
            if (!parsed.is_discarded())
            {
                detail = jsonString(parsed, "error");
            }
        }
        Gui::warn(i18n::localize("DATA_SEND_FAIL") + (detail.empty() ? "" : "\n" + detail));
    }
    return success;
}

void WirelessTransfer::persistChanges()
{
    // Called between finishEditing() and beginEditing(), so the bytes on hand are the ones a
    // console would read back.
    if (!persistedPath.empty())
    {
        writeSave(persistedPath, *TitleLoader::save);
        return;
    }

    // Nothing was stored on receive, so fall back to a dated copy that at least keeps the work.
    DateTime now = DateTime::now();
    mkdir("/3ds/PKSM/backups", 777);
    mkdir(WIRELESS_BACKUPS, 777);
    writeSave(std::format("{}/{:d}-{:d}-{:d}_{:d}-{:d}-{:d}.bak", WIRELESS_BACKUPS, now.year(),
                  now.month(), now.day(), now.hour(), now.minute(), now.second()),
        *TitleLoader::save);
}
