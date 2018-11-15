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

#include "HexEditScreen.hpp"
#include "loader.hpp"
#include <bitset>

static constexpr std::string_view marks[] = {
    "Circle",
    "Triangle",
    "Square",
    "Heart",
    "Star",
    "Diamond"
};
static constexpr std::string_view gen4ToggleTexts[] = {
    "Sinnoh Champ Ribbon",
    "Ability Ribbon",
    "Great Ability Ribbon",
    "Double Ability Ribbon",
    "Multi Ability Ribbon",
    "Pair Ability Ribbon",
    "World Ability Ribbon",
    "Alert Ribbon",
    "Shock Ribbon",
    "Downcast Ribbon",
    "Careless Ribbon",
    "Relax Ribbon",
    "Snooze Ribbon",
    "Smile Ribbon",
    "Gorgeous Ribbon",
    "Royal Ribbon",
    "Gorgeous Royal Ribbon",
    "Footprint Ribbon",
    "Record Ribbon",
    "History Ribbon",
    "Legend Ribbon",
    "Red Ribbon",
    "Green Ribbon",
    "Blue Ribbon",
    "Festival Ribbon",
    "Carnival Ribbon",
    "Classic Ribbon",
    "Premier Ribbon",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Cool Ribbon",
    "Cool Ribbon Super",
    "Cool Ribbon Hyper",
    "Cool Ribbon Master",
    "Beauty Ribbon",
    "Beauty Ribbon Super",
    "Beauty Ribbon Hyper",
    "Beauty Ribbon Master",
    "Cute Ribbon",
    "Cute Ribbon Super",
    "Cute Ribbon Hyper",
    "Cute Ribbon Master",
    "Smart Ribbon",
    "Smart Ribbon Super",
    "Smart Ribbon Hyper",
    "Smart Ribbon Master",
    "Tough Ribbon",
    "Tough Ribbon Super",
    "Tough Ribbon Hyper",
    "Tough Ribbon Master",
    "Champion Ribbon",
    "Winning Ribbon",
    "Victory Ribbon",
    "Artist Ribbon",
    "Effort Ribbon",
    "Battle Champion Ribbon",
    "Regional Champion Ribbon",
    "National Champion Ribbon",
    "Country Ribbon",
    "National Ribbon",
    "Earth Ribbon",
    "World Ribbon",
    "Cool Ribbon",
    "Cool Ribbon Great",
    "Cool Ribbon Ultra",
    "Cool Ribbon Master",
    "Beauty Ribbon",
    "Beauty Ribbon Great",
    "Beauty Ribbon Ultra",
    "Beauty Ribbon Master",
    "Cute Ribbon",
    "Cute Ribbon Great",
    "Cute Ribbon Ultra",
    "Cute Ribbon Master",
    "Smart Ribbon",
    "Smart Ribbon Great",
    "Smart Ribbon Ultra",
    "Smart Ribbon Master",
    "Tough Ribbon",
    "Tough Ribbon Great",
    "Tough Ribbon Ultra",
    "Tough Ribbon Master",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused"
};
static constexpr std::string_view gen5ToggleTexts[] = {
    "Sinnoh Champ Ribbon",
    "Ability Ribbon",
    "Great Ability Ribbon",
    "Double Ability Ribbon",
    "Multi Ability Ribbon",
    "Pair Ability Ribbon",
    "World Ability Ribbon",
    "Alert Ribbon",
    "Shock Ribbon",
    "Downcast Ribbon",
    "Careless Ribbon",
    "Relax Ribbon",
    "Snooze Ribbon",
    "Smile Ribbon",
    "Gorgeous Ribbon",
    "Royal Ribbon",
    "Gorgeous Royal Ribbon",
    "Footprint Ribbon",
    "Record Ribbon",
    "Event Ribbon",
    "Legend Ribbon",
    "World Champion Ribbon",
    "Birthday Ribbon",
    "Special Ribbon",
    "Souvenir Ribbon",
    "Wishing Ribbon",
    "Classic Ribbon",
    "Premier Ribbon",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Cool Ribbon",
    "Cool Ribbon Super",
    "Cool Ribbon Hyper",
    "Cool Ribbon Master",
    "Beauty Ribbon",
    "Beauty Ribbon Super",
    "Beauty Ribbon Hyper",
    "Beauty Ribbon Master",
    "Cute Ribbon",
    "Cute Ribbon Super",
    "Cute Ribbon Hyper",
    "Cute Ribbon Master",
    "Smart Ribbon",
    "Smart Ribbon Super",
    "Smart Ribbon Hyper",
    "Smart Ribbon Master",
    "Tough Ribbon",
    "Tough Ribbon Super",
    "Tough Ribbon Hyper",
    "Tough Ribbon Master",
    "Champion Ribbon",
    "Winning Ribbon",
    "Victory Ribbon",
    "Artist Ribbon",
    "Effort Ribbon",
    "Battle Champion Ribbon",
    "Regional Champion Ribbon",
    "National Champion Ribbon",
    "Country Ribbon",
    "National Ribbon",
    "Earth Ribbon",
    "World Ribbon",
    "Cool Ribbon",
    "Cool Ribbon Great",
    "Cool Ribbon Ultra",
    "Cool Ribbon Master",
    "Beauty Ribbon",
    "Beauty Ribbon Great",
    "Beauty Ribbon Ultra",
    "Beauty Ribbon Master",
    "Cute Ribbon",
    "Cute Ribbon Great",
    "Cute Ribbon Ultra",
    "Cute Ribbon Master",
    "Smart Ribbon",
    "Smart Ribbon Great",
    "Smart Ribbon Ultra",
    "Smart Ribbon Master",
    "Tough Ribbon",
    "Tough Ribbon Great",
    "Tough Ribbon Ultra",
    "Tough Ribbon Master",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused",
    "Unused"
};
static constexpr std::string_view gen67ToggleTexts[] = {
    "Unused",
    "Unused",
    "Sp. Attack Level 1",
    "HP Level 1",
    "Attack Level 1",
    "Sp. Defense Level 1",
    "Speed Level 1",
    "Defense Level 1",
    "Sp. Attack Level 2",
    "HP Level 2",
    "Attack Level 2",
    "Sp. Defense Level 2",
    "Speed Level 2",
    "Defense Level 2",
    "Sp. Attack Level 3",
    "HP Level 3",
    "Attack Level 3",
    "Sp. Defense Level 3",
    "Speed Level 3",
    "Defense Level 3",
    "The Troubles Keep on Coming?!",
    "The Leaf Stone Cup Begins!",
    "The Fire Stone Cup Begins!",
    "The Water Stone Cup Begins!",
    "Follow Those Fleeing Goals!",
    "Watch Out! That's One Tricky Second Half!",
    "An Opening of Lighting-Quick Attacks!",
    "Those Long Shots Are No Long Shot!",
    "Scatterbug Lugs Back!",
    "A Barrage of Bitbots!",
    "Drag Down Hydreigon!",
    "The Battle for the Best: Version X/Y!",
    "Kalos Champ Ribbon",
    "Champion Ribbon",
    "Sinnoh Champ Ribbon",
    "Best Friends Ribbon",
    "Training Ribbon",
    "Skillful Battler Ribbon",
    "Expert Battler Ribbon",
    "Effort Ribbon",
    "Alert Ribbon",
    "Shock Ribbon",
    "Downcast Ribbon",
    "Careless Ribbon",
    "Relax Ribbon",
    "Snooze Ribbon",
    "Smile Ribbon",
    "Gorgeous Ribbon",
    "Royal Ribbon",
    "Gorgeous Royal Ribbon",
    "Artist Ribbon",
    "Footprint Ribbon",
    "Record Ribbon",
    "Legend Ribbon",
    "Country Ribbon",
    "National Ribbon",
    "Earth Ribbon",
    "World Ribbon",
    "Classic Ribbon",
    "Premier Ribbon",
    "Event Ribbon",
    "Birthday Ribbon",
    "Special Ribbon",
    "Souvenir Ribbon",
    "Wishing Ribbon",
    "Battle Champion Ribbon",
    "Regional Champion Ribbon",
    "National Champion Ribbon",
    "World Champion Ribbon",
    "Unused",
    "Unused",
    "Hoenn Champion Ribbon",
    "Contest Star Ribbon",
    "Coolness Master Ribbon",
    "Beauty Master Ribbon",
    "Cuteness Master Ribbon",
    "Cleverness Master Ribbon",
    "Toughness Master Ribbon",
    "Unused",
    "Unused"
};

