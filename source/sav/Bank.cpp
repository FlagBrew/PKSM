/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
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

#include "Bank.hpp"
#include "Configuration.hpp"
#include "FSStream.hpp"
#include "archive.hpp"
#include "gui.hpp"
#include "PB7.hpp"

Bank::Bank()
{
    if (Configuration::getInstance().useExtData())
    {
        FSStream in(Archive::data(), u"/pksm_1.bnk", FS_OPEN_READ);
        if (in.good())
        {
            Gui::warn("Yep");
            size = in.size();
            data = new u8[size];
            in.read(data, size);
            in.close();
            if (size != sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30)
            {
                resize();
            }
        }
        else
        {
            Gui::warn("Creating bank", "Please wait");
            in.close();
            data = new u8[size = sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30];
            std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), data);
            *(int*)(data + 8) = BANK_VERSION;
            std::fill_n(data + sizeof(BankHeader), sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30, 0xFF);
            save();
        }
        
        in = FSStream(Archive::data(), u"/pksm_1.json", FS_OPEN_READ);
        if (in.good())
        {
            size_t jsonSize = in.size();
            char jsonData[jsonSize + 1];
            in.read(jsonData, jsonSize);
            in.close();
            jsonData[jsonSize] = '\0';
            boxNames = nlohmann::json::parse(jsonData);
        }
        else
        {
            in.close();
            boxNames = nlohmann::json::array();
            for (int i = 0; i < Configuration::getInstance().storageSize(); i++)
            {
                boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
            }
        }
    }
    else
    {
        std::fstream in("/3ds/PKSM/banks/pksm_1.bnk", std::ios::in);
        if (in.good())
        {
            Gui::waitFrame("Loading bank", "Please wait");
            in.seekg(0, std::ios::end);
            size = in.tellg();
            in.seekg(0, std::ios::beg);
            data = new u8[size];
            in.read((char*) data, size);
            if (size != sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30)
            {
                resize();
            }
        }
        else
        {
            Gui::waitFrame("Creating bank", "Please wait");
            in.close();
            data = new u8[size = sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30];
            std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), data);
            *(int*)(data + 8) = BANK_VERSION;
            std::fill_n(data + sizeof(BankHeader), sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30, 0xFF);
            save();
        }

        in = std::fstream("/3ds/PKSM/banks/pksm_1.json", std::ios::in);
        if (in.good())
        {
            in >> boxNames;
            in.close();
        }
        else
        {
            in.close();
            boxNames = nlohmann::json::array();
            for (int i = 0; i < Configuration::getInstance().storageSize(); i++)
            {
                boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
            }
        }
    }

    if (Configuration::getInstance().autoBackup())
    {
        backup();
    }
}

void Bank::save() const
{
    if (Configuration::getInstance().useExtData())
    {
        std::u16string path = StringUtils::UTF8toUTF16("/pksm_1.bnk");
        FSStream out(Archive::data(), path, FS_OPEN_CREATE | FS_OPEN_WRITE, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
        if (out.good())
        {
            out.write(data, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
            out.close();

            std::string jsonData = boxNames.dump(2);
            path = StringUtils::UTF8toUTF16("/pksm_1.json");
            out = FSStream(Archive::data(), path, FS_OPEN_CREATE | FS_OPEN_WRITE, jsonData.size());
            if (out.good())
            {
                out.write(jsonData.data(), jsonData.size());
            }
            else
            {
                Gui::warn("Could not save box names!");
            }
            out.close();
        }
        else
        {
            out.close();
            Gui::warn("Could not save bank!", std::to_string(out.result()));
        }
    }
    else
    {
        std::ofstream out("/3ds/PKSM/banks/pksm_1.bnk");
        if (out.good())
        {
            out.write((char*) data, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
            out.close();

            std::string jsonData = boxNames.dump(2);
            out = std::ofstream("/3ds/PKSM/banks/pksm_1.json");
            if (out.good())
            {
                out.write(jsonData.data(), jsonData.size());
            }
            else
            {
                Gui::warn("Could not save box names!");
            }
            out.close();
        }
        else
        {
            out.close();
            Gui::warn("Could not save bank!");
        }
    }
}

void Bank::resize()
{
    size_t newSize = sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30;
    if (newSize != size)
    {
        u8* newData = new u8[newSize];
        std::copy(data, data + size, newData);
        delete[] data;
        if (newSize > size)
        {
            std::fill_n(newData + size, newSize - size, 0xFF);
        }
        data = newData;

        if (Configuration::getInstance().useExtData())
        {
            FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, u"/pksm_1.bnk"));
            FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, u"/pksm_1.json"));
        }
        else
        {
            FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.bnk"));
            FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.json"));
        }

        for (int i = (size - sizeof(BankHeader)) / 30; i < Configuration::getInstance().storageSize() * 30; i++)
        {
            boxNames[i] = "Box " + std::to_string(i);
        }

        save();
    }
    size = newSize;
}

