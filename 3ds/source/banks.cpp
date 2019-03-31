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

nlohmann::json g_banks;

static Result saveJson()
{
    std::string jsonData = g_banks.dump(2);
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
        int ret = 0;
        if (out)
        {
            fwrite(jsonData.data(), 1, jsonData.size() + 1, out);
            ret = ferror(out);
        }
        else
        {
            ret = errno;
        }
        fclose(out);
        return -ret;
    }
}

static Result createJson()
{
    g_banks = nlohmann::json::object();
    g_banks["pksm_1"] = Configuration::getInstance().storageSize();
    return saveJson();
}

static int readSD()
{
    int ret = 0;
    FILE* in = fopen("/3ds/PKSM/banks.json", "rt");
    if (in)
    {
        g_banks = nlohmann::json::parse(in, nullptr, false);
        ret = ferror(in);
        fclose(in);
        return -ret;
    }
    else
    {
        fclose(in);
        return createJson();
    }
}

static Result readExtData()
{
    FSStream in(Archive::data(), u"/banks.json", FS_OPEN_READ);
    if (in.good())
    {
        size_t size = in.size();
        char data[size + 1];
        in.read(data, size);
        data[size] = '\0';
        in.close();
        g_banks = nlohmann::json::parse(data, nullptr, false);
    }
    else
    {
        in.close();
        return createJson();
    }
    return 0;
}

Result Banks::init()
{
    if (Configuration::getInstance().useExtData())
    {
        if (io::exists("/3ds/PKSM/banks.json"))
        {
            readSD();
            saveJson();
        }
        else
        {
            readExtData();
        }
    }
    else
    {
        FSStream in(Archive::data(), u"/banks.json", FS_OPEN_READ);
        if (in.good())
        {
            in.close();
            readExtData();
            saveJson();
        }
        else
        {
            in.close();
            readSD();
        }
    }
    if (g_banks.is_discarded())
        return -1;

    auto i = g_banks.find("pksm_1");
    if (i == g_banks.end())
    {
        i = g_banks.begin();
    }
    loadBank(i.key(), i.value());
    return 0;
}

bool Banks::loadBank(const std::string& name, std::optional<int> maxBoxes)
{
    if (!bank || bank->name() != name)
    {
        auto found = g_banks.find(name);
        if (found == g_banks.end())
        {
            g_banks[name] = maxBoxes.value_or(BANK_DEFAULT_SIZE);
            saveJson();
            found = g_banks.find(name);
        }
        bank = std::make_shared<Bank>(found.key(), found.value().get<int>());
        return true;
    }
    return false;
}

void Banks::removeBank(const std::string& name)
{
    if (g_banks.size() == 1)
    {
        return;
    }
    if (g_banks.count(name))
    {
        if (bank && bank->name() == name)
        {
            bank = nullptr;
            if (g_banks.begin().key() == name)
            {
                loadBank((g_banks.begin() + 1).key());
            }
            else
            {
                loadBank(g_banks.begin().key());
            }
        }
        remove(("/3ds/PKSM/banks/" + name + ".bnk").c_str());
        remove(("/3ds/PKSM/banks/" + name + ".json").c_str());
        FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, (u"/banks/" + StringUtils::UTF8toUTF16(name) + u".bnk").c_str()));
        FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, (u"/banks/" + StringUtils::UTF8toUTF16(name) + u".json").c_str()));
        for (auto i = g_banks.begin(); i != g_banks.end(); i++)
        {
            if (i.key() == name)
            {
                g_banks.erase(i);
                saveJson();
                break;
            }
        }
    }
}

std::vector<std::pair<std::string, int>> Banks::bankNames()
{
    std::vector<std::pair<std::string, int>> ret(g_banks.size());
    for (auto i = g_banks.begin(); i != g_banks.end(); i++)
    {
        ret[std::distance(g_banks.begin(), i)] = {i.key(), i.value().get<int>()};
    }
    return ret;
}

void Banks::renameBank(const std::string& oldName, const std::string& newName)
{
    if (oldName != newName && g_banks.count(oldName))
    {
        if (bank->name() == oldName)
        {
            if (!bank->setName(newName))
            {
                return;
            }
        }
        else
        {
            FSUSER_RenameFile(Archive::data(), fsMakePath(PATH_UTF16, (u"/banks/" + StringUtils::UTF8toUTF16(oldName) + u".bnk").c_str()), Archive::data(), fsMakePath(PATH_UTF16, (u"/banks/" + StringUtils::UTF8toUTF16(newName) + u".bnk").c_str()));
            FSUSER_RenameFile(Archive::data(), fsMakePath(PATH_UTF16, (u"/banks/" + StringUtils::UTF8toUTF16(oldName) + u".json").c_str()), Archive::data(), fsMakePath(PATH_UTF16, (u"/banks/" + StringUtils::UTF8toUTF16(newName) + u".json").c_str()));
            FSUSER_RenameFile(Archive::sd(), fsMakePath(PATH_UTF16, (u"/3ds/PKSM/banks/" + StringUtils::UTF8toUTF16(oldName) + u".bnk").c_str()), Archive::sd(), fsMakePath(PATH_UTF16, (u"/3ds/PKSM/banks/" + StringUtils::UTF8toUTF16(newName) + u".bnk").c_str()));
            FSUSER_RenameFile(Archive::sd(), fsMakePath(PATH_UTF16, (u"/3ds/PKSM/banks/" + StringUtils::UTF8toUTF16(oldName) + u".json").c_str()), Archive::sd(), fsMakePath(PATH_UTF16, (u"/3ds/PKSM/banks/" + StringUtils::UTF8toUTF16(newName) + u".json").c_str()));
        }
        g_banks[newName] = g_banks[oldName];
        g_banks.erase(oldName);
    }
}

void Banks::setBankSize(const std::string& name, int size)
{
    if (g_banks.count(name))
    {
        g_banks[name] = size;
        if (bank->name() == name && size != bank->boxes())
        {
            bank->resize(size);
        }
    }
}