static int currRibbon = 0;

namespace {
    void checkValues(std::shared_ptr<PKX> pkm)
    {
        if (pkm->ability() > TitleLoader::save->maxAbility())
        {
            pkm->ability(TitleLoader::save->maxAbility());
        }
        if (pkm->ball() > TitleLoader::save->maxBall())
        {
            pkm->ball(TitleLoader::save->maxBall());
        }
        for (int i = 0; i < 4; i++)
        {
            if (pkm->move(i) > TitleLoader::save->maxMove())
            {
                pkm->move(i, TitleLoader::save->maxMove());
            }
            if (pkm->generation() == Generation::SIX)
            {
                if (((PK6*)pkm.get())->relearnMove(i) > TitleLoader::save->maxMove())
                {
                    ((PK6*)pkm.get())->relearnMove(i, TitleLoader::save->maxMove());
                }
            }
            if (pkm->generation() == Generation::SEVEN)
            {
                if (((PK7*)pkm.get())->relearnMove(i) > TitleLoader::save->maxMove())
                {
                    ((PK7*)pkm.get())->relearnMove(i, TitleLoader::save->maxMove());
                }
            }
        }
        if (pkm->species() > TitleLoader::save->maxSpecies())
        {
            pkm->species(TitleLoader::save->maxSpecies());
        }
        if (pkm->heldItem() > TitleLoader::save->maxItem())
        {
            pkm->heldItem(TitleLoader::save->maxItem());
        }
        u8 (*formCounter)(u16);
        switch (TitleLoader::save->generation())
        {
            case Generation::FOUR:
                formCounter = [](u16 species) -> u8 {
                    if (species == 201)
                    {
                        return 28;
                    }
                    else
                    {
                        u8 count = PersonalDPPtHGSS::formCount(species);
                        if (count == 0)
                        {
                            return 1;
                        }
                        return count;
                    }
                };
                break;
            case Generation::FIVE:
                formCounter = PersonalBWB2W2::formCount;
                break;
            case Generation::SIX:
                formCounter = PersonalXYORAS::formCount;
                break;
            case Generation::SEVEN:
            default:
                formCounter = PersonalSMUSUM::formCount;
                break;
        }
        u8 count = formCounter(pkm->species());
        if (pkm->alternativeForm() >= count)
        {
            pkm->alternativeForm(count - 1);
        }
    }
}

bool HexEditScreen::toggleBit(int selected, int offset)
{
    pkm->rawData()[selected] ^= 0x1 << offset;
    return true;
}

bool HexEditScreen::editNumber(bool high, bool up)
{
    u8* chosen = pkm->rawData() + hid.fullIndex();
    if (high)
    {
        u8 original = *chosen >> 4;
        *chosen &= 0x0F;
        if (up)
        {
            (*chosen) |= (original + 1) << 4;
        }
        else
        {
            (*chosen) |= (original - 1) << 4;
        }
    }
    else
    {
        u8 original = *chosen & 0x0F;
        if (original == 0x0 && !up)
        {
            *chosen |= 0x0F;
        }
        else if (original == 0xF && up)
        {
            *chosen &= 0xF0;
        }
        else if (up)
        {
            (*chosen)++;
        }
        else
        {
            (*chosen)--;
        }
    }
    return true;
}

