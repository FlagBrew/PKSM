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
#include "PB7.hpp"
#include "PK4.hpp"
#include "PK5.hpp"
#include "PK6.hpp"
#include "PK7.hpp"
#include "PK8.hpp"
#include "archive.hpp"
#include "banks.hpp"
#include "gui.hpp"
#include "io.hpp"

#define BANK(paths) paths.first
#define JSON(paths) paths.second
#define ARCHIVE Configuration::getInstance().useExtData() ? Archive::data() : Archive::sd()
#define OTHERARCHIVE Configuration::getInstance().useExtData() ? Archive::sd() : Archive::data()

Bank::Bank(const std::string& name, int maxBoxes) : bankName(name)
{
    load(maxBoxes);
}

void Bank::load(int maxBoxes)
{
    bool create = false;
    if (data)
    {
        delete[] data;
        data = nullptr;
    }
    needsCheck = false;
    if (name() == "pksm_1" && io::exists("/3ds/PKSM/bank/bank.bin"))
    {
        convertFromBankBin();
    }
    else
    {
        auto paths    = this->paths();
        bool needSave = false;
        FSStream in(ARCHIVE, BANK(paths), FS_OPEN_READ);
        if (in.good())
        {
            Gui::waitFrame(i18n::localize("BANK_LOAD"));
            BankHeader h{"BAD_MGC", 0, 0};
            size = in.size();
            in.read((char*)&h, sizeof(BankHeader) - sizeof(u32));
            if (memcmp(&h, BANK_MAGIC.data(), 8))
            {
                Gui::warn(i18n::localize("BANK_CORRUPT"));
                in.close();
                createBank(maxBoxes);
                needSave = true;
            }
            else
            {
                // NOTE: THIS IS THE CONVERSION SECTION. WILL NEED TO BE MODIFIED WHEN THE FORMAT IS CHANGED
                struct OldEntry
                {
                    Generation gen;
                    u8 data[260];
                };
                if (h.version == 1)
                {
                    h.boxes  = (size - (sizeof(BankHeader) - sizeof(u32))) / sizeof(OldEntry) / 30;
                    maxBoxes = h.boxes;
                    extern nlohmann::json g_banks;
                    g_banks[bankName] = maxBoxes;
                    Banks::saveJson();
                    size      = sizeof(BankHeader) + h.boxes * 30 * sizeof(BankEntry);
                    data      = new u8[size];
                    h.version = BANK_VERSION;
                    needSave  = true;

                    std::copy((char*)&h, (char*)(&h + 1), data);
                    for (size_t i = 0; i < h.boxes * 30; i++)
                    {
                        in.read(data + sizeof(BankHeader) + sizeof(BankEntry) * i, sizeof(OldEntry));
                        std::fill_n(data + sizeof(BankHeader) + sizeof(BankEntry) * i + sizeof(OldEntry), sizeof(BankEntry) - sizeof(OldEntry), 0xFF);
                    }
                    in.close();
                }
                else if (h.version == 2)
                {
                    in.read(&h.boxes, sizeof(u32));
                    size      = sizeof(BankHeader) + sizeof(BankEntry) * h.boxes;
                    data      = new u8[size];
                    h.version = BANK_VERSION;
                    needSave  = true;

                    std::copy((char*)&h, (char*)(&h + 1), data);
                    for (size_t i = 0; i < h.boxes * 30; i++)
                    {
                        in.read(data + sizeof(BankHeader) + sizeof(BankEntry) * i, sizeof(OldEntry));
                        std::fill_n(data + sizeof(BankHeader) + sizeof(BankEntry) * i + sizeof(OldEntry), sizeof(BankEntry) - sizeof(OldEntry), 0xFF);
                    }
                    in.close();
                }
                else if (h.version == BANK_VERSION)
                {
                    data = new u8[size];
                    in.read(&h.boxes, sizeof(u32));

                    std::copy((char*)&h, (char*)(&h + 1), data);
                    in.read(data + sizeof(BankHeader), size - sizeof(BankHeader));
                    in.close();
                }
                else
                {
                    Gui::warn(i18n::localize("THE_FUCK") + '\n' + i18n::localize("DO_NOT_DOWNGRADE"));
                    Gui::waitFrame(i18n::localize("BANK_CREATE"));
                    in.close();
                    createBank(maxBoxes);
                    needSave = true;
                    create   = true;
                }
            }
        }
        else
        {
            Gui::waitFrame(i18n::localize("BANK_CREATE"));
            in.close();
            createBank(maxBoxes);
            needSave = true;
            create   = true;
        }

        in = FSStream(ARCHIVE, JSON(paths), FS_OPEN_READ);
        if (in.good())
        {
            size_t jsonSize = in.size();
            char jsonData[jsonSize + 1];
            in.read(jsonData, jsonSize);
            in.close();
            jsonData[jsonSize] = '\0';
            boxNames           = nlohmann::json::parse(jsonData, nullptr, false);
            if (boxNames.is_discarded())
            {
                createJSON();
                needSave = true;
            }
            else
            {
                for (int i = boxNames.size(); i < boxes(); i++)
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
            createJSON();
            needSave = true;
        }

        if (boxes() != maxBoxes)
        {
            resize(maxBoxes);
        }

        if (needSave)
        {
            if (create)
            {
                saveWithoutBackup();
            }
            else
            {
                save();
            }
        }
        else
        {
            sha256(prevHash.data(), data, size);
            std::string nameData = boxNames.dump(2);
            sha256(prevNameHash.data(), (u8*)nameData.data(), nameData.size());
        }
    }
}

bool Bank::saveWithoutBackup() const
{
    auto paths = this->paths();
    Gui::waitFrame(i18n::localize("BANK_SAVE"));
    Archive::deleteFile(ARCHIVE, BANK(paths));
    FSStream out(ARCHIVE, BANK(paths), FS_OPEN_WRITE, sizeof(BankHeader) + sizeof(BankEntry) * boxes() * 30);
    if (out.good())
    {
        out.write(data, sizeof(BankHeader) + sizeof(BankEntry) * boxes() * 30);
        out.close();

        std::string jsonData = boxNames.dump(2);
        Archive::deleteFile(ARCHIVE, JSON(paths));
        out = FSStream(ARCHIVE, JSON(paths), FS_OPEN_WRITE, jsonData.size());
        if (out.good())
        {
            out.write(jsonData.data(), jsonData.size() + 1);
            sha256(prevHash.data(), data, sizeof(BankHeader) + sizeof(BankEntry) * boxes() * 30);
            sha256(prevNameHash.data(), (u8*)jsonData.data(), jsonData.size());
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
        Gui::error(i18n::localize("BANK_SAVE_ERROR"), out.result());
        out.close();
        return false;
    }

    needsCheck = false;
}

bool Bank::save() const
{
    if (Configuration::getInstance().autoBackup())
    {
        if (!backup() && !Gui::showChoiceMessage(i18n::localize("BACKUP_FAIL_SAVE_1") + '\n' + i18n::localize("BACKUP_FAIL_SAVE_2")))
        {
            return false;
        }
    }
    return saveWithoutBackup();
}

void Bank::resize(size_t boxes)
{
    size_t newSize = sizeof(BankHeader) + sizeof(BankEntry) * boxes * 30;
    auto paths     = this->paths();
    if (newSize != size)
    {
        Gui::showResizeStorage();
        u8* newData = new u8[newSize];
        std::copy(data, data + std::min(newSize, size), newData);
        delete[] data;
        if (newSize > size)
        {
            std::fill_n(newData + size, newSize - size, 0xFF);
        }
        data = newData;

        ((BankHeader*)data)->boxes = boxes;

        for (size_t i = boxNames.size(); i < boxes; i++)
        {
            boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
        }

        size = newSize;
        save();
    }
}

std::shared_ptr<PKX> Bank::pkm(int box, int slot) const
{
    BankEntry* bank = (BankEntry*)(data + sizeof(BankHeader));
    int index       = box * 30 + slot;
    switch (bank[index].gen)
    {
        case Generation::FOUR:
            return std::make_shared<PK4>(bank[index].data, false, false);
        case Generation::FIVE:
            return std::make_shared<PK5>(bank[index].data, false, false);
        case Generation::SIX:
            return std::make_shared<PK6>(bank[index].data, false, false);
        case Generation::SEVEN:
            return std::make_shared<PK7>(bank[index].data, false, false);
        case Generation::LGPE:
            return std::make_shared<PB7>(bank[index].data, false);
        case Generation::EIGHT:
            return std::make_shared<PK8>(bank[index].data, false, false);
        case Generation::UNUSED:
            return std::make_shared<PK7>();
    }
    return nullptr;
}

void Bank::pkm(std::shared_ptr<PKX> pkm, int box, int slot)
{
    BankEntry* bank = (BankEntry*)(data + sizeof(BankHeader));
    int index       = box * 30 + slot;
    BankEntry newEntry;
    if (pkm->species() == 0)
    {
        std::fill_n((char*)&newEntry, sizeof(BankEntry), 0xFF);
        bank[index] = newEntry;
        needsCheck  = true;
        return;
    }
    newEntry.gen = pkm->generation();
    std::copy(pkm->rawData(), pkm->rawData() + pkm->getLength(), newEntry.data);
    if (pkm->getLength() < sizeof(BankEntry::data))
    {
        std::fill_n(newEntry.data + pkm->getLength(), sizeof(BankEntry::data) - pkm->getLength(), 0xFF);
    }
    bank[index] = newEntry;
    needsCheck  = true;
}

bool Bank::backup() const
{
    Gui::waitFrame(i18n::localize("BANK_BACKUP"));
    auto paths = this->paths();
    Archive::copyFile(Archive::sd(), "/3ds/PKSM/backups/" + bankName + ".bnk.bak", Archive::sd(), "/3ds/PKSM/backups/" + bankName + ".bnk.bak.old");
    Archive::copyFile(Archive::sd(), "/3ds/PKSM/backups/" + bankName + ".json.bak", Archive::sd(), "/3ds/PKSM/backups/" + bankName + ".json.bak.old");
    Result res = Archive::copyFile(ARCHIVE, BANK(paths), Archive::sd(), "/3ds/PKSM/backups/" + bankName + ".bnk.bak");
    if (R_FAILED(res))
    {
        return false;
    }
    Archive::copyFile(ARCHIVE, JSON(paths), Archive::sd(), "/3ds/PKSM/backups/" + bankName + ".json.bak");
    return true;
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
    for (int i = 0; i < boxes(); i++)
    {
        boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
    }
}

void Bank::createBank(int maxBoxes)
{
    if (data)
    {
        delete[] data;
    }
    data = new u8[size = sizeof(BankHeader) + sizeof(BankEntry) * maxBoxes * 30];
    std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), data);
    *(int*)(data + 8)  = BANK_VERSION;
    *(int*)(data + 12) = maxBoxes;
    std::fill_n(data + sizeof(BankHeader), sizeof(BankEntry) * boxes() * 30, 0xFF);
}

