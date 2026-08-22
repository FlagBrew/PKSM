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

#ifndef ASSETSTORE_HPP
#define ASSETSTORE_HPP

#include "utils/coretypes.h"
#include <optional>

// The spritesheets PKSM downloads, and one place that says whether they are right.
//
// "Are the assets correct?" used to be asked twice on the way up - once before deciding
// whether to download, once after the GUI was already drawing with them - by two functions
// that did not know about each other. Both hashed both multi-MB sheets, and both slurped
// each whole file into a fresh heap buffer to do it. On an ARM11 reading off an SD card
// that is the single most expensive thing in the boot sequence, paid twice.
//
// An AssetStore hashes what is on the card at most once per asset per boot, streaming it in
// CHUNK_SIZE reads, and remembers the verdict. Anyone who asks again reads the memo.
class AssetStore
{
public:
    enum class Verdict
    {
        // Every asset is on the card and hashes to what this build expects.
        Ok,
        // Every asset is there, but at least one is not the file this build wants.
        Stale,
        // At least one asset is not on the card at all.
        Missing,
    };

    // Big enough that the read syscall, not the loop, is what costs; small enough that it
    // is nothing next to the sheets themselves.
    static constexpr u32 CHUNK_SIZE = 64 * 1024;

    AssetStore() = default;

    AssetStore(const AssetStore&)            = delete;
    AssetStore& operator=(const AssetStore&) = delete;

    // Hash whatever is on the card and record what it found. Reads every asset once.
    Verdict verify();

    // What the last pass concluded, without touching the card again. Verifies on first
    // call so asking before anything else has is still answered correctly.
    Verdict verdict()
    {
        if (!mVerdict)
        {
            verify();
        }
        return *mVerdict;
    }

    // Verify, then download whatever is missing or stale and verify that much again, so
    // the verdict left behind describes what is on the card now. Returns 0, or the failure
    // that stopped it, following libctru's convention that negative is failure.
    //
    // This is the whole of what the "assets" startup step has to do: the later check reads
    // verdict() and does no I/O.
    s32 ensure();

private:
    std::optional<Verdict> mVerdict;
};

#endif