std::pair<std::string, HexEditScreen::SecurityLevel> HexEditScreen::describe(int i) const
{
    static const std::pair<std::string, HexEditScreen::SecurityLevel> UNKNOWN = std::make_pair("Unknown", UNRESTRICTED);
    static const std::pair<std::string, HexEditScreen::SecurityLevel> UNUSED = std::make_pair("Unused", UNRESTRICTED);
    if (pkm->generation() == Generation::SIX || pkm->generation() == Generation::SEVEN)
    {
        switch (i)
        {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
                return std::make_pair("Encryption Key", UNRESTRICTED);
            case 0x04:
            case 0x05:
                return std::make_pair("Sanity Placeholder", UNRESTRICTED);
            case 0x06:
            case 0x07:
                return std::make_pair("Checksum", UNRESTRICTED);
            case 0x08:
            case 0x09:
                return std::make_pair("Species", NORMAL);
            case 0x0A:
            case 0x0B:
                return std::make_pair("Item", NORMAL);
            case 0x0C:
            case 0x0D:
                return std::make_pair("OT ID", NORMAL);
            case 0x0E:
            case 0x0F:
                return std::make_pair("OT SID", NORMAL);
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
                return std::make_pair("Experience", OPEN);
            case 0x14:
                return std::make_pair("Ability", NORMAL);
            case 0x15:
                return std::make_pair("Ability Number", OPEN);
            case 0x16:
            case 0x17:
                return std::make_pair("Training bag hits left", NORMAL);
            case 0x18:
            case 0x19:
            case 0x1A:
            case 0x1B:
                return std::make_pair("PID", NORMAL);
            case 0x1C:
                return std::make_pair("Nature", OPEN);
            //Gender, fateful encounter, and form bits
            case 0x1D:
                return std::make_pair("Gender, Fateful Encounter, Form", OPEN);
            case 0x1E:
                return std::make_pair("HP EV", NORMAL);
            case 0x1F:
                return std::make_pair("Attack EV", NORMAL);
            case 0x20:
                return std::make_pair("Defense EV", NORMAL);
            case 0x21:
                return std::make_pair("Speed EV", NORMAL);
            case 0x22:
                return std::make_pair("SP Attack EV", NORMAL);
            case 0x23:
                return std::make_pair("SP Defense EV", NORMAL);
            case 0x24:
                return std::make_pair("Contest Value: Cool", NORMAL);
            case 0x25:
                return std::make_pair("Contest Value: Beauty", NORMAL);
            case 0x26:
                return std::make_pair("Contest Value: Cute", NORMAL);
            case 0x27:
                return std::make_pair("Contest Value: Smart", NORMAL);
            case 0x28:
                return std::make_pair("Contest Value: Tough", NORMAL);
            case 0x29:
                return std::make_pair("Contest Value: Sheen", NORMAL);
            case 0x2A:
                return std::make_pair("Markings", NORMAL);
            case 0x2B:
                return std::make_pair("Pok\u00E9rus", NORMAL);
            case 0x2C:
            case 0x2D:
            case 0x2E:
            case 0x2F:
                return std::make_pair("Super Training Flags", NORMAL);
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
                return std::make_pair("Ribbons", NORMAL);
            case 0x36:
            case 0x37:
                return UNUSED;
            case 0x38:
                return std::make_pair("Contest Memory Ribbon Count", NORMAL);
            case 0x39:
                return std::make_pair("Battle Memory Ribbon Count", NORMAL);
            case 0x3A:
                return std::make_pair("Distribution Super Training Flags", NORMAL);
            case 0x3B:
            case 0x3C:
            case 0x3D:
            case 0x3E:
            case 0x3F:
                return UNUSED;
            case 0x40:
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x49:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
            case 0x4F:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
                return std::make_pair("Nickname", NORMAL);
            case 0x58:
            case 0x59:
                return std::make_pair("Null Terminator", UNRESTRICTED);
            case 0x5A:
            case 0x5B:
                return std::make_pair("Move 1 ID", NORMAL);
            case 0x5C:
            case 0x5D:
                return std::make_pair("Move 2 ID", NORMAL);
            case 0x5E:
            case 0x5F:
                return std::make_pair("Move 3 ID", NORMAL);
            case 0x60:
            case 0x61:
                return std::make_pair("Move 4 ID", NORMAL);
            case 0x62:
                return std::make_pair("Move 1 Current PP", OPEN);
            case 0x63:
                return std::make_pair("Move 2 Current PP", OPEN);
            case 0x64:
                return std::make_pair("Move 3 Current PP", OPEN);
            case 0x65:
                return std::make_pair("Move 4 Current PP", OPEN);
            case 0x66:
                return std::make_pair("Move 1 PP Ups", NORMAL);
            case 0x67:
                return std::make_pair("Move 2 PP Ups", NORMAL);
            case 0x68:
                return std::make_pair("Move 3 PP Ups", NORMAL);
            case 0x69:
                return std::make_pair("Move 5 PP Ups", NORMAL);
            case 0x6A:
            case 0x6B:
                return std::make_pair("Relearn Move 1 ID", NORMAL);
            case 0x6C:
            case 0x6D:
                return std::make_pair("Relearn Move 2 ID", NORMAL);
            case 0x6E:
            case 0x6F:
                return std::make_pair("Relearn Move 3 ID", NORMAL);
            case 0x70:
            case 0x71:
                return std::make_pair("Relearn Move 4 ID", NORMAL);
            case 0x72:
                return std::make_pair("Secret Super Training Flag", NORMAL);
            case 0x73:
                return UNUSED;
            case 0x74:
            case 0x75:
            case 0x76:
                return std::make_pair("IVs", NORMAL);
            case 0x77:
                return std::make_pair("IVs; Egg and Nicknamed Flag", NORMAL);
            case 0x78:
            case 0x79:
            case 0x7A:
            case 0x7B:
            case 0x7C:
            case 0x7D:
            case 0x7E:
            case 0x7F:
            case 0x80:
            case 0x81:
            case 0x82:
            case 0x83:
            case 0x84:
            case 0x85:
            case 0x86:
            case 0x87:
            case 0x88:
            case 0x89:
            case 0x8A:
            case 0x8B:
            case 0x8C:
            case 0x8D:
            case 0x8E:
            case 0x8F:
                return std::make_pair("Current Trainer Name", NORMAL);
            case 0x90:
            case 0x91:
                return std::make_pair("Null Terminator", UNRESTRICTED);
            case 0x92:
                return std::make_pair("Current Trainer Gender", NORMAL);
            case 0x93:
                return std::make_pair("Current Handler", NORMAL);
            case 0x94:
            case 0x95:
                return std::make_pair("Geolocation 1", NORMAL);
            case 0x96:
            case 0x97:
                return std::make_pair("Geolocation 2", NORMAL);
            case 0x98:
            case 0x99:
                return std::make_pair("Geolocation 3", NORMAL);
            case 0x9A:
            case 0x9B:
                return std::make_pair("Geolocation 4", NORMAL);
            case 0x9C:
            case 0x9D:
                return std::make_pair("Geolocation 5", NORMAL);
            case 0x9E:
            case 0x9F:
            case 0xA0:
            case 0xA1:
                return UNUSED;
            case 0xA2:
                return std::make_pair("Current Trainer Friendship", NORMAL);
            case 0xA3:
                return std::make_pair("Current Trainer Affection", NORMAL);
            case 0xA4:
                return std::make_pair("Current Trainer Memory Intensity", OPEN);
            case 0xA5:
                return std::make_pair("Current Trainer Memory Line", OPEN);
            case 0xA6:
                return std::make_pair("Current Trainer Memory Feeling", OPEN);
            case 0xA7:
                return UNUSED;
            case 0xA8:
            case 0xA9:
                return std::make_pair("Current Trainer Memory TextVar", OPEN);
            case 0xAA:
            case 0xAB:
            case 0xAC:
            case 0xAD:
                return UNUSED;
            case 0xAE:
                return std::make_pair("Fullness", NORMAL);
            case 0xAF:
                return std::make_pair("Enjoyment", NORMAL);
            case 0xB0:
            case 0xB1:
            case 0xB2:
            case 0xB3:
            case 0xB4:
            case 0xB5:
            case 0xB6:
            case 0xB7:
            case 0xB8:
            case 0xB9:
            case 0xBA:
            case 0xBB:
            case 0xBC:
            case 0xBD:
            case 0xBE:
            case 0xBF:
            case 0xC0:
            case 0xC1:
            case 0xC2:
            case 0xC3:
            case 0xC4:
            case 0xC5:
            case 0xC6:
            case 0xC7:
                return std::make_pair("Original Trainer Name", NORMAL);
            case 0xC8:
            case 0xC9:
                return std::make_pair("Null Terminator", UNRESTRICTED);
            case 0xCA:
                return std::make_pair("Original Trainer Friendship", NORMAL);
            case 0xCB:
                return std::make_pair("Original Trainer Affection", NORMAL);
            case 0xCC:
                return std::make_pair("Original Trainer Memory Intensity", OPEN);
            case 0xCD:
                return std::make_pair("Original Trainer Memory Line", OPEN);
            case 0xCE:
            case 0xCF:
                return std::make_pair("Original Trainer Memory TextVar", OPEN);
            case 0xD0:
                return std::make_pair("Original Trainer Memory Feeling", OPEN);
            case 0xD1:
            case 0xD2:
            case 0xD3:
                return std::make_pair("Egg Received Date", NORMAL);
            case 0xD4:
            case 0xD5:
            case 0xD6:
                return std::make_pair("Met Date", NORMAL);
            case 0xD7:
                return UNKNOWN;
            case 0xD8:
            case 0xD9:
                return std::make_pair("Egg Location", NORMAL);
            case 0xDA:
            case 0xDB:
                return std::make_pair("Met Location", NORMAL);
            case 0xDC:
                return std::make_pair("Pok\u00E9 Ball", NORMAL);
            case 0xDD:
                return std::make_pair("Met Level & Original Trainer Gender", NORMAL);
            case 0xDE:
                return std::make_pair("Gen 4 Encounter Type", OPEN);
            case 0xDF:
                return std::make_pair("Original Trainer Game ID", OPEN);
            case 0xE0:
                return std::make_pair("Country ID", OPEN);
            case 0xE1:
                return std::make_pair("Region ID", OPEN);
            case 0xE2:
                return std::make_pair("3DS Region ID", OPEN);
            case 0xE3:
                return std::make_pair("Original Trainer Language ID", NORMAL);
            case 0xE4:
            case 0xE5:
            case 0xE6:
            case 0xE7:
                return UNUSED;
            case 0xE8:
                return std::make_pair("Status Conditions", NORMAL);
            case 0xE9:
                return std::make_pair("Unknown Flags", UNRESTRICTED);
            case 0xEA:
            case 0xEB:
                return UNKNOWN;
            case 0xEC:
                return std::make_pair("Level", NORMAL);
            // Refresh dirt
            case 0xED:
                if (pkm->generation() == Generation::SEVEN)
                {
                    return std::make_pair("Dirt Type", OPEN);
                }
            case 0xEE:
                if (pkm->generation() == Generation::SEVEN)
                {
                    return std::make_pair("Dirt Location", OPEN);
                }
            case 0xEF:
                return UNKNOWN;
            case 0xF0:
            case 0xF1:
                return std::make_pair("Current HP", OPEN);
            case 0xF2:
            case 0xF3:
                return std::make_pair("Max HP", OPEN);
            case 0xF4:
            case 0xF5:
                return std::make_pair("Attack", OPEN);
            case 0xF6:
            case 0xF7:
                return std::make_pair("Defense", OPEN);
            case 0xF8:
            case 0xF9:
                return std::make_pair("Speed", OPEN);
            case 0xFA:
            case 0xFB:
                return std::make_pair("Sp. Attack", OPEN);
            case 0xFC:
            case 0xFD:
                return std::make_pair("Sp. Defense", OPEN);
            case 0xFE:
            case 0xFF:
            case 0x100:
            case 0x101:
            case 0x102:
            case 0x103:
                return UNKNOWN;
        }
    }
    else if (pkm->generation() == Generation::FIVE)
    {
        switch(i)
        {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
                return std::make_pair("PID", NORMAL);
            case 0x04:
            case 0x05:
                return UNUSED;
            case 0x06:
            case 0x07:
                return std::make_pair("Checksum", UNRESTRICTED);
            case 0x08:
            case 0x09:
                return std::make_pair("Species", NORMAL);
            case 0x0A:
            case 0x0B:
                return std::make_pair("Item", NORMAL);
            case 0x0C:
            case 0x0D:
                return std::make_pair("Original Trainer ID", NORMAL);
            case 0x0E:
            case 0x0F:
                return std::make_pair("Original Trainer SID", NORMAL);
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
                return std::make_pair("Experience", NORMAL);
            case 0x14:
                return std::make_pair("Friendship", NORMAL);
            case 0x15:
                return std::make_pair("Ability", NORMAL);
            case 0x16:
                return std::make_pair("Markings", NORMAL);
            case 0x17:
                return std::make_pair("Original Language", OPEN);
            case 0x18:
                return std::make_pair("HP EV", NORMAL);
            case 0x19:
                return std::make_pair("Attack EV", NORMAL);
            case 0x1A:
                return std::make_pair("Defense EV", NORMAL);
            case 0x1B:
                return std::make_pair("Speed EV", NORMAL);
            case 0x1C:
                return std::make_pair("Sp. Attack EV", NORMAL);
            case 0x1D:
                return std::make_pair("Sp. Defense EV", NORMAL);
            case 0x1E:
                return std::make_pair("Cool Contest Value", NORMAL);
            case 0x1F:
                return std::make_pair("Beauty Contest Value", NORMAL);
            case 0x20:
                return std::make_pair("Cute Contest Value", NORMAL);
            case 0x21:
                return std::make_pair("Smart Contest Value", NORMAL);
            case 0x22:
                return std::make_pair("Tough Contest Value", NORMAL);
            case 0x23:
                return std::make_pair("Sheen Contest Value", NORMAL);
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
                return std::make_pair("Ribbons", NORMAL);
            case 0x28:
            case 0x29:
                return std::make_pair("Move 1", NORMAL);
            case 0x2A:
            case 0x2B:
                return std::make_pair("Move 2", NORMAL);
            case 0x2C:
            case 0x2D:
                return std::make_pair("Move 3", NORMAL);
            case 0x2E:
            case 0x2F:
                return std::make_pair("Move 4", NORMAL);
            case 0x30:
                return std::make_pair("Move 1 PP", NORMAL);
            case 0x31:
                return std::make_pair("Move 2 PP", NORMAL);
            case 0x32:
                return std::make_pair("Move 3 PP", NORMAL);
            case 0x33:
                return std::make_pair("Move 4 PP", NORMAL);
            case 0x34:
                return std::make_pair("Move 1 PP Ups", NORMAL);
            case 0x35:
                return std::make_pair("Move 2 PP Ups", NORMAL);
            case 0x36:
                return std::make_pair("Move 3 PP Ups", NORMAL);
            case 0x37:
                return std::make_pair("Move 4 PP Ups", NORMAL);
            case 0x38:
            case 0x39:
            case 0x3A:
            case 0x3B:
                return std::make_pair("IVs, Egg, and Nicknamed Flags", NORMAL);
            case 0x3C:
            case 0x3D:
            case 0x3E:
            case 0x3F:
                return std::make_pair("Hoenn Ribbons", NORMAL);
            case 0x40:
                return std::make_pair("Gender & Forms", NORMAL);
            case 0x41:
                return std::make_pair("Nature", NORMAL);
            case 0x42:
                return std::make_pair("Misc. Flags", NORMAL);
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
                return UNUSED;
            case 0x48:
            case 0x49:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
            case 0x4F:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
            case 0x58:
            case 0x59:
            case 0x5A:
            case 0x5B:
            case 0x5C:
            case 0x5D:
                return std::make_pair("Nickname", NORMAL);
            case 0x5E:
                return UNKNOWN;
            case 0x5F:
                return std::make_pair("Origin Game", NORMAL);
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
                return std::make_pair("Ribbons", NORMAL);
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
                return UNUSED;
            case 0x68:
            case 0x69:
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E:
            case 0x6F:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
                return std::make_pair("Original Trainer Name", NORMAL);
            case 0x78:
            case 0x79:
            case 0x7A:
                return std::make_pair("Egg Date", NORMAL);
            case 0x7B:
            case 0x7C:
            case 0x7D:
                return std::make_pair("Met Date", NORMAL);
            case 0x7E:
            case 0x7F:
                return std::make_pair("Egg Location", OPEN);
            case 0x80:
            case 0x81:
                return std::make_pair("Met Location", OPEN);
            case 0x82:
                return std::make_pair("Pok\u00E9rus", NORMAL);
            case 0x83:
                return std::make_pair("Pok\u00E9 Ball", NORMAL);
            case 0x84:
                return std::make_pair("Met Level & Original Trainer Gender", NORMAL);
            case 0x85:
                return std::make_pair("Encounter Type", NORMAL);
            case 0x86:
            case 0x87:
                return UNUSED;
            case 0x88:
                return std::make_pair("Status Conditions", NORMAL);
            case 0x89:
                return std::make_pair("Unknown Flags", UNRESTRICTED);
            case 0x8A:
            case 0x8B:
                return UNKNOWN;
            case 0x8C:
                return std::make_pair("Level", NORMAL);
            case 0x8D:
                return std::make_pair("Capsule Index (seals)", OPEN);
            case 0x8E:
            case 0x8F:
                return std::make_pair("Current HP", OPEN);
            case 0x90:
            case 0x91:
                return std::make_pair("Max HP", OPEN);
            case 0x92:
            case 0x93:
                return std::make_pair("Attack", OPEN);
            case 0x94:
            case 0x95:
                return std::make_pair("Defense", OPEN);
            case 0x96:
            case 0x97:
                return std::make_pair("Speed", OPEN);
            case 0x98:
            case 0x99:
                return std::make_pair("Sp. Attack", OPEN);
            case 0x9A:
            case 0x9B:
                return std::make_pair("Sp. Defense", OPEN);
            case 0x9C:
            case 0x9D:
            case 0x9E:
            case 0x9F:
            case 0xA0:
            case 0xA1:
            case 0xA2:
            case 0xA3:
            case 0xA4:
            case 0xA5:
            case 0xA6:
            case 0xA7:
            case 0xA8:
            case 0xA9:
            case 0xAA:
            case 0xAB:
            case 0xAC:
            case 0xAD:
            case 0xAE:
            case 0xAF:
            case 0xB0:
            case 0xB1:
            case 0xB2:
            case 0xB3:
            case 0xB4:
            case 0xB5:
            case 0xB6:
            case 0xB7:
            case 0xB8:
            case 0xB9:
            case 0xBA:
            case 0xBB:
            case 0xBC:
            case 0xBD:
            case 0xBE:
            case 0xBF:
            case 0xC0:
            case 0xC1:
            case 0xC2:
            case 0xC3:
            case 0xC4:
            case 0xC5:
            case 0xC6:
            case 0xC7:
            case 0xC8:
            case 0xC9:
            case 0xCA:
            case 0xCB:
            case 0xCC:
            case 0xCD:
            case 0xCE:
            case 0xCF:
            case 0xD0:
            case 0xD1:
            case 0xD2:
            case 0xD3:
                return std::make_pair("Mail message + OT Name", OPEN);
            case 0xD4:
            case 0xD5:
            case 0xD6:
            case 0xD7:
            case 0xD8:
            case 0xD9:
            case 0xDA:
            case 0xDB:
                return UNKNOWN;
        }
    }
    else if (pkm->generation() == Generation::FOUR)
    {
        switch(i)
        {
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
                return std::make_pair("PID", NORMAL);
            case 0x04:
            case 0x05:
                return UNUSED;
            case 0x06:
            case 0x07:
                return std::make_pair("Checksum", UNRESTRICTED);
            case 0x08:
            case 0x09:
                return std::make_pair("Species", NORMAL);
            case 0x0A:
            case 0x0B:
                return std::make_pair("Item", NORMAL);
            case 0x0C:
            case 0x0D:
                return std::make_pair("Original Trainer ID", NORMAL);
            case 0x0E:
            case 0x0F:
                return std::make_pair("Original Trainer SID", NORMAL);
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
                return std::make_pair("Experience", NORMAL);
            case 0x14:
                return std::make_pair("Friendship", NORMAL);
            case 0x15:
                return std::make_pair("Ability", NORMAL);
            case 0x16:
                return std::make_pair("Markings", NORMAL);
            case 0x17:
                return std::make_pair("Original Language", OPEN);
            case 0x18:
                return std::make_pair("HP EV", NORMAL);
            case 0x19:
                return std::make_pair("Attack EV", NORMAL);
            case 0x1A:
                return std::make_pair("Defense EV", NORMAL);
            case 0x1B:
                return std::make_pair("Speed EV", NORMAL);
            case 0x1C:
                return std::make_pair("Sp. Attack EV", NORMAL);
            case 0x1D:
                return std::make_pair("Sp. Defense EV", NORMAL);
            case 0x1E:
                return std::make_pair("Cool Contest Value", NORMAL);
            case 0x1F:
                return std::make_pair("Beauty Contest Value", NORMAL);
            case 0x20:
                return std::make_pair("Cute Contest Value", NORMAL);
            case 0x21:
                return std::make_pair("Smart Contest Value", NORMAL);
            case 0x22:
                return std::make_pair("Tough Contest Value", NORMAL);
            case 0x23:
                return std::make_pair("Sheen Contest Value", NORMAL);
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
                return std::make_pair("Ribbons", NORMAL);
            case 0x28:
            case 0x29:
                return std::make_pair("Move 1", NORMAL);
            case 0x2A:
            case 0x2B:
                return std::make_pair("Move 2", NORMAL);
            case 0x2C:
            case 0x2D:
                return std::make_pair("Move 3", NORMAL);
            case 0x2E:
            case 0x2F:
                return std::make_pair("Move 4", NORMAL);
            case 0x30:
                return std::make_pair("Move 1 PP", NORMAL);
            case 0x31:
                return std::make_pair("Move 2 PP", NORMAL);
            case 0x32:
                return std::make_pair("Move 3 PP", NORMAL);
            case 0x33:
                return std::make_pair("Move 4 PP", NORMAL);
            case 0x34:
                return std::make_pair("Move 1 PP Ups", NORMAL);
            case 0x35:
                return std::make_pair("Move 2 PP Ups", NORMAL);
            case 0x36:
                return std::make_pair("Move 3 PP Ups", NORMAL);
            case 0x37:
                return std::make_pair("Move 4 PP Ups", NORMAL);
            case 0x38:
            case 0x39:
            case 0x3A:
            case 0x3B:
                return std::make_pair("IVs, Egg, and Nicknamed Flags", NORMAL);
            case 0x3C:
            case 0x3D:
            case 0x3E:
            case 0x3F:
                return std::make_pair("Hoenn Ribbons", NORMAL);
            case 0x40:
                return std::make_pair("Gender & Forms", NORMAL);
            case 0x41:
                return std::make_pair("Shiny Leaves (HGSS)", NORMAL);
            case 0x42:
            case 0x43:
                return UNUSED;
            case 0x44:
            case 0x45:
                return std::make_pair("Egg Location (Platinum)", NORMAL);
            case 0x46:
            case 0x47:
                return std::make_pair("Met Location (Platinum)", NORMAL);
            case 0x48:
            case 0x49:
            case 0x4A:
            case 0x4B:
            case 0x4C:
            case 0x4D:
            case 0x4E:
            case 0x4F:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
            case 0x58:
            case 0x59:
            case 0x5A:
            case 0x5B:
            case 0x5C:
            case 0x5D:
                return std::make_pair("Nickname", NORMAL);
            case 0x5E:
                return UNUSED;
            case 0x5F:
                return std::make_pair("Origin Game", NORMAL);
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
                return std::make_pair("Ribbons", NORMAL);
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
                return UNUSED;
            case 0x68:
            case 0x69:
            case 0x6A:
            case 0x6B:
            case 0x6C:
            case 0x6D:
            case 0x6E:
            case 0x6F:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
                return std::make_pair("Original Trainer Name", NORMAL);
            case 0x78:
            case 0x79:
            case 0x7A:
                return std::make_pair("Egg Date", NORMAL);
            case 0x7B:
            case 0x7C:
            case 0x7D:
                return std::make_pair("Met Date", NORMAL);
            case 0x7E:
            case 0x7F:
                return std::make_pair("Egg Location (Diamond & Pearl)", OPEN);
            case 0x80:
            case 0x81:
                return std::make_pair("Met Location (Diamond & Pearl", OPEN);
            case 0x82:
                return std::make_pair("Pok\u00E9rus", NORMAL);
            case 0x83:
                return std::make_pair("Pok\u00E9 Ball", NORMAL);
            case 0x84:
                return std::make_pair("Met Level & Original Trainer Gender", NORMAL);
            case 0x85:
                return std::make_pair("Encounter Type", NORMAL);
            case 0x86:
                return std::make_pair("HGSS Pok\u00E9 Ball", NORMAL);
            case 0x87:
                return UNUSED;
            case 0x88:
                return std::make_pair("Status Conditions", NORMAL);
            case 0x89:
                return std::make_pair("Unknown Flags", UNRESTRICTED);
            case 0x8A:
            case 0x8B:
                return UNKNOWN;
            case 0x8C:
                return std::make_pair("Level", NORMAL);
            case 0x8D:
                return std::make_pair("Capsule Index (seals)", OPEN);
            case 0x8E:
            case 0x8F:
                return std::make_pair("Current HP", OPEN);
            case 0x90:
            case 0x91:
                return std::make_pair("Max HP", OPEN);
            case 0x92:
            case 0x93:
                return std::make_pair("Attack", OPEN);
            case 0x94:
            case 0x95:
                return std::make_pair("Defense", OPEN);
            case 0x96:
            case 0x97:
                return std::make_pair("Speed", OPEN);
            case 0x98:
            case 0x99:
                return std::make_pair("Sp. Attack", OPEN);
            case 0x9A:
            case 0x9B:
                return std::make_pair("Sp. Defense", OPEN);
            case 0x9C:
            case 0x9D:
            case 0x9E:
            case 0x9F:
            case 0xA0:
            case 0xA1:
            case 0xA2:
            case 0xA3:
            case 0xA4:
            case 0xA5:
            case 0xA6:
            case 0xA7:
            case 0xA8:
            case 0xA9:
            case 0xAA:
            case 0xAB:
            case 0xAC:
            case 0xAD:
            case 0xAE:
            case 0xAF:
            case 0xB0:
            case 0xB1:
            case 0xB2:
            case 0xB3:
            case 0xB4:
            case 0xB5:
            case 0xB6:
            case 0xB7:
            case 0xB8:
            case 0xB9:
            case 0xBA:
            case 0xBB:
            case 0xBC:
            case 0xBD:
            case 0xBE:
            case 0xBF:
            case 0xC0:
            case 0xC1:
            case 0xC2:
            case 0xC3:
            case 0xC4:
            case 0xC5:
            case 0xC6:
            case 0xC7:
            case 0xC8:
            case 0xC9:
            case 0xCA:
            case 0xCB:
            case 0xCC:
            case 0xCD:
            case 0xCE:
            case 0xCF:
            case 0xD0:
            case 0xD1:
            case 0xD2:
            case 0xD3:
                return std::make_pair("Mail message + OT Name", OPEN);
            case 0xD4:
            case 0xD5:
            case 0xD6:
            case 0xD7:
            case 0xD8:
            case 0xD9:
            case 0xDA:
            case 0xDB:
            case 0xDC:
            case 0xDD:
            case 0xDE:
            case 0xDF:
            case 0xE0:
            case 0xE1:
            case 0xE2:
            case 0xE3:
            case 0xE4:
            case 0xE5:
            case 0xE6:
            case 0xE7:
            case 0xE8:
            case 0xE9:
            case 0xEA:
            case 0xEB:
                return std::make_pair("Seal Coordinates", OPEN);
        }
    }
    return std::make_pair("Report this to FlagBrew", UNRESTRICTED);
}

