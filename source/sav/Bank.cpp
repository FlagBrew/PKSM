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
    if (io::exists("/3ds/PKSM/bank/bank.bin"))
    {
        bool deleteOld = true;
        std::ifstream in("/3ds/PKSM/bank/bank.bin", std::ios::binary | std::ios::ate);
        Gui::waitFrame(i18n::localize("BANK_CONVERT"));
        size_t oldSize = in.tellg();
        in.seekg(0, std::ios::beg);
        u8* oldData = new u8[oldSize];
        if (in.good())
        {
            in.read((char*)oldData, oldSize);
        }
        else
        {
            Gui::error(i18n::localize("BANK_BAD_CONVERT"), errno);
            deleteOld = false;
        }
        in.close();

        data = new u8[size = sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30];
        std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), data);
        *(int*)(data + 8) = BANK_VERSION;
        std::fill_n(data + sizeof(BankHeader), sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30, 0xFF);
        boxNames = nlohmann::json::array();

        for (int box = 0; box < std::min((int) oldSize / (232 * 30), Configuration::getInstance().storageSize()); box++)
        {
            for (int slot = 0; slot < 30; slot++)
            {
                u8* pkmData = oldData + box * (232 * 30) + slot * 232;
                std::unique_ptr<PKX> pkm = std::make_unique<PK6>(pkmData, false);
                if (pkm->species() == 0)
                {
                    this->pkm(*pkm, box, slot);
                    continue;
                }
                bool badMove = false;
                for (int i = 0; i < 4; i++)
                {
                    if (pkm->move(i) > 621)
                    {
                        badMove = true;
                        break;
                    }
                    if (((PK6*)pkm.get())->relearnMove(i) > 621)
                    {
                        badMove = true;
                        break;
                    }
                }
                if (pkm->version() > 27
                    || pkm->species() > 721
                    || pkm->ability() > 191
                    || pkm->heldItem() > 775
                    || badMove)
                {
                    pkm = std::make_unique<PK7>(pkmData, false);
                }
                else if (((PK6*)pkm.get())->encounterType() != 0)
                {
                    if (((PK6*)pkm.get())->level() == 100) // Can be hyper trained
                    {
                        if (!pkm->gen4() || ((PK6*)pkm.get())->encounterType() > 24) // Either isn't from Gen 4 or has invalid encounter type
                        {
                            pkm = std::make_unique<PK7>(pkmData, false);
                        }
                    }
                }
                this->pkm(*pkm, box, slot);
            }
        }

        for (int i = 0; i < Configuration::getInstance().storageSize(); i++)
        {
            boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
        }

        std::ofstream bkp("/3ds/PKSM/backups/bank.bin");
        bkp.write((char*)oldData, oldSize);
        bkp.close();
        
        delete[] oldData;

        if (deleteOld)
        {
            FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/bank/bank.bin"));
        }
        save();
    }
    else if (Configuration::getInstance().useExtData())
    {
        if (io::exists("/3ds/PKSM/banks/pksm_1.bnk"))
        {
            loadSD();
            if (save()) // Make sure it's saved to ExtData before deletion
            {
                FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.bnk"));
                FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.json"));
            }
        }
        else
        {
            loadExtData();
        }
    }
    else
    {
        FSStream exists(Archive::data(), u"/banks/pksm_1.bnk", FS_OPEN_READ);
        if (exists.good())
        {
            exists.close();
            loadExtData();
            if (save())
            {
                FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, u"/banks/pksm_1.bnk"));
                FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, u"/banks/pksm_1.json"));
            }
        }
        else
        {
            loadSD();
        }
    }

    if (Configuration::getInstance().autoBackup())
    {
        backup();
    }
}

