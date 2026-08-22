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

#include "TransferProtocol.hpp"
#include <algorithm>
#include <cctype>
#include <cstring>
#include <memory>

namespace TransferProtocol
{
    namespace
    {
        uint32_t crcTable[8][256];
        bool crcInitialized = false;

        void initializeCrc()
        {
            if (crcInitialized)
            {
                return;
            }
            for (uint32_t i = 0; i < 256; i++)
            {
                uint32_t crc = i;
                for (int bit = 0; bit < 8; bit++)
                {
                    crc = (crc & 1) ? (0xEDB88320u ^ (crc >> 1)) : (crc >> 1);
                }
                crcTable[0][i] = crc;
            }
            for (uint32_t i = 0; i < 256; i++)
            {
                uint32_t crc = crcTable[0][i];
                for (int table = 1; table < 8; table++)
                {
                    crc                = crcTable[0][crc & 0xFFu] ^ (crc >> 8);
                    crcTable[table][i] = crc;
                }
            }
            crcInitialized = true;
        }

        uint32_t updateCrc(uint32_t crc, const uint8_t* data, size_t size)
        {
            initializeCrc();
            while (size >= 8)
            {
                uint32_t low = (uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16) |
                                          ((uint32_t)data[3] << 24)) ^
                               crc;
                uint32_t high = (uint32_t)(data[4] | (data[5] << 8) | (data[6] << 16) |
                                           ((uint32_t)data[7] << 24));
                crc           = crcTable[7][low & 0xFFu] ^ crcTable[6][(low >> 8) & 0xFFu] ^
                      crcTable[5][(low >> 16) & 0xFFu] ^ crcTable[4][low >> 24] ^
                      crcTable[3][high & 0xFFu] ^ crcTable[2][(high >> 8) & 0xFFu] ^
                      crcTable[1][(high >> 16) & 0xFFu] ^ crcTable[0][high >> 24];
                data += 8;
                size -= 8;
            }
            for (size_t i = 0; i < size; i++)
            {
                crc = crcTable[0][(crc ^ data[i]) & 0xFFu] ^ (crc >> 8);
            }
            return crc;
        }

        bool safeRelativePath(const std::string& path)
        {
            if (path.empty() || path.front() == '/' || path.front() == '\\' ||
                path.find('\\') != std::string::npos || path.find(':') != std::string::npos)
            {
                return false;
            }

            size_t start = 0;
            while (start <= path.size())
            {
                size_t end  = path.find('/', start);
                size_t size = end == std::string::npos ? path.size() - start : end - start;
                if (path.substr(start, size) == "..")
                {
                    return false;
                }
                if (end == std::string::npos)
                {
                    break;
                }
                start = end + 1;
            }
            return true;
        }

        uint16_t readLe16(const uint8_t* data)
        {
            return (uint16_t)(data[0] | (data[1] << 8));
        }

