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

#ifndef ITEM_HPP
#define ITEM_HPP

#include <3ds.h>
#include "generation.hpp"

class Item4;
class Item5;
class Item6;
class Item7;
class Item7b;

class Item
{
public:
    virtual Generation generation(void) const = 0;
    virtual u16 id(void) const = 0;
    virtual u16 count(void) const = 0;
    virtual std::pair<u8*, int> bytes(void) const = 0;
    virtual void id(u16 id) = 0;
    virtual void count(u16 id) = 0;
    virtual operator Item4(void) const;
    virtual operator Item5(void) const;
    virtual operator Item6(void) const;
    virtual operator Item7(void) const;
    virtual operator Item7b(void) const;
};

class Item4 : public Item
{
private:
    struct {
        u16 id;
        u16 count;
    } itemData;
public:
    Item4(u8* data = nullptr)
    {
        if (data)
        {
            std::copy(data, data + 4, (u8*)&itemData);
        }
    }
    Generation generation(void) const override { return Generation::FOUR; }
    u16 id(void) const override { return itemData.id; }
    void id(u16 v) override { itemData.id = v; }
    u16 count(void) const override { return itemData.count; }
    void count(u16 v) override { itemData.count = v; }
    std::pair<u8*, int> bytes(void) const override { return { (u8*)&itemData, sizeof(itemData) }; }
};

class Item5 : public Item
{
private:
    struct {
        u16 id;
        u16 count;
    } itemData;
public:
    Item5(u8* data = nullptr)
    {
        if (data)
        {
            std::copy(data, data + 4, (u8*)&itemData);
        }
    }
    Generation generation(void) const override { return Generation::FIVE; }
    u16 id(void) const override { return itemData.id; }
    void id(u16 v) override { itemData.id = v; }
    u16 count(void) const override { return itemData.count; }
    void count(u16 v) override { itemData.count = v; }
    std::pair<u8*, int> bytes(void) const override { return { (u8*)&itemData, sizeof(itemData) }; }
};

class Item6 : public Item
{
private:
    struct {
        u16 id;
        u16 count;
    } itemData;
public:
    Item6(u8* data = nullptr)
    {
        if (data)
        {
            std::copy(data, data + 4, (u8*)&itemData);
        }
    }
    Generation generation(void) const override { return Generation::SIX; }
    u16 id(void) const override { return itemData.id; }
    void id(u16 v) override { itemData.id = v; }
    u16 count(void) const override { return itemData.count; }
    void count(u16 v) override { itemData.count = v; }
    std::pair<u8*, int> bytes(void) const override { return { (u8*)&itemData, sizeof(itemData) }; }
};

class Item7 : public Item
{
private:
    struct {
        unsigned int id : 10;
        unsigned int count : 10;
        unsigned int freeSpace : 10;
        unsigned int newFlag : 1;
        unsigned int reserved : 1;
    } itemData;
public:
    Item7(u8* data = nullptr)
    {
        if (data)
        {
            std::copy(data, data + 4, (u8*)&itemData);
        }
    }
    Generation generation(void) const override { return Generation::SEVEN; }
    u16 id(void) const override { return itemData.id; }
    void id(u16 v) override { itemData.id = std::min(v, (u16)0x3FF); }
    u16 count(void) const override { return itemData.count; }
    void count(u16 v) override { itemData.count = std::min(v, (u16)0x3FF); }
    u16 freeSpaceIndex(void) const { return itemData.freeSpace; }
    void freeSpaceIndex(u16 v) { itemData.freeSpace = std::min(v, (u16)0x3FF); }
    bool newFlag(void) const { return itemData.newFlag; }
    void newFlag(bool v) { itemData.newFlag = v ? 1 : 0; }
    bool reserved(void) const { return itemData.reserved; }
    void reserved(bool v) { itemData.reserved = v ? 1 : 0; }
    std::pair<u8*, int> bytes(void) const override { return { (u8*)&itemData, sizeof(itemData) }; }
    operator Item7b(void) const override;
};

class Item7b : public Item
{
private:
    struct {
        unsigned int id : 15;
        unsigned int count : 15;
        unsigned int newFlag : 1;
        unsigned int reserved : 1;
    } itemData;
public:
    Item7b(u8* data = nullptr)
    {
        if (data)
        {
            std::copy(data, data + 4, (u8*)&itemData);
        }
    }
    Generation generation(void) const override { return Generation::SEVEN; }
    u16 id(void) const override { return itemData.id; }
    void id(u16 v) override { itemData.id = std::min(v, (u16)0x7FFF); }
    u16 count(void) const override { return itemData.count; }
    void count(u16 v) override { itemData.count = std::min(v, (u16)0x7FFF); }
    bool newFlag(void) const { return itemData.newFlag; }
    void newFlag(bool v) { itemData.newFlag = v ? 1 : 0; }
    bool reserved(void) const { return itemData.reserved; }
    void reserved(bool v) { itemData.reserved = v ? 1 : 0; }
    std::pair<u8*, int> bytes(void) const override { return { (u8*)&itemData, sizeof(itemData) }; }
    operator Item7(void) const override;
};

#endif