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

#include "sav/SaveRecognition.hpp"
#include <cstdio>
#include <numeric>
#include <vector>

namespace
{
    int failures = 0;

    void check(bool condition, const char* what)
    {
        if (!condition)
        {
            failures++;
            std::printf("FAIL: %s\n", what);
        }
    }

    // Save data as a game leaves it: never a single repeated byte, which is what keeps
    // the padding rule from eating into it.
    std::vector<u8> saveData(size_t size)
    {
        std::vector<u8> data(size);
        std::iota(data.begin(), data.end(), u8(1));
        return data;
    }

    std::vector<u8> withTrailer(size_t dataSize, size_t trailerSize)
    {
        std::vector<u8> file = saveData(dataSize);
        for (size_t i = 0; i < trailerSize; i++)
        {
            file.push_back(u8(0x40 + i));
        }
        return file;
    }

    std::vector<u8> withPadding(size_t dataSize, size_t paddingSize, u8 filler)
    {
        std::vector<u8> file = saveData(dataSize);
        file.insert(file.end(), paddingSize, filler);
        return file;
    }

    void expectLayout(
        const std::vector<u8>& file, size_t dataSize, size_t trailerSize, const char* what)
    {
        const pksm::SaveLayout layout = pksm::recognizeSaveLayout(file);
        check(layout && layout.dataOffset == 0 && layout.dataSize == dataSize &&
                  layout.trailerSize == trailerSize,
            what);
    }

    void expectUnrecognized(const std::vector<u8>& file, const char* what)
    {
        check(!pksm::recognizeSaveLayout(file), what);
    }

    void desmumeSavesAreUnwrapped()
    {
        using namespace pksm::SaveSize;

        expectLayout(withTrailer(DS, 0x7A), DS, 0x7A, "a DeSmuME DS save is read as 512KB");
        expectLayout(withTrailer(GBA, 0x7A), GBA, 0x7A, "a DeSmuME GBA save is read as 128KB");
        expectLayout(withTrailer(GB_INT, 0x7A), GB_INT, 0x7A, "a DeSmuME GB save is read as 32KB");
        expectLayout(
            withTrailer(GB_JPN, 0x7A), GB_JPN, 0x7A, "a DeSmuME Japanese GB save is read as 64KB");
    }

    void realTimeClockTrailersAreUnwrapped()
    {
        using namespace pksm::SaveSize;

        // The sizes PKSM already knew about have to keep working.
        expectLayout(withTrailer(GB_INT, 0x10), GB_INT, 0x10, "a Gen I/II VC save is read as 32KB");
        expectLayout(
            withTrailer(GB_JPN, 0x10), GB_JPN, 0x10, "a Japanese Gen II VC save is read as 64KB");
        expectLayout(withTrailer(GB_INT, 0x30), GB_INT, 0x30, "a 0x30 GB trailer is read as 32KB");
        expectLayout(withTrailer(GB_JPN, 0x30), GB_JPN, 0x30, "a 0x30 GB trailer is read as 64KB");
        expectLayout(withTrailer(GBA, 0x10), GBA, 0x10, "a 0x10 GBA trailer is read as 128KB");

        // And the ones it did not: every even trailer in the window, plus the odd one
        // FlashGBX writes.
        for (size_t trailer = 0x0C; trailer <= 0x30; trailer += 2)
        {
            expectLayout(withTrailer(GBA, trailer), GBA, trailer,
                "every even trailer in the window unwraps a GBA save");
            expectLayout(withTrailer(GB_INT, trailer), GB_INT, trailer,
                "every even trailer in the window unwraps a GB save");
        }
        expectLayout(withTrailer(GB_INT, 0x07), GB_INT, 0x07, "the FlashGBX trailer is unwrapped");

        // Outside the window nothing is assumed.
        expectUnrecognized(withTrailer(GB_INT, 0x0A), "a trailer under the window is not stripped");
        expectUnrecognized(withTrailer(GB_INT, 0x0D), "an odd trailer is not stripped");
        expectUnrecognized(withTrailer(GB_INT, 0x32), "a trailer over the window is not stripped");

        // The DS games have no real-time clock trailer, so a DS-sized file plus one is
        // not a save PKSM should try to read.
        expectUnrecognized(withTrailer(DS, 0x10), "a DS save is not given a clock trailer");
    }

    void paddedSavesAreTrimmed()
    {
        using namespace pksm::SaveSize;

        expectLayout(withPadding(DS, DS, 0x00), DS, DS, "a DS save padded to 1MB with zeroes");
        expectLayout(withPadding(DS, DS, 0xFF), DS, DS, "a DS save padded to 1MB with 0xFF");
        expectLayout(withPadding(GBA, 0x20000, 0xFF), GBA, 0x20000, "a GBA save padded to 256KB");
        expectLayout(withPadding(GB_INT, 0x8000, 0x00), GB_INT, 0x8000, "a GB save padded to 64KB");

        // The larger save wins: a padded DS save also contains a GBA-sized prefix.
        const pksm::SaveLayout layout = pksm::recognizeSaveLayout(withPadding(DS, 0x1000, 0x00));
        check(layout && layout.dataSize == DS, "padding is trimmed down to the largest save size");

        // Real data after a known size is not padding.
        std::vector<u8> notPadding = saveData(DS + 0x1000);
        expectUnrecognized(notPadding, "a file whose tail holds data is left alone");

        // A file that is only filler is not a save either.
        expectUnrecognized(std::vector<u8>(DS * 2, 0x00), "an empty file is not a save");
    }

    void unrelatedFilesAreRejected()
    {
        expectUnrecognized({}, "an empty buffer is not a save");
        expectUnrecognized(saveData(0x100), "a tiny file is not a save");
        expectUnrecognized(saveData(0x8000 - 1), "a file just under a save size is not a save");
    }
}

int main()
{
    desmumeSavesAreUnwrapped();
    realTimeClockTrailersAreUnwrapped();
    paddedSavesAreTrimmed();
    unrelatedFilesAreRejected();

    if (failures == 0)
    {
        std::printf("SaveRecognition: all checks passed\n");
        return 0;
    }
    std::printf("SaveRecognition: %d check(s) failed\n", failures);
    return 1;
}
