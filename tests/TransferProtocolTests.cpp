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
#include <cassert>
#include <cstdint>
#include <cstring>
#include <string>

namespace
{
    void append16(std::string& output, uint16_t value)
    {
        output.push_back((char)(value & 0xFF));
        output.push_back((char)(value >> 8));
    }

    void append32(std::string& output, uint32_t value)
    {
        output.push_back((char)(value & 0xFF));
        output.push_back((char)((value >> 8) & 0xFF));
        output.push_back((char)((value >> 16) & 0xFF));
        output.push_back((char)(value >> 24));
    }

    uint32_t crc32(const std::string& data)
    {
        uint32_t crc = 0xFFFFFFFFu;
        for (unsigned char value : data)
        {
            crc ^= value;
            for (int bit = 0; bit < 8; bit++)
            {
                crc = (crc & 1) ? 0xEDB88320u ^ (crc >> 1) : crc >> 1;
            }
        }
        return crc ^ 0xFFFFFFFFu;
    }

    std::string zipEntry(const std::string& name, const std::string& data)
    {
        std::string output;
        append32(output, 0x04034B50);
        append16(output, 20);
        append16(output, 0x0008);
        append16(output, 0);
        append16(output, 0);
        append16(output, 0);
        append32(output, 0);
        append32(output, data.size());
        append32(output, data.size());
        append16(output, name.size());
        append16(output, 0);
        output += name;
        output += data;
        append32(output, 0x08074B50);
        append32(output, crc32(data));
        append32(output, data.size());
        append32(output, data.size());
        return output;
    }

    struct Reader : TransferProtocol::ByteReader
    {
        const std::string& data;
        size_t position = 0;

        explicit Reader(const std::string& data) : data(data) {}

        size_t read(void* output, size_t size) override
        {
            size = std::min(size, data.size() - position);
            std::memcpy(output, data.data() + position, size);
            position += size;
            return size;
        }
    };

    struct Sink : TransferProtocol::ExtractSink
    {
        std::string name;
        std::string data;

        bool directory(const std::string&) override { return true; }

        bool beginFile(const std::string& relativePath, uint32_t size) override
        {
            name = relativePath;
            data.clear();
            data.reserve(size);
            return true;
        }

        bool writeFile(const void* input, size_t size) override
        {
            data.append(static_cast<const char*>(input), size);
            return true;
        }

        void endFile() override {}
    };
}

int main()
{
    const std::string headers =
        "POST /transfer/upload HTTP/1.1\r\nX-Cp-Token: 1234\r\nContent-Length:\t42  \r\n";
    assert(TransferProtocol::headerValue(headers, "X-CP-Token") == "1234");
    assert(TransferProtocol::headerValue(headers, "content-length") == "42");
    assert(TransferProtocol::headerValue(headers, "Missing").empty());
    assert(TransferProtocol::constantTimeEquals("1234", "1234"));
    assert(!TransferProtocol::constantTimeEquals("1234", "1235"));
    assert(!TransferProtocol::constantTimeEquals("1234", "123"));

    const std::string boundary = "test-boundary";
    const std::string metadata = "{\"isZip\":false}";
    const std::string payload  = "save-data";
    std::string body =
        "--" + boundary + "\r\nContent-Disposition: form-data; name=\"meta\"\r\n\r\n" + metadata +
        "\r\n--" + boundary +
        "\r\nContent-Disposition: form-data; name=\"file\"; filename=\"main\"\r\n\r\n" + payload +
        "\r\n--" + boundary + "--\r\n";
    std::string parsedMetadata;
    std::string error;
    uint64_t offset = 0;
    uint64_t length = 0;
    assert(TransferProtocol::parseMultipart(
        body, body.size(), boundary, parsedMetadata, offset, length, error));
    assert(parsedMetadata == metadata);
    assert(length == payload.size());
    assert(body.substr(offset, length) == payload);

    std::string zip = zipEntry("main", payload);
    Reader reader(zip);
    Sink sink;
    assert(TransferProtocol::extractZip(reader, zip.size(), sink, {}, {}, error));
    assert(sink.name == "main");
    assert(sink.data == payload);

    std::string unsafeZip = zipEntry("../main", payload);
    Reader unsafeReader(unsafeZip);
    Sink unsafeSink;
    assert(
        !TransferProtocol::extractZip(unsafeReader, unsafeZip.size(), unsafeSink, {}, {}, error));

    zip[zip.find(payload)] ^= 1;
    Reader corruptReader(zip);
    Sink corruptSink;
    assert(!TransferProtocol::extractZip(corruptReader, zip.size(), corruptSink, {}, {}, error));
}
