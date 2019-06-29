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

#ifndef STDIRECTORY_HPP
#define STDIRECTORY_HPP

#include "types.h"
#include <dirent.h>
#include <errno.h>
#include <string>
#include <vector>

struct STDirectoryEntry
{
    STDirectoryEntry(const std::string& name, bool directory) : name(name), directory(directory) {}
    std::string name;
    bool directory;
};

class STDirectory
{
public:
    STDirectory(const std::string& root);
    ~STDirectory(void){};

    Result error(void);
    std::string item(size_t index);
    bool folder(size_t index);
    bool good(void);
    size_t count(void);

private:
    std::vector<STDirectoryEntry> mList;
    Result mError;
    bool mGood;
};

#endif