bool Bank::hasChanged() const
{
    if (!needsCheck)
    {
        return false;
    }
    u8 hash[SHA256_BLOCK_SIZE];
    sha256(hash, data, sizeof(BankHeader) + sizeof(BankEntry) * boxes() * 30);
    if (memcmp(hash, prevHash.data(), SHA256_BLOCK_SIZE))
    {
        return true;
    }
    std::string jsonData = boxNames.dump(2);
    sha256(hash, (u8*)jsonData.data(), jsonData.size());
    if (memcmp(hash, prevNameHash.data(), SHA256_BLOCK_SIZE))
    {
        return true;
    }
    needsCheck = false;
    return false;
}

void Bank::convertFromBankBin()
{
    Gui::waitFrame(i18n::localize("BANK_CONVERT"));
    FSStream stream(Archive::sd(), "/3ds/PKSM/bank/bank.bin", FS_OPEN_READ);
    size_t oldSize = stream.size();
    u8* oldData    = new u8[oldSize];
    if (stream.good())
    {
        stream.read(oldData, oldSize);
    }
    else
    {
        Gui::error(i18n::localize("BANK_BAD_CONVERT"), stream.result());
        delete[] oldData;
        stream.close();
        return;
    }
    stream.close();

    // ANOTHER CONVERSION SECTION
    data = new u8[size = sizeof(BankHeader) + sizeof(BankEntry) * oldSize / 232];
    std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), data);
    ((BankHeader*)data)->version = BANK_VERSION;
    ((BankHeader*)data)->boxes   = oldSize / 232 / 30;
    extern nlohmann::json g_banks;
    g_banks["pksm_1"] = ((BankHeader*)data)->boxes;
    std::fill_n(data + sizeof(BankHeader), sizeof(BankEntry) * boxes() * 30, 0xFF);
    boxNames = nlohmann::json::array();

    for (int box = 0; box < std::min((int)oldSize / (232 * 30), boxes()); box++)
    {
        for (int slot = 0; slot < 30; slot++)
        {
            u8* pkmData              = oldData + box * (232 * 30) + slot * 232;
            std::shared_ptr<PKX> pkm = std::make_shared<PK6>(pkmData, false);
            if (pkm->species() == 0)
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
                if (pkm->relearnMove(i) > 621)
                {
                    badMove = true;
                    break;
                }
            }
            if (pkm->version() > 27 || pkm->species() > 721 || pkm->ability() > 191 || pkm->heldItem() > 775 || badMove)
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

    for (int i = 0; i < boxes(); i++)
    {
        boxNames[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
    }

    stream = FSStream(Archive::sd(), "/3ds/PKSM/backups/bank.bin", FS_OPEN_WRITE, oldSize);
    stream.write(oldData, oldSize);
    stream.close();

    delete[] oldData;

    if (save())
    {
        Archive::deleteFile(Archive::sd(), u"/3ds/PKSM/bank/bank.bin");
    }
}

const std::string& Bank::name() const
{
    return bankName;
}

int Bank::boxes() const
{
    return ((BankHeader*)data)->boxes;
}

bool Bank::setName(const std::string& name)
{
    auto oldPaths       = paths();
    std::string oldName = bankName;
    bankName            = name;
    auto newPaths       = paths();
    if (R_FAILED(Archive::moveFile(ARCHIVE, BANK(oldPaths), ARCHIVE, BANK(newPaths))))
    {
        bankName = oldName;
        return false;
    }
    if (R_FAILED(Archive::moveFile(ARCHIVE, JSON(oldPaths), ARCHIVE, JSON(newPaths))))
    {
        bankName = oldName;
        if (R_FAILED(Archive::moveFile(ARCHIVE, BANK(newPaths), ARCHIVE, BANK(oldPaths))))
        {
            Gui::warn(i18n::localize("CRITICAL_BANK_ERROR_1") + '\n' + i18n::localize("CRITICAL_BANK_ERROR_2"));
            return false;
        }
        return false;
    }
    return true;
}

std::pair<std::string, std::string> Bank::paths() const
{
    if (Configuration::getInstance().useExtData())
    {
        return {"/banks/" + bankName + ".bnk", "/banks/" + bankName + ".json"};
    }
    else
    {
        return {"/3ds/PKSM/banks/" + bankName + ".bnk", "/3ds/PKSM/banks/" + bankName + ".json"};
    }
}
