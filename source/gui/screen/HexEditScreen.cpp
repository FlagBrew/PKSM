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
    "CIRCLE",
    "TRIANGLE",
    "SQUARE",
    "HEART",
    "STAR",
    "DIAMOND"
};
static constexpr std::string_view gen4ToggleTexts[] = {
    "SINNOH_CHAMP_RIBBON",
    "ABILITY_RIBBON",
    "GREAT_ABILITY_RIBBON",
    "DOUBLE_ABILITY_RIBBON",
    "MULTI_ABILITY_RIBBON",
    "PAIR_ABILITY_RIBBON",
    "WORLD_ABILITY_RIBBON",
    "ALERT_RIBBON",
    "SHOCK_RIBBON",
    "DOWNCAST_RIBBON",
    "CARELESS_RIBBON",
    "RELAX_RIBBON",
    "SNOOZE_RIBBON",
    "SMILE_RIBBON",
    "GORGEOUS_RIBBON",
    "ROYAL_RIBBON",
    "GORGEOUS_ROYAL_RIBBON",
    "FOOTPRINT_RIBBON",
    "RECORD_RIBBON",
    "HISTORY_RIBBON",
    "LEGEND_RIBBON",
    "RED_RIBBON",
    "GREEN_RIBBON",
    "BLUE_RIBBON",
    "FESTIVAL_RIBBON",
    "CARNIVAL_RIBBON",
    "CLASSIC_RIBBON",
    "PREMIER_RIBBON",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "COOL_RIBBON",
    "COOL_RIBBON_SUPER",
    "COOL_RIBBON_HYPER",
    "COOL_RIBBON_MASTER",
    "BEAUTY_RIBBON",
    "BEAUTY_RIBBON_SUPER",
    "BEAUTY_RIBBON_HYPER",
    "BEAUTY_RIBBON_MASTER",
    "CUTE_RIBBON",
    "CUTE_RIBBON_SUPER",
    "CUTE_RIBBON_HYPER",
    "CUTE_RIBBON_MASTER",
    "SMART_RIBBON",
    "SMART_RIBBON_SUPER",
    "SMART_RIBBON_HYPER",
    "SMART_RIBBON_MASTER",
    "TOUGH_RIBBON",
    "TOUGH_RIBBON_SUPER",
    "TOUGH_RIBBON_HYPER",
    "TOUGH_RIBBON_MASTER",
    "CHAMPION_RIBBON",
    "WINNING_RIBBON",
    "VICTORY_RIBBON",
    "ARTIST_RIBBON",
    "EFFORT_RIBBON",
    "BATTLE_CHAMPION_RIBBON",
    "REGIONAL_CHAMPION_RIBBON",
    "NATIONAL_CHAMPION_RIBBON",
    "COUNTRY_RIBBON",
    "NATIONAL_RIBBON",
    "EARTH_RIBBON",
    "WORLD_RIBBON",
    "COOL_RIBBON",
    "COOL_RIBBON_GREAT",
    "COOL_RIBBON_ULTRA",
    "COOL_RIBBON_MASTER",
    "BEAUTY_RIBBON",
    "BEAUTY_RIBBON_GREAT",
    "BEAUTY_RIBBON_ULTRA",
    "BEAUTY_RIBBON_MASTER",
    "CUTE_RIBBON",
    "CUTE_RIBBON_GREAT",
    "CUTE_RIBBON_ULTRA",
    "CUTE_RIBBON_MASTER",
    "SMART_RIBBON",
    "SMART_RIBBON_GREAT",
    "SMART_RIBBON_ULTRA",
    "SMART_RIBBON_MASTER",
    "TOUGH_RIBBON",
    "TOUGH_RIBBON_GREAT",
    "TOUGH_RIBBON_ULTRA",
    "TOUGH_RIBBON_MASTER",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED"
};
static constexpr std::string_view gen5ToggleTexts[] = {
    "SINNOH_CHAMP_RIBBON",
    "ABILITY_RIBBON",
    "GREAT_ABILITY_RIBBON",
    "DOUBLE_ABILITY_RIBBON",
    "MULTI_ABILITY_RIBBON",
    "PAIR_ABILITY_RIBBON",
    "WORLD_ABILITY_RIBBON",
    "ALERT_RIBBON",
    "SHOCK_RIBBON",
    "DOWNCAST_RIBBON",
    "CARELESS_RIBBON",
    "RELAX_RIBBON",
    "SNOOZE_RIBBON",
    "SMILE_RIBBON",
    "GORGEOUS_RIBBON",
    "ROYAL_RIBBON",
    "GORGEOUS_ROYAL_RIBBON",
    "FOOTPRINT_RIBBON",
    "RECORD_RIBBON",
    "EVENT_RIBBON",
    "LEGEND_RIBBON",
    "WORLD_CHAMPION_RIBBON",
    "BIRTHDAY_RIBBON",
    "SPECIAL_RIBBON",
    "SOUVENIR_RIBBON",
    "WISHING_RIBBON",
    "CLASSIC_RIBBON",
    "PREMIER_RIBBON",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "COOL_RIBBON",
    "COOL_RIBBON_SUPER",
    "COOL_RIBBON_HYPER",
    "COOL_RIBBON_MASTER",
    "BEAUTY_RIBBON",
    "BEAUTY_RIBBON_SUPER",
    "BEAUTY_RIBBON_HYPER",
    "BEAUTY_RIBBON_MASTER",
    "CUTE_RIBBON",
    "CUTE_RIBBON_SUPER",
    "CUTE_RIBBON_HYPER",
    "CUTE_RIBBON_MASTER",
    "SMART_RIBBON",
    "SMART_RIBBON_SUPER",
    "SMART_RIBBON_HYPER",
    "SMART_RIBBON_MASTER",
    "TOUGH_RIBBON",
    "TOUGH_RIBBON_SUPER",
    "TOUGH_RIBBON_HYPER",
    "TOUGH_RIBBON_MASTER",
    "CHAMPION_RIBBON",
    "WINNING_RIBBON",
    "VICTORY_RIBBON",
    "ARTIST_RIBBON",
    "EFFORT_RIBBON",
    "BATTLE_CHAMPION_RIBBON",
    "REGIONAL_CHAMPION_RIBBON",
    "NATIONAL_CHAMPION_RIBBON",
    "COUNTRY_RIBBON",
    "NATIONAL_RIBBON",
    "EARTH_RIBBON",
    "WORLD_RIBBON",
    "COOL_RIBBON",
    "COOL_RIBBON_GREAT",
    "COOL_RIBBON_ULTRA",
    "COOL_RIBBON_MASTER",
    "BEAUTY_RIBBON",
    "BEAUTY_RIBBON_GREAT",
    "BEAUTY_RIBBON_ULTRA",
    "BEAUTY_RIBBON_MASTER",
    "CUTE_RIBBON",
    "CUTE_RIBBON_GREAT",
    "CUTE_RIBBON_ULTRA",
    "CUTE_RIBBON_MASTER",
    "SMART_RIBBON",
    "SMART_RIBBON_GREAT",
    "SMART_RIBBON_ULTRA",
    "SMART_RIBBON_MASTER",
    "TOUGH_RIBBON",
    "TOUGH_RIBBON_GREAT",
    "TOUGH_RIBBON_ULTRA",
    "TOUGH_RIBBON_MASTER",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED",
    "UNUSED"
};
static constexpr std::string_view gen67ToggleTexts[] = {
    "UNUSED",
    "UNUSED",
    "SPATK_LEVEL_1",
    "HP_LEVEL_1",
    "ATTACK_LEVEL_1",
    "SPDEF_LEVEL_1",
    "SPEED_LEVEL_1",
    "DEFENSE_LEVEL_1",
    "SPATK_LEVEL_2",
    "HP_LEVEL_2",
    "ATTACK_LEVEL_2",
    "SPDEF_LEVEL_2",
    "SPEED_LEVEL_2",
    "DEFENSE_LEVEL_2",
    "SPATK_LEVEL_3",
    "HP_LEVEL_3",
    "ATTACK_LEVEL_3",
    "SPDEF_LEVEL_3",
    "SPEED_LEVEL_3",
    "DEFENSE_LEVEL_3",
    "THE_TROUBLES_KEEP_ON_COMING?",
    "THE_LEAF_STONE_CUP_BEGINS",
    "THE_FIRE_STONE_CUP_BEGINS",
    "THE_WATER_STONE_CUP_BEGINS",
    "FOLLOW_THOSE_FLEEING_GOALS",
    "WATCH_OUT_THATS_ONE_TRICKY_SECOND_HALF",
    "AN_OPENING_OF_LIGHTING_QUICK_ATTACKS",
    "THOSE_LONG_SHOTS_ARE_NO_LONG_SHOT",
    "SCATTERBUG_LUGS_BACK",
    "A_BARRAGE_OF_BITBOTS",
    "DRAG_DOWN_HYDREIGON",
    "THE_BATTLE_FOR_THE_BEST_VERSION_X/Y",
    "KALOS_CHAMP_RIBBON",
    "CHAMPION_RIBBON",
    "SINNOH_CHAMP_RIBBON",
    "BEST_FRIENDS_RIBBON",
    "TRAINING_RIBBON",
    "SKILLFUL_BATTLER_RIBBON",
    "EXPERT_BATTLER_RIBBON",
    "EFFORT_RIBBON",
    "ALERT_RIBBON",
    "SHOCK_RIBBON",
    "DOWNCAST_RIBBON",
    "CARELESS_RIBBON",
    "RELAX_RIBBON",
    "SNOOZE_RIBBON",
    "SMILE_RIBBON",
    "GORGEOUS_RIBBON",
    "ROYAL_RIBBON",
    "GORGEOUS_ROYAL_RIBBON",
    "ARTIST_RIBBON",
    "FOOTPRINT_RIBBON",
    "RECORD_RIBBON",
    "LEGEND_RIBBON",
    "COUNTRY_RIBBON",
    "NATIONAL_RIBBON",
    "EARTH_RIBBON",
    "WORLD_RIBBON",
    "CLASSIC_RIBBON",
    "PREMIER_RIBBON",
    "EVENT_RIBBON",
    "BIRTHDAY_RIBBON",
    "SPECIAL_RIBBON",
    "SOUVENIR_RIBBON",
    "WISHING_RIBBON",
    "BATTLE_CHAMPION_RIBBON",
    "REGIONAL_CHAMPION_RIBBON",
    "NATIONAL_CHAMPION_RIBBON",
    "WORLD_CHAMPION_RIBBON",
    "UNUSED",
    "UNUSED",
    "HOENN_CHAMPION_RIBBON",
    "CONTEST_STAR_RIBBON",
    "COOLNESS_MASTER_RIBBON",
    "BEAUTY_MASTER_RIBBON",
    "CUTENESS_MASTER_RIBBON",
    "CLEVERNESS_MASTER_RIBBON",
    "TOUGHNESS_MASTER_RIBBON",
    "UNUSED",
    "UNUSED"
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
                formCounter = PersonalDPPtHGSS::formCount;
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
    static const std::pair<std::string, HexEditScreen::SecurityLevel> UNKNOWN = std::make_pair(i18n::localize("UNKNOWN"), UNRESTRICTED);
    static const std::pair<std::string, HexEditScreen::SecurityLevel> UNUSED = std::make_pair(i18n::localize("UNUSED"), UNRESTRICTED);
    if (pkm->generation() == Generation::SIX || pkm->generation() == Generation::SEVEN)
    {
        switch (i)
        {
            case 0x00 ... 0x03:
                return std::make_pair(i18n::localize("ENCRYPTION_KEY"), UNRESTRICTED);
            case 0x04 ... 0x05:
                return std::make_pair(i18n::localize("SANITY_PLACEHOLDER"), UNRESTRICTED);
            case 0x06 ... 0x07:
                return std::make_pair(i18n::localize("CHECKSUM"), UNRESTRICTED);
            case 0x08 ... 0x09:
                return std::make_pair(i18n::localize("SPECIES"), NORMAL);
            case 0x0A ... 0x0B:
                return std::make_pair(i18n::localize("ITEM"), NORMAL);
            case 0x0C ... 0x0D:
                return std::make_pair(i18n::localize("OT_ID"), NORMAL);
            case 0x0E ... 0x0F:
                return std::make_pair(i18n::localize("OT_SID"), NORMAL);
            case 0x10 ... 0x13:
                return std::make_pair(i18n::localize("EXPERIENCE"), OPEN);
            case 0x14:
                return std::make_pair(i18n::localize("ABILITY"), NORMAL);
            case 0x15:
                return std::make_pair(i18n::localize("ABILITY_NUMBER"), OPEN);
            case 0x16 ... 0x17:
                return std::make_pair(i18n::localize("TRAINING_BAG_HITS_LEFT"), NORMAL);
            case 0x18 ... 0x1B:
                return std::make_pair(i18n::localize("PID"), NORMAL);
            case 0x1C:
                return std::make_pair(i18n::localize("NATURE"), OPEN);
            //Gender, fateful encounter, and form bits
            case 0x1D:
                return std::make_pair(i18n::localize("GENDER_FATEFUL_ENCOUNTER_FORM"), OPEN);
            case 0x1E:
                return std::make_pair(i18n::localize("HP_EV"), NORMAL);
            case 0x1F:
                return std::make_pair(i18n::localize("ATTACK_EV"), NORMAL);
            case 0x20:
                return std::make_pair(i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x21:
                return std::make_pair(i18n::localize("SPEED_EV"), NORMAL);
            case 0x22:
                return std::make_pair(i18n::localize("SPATK_EV"), NORMAL);
            case 0x23:
                return std::make_pair(i18n::localize("SPDEF_EV"), NORMAL);
            case 0x24:
                return std::make_pair(i18n::localize("CONTEST_VALUE_COOL"), NORMAL);
            case 0x25:
                return std::make_pair(i18n::localize("CONTEST_VALUE_BEAUTY"), NORMAL);
            case 0x26:
                return std::make_pair(i18n::localize("CONTEST_VALUE_CUTE"), NORMAL);
            case 0x27:
                return std::make_pair(i18n::localize("CONTEST_VALUE_SMART"), NORMAL);
            case 0x28:
                return std::make_pair(i18n::localize("CONTEST_VALUE_TOUGH"), NORMAL);
            case 0x29:
                return std::make_pair(i18n::localize("CONTEST_VALUE_SHEEN"), NORMAL);
            case 0x2A:
                return std::make_pair(i18n::localize("MARKINGS"), NORMAL);
            case 0x2B:
                return std::make_pair(i18n::localize("POKERUS"), NORMAL);
            case 0x2C ... 0x2F:
                return std::make_pair(i18n::localize("SUPER_TRAINING_FLAGS"), NORMAL);
            case 0x30 ... 0x35:
                return std::make_pair(i18n::localize("RIBBONS"), NORMAL);
            case 0x36 ... 0x37:
                return UNUSED;
            case 0x38:
                return std::make_pair(i18n::localize("CONTEST_MEMORY_RIBBON_COUNT"), NORMAL);
            case 0x39:
                return std::make_pair(i18n::localize("BATTLE_MEMORY_RIBBON_COUNT"), NORMAL);
            case 0x3A:
                return std::make_pair(i18n::localize("DISTRIBUTION_SUPER_TRAINING_FLAGS"), NORMAL);
            case 0x3B ... 0x3F:
                return UNUSED;
            case 0x40 ... 0x57:
                return std::make_pair(i18n::localize("NICKNAME"), NORMAL);
            case 0x58 ... 0x59:
                return std::make_pair(i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
            case 0x5A ... 0x5B:
                return std::make_pair(i18n::localize("MOVE_1_ID"), NORMAL);
            case 0x5C ... 0x5D:
                return std::make_pair(i18n::localize("MOVE_2_ID"), NORMAL);
            case 0x5E ... 0x5F:
                return std::make_pair(i18n::localize("MOVE_3_ID"), NORMAL);
            case 0x60 ... 0x61:
                return std::make_pair(i18n::localize("MOVE_4_ID"), NORMAL);
            case 0x62:
                return std::make_pair(i18n::localize("MOVE_1_CURRENT_PP"), OPEN);
            case 0x63:
                return std::make_pair(i18n::localize("MOVE_2_CURRENT_PP"), OPEN);
            case 0x64:
                return std::make_pair(i18n::localize("MOVE_3_CURRENT_PP"), OPEN);
            case 0x65:
                return std::make_pair(i18n::localize("MOVE_4_CURRENT_PP"), OPEN);
            case 0x66:
                return std::make_pair(i18n::localize("MOVE_1_PP_UPS"), NORMAL);
            case 0x67:
                return std::make_pair(i18n::localize("MOVE_2_PP_UPS"), NORMAL);
            case 0x68:
                return std::make_pair(i18n::localize("MOVE_3_PP_UPS"), NORMAL);
            case 0x69:
                return std::make_pair(i18n::localize("MOVE_5_PP_UPS"), NORMAL);
            case 0x6A ... 0x6B:
                return std::make_pair(i18n::localize("RELEARN_MOVE_1_ID"), NORMAL);
            case 0x6C ... 0x6D:
                return std::make_pair(i18n::localize("RELEARN_MOVE_2_ID"), NORMAL);
            case 0x6E ... 0x6F:
                return std::make_pair(i18n::localize("RELEARN_MOVE_3_ID"), NORMAL);
            case 0x70 ... 0x71:
                return std::make_pair(i18n::localize("RELEARN_MOVE_4_ID"), NORMAL);
            case 0x72:
                return std::make_pair(i18n::localize("SECRET_SUPER_TRAINING_FLAG"), NORMAL);
            case 0x73:
                return UNUSED;
            case 0x74 ... 0x76:
                return std::make_pair(i18n::localize("IVS"), NORMAL);
            case 0x77:
                return std::make_pair(i18n::localize("IVS_EGG_AND_NICKNAMED_FLAGS"), NORMAL);
            case 0x78 ... 0x8F:
                return std::make_pair(i18n::localize("CURRENT_TRAINER_NAME"), NORMAL);
            case 0x90 ... 0x91:
                return std::make_pair(i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
            case 0x92:
                return std::make_pair(i18n::localize("CURRENT_TRAINER_GENDER"), NORMAL);
            case 0x93:
                return std::make_pair(i18n::localize("CURRENT_HANDLER"), NORMAL);
            case 0x94 ... 0x95:
                return std::make_pair(i18n::localize("GEOLOCATION_1"), NORMAL);
            case 0x96 ... 0x97:
                return std::make_pair(i18n::localize("GEOLOCATION_2"), NORMAL);
            case 0x98 ... 0x99:
                return std::make_pair(i18n::localize("GEOLOCATION_3"), NORMAL);
            case 0x9A ... 0x9B:
                return std::make_pair(i18n::localize("GEOLOCATION_4"), NORMAL);
            case 0x9C ... 0x9D:
                return std::make_pair(i18n::localize("GEOLOCATION_5"), NORMAL);
            case 0x9E ... 0xA1:
                return UNUSED;
            case 0xA2:
                return std::make_pair(i18n::localize("CURRENT_TRAINER_FRIENDSHIP"), NORMAL);
            case 0xA3:
                return std::make_pair(i18n::localize("CURRENT_TRAINER_AFFECTION"), NORMAL);
            case 0xA4:
                return std::make_pair(i18n::localize("CURRENT_TRAINER_MEMORY_INTENSITY"), OPEN);
            case 0xA5:
                return std::make_pair(i18n::localize("CURRENT_TRAINER_MEMORY_LINE"), OPEN);
            case 0xA6:
                return std::make_pair(i18n::localize("CURRENT_TRAINER_MEMORY_FEELING"), OPEN);
            case 0xA7:
                return UNUSED;
            case 0xA8 ... 0xA9:
                return std::make_pair(i18n::localize("CURRENT_TRAINER_MEMORY_TEXTVAR"), OPEN);
            case 0xAA ... 0xAD:
                return UNUSED;
            case 0xAE:
                return std::make_pair(i18n::localize("FULLNESS"), NORMAL);
            case 0xAF:
                return std::make_pair(i18n::localize("ENJOYMENT"), NORMAL);
            case 0xB0 ... 0xC7:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_NAME"), NORMAL);
            case 0xC8 ... 0xC9:
                return std::make_pair(i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
            case 0xCA:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_FRIENDSHIP"), NORMAL);
            case 0xCB:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_AFFECTION"), NORMAL);
            case 0xCC:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_MEMORY_INTENSITY"), OPEN);
            case 0xCD:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_MEMORY_LINE"), OPEN);
            case 0xCE:
            case 0xCF:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_MEMORY_TEXTVAR"), OPEN);
            case 0xD0:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_MEMORY_FEELING"), OPEN);
            case 0xD1 ... 0xD3:
                return std::make_pair(i18n::localize("EGG_RECEIVED_DATE"), NORMAL);
            case 0xD4 ... 0xD6:
                return std::make_pair(i18n::localize("MET_DATE"), NORMAL);
            case 0xD7:
                return UNKNOWN;
            case 0xD8 ... 0xD9:
                return std::make_pair(i18n::localize("EGG_LOCATION"), NORMAL);
            case 0xDA ... 0xDB:
                return std::make_pair(i18n::localize("MET_LOCATION"), NORMAL);
            case 0xDC:
                return std::make_pair(i18n::localize("POKEBALL"), NORMAL);
            case 0xDD:
                return std::make_pair(i18n::localize("MET_LEVEL_&_ORIGINAL_TRAINER_GENDER"), NORMAL);
            case 0xDE:
                return std::make_pair(i18n::localize("GEN_4_ENCOUNTER_TYPE"), OPEN);
            case 0xDF:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_GAME_ID"), OPEN);
            case 0xE0:
                return std::make_pair(i18n::localize("COUNTRY_ID"), OPEN);
            case 0xE1:
                return std::make_pair(i18n::localize("REGION_ID"), OPEN);
            case 0xE2:
                return std::make_pair(i18n::localize("3DS_REGION_ID"), OPEN);
            case 0xE3:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_LANGUAGE_ID"), NORMAL);
            case 0xE4 ... 0xE7:
                return UNUSED;
            case 0xE8:
                return std::make_pair(i18n::localize("STATUS_CONDITIONS"), NORMAL);
            case 0xE9:
                return std::make_pair(i18n::localize("UNKNOWN_FLAGS"), UNRESTRICTED);
            case 0xEA ... 0xEB:
                return UNKNOWN;
            case 0xEC:
                return std::make_pair(i18n::localize("LEVEL"), NORMAL);
            // Refresh dirt
            case 0xED:
                if (pkm->generation() == Generation::SEVEN)
                {
                    return std::make_pair(i18n::localize("DIRT_TYPE"), OPEN);
                }
            case 0xEE:
                if (pkm->generation() == Generation::SEVEN)
                {
                    return std::make_pair(i18n::localize("DIRT_LOCATION"), OPEN);
                }
            case 0xEF:
                return UNKNOWN;
            case 0xF0 ... 0xF1:
                return std::make_pair(i18n::localize("CURRENT_HP"), OPEN);
            case 0xF2 ...  0xF3:
                return std::make_pair(i18n::localize("MAX_HP"), OPEN);
            case 0xF4 ... 0xF5:
                return std::make_pair(i18n::localize("ATTACK"), OPEN);
            case 0xF6 ... 0xF7:
                return std::make_pair(i18n::localize("DEFENSE"), OPEN);
            case 0xF8 ... 0xF9:
                return std::make_pair(i18n::localize("SPEED"), OPEN);
            case 0xFA ... 0xFB:
                return std::make_pair(i18n::localize("SPATK"), OPEN);
            case 0xFC ... 0xFD:
                return std::make_pair(i18n::localize("SPDEF"), OPEN);
            case 0xFE ... 0x103:
                return UNKNOWN;
        }
    }
    else if (pkm->generation() == Generation::FIVE)
    {
        switch(i)
        {
            case 0x00 ... 0x03:
                return std::make_pair(i18n::localize("PID"), NORMAL);
            case 0x04 ... 0x05:
                return UNUSED;
            case 0x06 ... 0x07:
                return std::make_pair(i18n::localize("CHECKSUM"), UNRESTRICTED);
            case 0x08 ... 0x09:
                return std::make_pair(i18n::localize("SPECIES"), NORMAL);
            case 0x0A ... 0x0B:
                return std::make_pair(i18n::localize("ITEM"), NORMAL);
            case 0x0C ... 0x0D:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_ID"), NORMAL);
            case 0x0E ... 0x0F:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_SID"), NORMAL);
            case 0x10 ... 0x13:
                return std::make_pair(i18n::localize("EXPERIENCE"), NORMAL);
            case 0x14:
                return std::make_pair(i18n::localize("FRIENDSHIP"), NORMAL);
            case 0x15:
                return std::make_pair(i18n::localize("ABILITY"), NORMAL);
            case 0x16:
                return std::make_pair(i18n::localize("MARKINGS"), NORMAL);
            case 0x17:
                return std::make_pair(i18n::localize("ORIGINAL_LANGUAGE"), OPEN);
            case 0x18:
                return std::make_pair(i18n::localize("HP_EV"), NORMAL);
            case 0x19:
                return std::make_pair(i18n::localize("ATTACK_EV"), NORMAL);
            case 0x1A:
                return std::make_pair(i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x1B:
                return std::make_pair(i18n::localize("SPEED_EV"), NORMAL);
            case 0x1C:
                return std::make_pair(i18n::localize("SPATK_EV"), NORMAL);
            case 0x1D:
                return std::make_pair(i18n::localize("SPDEF_EV"), NORMAL);
            case 0x1E:
                return std::make_pair(i18n::localize("COOL_CONTEST_VALUE"), NORMAL);
            case 0x1F:
                return std::make_pair(i18n::localize("BEAUTY_CONTEST_VALUE"), NORMAL);
            case 0x20:
                return std::make_pair(i18n::localize("CUTE_CONTEST_VALUE"), NORMAL);
            case 0x21:
                return std::make_pair(i18n::localize("SMART_CONTEST_VALUE"), NORMAL);
            case 0x22:
                return std::make_pair(i18n::localize("TOUGH_CONTEST_VALUE"), NORMAL);
            case 0x23:
                return std::make_pair(i18n::localize("SHEEN_CONTEST_VALUE"), NORMAL);
            case 0x24 ... 0x27:
                return std::make_pair(i18n::localize("RIBBONS"), NORMAL);
            case 0x28 ... 0x29:
                return std::make_pair(i18n::localize("MOVE_1"), NORMAL);
            case 0x2A ... 0x2B:
                return std::make_pair(i18n::localize("MOVE_2"), NORMAL);
            case 0x2C ... 0x2D:
                return std::make_pair(i18n::localize("MOVE_3"), NORMAL);
            case 0x2E ... 0x2F:
                return std::make_pair(i18n::localize("MOVE_4"), NORMAL);
            case 0x30:
                return std::make_pair(i18n::localize("MOVE_1_PP"), NORMAL);
            case 0x31:
                return std::make_pair(i18n::localize("MOVE_2_PP"), NORMAL);
            case 0x32:
                return std::make_pair(i18n::localize("MOVE_3_PP"), NORMAL);
            case 0x33:
                return std::make_pair(i18n::localize("MOVE_4_PP"), NORMAL);
            case 0x34:
                return std::make_pair(i18n::localize("MOVE_1_PP_UPS"), NORMAL);
            case 0x35:
                return std::make_pair(i18n::localize("MOVE_2_PP_UPS"), NORMAL);
            case 0x36:
                return std::make_pair(i18n::localize("MOVE_3_PP_UPS"), NORMAL);
            case 0x37:
                return std::make_pair(i18n::localize("MOVE_4_PP_UPS"), NORMAL);
            case 0x38 ... 0x3B:
                return std::make_pair(i18n::localize("IVS_EGG_AND_NICKNAMED_FLAGS"), NORMAL);
            case 0x3C ... 0x3F:
                return std::make_pair(i18n::localize("HOENN_RIBBONS"), NORMAL);
            case 0x40:
                return std::make_pair(i18n::localize("GENDER_&_FORMS"), NORMAL);
            case 0x41:
                return std::make_pair(i18n::localize("NATURE"), NORMAL);
            case 0x42:
                return std::make_pair(i18n::localize("MISC_FLAGS"), NORMAL);
            case 0x43 ... 0x47:
                return UNUSED;
            case 0x48 ... 0x5D:
                return std::make_pair(i18n::localize("NICKNAME"), NORMAL);
            case 0x5E:
                return UNKNOWN;
            case 0x5F:
                return std::make_pair(i18n::localize("ORIGIN_GAME"), NORMAL);
            case 0x60 ... 0x63:
                return std::make_pair(i18n::localize("RIBBONS"), NORMAL);
            case 0x64 ... 0x67:
                return UNUSED;
            case 0x68 ... 0x77:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_NAME"), NORMAL);
            case 0x78 ... 0x7A:
                return std::make_pair(i18n::localize("EGG_DATE"), NORMAL);
            case 0x7B ... 0x7D:
                return std::make_pair(i18n::localize("MET_DATE"), NORMAL);
            case 0x7E ... 0x7F:
                return std::make_pair(i18n::localize("EGG_LOCATION"), OPEN);
            case 0x80 ... 0x81:
                return std::make_pair(i18n::localize("MET_LOCATION"), OPEN);
            case 0x82:
                return std::make_pair(i18n::localize("POKERUS"), NORMAL);
            case 0x83:
                return std::make_pair(i18n::localize("POKEBALL"), NORMAL);
            case 0x84:
                return std::make_pair(i18n::localize("MET_LEVEL_&_ORIGINAL_TRAINER_GENDER"), NORMAL);
            case 0x85:
                return std::make_pair(i18n::localize("ENCOUNTER_TYPE"), NORMAL);
            case 0x86 ... 0x87:
                return UNUSED;
            case 0x88:
                return std::make_pair(i18n::localize("STATUS_CONDITIONS"), NORMAL);
            case 0x89:
                return std::make_pair(i18n::localize("UNKNOWN_FLAGS"), UNRESTRICTED);
            case 0x8A ... 0x8B:
                return UNKNOWN;
            case 0x8C:
                return std::make_pair(i18n::localize("LEVEL"), NORMAL);
            case 0x8D:
                return std::make_pair(i18n::localize("CAPSULE_INDEX_(SEALS)"), OPEN);
            case 0x8E ... 0x8F:
                return std::make_pair(i18n::localize("CURRENT_HP"), OPEN);
            case 0x90 ... 0x91:
                return std::make_pair(i18n::localize("MAX_HP"), OPEN);
            case 0x92 ... 0x93:
                return std::make_pair(i18n::localize("ATTACK"), OPEN);
            case 0x94 ... 0x95:
                return std::make_pair(i18n::localize("DEFENSE"), OPEN);
            case 0x96 ... 0x97:
                return std::make_pair(i18n::localize("SPEED"), OPEN);
            case 0x98 ... 0x99:
                return std::make_pair(i18n::localize("SPATK"), OPEN);
            case 0x9A ... 0x9B:
                return std::make_pair(i18n::localize("SPDEF"), OPEN);
            case 0x9C ... 0xD3:
                return std::make_pair(i18n::localize("MAIL_MESSAGE_+_OT_NAME"), OPEN);
            case 0xD4 ... 0xDB:
                return UNKNOWN;
        }
    }
    else if (pkm->generation() == Generation::FOUR)
    {
        switch(i)
        {
            case 0x00 ... 0x03:
                return std::make_pair(i18n::localize("PID"), NORMAL);
            case 0x04 ... 0x05:
                return UNUSED;
            case 0x06 ... 0x07:
                return std::make_pair(i18n::localize("CHECKSUM"), UNRESTRICTED);
            case 0x08 ... 0x09:
                return std::make_pair(i18n::localize("SPECIES"), NORMAL);
            case 0x0A ... 0x0B:
                return std::make_pair(i18n::localize("ITEM"), NORMAL);
            case 0x0C ... 0x0D:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_ID"), NORMAL);
            case 0x0E ... 0x0F:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_SID"), NORMAL);
            case 0x10 ... 0x13:
                return std::make_pair(i18n::localize("EXPERIENCE"), NORMAL);
            case 0x14:
                return std::make_pair(i18n::localize("FRIENDSHIP"), NORMAL);
            case 0x15:
                return std::make_pair(i18n::localize("ABILITY"), NORMAL);
            case 0x16:
                return std::make_pair(i18n::localize("MARKINGS"), NORMAL);
            case 0x17:
                return std::make_pair(i18n::localize("ORIGINAL_LANGUAGE"), OPEN);
            case 0x18:
                return std::make_pair(i18n::localize("HP_EV"), NORMAL);
            case 0x19:
                return std::make_pair(i18n::localize("ATTACK_EV"), NORMAL);
            case 0x1A:
                return std::make_pair(i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x1B:
                return std::make_pair(i18n::localize("SPEED_EV"), NORMAL);
            case 0x1C:
                return std::make_pair(i18n::localize("SPATK_EV"), NORMAL);
            case 0x1D:
                return std::make_pair(i18n::localize("SPDEF_EV"), NORMAL);
            case 0x1E:
                return std::make_pair(i18n::localize("COOL_CONTEST_VALUE"), NORMAL);
            case 0x1F:
                return std::make_pair(i18n::localize("BEAUTY_CONTEST_VALUE"), NORMAL);
            case 0x20:
                return std::make_pair(i18n::localize("CUTE_CONTEST_VALUE"), NORMAL);
            case 0x21:
                return std::make_pair(i18n::localize("SMART_CONTEST_VALUE"), NORMAL);
            case 0x22:
                return std::make_pair(i18n::localize("TOUGH_CONTEST_VALUE"), NORMAL);
            case 0x23:
                return std::make_pair(i18n::localize("SHEEN_CONTEST_VALUE"), NORMAL);
            case 0x24 ... 0x27:
                return std::make_pair(i18n::localize("RIBBONS"), NORMAL);
            case 0x28 ... 0x29:
                return std::make_pair(i18n::localize("MOVE_1"), NORMAL);
            case 0x2A ... 0x2B:
                return std::make_pair(i18n::localize("MOVE_2"), NORMAL);
            case 0x2C ... 0x2D:
                return std::make_pair(i18n::localize("MOVE_3"), NORMAL);
            case 0x2E ... 0x2F:
                return std::make_pair(i18n::localize("MOVE_4"), NORMAL);
            case 0x30:
                return std::make_pair(i18n::localize("MOVE_1_PP"), NORMAL);
            case 0x31:
                return std::make_pair(i18n::localize("MOVE_2_PP"), NORMAL);
            case 0x32:
                return std::make_pair(i18n::localize("MOVE_3_PP"), NORMAL);
            case 0x33:
                return std::make_pair(i18n::localize("MOVE_4_PP"), NORMAL);
            case 0x34:
                return std::make_pair(i18n::localize("MOVE_1_PP_UPS"), NORMAL);
            case 0x35:
                return std::make_pair(i18n::localize("MOVE_2_PP_UPS"), NORMAL);
            case 0x36:
                return std::make_pair(i18n::localize("MOVE_3_PP_UPS"), NORMAL);
            case 0x37:
                return std::make_pair(i18n::localize("MOVE_4_PP_UPS"), NORMAL);
            case 0x38 ... 0x3B:
                return std::make_pair(i18n::localize("IVS_EGG_AND_NICKNAMED_FLAGS"), NORMAL);
            case 0x3C ... 0x3F:
                return std::make_pair(i18n::localize("HOENN_RIBBONS"), NORMAL);
            case 0x40:
                return std::make_pair(i18n::localize("GENDER_&_FORMS"), NORMAL);
            case 0x41:
                return std::make_pair(i18n::localize("SHINY_LEAVES_(HGSS)"), NORMAL);
            case 0x42 ... 0x43:
                return UNUSED;
            case 0x44 ... 0x45:
                return std::make_pair(i18n::localize("EGG_LOCATION_(PLATINUM)"), NORMAL);
            case 0x46 ... 0x47:
                return std::make_pair(i18n::localize("MET_LOCATION_(PLATINUM)"), NORMAL);
            case 0x48 ... 0x5D:
                return std::make_pair(i18n::localize("NICKNAME"), NORMAL);
            case 0x5E:
                return UNUSED;
            case 0x5F:
                return std::make_pair(i18n::localize("ORIGIN_GAME"), NORMAL);
            case 0x60 ... 0x63:
                return std::make_pair(i18n::localize("RIBBONS"), NORMAL);
            case 0x64 ... 0x67:
                return UNUSED;
            case 0x68 ... 0x77:
                return std::make_pair(i18n::localize("ORIGINAL_TRAINER_NAME"), NORMAL);
            case 0x78 ... 0x7A:
                return std::make_pair(i18n::localize("EGG_DATE"), NORMAL);
            case 0x7B ... 0x7D:
                return std::make_pair(i18n::localize("MET_DATE"), NORMAL);
            case 0x7E ... 0x7F:
                return std::make_pair(i18n::localize("EGG_LOCATION_(DIAMOND_&_PEARL)"), OPEN);
            case 0x80 ... 0x81:
                return std::make_pair(i18n::localize("MET_LOCATION_(DIAMOND_&_PEARL"), OPEN);
            case 0x82:
                return std::make_pair(i18n::localize("POKERUS"), NORMAL);
            case 0x83:
                return std::make_pair(i18n::localize("POKEBALL"), NORMAL);
            case 0x84:
                return std::make_pair(i18n::localize("MET_LEVEL_&_ORIGINAL_TRAINER_GENDER"), NORMAL);
            case 0x85:
                return std::make_pair(i18n::localize("ENCOUNTER_TYPE"), NORMAL);
            case 0x86:
                return std::make_pair(i18n::localize("HGSS_POKEBALL"), NORMAL);
            case 0x87:
                return UNUSED;
            case 0x88:
                return std::make_pair(i18n::localize("STATUS_CONDITIONS"), NORMAL);
            case 0x89:
                return std::make_pair(i18n::localize("UNKNOWN_FLAGS"), UNRESTRICTED);
            case 0x8A ... 0x8B:
                return UNKNOWN;
            case 0x8C:
                return std::make_pair(i18n::localize("LEVEL"), NORMAL);
            case 0x8D:
                return std::make_pair(i18n::localize("CAPSULE_INDEX_(SEALS)"), OPEN);
            case 0x8E ... 0x8F:
                return std::make_pair(i18n::localize("CURRENT_HP"), OPEN);
            case 0x90 ... 0x91:
                return std::make_pair(i18n::localize("MAX_HP"), OPEN);
            case 0x92 ... 0x93:
                return std::make_pair(i18n::localize("ATTACK"), OPEN);
            case 0x94 ... 0x95:
                return std::make_pair(i18n::localize("DEFENSE"), OPEN);
            case 0x96 ... 0x97:
                return std::make_pair(i18n::localize("SPEED"), OPEN);
            case 0x98 ... 0x99:
                return std::make_pair(i18n::localize("SPATK"), OPEN);
            case 0x9A ... 0x9B:
                return std::make_pair(i18n::localize("SPDEF"), OPEN);
            case 0x9C ... 0xD3:
                return std::make_pair(i18n::localize("MAIL_MESSAGE_+_OT_NAME"), OPEN);
            case 0xD4 ... 0xEB:
                return std::make_pair(i18n::localize("SEAL_COORDINATES"), OPEN);
        }
    }
    return std::make_pair(i18n::localize("REPORT_THIS_TO_FLAGBREW"), UNRESTRICTED);
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
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("FATEFUL_ENCOUNTER"), true, 0));
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
                        buttons[i].push_back(new HexEditButton(30, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, i18n::localize(std::string(marks[j])), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                    }
                    break;
                // Super Training Flags
                case 0x2C ... 0x2F:
                // Ribbons
                case 0x30 ... 0x35:
                // Distribution Super Training (???)
                case 0x3A:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        buttons[i].push_back(new HexEditButton(30, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, i18n::localize(std::string(gen67ToggleTexts[currRibbon])), true, j));
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
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("SECRET_SUPER_TRAINING"), true, 0));
                    buttons[i].back()->setToggled(pkm->rawData()[i] & 0x1);
                    break;
                // Egg, & Nicknamed Flag
                case 0x77:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("EGG"), true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 106, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("NICKNAMED"), true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // OT Gender
                case 0xDD:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("FEMALE_OT"), true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Status
                case 0xE8:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 3); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("POISONED"), true, 3));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 106, 13, 13, [this, i](){ return this->toggleBit(i, 4); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("BURNED"), true, 4));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 122, 13, 13, [this, i](){ return this->toggleBit(i, 5); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("FROZEN"), true, 5));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 138, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("PARALYZED"), true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 154, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("TOXIC"), true, 7));
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
                        buttons[i].push_back(new HexEditButton(30, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, i18n::localize(std::string(marks[j])), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                    }
                    break;
                // Ribbons
                case 0x24 ... 0x27:
                case 0x3C ... 0x3F:
                case 0x60 ... 0x63:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        buttons[i].push_back(new HexEditButton(30, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, i18n::localize(std::string(gen5ToggleTexts[currRibbon])), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // Egg and Nicknamed Flags
                case 0x3B:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("EGG"), true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 106, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("NICKNAMED"), true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Fateful Encounter
                case 0x40:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("FATEFUL_ENCOUNTER"), true, 0));
                    buttons[i].back()->setToggled(pkm->rawData()[i] & 0x1);
                    break;
                // DreamWorldAbility & N's Pokemon Flags
                case 0x42:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("HIDDEN_ABILITY?"), true, 0));
                    buttons[i].back()->setToggled(pkm->rawData()[i] & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 106, 13, 13, [this, i](){ return this->toggleBit(i, 1); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("NS_POKEMON?"), true, 1));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 1) & 0x1);
                    break;
                // OT Gender
                case 0x84:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("FEMALE_OT"), true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Status
                case 0x88:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 3); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("POISONED"), true, 3));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 106, 13, 13, [this, i](){ return this->toggleBit(i, 4); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("BURNED"), true, 4));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 122, 13, 13, [this, i](){ return this->toggleBit(i, 5); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("FROZEN"), true, 5));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 138, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("PARALYZED"), true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 154, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("TOXIC"), true, 7));
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
                        buttons[i].push_back(new HexEditButton(30, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, i18n::localize(std::string(marks[j])), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                    }
                    break;
                // Ribbons
                case 0x24 ... 0x27:
                case 0x3C ... 0x3F:
                case 0x60 ... 0x63:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        buttons[i].push_back(new HexEditButton(30, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, i18n::localize(std::string(gen4ToggleTexts[currRibbon])), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // Egg and Nicknamed Flags
                case 0x3B:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("EGG"), true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 106, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("NICKNAMED"), true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Fateful Encounter
                case 0x40:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 0); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("FATEFUL_ENCOUNTER"), true, 0));
                    buttons[i].back()->setToggled(pkm->rawData()[i] & 0x1);
                    break;
                // Gold Leaves & Crown
                case 0x41:
                    for (int j = 0; j < 4; j++)
                    {
                        delete buttons[i].back();
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 5; j++)
                    {
                        buttons[i].push_back(new HexEditButton(30, 90 + j * 16, 13, 13, [this, i, j](){ return this->toggleBit(i, j); }, ui_sheet_emulated_toggle_green_idx, (i18n::localize("SHINY_LEAF") + ' ') + (char)('A' + j), true, j));
                        buttons[i].back()->setToggled((pkm->rawData()[i] >> j) & 0x1);
                    }
                    buttons[i].push_back(new HexEditButton(30, 170, 13, 13, [this, i](){ return this->toggleBit(i, 5); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("SHINY_CROWN"), true, 5));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 5) & 0x1);
                    break;
                // OT Gender
                case 0x84:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("FEMALE_OT"), true, 7));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 7) & 0x1);
                    break;
                // Status
                case 0x88:
                    buttons[i].push_back(new HexEditButton(30, 90, 13, 13, [this, i](){ return this->toggleBit(i, 3); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("POISONED"), true, 3));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 106, 13, 13, [this, i](){ return this->toggleBit(i, 4); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("BURNED"), true, 4));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 122, 13, 13, [this, i](){ return this->toggleBit(i, 5); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("FROZEN"), true, 5));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 138, 13, 13, [this, i](){ return this->toggleBit(i, 6); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("PARALYZED"), true, 6));
                    buttons[i].back()->setToggled((pkm->rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(new HexEditButton(30, 154, 13, 13, [this, i](){ return this->toggleBit(i, 7); }, ui_sheet_emulated_toggle_green_idx, i18n::localize("TOXIC"), true, 7));
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
    Gui::staticText(GFX_BOTTOM, 2, i18n::localize("HEX_SELECTED_BYTE"), FONT_SIZE_14, FONT_SIZE_14, COLOR_WHITE);
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