        uint32_t readLe32(const uint8_t* data)
        {
            return (
                uint32_t)(data[0] | (data[1] << 8) | (data[2] << 16) | ((uint32_t)data[3] << 24));
        }
    }

    std::string headerValue(const std::string& headers, const std::string& key)
    {
        size_t lineStart = 0;
        while (lineStart < headers.size())
        {
            size_t lineEnd = headers.find("\r\n", lineStart);
            if (lineEnd == std::string::npos)
            {
                lineEnd = headers.size();
            }

            size_t colon = headers.find(':', lineStart);
            if (colon != std::string::npos && colon < lineEnd && colon - lineStart == key.size())
            {
                bool matches = true;
                for (size_t i = 0; i < key.size(); i++)
                {
                    if (std::tolower((unsigned char)headers[lineStart + i]) !=
                        std::tolower((unsigned char)key[i]))
                    {
                        matches = false;
                        break;
                    }
                }
                if (matches)
                {
                    size_t valueStart = colon + 1;
                    while (valueStart < lineEnd &&
                           (headers[valueStart] == ' ' || headers[valueStart] == '\t'))
                    {
                        valueStart++;
                    }
                    size_t valueEnd = lineEnd;
                    while (valueEnd > valueStart &&
                           (headers[valueEnd - 1] == ' ' || headers[valueEnd - 1] == '\t'))
                    {
                        valueEnd--;
                    }
                    return headers.substr(valueStart, valueEnd - valueStart);
                }
            }

            if (lineEnd == headers.size())
            {
                break;
            }
            lineStart = lineEnd + 2;
        }
        return "";
    }

    bool constantTimeEquals(const std::string& a, const std::string& b)
    {
        if (a.size() != b.size())
        {
            return false;
        }
        unsigned char difference = 0;
        for (size_t i = 0; i < a.size(); i++)
        {
            difference |= (unsigned char)(a[i] ^ b[i]);
        }
        return difference == 0;
    }

    bool parseMultipart(const std::string& head, uint64_t bodyLength, const std::string& boundary,
        std::string& outMetadata, uint64_t& outFileOffset, uint64_t& outFileLength,
        std::string& outError)
    {
        outMetadata.clear();
        outFileOffset = 0;
        outFileLength = 0;

        if (boundary.empty())
        {
            outError = "Missing boundary.";
            return false;
        }

        const std::string marker     = "--" + boundary;
        const std::string nextMarker = "\r\n" + marker;
        size_t metadataPosition      = head.find(marker);
        size_t filePosition          = head.find("name=\"file\"");
        if (metadataPosition == std::string::npos || filePosition == std::string::npos)
        {
            outError = "Incomplete form data.";
            return false;
        }

        size_t metadataHeaderEnd = head.find("\r\n\r\n", metadataPosition);
        if (metadataHeaderEnd == std::string::npos)
        {
            outError = "Incomplete form data.";
            return false;
        }
        size_t metadataStart = metadataHeaderEnd + 4;
        size_t metadataEnd   = head.find(nextMarker, metadataStart);
        if (metadataEnd == std::string::npos || metadataEnd > filePosition)
        {
            outError = "Incomplete form data.";
            return false;
        }
        outMetadata = head.substr(metadataStart, metadataEnd - metadataStart);

        size_t fileHeaderEnd = head.find("\r\n\r\n", filePosition);
        if (fileHeaderEnd == std::string::npos)
        {
            outError = "Incomplete form data.";
            return false;
        }
        uint64_t fileStart        = (uint64_t)fileHeaderEnd + 4;
        const std::string trailer = "\r\n" + marker + "--\r\n";
        if (bodyLength < fileStart + trailer.size())
        {
            outError = "Incomplete form data.";
            return false;
        }

        outFileOffset = fileStart;
        outFileLength = bodyLength - fileStart - trailer.size();
        return true;
    }

    bool extractZip(ByteReader& input, uint64_t limit, ExtractSink& output,
        const CancelFunction& cancelled, const ProgressFunction& progress, std::string& outError)
    {
        uint64_t consumed = 0;
        auto read         = [&](void* destination, size_t size) -> size_t
        {
            if (consumed + size > limit)
            {
                size = (size_t)(limit - consumed);
            }
            if (size == 0)
            {
                return 0;
            }
            size_t result = input.read(destination, size);
            consumed     += result;
            return result;
        };

        constexpr size_t BUFFER_SIZE = 0x40000;
        std::unique_ptr<uint8_t[]> buffer(new uint8_t[BUFFER_SIZE]);
        bool sawEntry = false;

        while (consumed + 4 <= limit)
        {
            uint8_t signatureData[4];
            if (read(signatureData, sizeof(signatureData)) != sizeof(signatureData))
            {
                outError = "Corrupted ZIP header.";
                return false;
            }
            uint32_t signature = readLe32(signatureData);
            if (signature != 0x04034B50)
            {
                return sawEntry;
            }
            sawEntry = true;

            uint8_t header[26];
            if (read(header, sizeof(header)) != sizeof(header))
            {
                outError = "Corrupted ZIP header.";
                return false;
            }
            uint16_t flags        = readLe16(header + 2);
            uint16_t compression  = readLe16(header + 4);
            uint32_t storedCrc    = readLe32(header + 10);
            uint32_t compressed   = readLe32(header + 14);
            uint32_t uncompressed = readLe32(header + 18);
            uint16_t nameLength   = readLe16(header + 22);
            uint16_t extraLength  = readLe16(header + 24);

            std::string name(nameLength, '\0');
            if (nameLength > 0 && read(name.data(), nameLength) != nameLength)
            {
                outError = "Corrupted ZIP header.";
                return false;
            }
            if (extraLength > 0)
            {
                std::unique_ptr<uint8_t[]> extra(new uint8_t[extraLength]);
                if (read(extra.get(), extraLength) != extraLength)
                {
                    outError = "Corrupted ZIP header.";
                    return false;
                }
            }

            if (compression != 0)
            {
                outError = "Unsupported ZIP compression.";
                return false;
            }
            if (!safeRelativePath(name))
            {
                outError = "Invalid ZIP entry path.";
                return false;
            }
            if (compressed != uncompressed)
            {
                outError = "Invalid store-only ZIP entry.";
                return false;
            }

            if (!name.empty() && name.back() == '/')
            {
                if (!output.directory(name))
                {
                    outError = "Failed to process ZIP directory.";
                    return false;
                }
                continue;
            }

            if (!output.beginFile(name, uncompressed))
            {
                outError = "Failed to prepare ZIP entry.";
                return false;
            }

            uint32_t computedCrc = 0xFFFFFFFFu;
            uint32_t remaining   = compressed;
            while (remaining > 0)
            {
                if (cancelled && cancelled())
                {
                    output.endFile();
                    outError = "Transfer cancelled.";
                    return false;
                }
                uint32_t chunk  = remaining > BUFFER_SIZE ? (uint32_t)BUFFER_SIZE : remaining;
                size_t received = read(buffer.get(), chunk);
                if (received == 0)
                {
                    output.endFile();
                    outError = "Corrupted ZIP payload.";
                    return false;
                }
                computedCrc = updateCrc(computedCrc, buffer.get(), received);
                if (!output.writeFile(buffer.get(), received))
                {
                    output.endFile();
                    outError = "Failed to process ZIP entry.";
                    return false;
                }
                remaining -= (uint32_t)received;
                if (progress)
                {
                    progress(received);
                }
            }
            output.endFile();

            if (flags & 0x08)
            {
                uint8_t descriptor[16];
                if (read(descriptor, 4) != 4)
                {
                    outError = "Corrupted ZIP payload.";
                    return false;
                }
                uint32_t first = readLe32(descriptor);
                if (first == 0x08074B50)
                {
                    if (read(descriptor + 4, 12) != 12)
                    {
                        outError = "Corrupted ZIP payload.";
                        return false;
                    }
                    storedCrc = readLe32(descriptor + 4);
                }
                else
                {
                    if (read(descriptor + 4, 8) != 8)
                    {
                        outError = "Corrupted ZIP payload.";
                        return false;
                    }
                    storedCrc = first;
                }
            }

            if ((computedCrc ^ 0xFFFFFFFFu) != storedCrc)
            {
                outError = "Checksum mismatch in received file.";
                return false;
            }
        }

        if (!sawEntry)
        {
            outError = "Empty ZIP archive.";
        }
        return sawEntry;
    }
}
