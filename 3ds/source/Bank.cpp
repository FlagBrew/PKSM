/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2020 Bernardo Giordano, Admiral Fish, piepie62
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
#include "Archive.hpp"
#include "Configuration.hpp"
#include "banks.hpp"
#include "format.h"
#include "gui.hpp"
#include "io.hpp"
#include "nlohmann/json.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK3.hpp"
#include "pkx/PK4.hpp"
#include "pkx/PK5.hpp"
#include "pkx/PK6.hpp"
#include "pkx/PK7.hpp"
#include "pkx/PK8.hpp"

#define BANK(paths) paths.first
#define JSON(paths) paths.second
#define ARCHIVE (Configuration::getInstance().useExtData() ? Archive::data() : Archive::sd())
#define OTHERARCHIVE (Configuration::getInstance().useExtData() ? Archive::sd() : Archive::data())

class BankException : public std::exception
{
public:
    BankException(u32 badVal) : string(fmt::format("BankException: Bad generation value: 0x{:X}", badVal)) {}

    const char* what() const noexcept override { return string.c_str(); }

private:
    std::string string;
};

Bank::Bank(const std::string& name, int maxBoxes) : bankName(name)
{
    load(maxBoxes);
}

Bank::~Bank()
{
    if (entries)
    {
        delete[] entries;
    }
}

