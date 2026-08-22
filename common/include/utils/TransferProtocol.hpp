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

#ifndef TRANSFER_PROTOCOL_HPP
#define TRANSFER_PROTOCOL_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>

// Pure framing and validation for Checkpoint's wireless-transfer protocol.
// Filesystem and socket IO stay behind the reader/sink seam so this code is
// shared by the 3DS implementation and host tests.
namespace TransferProtocol
{
    std::string headerValue(const std::string& headers, const std::string& key);
    bool constantTimeEquals(const std::string& a, const std::string& b);

    bool parseMultipart(const std::string& head, uint64_t bodyLength, const std::string& boundary,
        std::string& outMetadata, uint64_t& outFileOffset, uint64_t& outFileLength,
        std::string& outError);

    struct ByteReader
    {
        virtual ~ByteReader()                          = default;
        virtual size_t read(void* output, size_t size) = 0;
    };

    struct ExtractSink
    {
        virtual ~ExtractSink()                                                 = default;
        virtual bool directory(const std::string& relativePath)                = 0;
        virtual bool beginFile(const std::string& relativePath, uint32_t size) = 0;
        virtual bool writeFile(const void* data, size_t size)                  = 0;
        virtual void endFile()                                                 = 0;
    };

    using CancelFunction   = std::function<bool()>;
    using ProgressFunction = std::function<void(size_t)>;

    // Checkpoint and chlink use store-only ZIP archives for multi-file saves.
    // Each entry is path-checked and CRC-verified before success is reported.
    bool extractZip(ByteReader& input, uint64_t limit, ExtractSink& output,
        const CancelFunction& cancelled, const ProgressFunction& progress, std::string& outError);
}

#endif
