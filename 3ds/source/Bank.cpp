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

#include "Bank.hpp"
#include "Configuration.hpp"
#include "FSStream.hpp"
#include "archive.hpp"
#include "gui.hpp"
#include "PB7.hpp"

Bank::Bank()
{
    load();
}

void Bank::load(std::optional<bool> backupOverride)
{
    if (data)
    {
        delete[] data;
        data = nullptr;
    }
    if (io::exists("/3ds/PKSM/bank/bank.bin"))
    {
        bool deleteOld = true;
        FILE* in = fopen("/3ds/PKSM/bank/bank.bin", "rb");
        fseek(in, 0, SEEK_END);
        Gui::waitFrame(i18n::localize("BANK_CONVERT"));
        size_t oldSize = ftell(in);
        fseek(in, 0, SEEK_SET);
        u8* oldData = new u8[oldSize];
        if (!ferror(in))
        {
            fread(oldData, 1, oldSize, in);
        }
        else
        {
            Gui::error(i18n::localize("BANK_BAD_CONVERT"), errno);
            deleteOld = false;
        }
        fclose(in);

        Configuration::getInstance().storageSize(oldSize / 232 / 30);
        Configuration::getInstance().save();

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
                std::shared_ptr<PKX> pkm = std::make_shared<PK6>(pkmData, false);
                if ((pkm->encryptionConstant() == 0 && pkm->species() == 0))
                {
                    this->pkm(pkm, box, slot);
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
                    pkm = std::make_shared<PK7>(pkmData, false);
                }
                else if (((PK6*)pkm.get())->encounterType() != 0)
                {
                    if (((PK6*)pkm.get())->level() == 100) // Can be hyper trained
                    {
                        if (!pkm->gen4() || ((PK6*)pkm.get())->encounterType() > 24) // Either isn't from Gen 4 or has invalid encounter type
                        {
                            pkm = std::make_shared<PK7>(pkmData, false);
                        }
                    }
                }
                this->pkm(pkm, box, slot);
            }
        }

        for (int i = 0; i < Configuration::getInstance().storageSize(); i++)
        {
            boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
        }

        FILE* bkp = fopen("/3ds/PKSM/backups/bank.bin", "wb");
        fwrite(oldData, 1, oldSize, bkp);
        fclose(bkp);
        
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

    if (backupOverride.value_or(Configuration::getInstance().autoBackup()))
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
            createBank();
            needSave = true;
        }
    }
    else
    {
        Gui::waitFrame(i18n::localize("BANK_CREATE"));
        in.close();
        createBank();
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
        boxNames = nlohmann::json::parse(jsonData, nullptr, false);
        if (boxNames.is_discarded())
        {
            createJSON();
            needSave = true;
        }
        else
        {
            for (int i = boxNames.size(); i < Configuration::getInstance().storageSize(); i++)
            {
                boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
                if (!needSave)
                {
                    needSave = true;
                }
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
    else
    {
        sha256(prevHash.data(), data, size);
    }
}

void Bank::loadSD()
{
    bool needResize = false, needSave = false;
    FILE* in = fopen("/3ds/PKSM/banks/pksm_1.bnk", "rb");
    fseek(in, 0, SEEK_END);
    if (!ferror(in))
    {
        Gui::waitFrame(i18n::localize("BANK_LOAD"));
        size = ftell(in);
        fseek(in, 0, SEEK_SET);
        data = new u8[size];
        fread(data, 1, size, in);
        fclose(in);
        if (size != sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30)
        {
            needResize = true;
            needSave = true;
        }
        if (memcmp(data, BANK_MAGIC.data(), 8))
        {
            Gui::warn(i18n::localize("BANK_CORRUPT"));
            createBank();
            needSave = true;
            needResize = false;
        }
    }
    else
    {
        Gui::waitFrame(i18n::localize("BANK_CREATE"));
        fclose(in);
        createBank();
        needSave = true;
    }

    in = fopen("/3ds/PKSM/banks/pksm_1.json", "rb");
    if (!ferror(in))
    {
        boxNames = nlohmann::json::parse(in, nullptr, false);
        if (boxNames.is_discarded())
        {
            createJSON();
            needSave = true;
        }
        else
        {
            for (int i = boxNames.size(); i < Configuration::getInstance().storageSize(); i++)
            {
                boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
                if (!needSave)
                {
                    needSave = true;
                }
            }
        }
        fclose(in);
    }
    else
    {
        fclose(in);
        createJSON();
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
    else
    {
        sha256(prevHash.data(), data, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
    }
}

bool Bank::save() const
{
    Gui::waitFrame(i18n::localize("BANK_SAVE"));
    if (Configuration::getInstance().useExtData())
    {
        FSUSER_CreateDirectory(Archive::data(), fsMakePath(PATH_UTF16, u"/banks"), 0);
        FSStream out(Archive::data(), u"/banks/pksm_1.bnk", FS_OPEN_WRITE, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
        if (out.good())
        {
            out.write(data, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
            out.close();

            std::string jsonData = boxNames.dump(2);
            FSUSER_DeleteFile(Archive::data(), fsMakePath(PATH_UTF16, u"/banks/pksm_1.json"));
            out = FSStream(Archive::data(), u"/banks/pksm_1.json", FS_OPEN_WRITE, jsonData.size());
            if (out.good())
            {
                out.write(jsonData.data(), jsonData.size() + 1);
                sha256(prevHash.data(), data, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
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
        FILE* out = fopen("/3ds/PKSM/banks/pksm_1.bnk", "wb");
        if (!ferror(out))
        {
            fwrite(data, 1, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30, out);
            fclose(out);

            std::string jsonData = boxNames.dump(2);
            out = fopen("/3ds/PKSM/banks/pksm_1.json", "wt");
            if (!ferror(out))
            {
                fwrite(jsonData.data(), 1, jsonData.size(), out);
                sha256(prevHash.data(), data, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
            }
            else
            {
                Gui::error(i18n::localize("BANK_NAME_ERROR"), errno);
            }
            fclose(out);
            return true;
        }
        else
        {
            fclose(out);
            Gui::error(i18n::localize("BANK_SAVE_ERROR"), errno);
            return false;
        }
    }

    needsCheck = false;
}

void Bank::resize()
{
    Gui::showResizeStorage();
    size_t newSize = sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30;
    if (newSize != size)
    {
        u8* newData = new u8[newSize];
        std::copy(data, data + std::min(newSize, size), newData);
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

std::shared_ptr<PKX> Bank::pkm(int box, int slot) const
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
            return std::make_shared<PK4>(bank[index].data, false, party);

        case Generation::FIVE:
            for (int i = 260; i > 136; i--)
            {
                if (bank[index].data[i] != 0xFF)
                {
                    party = true;
                    break;
                }
            }
            return std::make_shared<PK5>(bank[index].data, false, party);

        case Generation::SIX:
            for (int i = 260; i > 232; i--)
            {
                if (bank[index].data[i] != 0xFF)
                {
                    party = true;
                    break;
                }
            }
            return std::make_shared<PK6>(bank[index].data, false, party);

        case Generation::SEVEN:
            for (int i = 260; i > 232; i--)
            {
                if (bank[index].data[i] != 0xFF)
                {
                    party = true;
                    break;
                }
            }
            return std::make_shared<PK7>(bank[index].data, false, party);

        case Generation::LGPE:
            return std::make_shared<PB7>(bank[index].data, false);

        case Generation::UNUSED:
        default:
            return std::make_shared<PK7>();
    }
}

void Bank::pkm(std::shared_ptr<PKX> pkm, int box, int slot)
{
    BankEntry* bank = (BankEntry*)(data + sizeof(BankHeader));
    int index = box * 30 + slot;
    BankEntry newEntry;
    if (pkm->species() == 0)
    {
        std::fill_n((char*) &newEntry, sizeof(BankEntry), 0xFF);
        bank[index] = newEntry;
        needsCheck = true;
        return;
    }
    newEntry.gen = pkm->generation();
    std::copy(pkm->rawData(), pkm->rawData() + pkm->getLength(), newEntry.data);
    if (pkm->getLength() < 260)
    {
        std::fill_n(newEntry.data + pkm->getLength(), 260 - pkm->getLength(), 0xFF);
    }
    bank[index] = newEntry;
    needsCheck = true;
}

void Bank::backup() const
{
    Gui::waitFrame(i18n::localize("BANK_BACKUP"));
    FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.bnk.bak"));
    FSUSER_DeleteFile(Archive::sd(), fsMakePath(PATH_UTF16, u"/3ds/PKSM/banks/pksm_1.json.bak"));

    FILE* out = fopen("/3ds/PKSM/banks/pksm_1.bnk.bak", "wb");
    fwrite(data, 1, size, out);
    fclose(out);

    std::string jsonData = boxNames.dump(2);
    out = fopen("/3ds/PKSM/banks/pksm_1.json.bak", "wt");
    fwrite(jsonData.data(), 1, jsonData.size(), out);
    fclose(out);
}

std::string Bank::boxName(int box) const
{
    return boxNames[box].get<std::string>();
}

void Bank::boxName(std::string name, int box)
{
    boxNames[box] = name;
}

void Bank::createJSON()
{
    boxNames = nlohmann::json::array();
    for (int i = 0; i < Configuration::getInstance().storageSize(); i++)
    {
        boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
    }
}

void Bank::createBank()
{
    if (data)
    {
        delete[] data;
    }
    data = new u8[size = sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30];
    std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), data);
    *(int*)(data + 8) = BANK_VERSION;
    std::fill_n(data + sizeof(BankHeader), sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30, 0xFF);
}

bool Bank::hasChanged() const
{
    if (!needsCheck)
    {
        return false;
    }
    u8 hash[SHA256_BLOCK_SIZE];
    sha256(hash, data, sizeof(BankHeader) + sizeof(BankEntry) * Configuration::getInstance().storageSize() * 30);
    if (memcmp(hash, prevHash.data(), SHA256_BLOCK_SIZE))
    {
        return true;
    }
    return false;
}