void Bank::load(int maxBoxes)
{
    bool create = false;
    if (entries)
    {
        delete[] entries;
        entries = nullptr;
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
        auto in       = ARCHIVE.file(BANK(paths), FS_OPEN_READ);
        if (in)
        {
            Gui::waitFrame(i18n::localize("BANK_LOAD"));
            u32 size = in->size();
            in->read((char*)&header, sizeof(BankHeader::MAGIC) + sizeof(BankHeader::version));
            if (memcmp(header.MAGIC, BANK_MAGIC.data(), 8))
            {
                Gui::warn(i18n::localize("BANK_CORRUPT"));
                in->close();
                createBank(maxBoxes);
                needSave = true;
            }
            else
            {
                // NOTE: THIS IS THE CONVERSION SECTION. WILL NEED TO BE MODIFIED WHEN THE FORMAT IS CHANGED
                struct G7Entry
                {
                    pksm::Generation gen;
                    u8 data[260];
                };
                static_assert(sizeof(G7Entry) == 264);
                if (header.version == 1)
                {
                    header.boxes = (size - (sizeof(BankHeader) - sizeof(u32))) / sizeof(G7Entry) / 30;
                    maxBoxes     = header.boxes;
                    extern nlohmann::json g_banks;
                    g_banks[bankName] = maxBoxes;
                    Banks::saveJson();
                    entries        = new BankEntry[boxes() * 30];
                    header.version = BANK_VERSION;
                    needSave       = true;

                    for (int i = 0; i < boxes() * 30; i++)
                    {
                        in->read(entries + i, sizeof(G7Entry));
                        std::fill_n((u8*)(entries + i) + sizeof(G7Entry), sizeof(BankEntry) - sizeof(G7Entry), 0xFF);
                    }
                    in->close();
                }
                else if (header.version == 2)
                {
                    in->read(&header.boxes, sizeof(u32));
                    entries        = new BankEntry[boxes() * 30];
                    header.version = BANK_VERSION;
                    needSave       = true;

                    for (int i = 0; i < boxes() * 30; i++)
                    {
                        in->read(entries + i, sizeof(G7Entry));
                        std::fill_n((u8*)(entries + i) + sizeof(G7Entry), sizeof(BankEntry) - sizeof(G7Entry), 0xFF);
                    }
                    in->close();
                }
                else if (header.version == BANK_VERSION)
                {
                    in->read(&header.boxes, sizeof(u32));
                    entries = new BankEntry[boxes() * 30];

                    in->read(entries, size - sizeof(BankHeader));
                    in->close();
                }
                else
                {
                    Gui::warn(i18n::localize("THE_FUCK") + '\n' + i18n::localize("DO_NOT_DOWNGRADE"));
                    Gui::waitFrame(i18n::localize("BANK_CREATE"));
                    in->close();
                    createBank(maxBoxes);
                    needSave = true;
                    create   = true;
                }
            }
        }
        else
        {
            Gui::waitFrame(i18n::localize("BANK_CREATE"));
            createBank(maxBoxes);
            needSave = true;
            create   = true;
        }

        auto json = ARCHIVE.file(JSON(paths), FS_OPEN_READ);
        if (json)
        {
            size_t jsonSize = in->size();
            char* jsonData  = new char[jsonSize + 1];
            json->read(jsonData, jsonSize);
            json->close();
            jsonData[jsonSize] = '\0';
            boxNames           = std::make_unique<nlohmann::json>(nlohmann::json::parse(jsonData, nullptr, false));
            delete[] jsonData;
            if (boxNames->is_discarded())
            {
                createJSON();
                needSave = true;
            }
            else
            {
                for (int i = boxNames->size(); i < boxes(); i++)
                {
                    (*boxNames)[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
                    if (!needSave)
                    {
                        needSave = true;
                    }
                }
            }
        }
        else
        {
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
            prevHash             = pksm::crypto::sha256((u8*)entries, sizeof(BankEntry) * boxes() * 30);
            std::string nameData = boxNames->dump(2);
            prevNameHash         = pksm::crypto::sha256((u8*)nameData.data(), nameData.size());
        }
    }
}

bool Bank::saveWithoutBackup() const
{
    auto paths = this->paths();
    Gui::waitFrame(i18n::localize("BANK_SAVE"));
    ARCHIVE.deleteFile(BANK(paths));
    ARCHIVE.createFile(BANK(paths), 0, sizeof(BankHeader) + sizeof(BankEntry) * boxes() * 30);
    auto out = ARCHIVE.file(BANK(paths), FS_OPEN_WRITE);
    if (out)
    {
        out->write(&header, sizeof(BankHeader));
        out->write(entries, sizeof(BankEntry) * boxes() * 30);
        out->close();

        std::string jsonData = boxNames->dump(2);
        ARCHIVE.deleteFile(JSON(paths));
        ARCHIVE.createFile(JSON(paths), 0, jsonData.size() + 1);
        out = ARCHIVE.file(JSON(paths), FS_OPEN_WRITE, jsonData.size() + 1);
        if (out)
        {
            out->write(jsonData.data(), jsonData.size() + 1);
            prevHash     = pksm::crypto::sha256((u8*)entries, sizeof(BankEntry) * boxes() * 30);
            prevNameHash = pksm::crypto::sha256((u8*)jsonData.data(), jsonData.size());
            out->close();
        }
        else
        {
            Gui::error(i18n::localize("BANK_NAME_ERROR"), ARCHIVE.result());
        }
        needsCheck = false;
        return true;
    }
    else
    {
        Gui::error(i18n::localize("BANK_SAVE_ERROR"), ARCHIVE.result());
        return false;
    }
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

void Bank::resize(int boxes)
{
    auto paths = this->paths();
    if (this->boxes() != boxes)
    {
        Gui::showResizeStorage();
        BankEntry* newEntries = new BankEntry[boxes * 30];
        std::copy(entries, entries + std::min(boxes, this->boxes()) * 30, newEntries);
        delete[] entries;
        if (boxes > this->boxes())
        {
            std::fill_n((u8*)(newEntries + this->boxes() * 30), (boxes - this->boxes()) * 30 * sizeof(BankEntry), 0xFF);
        }
        entries = newEntries;

        header.boxes = boxes;

        for (int i = boxNames->size(); i < boxes; i++)
        {
            (*boxNames)[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
        }

        save();
    }
}

std::unique_ptr<pksm::PKX> Bank::pkm(int box, int slot) const
{
    int index = box * 30 + slot;
    auto ret  = pksm::PKX::getPKM(entries[index].gen, entries[index].data, false);
    if (ret)
    {
        return ret;
    }
    else if (entries[index].gen == pksm::Generation::UNUSED)
    {
        return pksm::PKX::getPKM<pksm::Generation::SEVEN>(nullptr);
    }

    throw BankException(u32(entries[index].gen));
}

void Bank::pkm(const pksm::PKX& pkm, int box, int slot)
{
    int index = box * 30 + slot;
    BankEntry newEntry;
    if (pkm.species() == pksm::Species::None)
    {
        std::fill_n((char*)&newEntry, sizeof(BankEntry), 0xFF);
        entries[index] = newEntry;
        needsCheck     = true;
        return;
    }
    newEntry.gen = pkm.generation();
    std::copy(pkm.rawData(), pkm.rawData() + std::min((u32)sizeof(BankEntry::data), pkm.getLength()), newEntry.data);
    if (pkm.getLength() < sizeof(BankEntry::data))
    {
        std::fill_n(newEntry.data + pkm.getLength(), sizeof(BankEntry::data) - pkm.getLength(), 0xFF);
    }
    entries[index] = newEntry;
    needsCheck     = true;
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
    return (*boxNames)[box].get<std::string>();
}

void Bank::boxName(std::string name, int box)
{
    (*boxNames)[box] = name;
}

void Bank::createJSON()
{
    boxNames = std::make_unique<nlohmann::json>(nlohmann::json::array());
    for (int i = 0; i < boxes(); i++)
    {
        (*boxNames)[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
    }
}

void Bank::createBank(int maxBoxes)
{
    std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), header.MAGIC);
    header.version = BANK_VERSION;
    header.boxes   = maxBoxes;
    if (entries)
    {
        delete[] entries;
    }
    entries = new BankEntry[maxBoxes * 30];
    std::fill_n((u8*)entries, sizeof(BankEntry) * boxes() * 30, 0xFF);
}

bool Bank::hasChanged() const
{
    if (!needsCheck)
    {
        return false;
    }
    auto hash = pksm::crypto::sha256((u8*)entries, sizeof(BankEntry) * boxes() * 30);
    if (hash != prevHash)
    {
        return true;
    }
    std::string jsonData = boxNames->dump(2);
    hash                 = pksm::crypto::sha256((u8*)jsonData.data(), jsonData.size());
    if (hash != prevNameHash)
    {
        return true;
    }
    needsCheck = false;
    return false;
}

void Bank::convertFromBankBin()
{
    Gui::waitFrame(i18n::localize("BANK_CONVERT"));
    auto inStream   = Archive::sd().file("/3ds/PKSM/bank/bank.bin", FS_OPEN_READ);
    Result inResult = Archive::sd().result();
    Archive::sd().createFile("/3ds/PKSM/backups/bank.bin", 0, 1);
    auto outStream   = Archive::sd().file("/3ds/PKSM/backups/bank.bin", FS_OPEN_WRITE);
    Result outResult = Archive::sd().result();
    if (inStream && outStream && inStream->size() % pksm::PK6::BOX_LENGTH == 0 && (inStream->size() / pksm::PK6::BOX_LENGTH) % 30 == 0 &&
        R_SUCCEEDED(outStream->resize(inStream->size())))
    {
        size_t oldSize = inStream->size();
        std::array<u8, pksm::PK6::BOX_LENGTH> pkmData;
        // ANOTHER CONVERSION SECTION
        entries = new BankEntry[oldSize / pksm::PK6::BOX_LENGTH];
        std::copy(BANK_MAGIC.data(), BANK_MAGIC.data() + BANK_MAGIC.size(), header.MAGIC);
        header.version = BANK_VERSION;
        header.boxes   = oldSize / pksm::PK6::BOX_LENGTH / 30;
        extern nlohmann::json g_banks;
        g_banks["pksm_1"] = header.boxes;
        std::fill_n((u8*)entries, sizeof(BankEntry) * boxes() * 30, 0xFF);
        boxNames = std::make_unique<nlohmann::json>(nlohmann::json::array());

        for (int box = 0; box < std::min((int)(oldSize / (pksm::PK6::BOX_LENGTH * 30)), boxes()); box++)
        {
            for (int slot = 0; slot < 30; slot++)
            {
                inStream->read(pkmData.data(), pkmData.size());
                outStream->write(pkmData.data(), pkmData.size());
                std::unique_ptr<pksm::PKX> pkm = pksm::PKX::getPKM<pksm::Generation::SIX>(pkmData.data());
                if (pkm->species() == pksm::Species::None)
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
                    if (pkm->relearnMove(i) > 621)
                    {
                        badMove = true;
                        break;
                    }
                }
                if (pkm->version() > pksm::GameVersion::OR || pkm->species() >= pksm::Species::Rowlet ||
                    pkm->ability() > pksm::Ability::DeltaStream || pkm->heldItem() > 775 || badMove)
                {
                    pkm = pksm::PKX::getPKM<pksm::Generation::SEVEN>(pkmData.data());
                }
                else if (((pksm::PK6*)pkm.get())->encounterType() != 0)
                {
                    if (((pksm::PK6*)pkm.get())->level() == 100) // Can be hyper trained
                    {
                        if (!pkm->originGen4() ||
                            ((pksm::PK6*)pkm.get())->encounterType() > 24) // Either isn't from Gen 4 or has invalid encounter type
                        {
                            pkm = pksm::PKX::getPKM<pksm::Generation::SEVEN>(pkmData.data());
                        }
                    }
                }
                this->pkm(*pkm, box, slot);
            }
        }

        inStream->close();
        outStream->close();

        for (int i = 0; i < boxes(); i++)
        {
            (*boxNames)[i] = i18n::localize("STORAGE") + " " + std::to_string(i + 1);
        }

        if (save())
        {
            Archive::sd().deleteFile(u"/3ds/PKSM/bank/bank.bin");
        }
    }
    else
    {
        Gui::error(i18n::localize("BANK_BAD_CONVERT"), R_FAILED(inResult) ? inResult : outResult);
    }
}

const std::string& Bank::name() const
{
    return bankName;
}

int Bank::boxes() const
{
    return header.boxes;
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
