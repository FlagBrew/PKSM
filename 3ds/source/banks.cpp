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

#include "banks.hpp"
#include "FSStream.hpp"
#include "json.hpp"
#include "archive.hpp"

std::shared_ptr<Bank> Banks::bank = nullptr;

static nlohmann::json banks;

static Result saveJson()
{
    std::string jsonData = banks.dump(2);
    if (Configuration::getInstance().useExtData())
    {
        FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, u"/banks.json"));
        FSStream out(Archive::data(), u"/banks.json", FS_OPEN_WRITE, jsonData.size());
        if (out.good())
        {
            out.write(jsonData.data(), jsonData.size() + 1);
        }
        out.close();
        return out.result();
    }
    else
    {
        FILE* out = fopen("/3ds/PKSM/banks.json", "wt");
        if (out && !ferror(out))
        {
            fwrite(jsonData.data(), 1, jsonData.size() + 1, out);
        }
        int ret = ferror(out);
        fclose(out);
        return ret;
    }
}

static Result createJson()
{
    banks = nlohmann::json::array();
    banks.push_back("pksm_1");
    return saveJson();
}

Result Banks::init()
{
    Result res = 0;
    if (Configuration::getInstance().useExtData())
    {
        FSStream in = FSStream(Archive::data(), u"/banks.json", FS_OPEN_READ);
        if (in.good())
        {
            size_t size = in.size();
            char data[size];
            in.read(data, size);
            in.close();
            banks = nlohmann::json::parse(data, nullptr, false);
        }
        else
        {
            in.close();
            if (R_FAILED(res = createJson()))
                return res;
        }
    }
    else
    {
        FILE* in = fopen("/3ds/PKSM/banks.json", "rt");
        if (in)
        {
            banks = nlohmann::json::parse(in, nullptr, false);
            fclose(in);
        }
        else
        {
            fclose(in);
            if (R_FAILED(res = createJson()))
                return res;
        }
    }
    if (banks.is_discarded())
        return -1;

    loadBank(banks[0]);
    return 0;
}

void Banks::loadBank(const std::string& name)
{
    bank = std::make_shared<Bank>(name);
}

void Banks::removeBank(const std::string& name)
{
    if (banks.size() == 1)
    {
        return;
    }
    if (bank && bank->name() == name)
    {
        bank = nullptr;
    }
    remove(("/3ds/PKSM/banks/" + name + ".bnk").c_str());
    remove(("/3ds/PKSM/banks/" + name + ".json").c_str());
    FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, (u"/banks/" + StringUtils::UTF8toUTF16(name) + u".bnk").c_str()));
    FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, (u"/banks/" + StringUtils::UTF8toUTF16(name) + u".json").c_str()));
    for (auto i = banks.begin(); i != banks.end(); i++)
    {
        if (i->get<std::string>() == name)
        {
            banks.erase(i);
            break;
        }
    }
    loadBank(banks[0].get<std::string>());
}

std::vector<std::string> Banks::bankNames()
{
    return banks.get<std::vector<std::string>>();
}

void Banks::swapBank(size_t which1, size_t which2)
{
    if (which1 < banks.size() && which2 < banks.size())
    {
        banks[which1].swap(banks[which2]);
    }
}