void Bank::loadExtData()
{
    bool needResize = false, needSave = false;
    FSStream in(Archive::data(), u"/banks/pksm_1.bnk", FS_OPEN_READ);
    if (in.good())
    {
        Gui::waitFrame(i18n::localize("BANK_LOAD"));
        size = in.size();
        data = new u8[size];
        in.read(data, size);
        in.close();
        if (size != sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30)
        {
            needResize = true;
            needSave = true;
        }
        if (memcmp(data, BANK_MAGIC.data(), 8))
        {
            Gui::warn(i18n::localize("BANK_CORRUPT"));
            goto createBank;
        }
    }
    else
    {
createBank:
        Gui::waitFrame(i18n::localize("BANK_CREATE"));
        in.close();
        data = new u8[size = sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30];
        std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), data);
        *(int*)(data + 8) = BANK_VERSION;
        std::fill_n(data + sizeof(BankHeader), sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30, 0xFF);
        needSave = true;
    }
    
    in = FSStream(Archive::data(), u"/banks/pksm_1.json", FS_OPEN_READ);
    if (in.good())
    {
        size_t jsonSize = in.size();
        char jsonData[jsonSize + 1];
        in.read(jsonData, jsonSize);
        in.close();
        jsonData[jsonSize] = '\0';
        boxNames = nlohmann::json::parse(jsonData);
        for (int i = boxNames.size(); i < Configuration::getInstance().storageSize(); i++)
        {
            boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
            if (!needSave)
            {
                needSave = true;
            }
        }
    }
    else
    {
        in.close();
        boxNames = nlohmann::json::array();
        for (int i = 0; i < Configuration::getInstance().storageSize(); i++)
        {
            boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
        }

        needSave = true;
    }
    if (needResize)
    {
        resize();
    }
    if (needSave)
    {
        save();
    }
}

void Bank::loadSD()
{
    bool needResize = false, needSave = false;
    std::fstream in("/3ds/PKSM/banks/pksm_1.bnk", std::ios::in | std::ios::binary | std::ios::ate);
    if (in.good())
    {
        Gui::waitFrame(i18n::localize("BANK_LOAD"));
        size = in.tellg();
        in.seekg(0, std::ios::beg);
        data = new u8[size];
        in.read((char*) data, size);
        in.close();
        if (size != sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30)
        {
            needResize = true;
            needSave = true;
        }
        if (memcmp(data, BANK_MAGIC.data(), 8))
        {
            Gui::warn(i18n::localize("BANK_CORRUPT"));
            goto createBank;
        }
    }
    else
    {
createBank:
        Gui::waitFrame(i18n::localize("BANK_LOAD"));
        in.close();
        data = new u8[size = sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30];
        std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), data);
        *(int*)(data + 8) = BANK_VERSION;
        std::fill_n(data + sizeof(BankHeader), sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30, 0xFF);
        needSave = true;
    }

    in = std::fstream("/3ds/PKSM/banks/pksm_1.json", std::ios::in);
    if (in.good())
    {
        in >> boxNames;
        for (int i = boxNames.size(); i < Configuration::getInstance().storageSize(); i++)
        {
            boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
            if (!needSave)
            {
                needSave = true;
            }
        }
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
        needSave = true;
    }

    if (needResize)
    {
        resize();
    }
    if (needSave)
    {
        save();
    }
}

bool Bank::save() const
{
    Gui::waitFrame(i18n::localize("BANK_SAVE"));
    if (Configuration::getInstance().useExtData())
    {
        FSUSER_CreateDirectory(Archive::data(), fsMakePath(PATH_UTF16, u"/banks"), 0);
        std::u16string path = StringUtils::UTF8toUTF16("/banks/pksm_1.bnk");
        FSStream out(Archive::data(), path, FS_OPEN_WRITE, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
        if (out.good())
        {
            out.write(data, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
            out.close();

            std::string jsonData = boxNames.dump(2);
            path = StringUtils::UTF8toUTF16("/banks/pksm_1.json");
            out = FSStream(Archive::data(), path, FS_OPEN_WRITE, jsonData.size());
            if (out.good())
            {
                out.write(jsonData.data(), jsonData.size() + 1);
            }
            else
            {
                Gui::error(i18n::localize("BANK_NAME_ERROR"), out.result());
            }
            out.close();
            return true;
        }
        else
        {
            out.close();
            Gui::error(i18n::localize("BANK_SAVE_ERROR"), out.result());
            return false;
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
                Gui::error(i18n::localize("BANK_NAME_ERROR"), errno);
            }
            out.close();
            return true;
        }
        else
        {
            out.close();
            Gui::error(i18n::localize("BANK_SAVE_ERROR"), errno);
            return false;
        }
    }
}

void Bank::resize()
{
    Gui::showResizeStorage();
    size_t newSize = sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30;
    if (newSize != size)
    {
        u8* newData = new u8[newSize];
        std::copy(data, data + newSize, newData);
        delete[] data;
        if (newSize > size)
        {
            std::fill_n(newData + size, newSize - size, 0xFF);
        }
        data = newData;

        if (Configuration::getInstance().useExtData())
        {
            FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, u"/banks/pksm_1.bnk"));
            FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, u"/banks/pksm_1.json"));
        }
        else
        {
            FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.bnk"));
            FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.json"));
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

        case Generation::UNUSED:
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
    Gui::waitFrame(i18n::localize("BANK_BACKUP"));
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