HexEditScreen::HexEditScreen(std::shared_ptr<PKX> pkm) : pkm(pkm), hid(240, 16)
{
    currRibbon = 0;
    for (u32 i = 0; i < pkm->length; i++)
    {
        std::vector<HexEditButton*> newButtons;
        buttons.push_back(newButtons);
        auto edit = [i, this](bool high, bool up) {
            editNumber(high, up);
            for (size_t j = 4; j < buttons[i].size(); j++)
            {
                buttons[i][j]->setToggled((this->pkm->rawData()[i] >> buttons[i][j]->bit()) & 0x1);
            }
            return true;
        };
        buttons[i].push_back(new HexEditButton(146, 35, 13, 13, [edit](){ return edit(true, true); }, ui_sheet_button_plus_small_idx, "", false, 0));
        buttons[i].push_back(new HexEditButton(161, 35, 13, 13, [edit](){ return edit(false, true); }, ui_sheet_button_plus_small_idx, "", false, 0));
        buttons[i].push_back(new HexEditButton(146, 75, 13, 13, [edit](){ return edit(true, false); }, ui_sheet_button_minus_small_idx, "", false, 0));
        buttons[i].push_back(new HexEditButton(161, 75, 13, 13, [edit](){ return edit(false, false); }, ui_sheet_button_minus_small_idx, "", false, 0));
        if (pkm->gen6() || pkm->gen7())
        {
            switch (i)
            {
                // Fateful Encounter
                case 0x1D:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, "Fateful Encounter", true, 0));
                    buttons[i].back()->setToggled(pkm->rawData()[i] & 0x1);
                    break;
                // Markings
                case 0x2A:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 6; j++)
                    {
                        buttons[i].push_back(new HexEditButton(70, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, std::string(marks[j]), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                    }
                    break;
                // Super Training Flags
                case 0x2C:
                case 0x2D:
                case 0x2E:
                case 0x2F:
                // Ribbons
                case 0x30:
                case 0x31:
                case 0x32:
                case 0x33:
                case 0x34:
                case 0x35:
                // Distribution Super Training (???)
                case 0x3A:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        buttons[i].push_back(new HexEditButton(70, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, std::string(gen67ToggleTexts[currRibbon]), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // Secret Super Training Flag
                case 0x72:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, "Secret Super Training", true, 0));
                    buttons[i].back()->setToggled(pkm->rawData()[i] & 0x1);
                    break;
                // Egg, & Nicknamed Flag
                case 0x77:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, "Egg", true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 106, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, "Nicknamed", true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // OT Gender
                case 0xDD:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, "Female OT", true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Status
                case 0xE8:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 3); }, ui_sheet_emulated_toggle_green_idx, "Poisoned", true, 3));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 106, 13, 13, [this, i](){ return this->toggleBit(i, 4); }, ui_sheet_emulated_toggle_green_idx, "Burned", true, 4));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 122, 13, 13, [this, i](){ return this->toggleBit(i, 5); }, ui_sheet_emulated_toggle_green_idx, "Frozen", true, 5));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 138, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, "Paralyzed", true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 154, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, "Toxic", true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
            }
        }
        else if (pkm->gen5())
        {
            switch (i)
            {
                // Markings
                case 0x16:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 6; j++)
                    {
                        buttons[i].push_back(new HexEditButton(70, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, std::string(marks[j]), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                    }
                    break;
                // Ribbons
                case 0x24:
                case 0x25:
                case 0x26:
                case 0x27:
                case 0x3C:
                case 0x3D:
                case 0x3E:
                case 0x3F:
                case 0x60:
                case 0x61:
                case 0x62:
                case 0x63:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        buttons[i].push_back(new HexEditButton(70, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, std::string(gen5ToggleTexts[currRibbon]), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // Egg and Nicknamed Flags
                case 0x3B:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, "Egg", true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 106, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, "Nicknamed", true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Fateful Encounter
                case 0x40:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, "Fateful Encounter", true, 0));
                    buttons[i].back()->setToggled(pkm->rawData()[i] & 0x1);
                    break;
                // DreamWorldAbility & N's Pokemon Flags
                case 0x42:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, "Hidden Ability?", true, 0));
                    buttons[i].back()->setToggled(pkm->rawData()[i] & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 106, 13, 13, [this, i](){ return this->toggleBit(i, 1); }, ui_sheet_emulated_toggle_green_idx, "N\'s Pokemon?", true, 1));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 1) & 0x1);
                    break;
                // OT Gender
                case 0x84:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, "Female OT", true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Status
                case 0x88:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 3); }, ui_sheet_emulated_toggle_green_idx, "Poisoned", true, 3));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 106, 13, 13, [this, i](){ return this->toggleBit(i, 4); }, ui_sheet_emulated_toggle_green_idx, "Burned", true, 4));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 122, 13, 13, [this, i](){ return this->toggleBit(i, 5); }, ui_sheet_emulated_toggle_green_idx, "Frozen", true, 5));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 138, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, "Paralyzed", true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 154, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, "Toxic", true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
            }
        }
        else if (pkm->gen4())
        {
            switch (i)
            {
                // Markings
                case 0x16:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 6; j++)
                    {
                        buttons[i].push_back(new HexEditButton(70, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, std::string(marks[j]), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                    }
                    break;
                // Ribbons
                case 0x24:
                case 0x25:
                case 0x26:
                case 0x27:
                case 0x3C:
                case 0x3D:
                case 0x3E:
                case 0x3F:
                case 0x60:
                case 0x61:
                case 0x62:
                case 0x63:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        buttons[i].push_back(new HexEditButton(70, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, std::string(gen4ToggleTexts[currRibbon]), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // Egg and Nicknamed Flags
                case 0x3B:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, "Egg", true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 106, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, "Nicknamed", true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Fateful Encounter
                case 0x40:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, "Fateful Encounter", true, 0));
                    buttons[i].back()->setToggled(pkm->rawData()[i] & 0x1);
                    break;
                // Gold Leaves & Crown
                case 0x41:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 6; j++)
                    {
                        buttons[i].push_back(new HexEditButton(70, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, "", true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                    }
                    break;
                // OT Gender
                case 0x84:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, "", true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Status
                case 0x88:
                    buttons[i].push_back(new HexEditButton(70, 90, 13, 13, [this, i](){ return this->toggleBit(i, 3); }, ui_sheet_emulated_toggle_green_idx, "Poisoned", true, 3));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 106, 13, 13, [this, i](){ return this->toggleBit(i, 4); }, ui_sheet_emulated_toggle_green_idx, "Burned", true, 4));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 122, 13, 13, [this, i](){ return this->toggleBit(i, 5); }, ui_sheet_emulated_toggle_green_idx, "Frozen", true, 5));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 138, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, "Paralyzed", true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(70, 154, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, "Toxic", true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
            }
        }
    }
    hid.update(pkm->length);
}

