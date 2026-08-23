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

#include "AssetStore.hpp"
#include "Archive.hpp"
#include "fetch.hpp"
#include "utils/crypto.hpp"
#include "website.h"
#include <3ds.h>
#include <algorithm>
#include <array>
#include <cstdio>
#include <span>
#include <string>
#include <vector>

namespace
{
    struct Asset
    {
        std::string url;
        std::string path;
        std::array<u8, 32> hash;
    };

    // What this build considers correct. The only copy of it.
    const Asset assets[] = {
        {CDN_URL "assets/pkm_spritesheet.t3x",   "/3ds/PKSM/assets/pkm_spritesheet.t3x",
         {0xc5, 0x4b, 0x46, 0x4d, 0xe9, 0xe5, 0x6f, 0x5b, 0x04, 0xc7, 0xd6, 0x79, 0xbd, 0xf0,
                0xb9, 0xb6, 0xc8, 0x4d, 0xbe, 0xa5, 0x55, 0x5b, 0xb7, 0xae, 0x62, 0x86, 0x2b, 0x18,
                0x62, 0x08, 0x10, 0x32}},
        {CDN_URL "assets/types_spritesheet.t3x", "/3ds/PKSM/assets/types_spritesheet.t3x",
         {0x9f, 0xba, 0xa1, 0x0f, 0xe2, 0x05, 0xce, 0x57, 0xcf, 0x87, 0x32, 0xc3, 0x7f, 0x72,
                0x42, 0x02, 0x04, 0xf9, 0x06, 0xd7, 0x5c, 0x65, 0xff, 0xae, 0xe8, 0xbf, 0x61, 0x5a,
                0x08, 0xe4, 0x86, 0x85}}
    };

    // Missing beats stale beats ok: the verdict for a set of assets is the worst of them.
    AssetStore::Verdict worse(AssetStore::Verdict lhs, AssetStore::Verdict rhs)
    {
        return std::max(lhs, rhs);
    }

    // One asset, hashed straight out of the file through `buffer`. Never holds more than
    // a chunk of it, however large the sheet is.
    AssetStore::Verdict inspect(const Asset& asset, std::span<u8> buffer)
    {
        auto in = Archive::sd().file(asset.path, FS_OPEN_READ);
        if (!in)
        {
            return AssetStore::Verdict::Missing;
        }

        pksm::crypto::SHA256 hash;
        u64 left = in->size();
        while (left > 0)
        {
            const u32 want = u32(std::min<u64>(left, buffer.size()));
            const u32 got  = in->read(buffer.data(), want);
            // A file that stops giving bytes before its own size is not the file we want,
            // whatever the rest of it would have hashed to.
            if (got == 0)
            {
                in->close();
                return AssetStore::Verdict::Stale;
            }
            hash.update(buffer.first(got));
            left -= got;
        }
        in->close();

        return hash.finish() == asset.hash ? AssetStore::Verdict::Ok : AssetStore::Verdict::Stale;
    }

    s32 fetch(const Asset& asset)
    {
        u32 status;
        ACU_GetWifiStatus(&status);
        if (status == 0)
        {
            return -1;
        }
        return Fetch::download(asset.url, asset.path);
    }
}

AssetStore::Verdict AssetStore::verify()
{
    // One buffer for the whole pass, instead of one whole-file buffer per asset.
    std::vector<u8> buffer(CHUNK_SIZE);

    Verdict result = Verdict::Ok;
    for (const auto& asset : assets)
    {
        result = worse(result, inspect(asset, buffer));
    }

    mVerdict = result;
    return result;
}

s32 AssetStore::ensure()
{
    std::vector<u8> buffer(CHUNK_SIZE);

    Verdict result = Verdict::Ok;
    for (const auto& asset : assets)
    {
        Verdict state = inspect(asset, buffer);
        if (state != Verdict::Ok)
        {
            if (state == Verdict::Stale)
            {
                std::remove(asset.path.c_str());
            }
            if (const s32 res = fetch(asset); R_FAILED(res))
            {
                // Nothing else was inspected, so there is no verdict to leave behind.
                mVerdict.reset();
                return res;
            }
            // Only what was just written is hashed a second time. Assets that were
            // already right are not read again, here or after startup.
            state = inspect(asset, buffer);
        }
        result = worse(result, state);
    }

    mVerdict = result;
    return 0;
}