std::unique_ptr<PKX> Bank::pkm(int box, int slot) const
{
    BankEntry* bank = (BankEntry*)(data + sizeof(BankHeader));
    int index = box * 30 + slot;
    bool party = false;
    switch (bank[index].gen)
    {
        case Generation::FOUR:
            for (int i = 260; i > 136; i--)
            {
                if (bank[index].data[i] != 0xFF)
                {
                    party = true;
                    break;
                }
            }
            return std::make_unique<PK4>(bank[index].data, false, party);

        case Generation::FIVE:
            for (int i = 260; i > 136; i--)
            {
                if (bank[index].data[i] != 0xFF)
                {
                    party = true;
                    break;
                }
            }
            return std::make_unique<PK5>(bank[index].data, false, party);

        case Generation::SIX:
            for (int i = 260; i > 232; i--)
            {
                if (bank[index].data[i] != 0xFF)
                {
                    party = true;
                    break;
                }
            }
            return std::make_unique<PK6>(bank[index].data, false, party);

        case Generation::SEVEN:
            for (int i = 260; i > 232; i--)
            {
                if (bank[index].data[i] != 0xFF)
                {
                    party = true;
                    break;
                }
            }
            return std::make_unique<PK7>(bank[index].data, false, party);

        case Generation::LGPE:
            return std::make_unique<PB7>(bank[index].data, false);

        case Generation(0xFF):
        default:
            return std::make_unique<PK7>();
    }
}

void Bank::pkm(PKX& pkm, int box, int slot)
{
    BankEntry* bank = (BankEntry*)(data + sizeof(BankHeader));
    int index = box * 30 + slot;
    BankEntry newEntry;
    if (pkm.species() == 0)
    {
        std::fill_n((char*) &newEntry, sizeof(BankEntry), 0xFF);
        bank[index] = newEntry;
        return;
    }
    newEntry.gen = pkm.generation();
    std::copy(pkm.rawData(), pkm.rawData() + pkm.getLength(), newEntry.data);
    if (pkm.getLength() < 260)
    {
        std::fill_n(newEntry.data + pkm.getLength(), 260 - pkm.getLength(), 0xFF);
    }
    bank[index] = newEntry;
}

void Bank::backup() const
{
    FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.bnk.bak"));
    FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.json.bak"));

    std::ofstream out("/3ds/PKSM/banks/pksm_1.bnk.bak");
    out.write((char*)data, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
    out.close();

    std::string jsonData = boxNames.dump(2);
    out = std::ofstream("/3ds/PKSM/banks/pksm_1.json.bak");
    out.write(jsonData.data(), jsonData.size());
    out.close();
}

std::string Bank::boxName(int box) const
{
    return boxNames[box].get<std::string>();
}

void Bank::boxName(std::string name, int box)
{
    boxNames[box] = name;
}