void HexEditScreen::draw() const
{
    C2D_SceneBegin(g_renderTargetTop);
    Gui::sprite(ui_sheet_part_mtx_15x16_idx, 0, 0);
    
    // Selected box
    C2D_DrawRectSolid((hid.index() % 16) * 25, (hid.index() / 16) * 15, 0.5f, 24, 14, C2D_Color32(15, 22, 89, 0));
    C2D_DrawRectSolid((hid.index() % 16) * 25, (hid.index() / 16) * 15, 0.5f, 1, 14, COLOR_YELLOW);
    C2D_DrawRectSolid((hid.index() % 16) * 25, (hid.index() / 16) * 15, 0.5f, 24, 1, COLOR_YELLOW);
    C2D_DrawRectSolid((hid.index() % 16) * 25, (hid.index() / 16) * 15 + 13, 0.5f, 24, 1, COLOR_YELLOW);
    C2D_DrawRectSolid((hid.index() % 16) * 25 + 23, (hid.index() / 16) * 15, 0.5f, 1, 14, COLOR_YELLOW);
    for (int y = 0; y < 15; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            if (x + y * 16 + hid.page() * hid.maxVisibleEntries() < pkm->length)
            {
                std::pair<std::string, SecurityLevel> description = describe(x + y * 16 + hid.page() * hid.maxVisibleEntries());
                u32 color = COLOR_WHITE;
                if (level < description.second)
                {
                    color = C2D_Color32(0, 0, 0, 120);
                }
                Gui::dynamicText(x * 25, y * 15 + 1, 24, StringUtils::format("%02X", pkm->rawData()[x + y * 16 + hid.page() * hid.maxVisibleEntries()]), FONT_SIZE_9, FONT_SIZE_9, color);
            }
            else
            {
                break;
            }
        }
    }

    Gui::dynamicText(GFX_TOP, 226, selectedDescription.first, FONT_SIZE_9, FONT_SIZE_9, COLOR_BLACK);

    C2D_SceneBegin(g_renderTargetBottom);
    Gui::backgroundBottom(false);
    Gui::staticText(GFX_BOTTOM, 2, "Selected Byte:", FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
    Gui::dynamicText(GFX_BOTTOM, 17, StringUtils::format("0x%02X", hid.fullIndex()), FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);

    Gui::sprite(ui_sheet_box_hex_value_idx, 140, 50);
    Gui::dynamicText(GFX_BOTTOM, 52, StringUtils::format("%01X %01X", pkm->rawData()[hid.fullIndex()] >> 4, pkm->rawData()[hid.fullIndex()] & 0x0F), FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
    if (level >= selectedDescription.second)
    {
        for (auto button : buttons[hid.fullIndex()])
        {
            button->draw();
        }
    }
}

void HexEditScreen::update(touchPosition* touch)
{
    u32 down = hidKeysDown();
    static int superSecretTimer = 300;
    static std::bitset<8> superSecretCornersPressed = {false};
    static bool countDownSecretTimer = false;
    //u32 held = hidKeysHeld();

    if (down & KEY_B)
    {
        checkValues(pkm);
        Gui::screenBack();
        return;
    }

    if (down & KEY_TOUCH && level < UNRESTRICTED)
    {
        if (touch->px < 20 && touch->py < 20)
        {
            if (superSecretCornersPressed[0])
            {
                superSecretCornersPressed[4] = true;
            }
            superSecretCornersPressed[0] = true;
            countDownSecretTimer = true;
            superSecretTimer = 300;
        }
        else if (touch->px > 300 && touch->py < 20)
        {
            if (superSecretCornersPressed[1])
            {
                superSecretCornersPressed[5] = true;
            }
            superSecretCornersPressed[1] = true;
            countDownSecretTimer = true;
            superSecretTimer = 300;
        }
        else if (touch->px < 20 && touch->py > 220)
        {
            if (superSecretCornersPressed[2])
            {
                superSecretCornersPressed[6] = true;
            }
            superSecretCornersPressed[2] = true;
            countDownSecretTimer = true;
            superSecretTimer = 300;
        }
        else if (touch->px > 300 && touch->py > 220)
        {
            if (superSecretCornersPressed[3])
            {
                superSecretCornersPressed[7] = true;
            }
            superSecretCornersPressed[3] = true;
            countDownSecretTimer = true;
            superSecretTimer = 300;
        }
        if (level == NORMAL)
        {
            if (superSecretCornersPressed[0] && superSecretCornersPressed[1]
                && superSecretCornersPressed[2] && superSecretCornersPressed[3])
            {
                level = OPEN;
                superSecretTimer = 0;
            }
        }
        else if (level == OPEN)
        {
            if (superSecretCornersPressed[4] && superSecretCornersPressed[5]
                && superSecretCornersPressed[6] && superSecretCornersPressed[7])
            {
                level = UNRESTRICTED;
                superSecretTimer = 0;
            }
        }
    }

    hid.update(pkm->length);

    selectedDescription = describe(hid.fullIndex());
    if (level >= selectedDescription.second)
    {
        for (size_t i = 0; i < buttons[hid.fullIndex()].size(); i++)
        {
            buttons[hid.fullIndex()][i]->update(touch);
        }
    }
    if (countDownSecretTimer)
    {
        if (superSecretTimer > 0)
        {
            superSecretTimer--;
        }
        else
        {
            countDownSecretTimer = false;
            for (int i = 0; i < 8; i++)
            {
                superSecretCornersPressed[i] = false;
            }
        }
    }
}