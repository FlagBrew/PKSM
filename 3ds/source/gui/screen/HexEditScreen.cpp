/*
 *   This file is part of PKSM
 *   Copyright (C) 2016-2021 Bernardo Giordano, Admiral Fish, piepie62
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
#include "Configuration.hpp"
#include "endian.hpp"
#include "format.h"
#include "i18n_ext.hpp"
#include "loader.hpp"
#include "pkx/PB7.hpp"
#include "pkx/PK1.hpp"
#include "pkx/PK2.hpp"
#include "pkx/PK3.hpp"
#include "pkx/PK4.hpp"
#include "pkx/PK5.hpp"
#include "pkx/PK6.hpp"
#include "pkx/PK7.hpp"
#include "sav/Sav.hpp"

namespace
{
    constexpr std::string_view hyperVals[] = {
        "HYPER_HP", "HYPER_ATTACK", "HYPER_DEFENSE", "HYPER_SPATK", "HYPER_SPDEF", "HYPER_SPEED"};
    constexpr std::string_view marks[] = {
        "CIRCLE", "TRIANGLE", "SQUARE", "HEART", "STAR", "DIAMOND"};
    constexpr std::string_view gen3ToggleTexts[]       = {"BAD_EGG", "HAS_SPECIES", "EGG",
              "HOENN_CHAMPION_RIBBON", "WINNING_RIBBON", "VICTORY_RIBBON", "ARTIST_RIBBON",
              "EFFORT_RIBBON", "BATTLE_CHAMPION_RIBBON", "REGIONAL_CHAMPION_RIBBON",
              "NATIONAL_CHAMPION_RIBBON", "COUNTRY_RIBBON", "NATIONAL_RIBBON", "EARTH_RIBBON",
              "WORLD_RIBBON", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "FATEFUL_ENCOUNTER"};
    constexpr std::string_view gen4ToggleTexts[]       = {"SINNOH_CHAMP_RIBBON", "ABILITY_RIBBON",
              "GREAT_ABILITY_RIBBON", "DOUBLE_ABILITY_RIBBON", "MULTI_ABILITY_RIBBON",
              "PAIR_ABILITY_RIBBON", "WORLD_ABILITY_RIBBON", "ALERT_RIBBON", "SHOCK_RIBBON",
              "DOWNCAST_RIBBON", "CARELESS_RIBBON", "RELAX_RIBBON", "SNOOZE_RIBBON", "SMILE_RIBBON",
              "GORGEOUS_RIBBON", "ROYAL_RIBBON", "GORGEOUS_ROYAL_RIBBON", "FOOTPRINT_RIBBON",
              "RECORD_RIBBON", "HISTORY_RIBBON", "LEGEND_RIBBON", "RED_RIBBON", "GREEN_RIBBON",
              "BLUE_RIBBON", "FESTIVAL_RIBBON", "CARNIVAL_RIBBON", "CLASSIC_RIBBON", "PREMIER_RIBBON",
              "UNUSED", "UNUSED", "UNUSED", "UNUSED", "COOL_RIBBON", "COOL_RIBBON_SUPER",
              "COOL_RIBBON_HYPER", "COOL_RIBBON_MASTER", "BEAUTY_RIBBON", "BEAUTY_RIBBON_SUPER",
              "BEAUTY_RIBBON_HYPER", "BEAUTY_RIBBON_MASTER", "CUTE_RIBBON", "CUTE_RIBBON_SUPER",
              "CUTE_RIBBON_HYPER", "CUTE_RIBBON_MASTER", "SMART_RIBBON", "SMART_RIBBON_SUPER",
              "SMART_RIBBON_HYPER", "SMART_RIBBON_MASTER", "TOUGH_RIBBON", "TOUGH_RIBBON_SUPER",
              "TOUGH_RIBBON_HYPER", "TOUGH_RIBBON_MASTER", "CHAMPION_RIBBON", "WINNING_RIBBON",
              "VICTORY_RIBBON", "ARTIST_RIBBON", "EFFORT_RIBBON", "BATTLE_CHAMPION_RIBBON",
              "REGIONAL_CHAMPION_RIBBON", "NATIONAL_CHAMPION_RIBBON", "COUNTRY_RIBBON", "NATIONAL_RIBBON",
              "EARTH_RIBBON", "WORLD_RIBBON", "COOL_RIBBON", "COOL_RIBBON_GREAT", "COOL_RIBBON_ULTRA",
              "COOL_RIBBON_MASTER", "BEAUTY_RIBBON", "BEAUTY_RIBBON_GREAT", "BEAUTY_RIBBON_ULTRA",
              "BEAUTY_RIBBON_MASTER", "CUTE_RIBBON", "CUTE_RIBBON_GREAT", "CUTE_RIBBON_ULTRA",
              "CUTE_RIBBON_MASTER", "SMART_RIBBON", "SMART_RIBBON_GREAT", "SMART_RIBBON_ULTRA",
              "SMART_RIBBON_MASTER", "TOUGH_RIBBON", "TOUGH_RIBBON_GREAT", "TOUGH_RIBBON_ULTRA",
              "TOUGH_RIBBON_MASTER", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED",
              "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED"};
    constexpr std::string_view gen5ToggleTexts[]       = {"SINNOH_CHAMP_RIBBON", "ABILITY_RIBBON",
              "GREAT_ABILITY_RIBBON", "DOUBLE_ABILITY_RIBBON", "MULTI_ABILITY_RIBBON",
              "PAIR_ABILITY_RIBBON", "WORLD_ABILITY_RIBBON", "ALERT_RIBBON", "SHOCK_RIBBON",
              "DOWNCAST_RIBBON", "CARELESS_RIBBON", "RELAX_RIBBON", "SNOOZE_RIBBON", "SMILE_RIBBON",
              "GORGEOUS_RIBBON", "ROYAL_RIBBON", "GORGEOUS_ROYAL_RIBBON", "FOOTPRINT_RIBBON",
              "RECORD_RIBBON", "EVENT_RIBBON", "LEGEND_RIBBON", "WORLD_CHAMPION_RIBBON",
              "BIRTHDAY_RIBBON", "SPECIAL_RIBBON", "SOUVENIR_RIBBON", "WISHING_RIBBON", "CLASSIC_RIBBON",
              "PREMIER_RIBBON", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "COOL_RIBBON",
              "COOL_RIBBON_SUPER", "COOL_RIBBON_HYPER", "COOL_RIBBON_MASTER", "BEAUTY_RIBBON",
              "BEAUTY_RIBBON_SUPER", "BEAUTY_RIBBON_HYPER", "BEAUTY_RIBBON_MASTER", "CUTE_RIBBON",
              "CUTE_RIBBON_SUPER", "CUTE_RIBBON_HYPER", "CUTE_RIBBON_MASTER", "SMART_RIBBON",
              "SMART_RIBBON_SUPER", "SMART_RIBBON_HYPER", "SMART_RIBBON_MASTER", "TOUGH_RIBBON",
              "TOUGH_RIBBON_SUPER", "TOUGH_RIBBON_HYPER", "TOUGH_RIBBON_MASTER", "CHAMPION_RIBBON",
              "WINNING_RIBBON", "VICTORY_RIBBON", "ARTIST_RIBBON", "EFFORT_RIBBON",
              "BATTLE_CHAMPION_RIBBON", "REGIONAL_CHAMPION_RIBBON", "NATIONAL_CHAMPION_RIBBON",
              "COUNTRY_RIBBON", "NATIONAL_RIBBON", "EARTH_RIBBON", "WORLD_RIBBON", "COOL_RIBBON",
              "COOL_RIBBON_GREAT", "COOL_RIBBON_ULTRA", "COOL_RIBBON_MASTER", "BEAUTY_RIBBON",
              "BEAUTY_RIBBON_GREAT", "BEAUTY_RIBBON_ULTRA", "BEAUTY_RIBBON_MASTER", "CUTE_RIBBON",
              "CUTE_RIBBON_GREAT", "CUTE_RIBBON_ULTRA", "CUTE_RIBBON_MASTER", "SMART_RIBBON",
              "SMART_RIBBON_GREAT", "SMART_RIBBON_ULTRA", "SMART_RIBBON_MASTER", "TOUGH_RIBBON",
              "TOUGH_RIBBON_GREAT", "TOUGH_RIBBON_ULTRA", "TOUGH_RIBBON_MASTER", "UNUSED", "UNUSED",
              "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED", "UNUSED",
              "UNUSED"};
    constexpr std::string_view gen67ToggleTexts[]      = {"UNUSED", "UNUSED", "SPATK_LEVEL_1",
             "HP_LEVEL_1", "ATTACK_LEVEL_1", "SPDEF_LEVEL_1", "SPEED_LEVEL_1", "DEFENSE_LEVEL_1",
             "SPATK_LEVEL_2", "HP_LEVEL_2", "ATTACK_LEVEL_2", "SPDEF_LEVEL_2", "SPEED_LEVEL_2",
             "DEFENSE_LEVEL_2", "SPATK_LEVEL_3", "HP_LEVEL_3", "ATTACK_LEVEL_3", "SPDEF_LEVEL_3",
             "SPEED_LEVEL_3", "DEFENSE_LEVEL_3", "THE_TROUBLES_KEEP_ON_COMING?",
             "THE_LEAF_STONE_CUP_BEGINS", "THE_FIRE_STONE_CUP_BEGINS", "THE_WATER_STONE_CUP_BEGINS",
             "FOLLOW_THOSE_FLEEING_GOALS", "WATCH_OUT_THATS_ONE_TRICKY_SECOND_HALF",
             "AN_OPENING_OF_LIGHTING_QUICK_ATTACKS", "THOSE_LONG_SHOTS_ARE_NO_LONG_SHOT",
             "SCATTERBUG_LUGS_BACK", "A_BARRAGE_OF_BITBOTS", "DRAG_DOWN_HYDREIGON",
             "THE_BATTLE_FOR_THE_BEST_VERSION_X/Y", "KALOS_CHAMP_RIBBON", "CHAMPION_RIBBON",
             "SINNOH_CHAMP_RIBBON", "BEST_FRIENDS_RIBBON", "TRAINING_RIBBON", "SKILLFUL_BATTLER_RIBBON",
             "EXPERT_BATTLER_RIBBON", "EFFORT_RIBBON", "ALERT_RIBBON", "SHOCK_RIBBON", "DOWNCAST_RIBBON",
             "CARELESS_RIBBON", "RELAX_RIBBON", "SNOOZE_RIBBON", "SMILE_RIBBON", "GORGEOUS_RIBBON",
             "ROYAL_RIBBON", "GORGEOUS_ROYAL_RIBBON", "ARTIST_RIBBON", "FOOTPRINT_RIBBON",
             "RECORD_RIBBON", "LEGEND_RIBBON", "COUNTRY_RIBBON", "NATIONAL_RIBBON", "EARTH_RIBBON",
             "WORLD_RIBBON", "CLASSIC_RIBBON", "PREMIER_RIBBON", "EVENT_RIBBON", "BIRTHDAY_RIBBON",
             "SPECIAL_RIBBON", "SOUVENIR_RIBBON", "WISHING_RIBBON", "BATTLE_CHAMPION_RIBBON",
             "REGIONAL_CHAMPION_RIBBON", "NATIONAL_CHAMPION_RIBBON", "WORLD_CHAMPION_RIBBON", "UNUSED",
             "UNUSED", "HOENN_CHAMPION_RIBBON", "CONTEST_STAR_RIBBON", "COOLNESS_MASTER_RIBBON",
             "BEAUTY_MASTER_RIBBON", "CUTENESS_MASTER_RIBBON", "CLEVERNESS_MASTER_RIBBON",
             "TOUGHNESS_MASTER_RIBBON", "ALOLA_CHAMPION_RIBBON", "BATTLE_ROYAL_RIBBON",
             "BATTLE_TREE_GREAT_RIBBON", "BATTLE_TREE_MASTER_RIBBON", "UNUSED", "UNUSED", "UNUSED",
             "UNUSED", "UNUSED", "UNUSED", "FEARSOME_TWIN_TALES_OF_JUTTING_JAWS",
             "DANGER_ZIPPED_UP_TIGHT", "STUCK_BETWEEN_STRONG_AND_STRONG",
             "DAZZLING_DIZZYING_DANCE_SPOONS", "WHAT_UPSTART_MAGIKARP_MOVING_UP", "WATCH_MULTIPLE_MEGA",
             "UNUSED", "UNUSED"};
    constexpr std::array<pksm::Ribbon, 98> gen8Ribbons = {pksm::Ribbon::ChampionKalos,
        pksm::Ribbon::ChampionG3Hoenn, pksm::Ribbon::ChampionSinnoh, pksm::Ribbon::BestFriends,
        pksm::Ribbon::Training, pksm::Ribbon::BattlerSkillful, pksm::Ribbon::BattlerExpert,
        pksm::Ribbon::Effort, pksm::Ribbon::Alert, pksm::Ribbon::Shock, pksm::Ribbon::Downcast,
        pksm::Ribbon::Careless, pksm::Ribbon::Relax, pksm::Ribbon::Snooze, pksm::Ribbon::Smile,
        pksm::Ribbon::Gorgeous, pksm::Ribbon::Royal, pksm::Ribbon::GorgeousRoyal,
        pksm::Ribbon::Artist, pksm::Ribbon::Footprint, pksm::Ribbon::Record, pksm::Ribbon::Legend,
        pksm::Ribbon::Country, pksm::Ribbon::National, pksm::Ribbon::Earth, pksm::Ribbon::World,
        pksm::Ribbon::Classic, pksm::Ribbon::Premier, pksm::Ribbon::Event, pksm::Ribbon::Birthday,
        pksm::Ribbon::Special, pksm::Ribbon::Souvenir, pksm::Ribbon::Wishing,
        pksm::Ribbon::ChampionBattle, pksm::Ribbon::ChampionRegional,
        pksm::Ribbon::ChampionNational, pksm::Ribbon::ChampionWorld, pksm::Ribbon::MemoryContest,
        pksm::Ribbon::MemoryBattle, pksm::Ribbon::ChampionG6Hoenn, pksm::Ribbon::ContestStar,
        pksm::Ribbon::MasterCoolness, pksm::Ribbon::MasterBeauty, pksm::Ribbon::MasterCuteness,
        pksm::Ribbon::MasterCleverness, pksm::Ribbon::MasterToughness, pksm::Ribbon::ChampionAlola,
        pksm::Ribbon::BattleRoyale, pksm::Ribbon::BattleTreeGreat, pksm::Ribbon::BattleTreeMaster,
        pksm::Ribbon::ChampionGalar, pksm::Ribbon::TowerMaster, pksm::Ribbon::MasterRank,
        pksm::Ribbon::MarkLunchtime, pksm::Ribbon::MarkSleepyTime, pksm::Ribbon::MarkDusk,
        pksm::Ribbon::MarkDawn, pksm::Ribbon::MarkCloudy, pksm::Ribbon::MarkRainy,
        pksm::Ribbon::MarkStormy, pksm::Ribbon::MarkSnowy, pksm::Ribbon::MarkBlizzard,
        pksm::Ribbon::MarkDry, pksm::Ribbon::MarkSandstorm, pksm::Ribbon::MarkMisty,
        pksm::Ribbon::MarkDestiny, pksm::Ribbon::MarkFishing, pksm::Ribbon::MarkCurry,
        pksm::Ribbon::MarkUncommon, pksm::Ribbon::MarkRare, pksm::Ribbon::MarkRowdy,
        pksm::Ribbon::MarkAbsentMinded, pksm::Ribbon::MarkJittery, pksm::Ribbon::MarkExcited,
        pksm::Ribbon::MarkCharismatic, pksm::Ribbon::MarkCalmness, pksm::Ribbon::MarkIntense,
        pksm::Ribbon::MarkZonedOut, pksm::Ribbon::MarkJoyful, pksm::Ribbon::MarkAngry,
        pksm::Ribbon::MarkSmiley, pksm::Ribbon::MarkTeary, pksm::Ribbon::MarkUpbeat,
        pksm::Ribbon::MarkPeeved, pksm::Ribbon::MarkIntellectual, pksm::Ribbon::MarkFerocious,
        pksm::Ribbon::MarkCrafty, pksm::Ribbon::MarkScowling, pksm::Ribbon::MarkKindly,
        pksm::Ribbon::MarkFlustered, pksm::Ribbon::MarkPumpedUp, pksm::Ribbon::MarkZeroEnergy,
        pksm::Ribbon::MarkPrideful, pksm::Ribbon::MarkUnsure, pksm::Ribbon::MarkHumble,
        pksm::Ribbon::MarkThorny, pksm::Ribbon::MarkVigor, pksm::Ribbon::MarkSlump};
}

bool HexEditScreen::toggleBit(int selected, int offset)
{
    pkm.rawData()[selected] ^= 0x1 << offset;
    return true;
}

bool HexEditScreen::checkValue()
{
    if (level != NORMAL)
    {
        return true;
    }
    if (pkm.generation() == pksm::Generation::SIX || pkm.generation() == pksm::Generation::SEVEN ||
        pkm.generation() == pksm::Generation::LGPE)
    {
        int i = hid.fullIndex();
        switch (i)
        {
            case 0x8 ... 0x9:
                if (TitleLoader::save->availableSpecies().count(pkm.species()) == 0)
                {
                    return false;
                }
                return true;
            case 0xA ... 0xB:
                if (TitleLoader::save->availableItems().count(pkm.heldItem()) == 0)
                {
                    return false;
                }
                return true;
            case 0x14:
                if (TitleLoader::save->availableAbilities().count(pkm.ability()) == 0)
                {
                    return false;
                }
                return true;
            case 0x5A ... 0x61:
                if (TitleLoader::save->availableMoves().count(pkm.move((i - 0x5A) / 2)) == 0)
                {
                    return false;
                }
                return true;
            case 0x66 ... 0x69:
                if (pkm.PPUp(i - 0x66) > 3)
                {
                    return false;
                }
                return true;
            case 0x6A ... 0x71:
                if (TitleLoader::save->availableMoves().count(pkm.relearnMove((i - 0x6A) / 2)) == 0)
                {
                    return false;
                }
                return true;
            case 0xD2:
                if (pkm.eggDate().month() > 12 || pkm.eggDate().month() == 0)
                {
                    return false;
                }
                return true;
            case 0xD5:
                if (pkm.metDate().month() > 12 || pkm.metDate().month() == 0)
                {
                    return false;
                }
                return true;
            case 0xD3:
                if (pkm.eggDate().day() > 31 || pkm.eggDate().day() == 0)
                {
                    return false;
                }
                return true;
            case 0xD6:
                if (pkm.metDate().day() > 31 || pkm.metDate().day() == 0)
                {
                    return false;
                }
                return true;
            default:
                return true;
        }
    }
    else if (pkm.generation() == pksm::Generation::FOUR ||
             pkm.generation() == pksm::Generation::FIVE)
    {
        int i = hid.fullIndex();
        switch (i)
        {
            case 0x8 ... 0x9:
                if (TitleLoader::save->availableSpecies().count(pkm.species()) == 0)
                {
                    return false;
                }
                return true;
            case 0xA ... 0xB:
                if (TitleLoader::save->availableItems().count(pkm.heldItem()) == 0)
                {
                    return false;
                }
                return true;
            case 0x15:
                if (TitleLoader::save->availableAbilities().count(pkm.ability()) == 0)
                {
                    return false;
                }
                return true;
            case 0x28 ... 0x2F:
                if (TitleLoader::save->availableMoves().count(pkm.move((i - 0x28) / 2)) == 0)
                {
                    return false;
                }
                return true;
            case 0x34 ... 0x37:
                if (pkm.PPUp(i - 0x34) > 3)
                {
                    return false;
                }
                return true;
            case 0x79:
                if (pkm.eggDate().month() > 12 || pkm.eggDate().month() == 0)
                {
                    return false;
                }
                return true;
            case 0x7C:
                if (pkm.metDate().month() > 12 || pkm.metDate().month() == 0)
                {
                    return false;
                }
                return true;
            case 0x7A:
                if (pkm.eggDate().day() > 31 || pkm.eggDate().day() == 0)
                {
                    return false;
                }
                return true;
            case 0x7D:
                if (pkm.metDate().day() > 31 || pkm.metDate().day() == 0)
                {
                    return false;
                }
                return true;
            default:
                return true;
        }
    }
    else if (pkm.generation() == pksm::Generation::THREE)
    {
        int i = hid.fullIndex();
        switch (i)
        {
            case 0x20 ... 0x21:
                if (TitleLoader::save->availableSpecies().count(pkm.species()) == 0)
                {
                    return false;
                }
                return true;
            case 0x22 ... 0x23:
                if (TitleLoader::save->availableItems().count(pkm.heldItem()) == 0)
                {
                    return false;
                }
                return true;
            case 0x2C ... 0x33:
                if (TitleLoader::save->availableMoves().count(pkm.move((i - 0x2C) / 2)) == 0)
                {
                    return false;
                }
                return true;
            case 0x28:
                // Technically cannot happen. Going to leave it here anyways.
                for (int j = 0; j < 4; j++)
                {
                    if (pkm.PPUp(j) > 3)
                    {
                        return false;
                    }
                }
                return true;
            default:
                return true;
        }
    }
    else if (pkm.generation() == pksm::Generation::EIGHT)
    {
        int i = hid.fullIndex();
        switch (i)
        {
            case 0x8 ... 0x9:
                if (TitleLoader::save->availableSpecies().count(pkm.species()) == 0)
                {
                    return false;
                }
                return true;
            case 0xA ... 0xB:
                if (TitleLoader::save->availableItems().count(pkm.heldItem()) == 0)
                {
                    return false;
                }
                return true;
            case 0x14 ... 0x15:
                if (TitleLoader::save->availableAbilities().count(pkm.ability()) == 0)
                {
                    return false;
                }
                return true;
            case 0x72 ... 0x79:
                if (TitleLoader::save->availableMoves().count(pkm.move((i - 0x72) / 2)) == 0)
                {
                    return false;
                }
                return true;
            case 0x7A ... 0x7D:
                if (pkm.PPUp(i - 0x7A) > 3)
                {
                    return false;
                }
                return true;
            case 0x82 ... 0x89:
                if (TitleLoader::save->availableMoves().count(pkm.relearnMove((i - 0x82) / 2)) == 0)
                {
                    return false;
                }
                return true;
            case 0x11A:
                if (pkm.eggDate().month() > 12 || pkm.eggDate().month() == 0)
                {
                    return false;
                }
                return true;
            case 0x11B:
                if (pkm.metDate().month() > 12 || pkm.metDate().month() == 0)
                {
                    return false;
                }
                return true;
            case 0x11D:
                if (pkm.eggDate().day() > 31 || pkm.eggDate().day() == 0)
                {
                    return false;
                }
                return true;
            case 0x11E:
                if (pkm.metDate().day() > 31 || pkm.metDate().day() == 0)
                {
                    return false;
                }
                return true;
            default:
                return true;
        }
    }
    else if (pkm.generation() == pksm::Generation::ONE)
    {
        int i = hid.fullIndex();
        switch (i)
        {
            case 0x3:
                if (TitleLoader::save->availableSpecies().count(pkm.species()) == 0)
                {
                    return false;
                }
                return true;
            case 0xB ... 0xD:
                if (TitleLoader::save->availableMoves().count(pkm.move(i - 0xB)) == 0)
                {
                    return false;
                }
                return true;
            default:
                return true;
        }
    }
    else if (pkm.generation() == pksm::Generation::TWO)
    {
        int i = hid.fullIndex();
        switch (i)
        {
            case 0x3:
                if (TitleLoader::save->availableSpecies().count(pkm.species()) == 0)
                {
                    return false;
                }
                return true;
            case 0x4:
                if (TitleLoader::save->availableItems().count(pkm.heldItem()) == 0)
                {
                    return false;
                }
                return true;
            case 0x5 ... 0x8:
                if (TitleLoader::save->availableMoves().count(pkm.move(i - 0x8)) == 0)
                {
                    return false;
                }
                return true;
            default:
                return true;
        }
    }
    return true;
}

bool HexEditScreen::editNumber(bool high, bool up)
{
    u8* chosen  = pkm.rawData().data() + hid.fullIndex();
    u8 oldValue = *chosen;
    if (high)
    {
        u8 original = *chosen >> 4;
        *chosen     &= 0x0F;
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
        if (up)
        {
            (*chosen)++;
        }
        else
        {
            (*chosen)--;
        }
    }
    if (!checkValue())
    {
        *chosen = oldValue;
        return true;
    }
    if (level != UNRESTRICTED)
    {
        if (pkm.generation() == pksm::Generation::ONE)
        {
            if (hid.fullIndex() == 0x3)
            {
                pkm.species(pkm.species()); // sets header species byte and changes types
            }
            else if (hid.fullIndex() == 0x24)
            {
                pkm.rawData()[0x6] = *chosen;
            }
            else if (pkm.isParty() && hid.fullIndex() == 0x6)
            {
                pkm.rawData()[0x24] = *chosen;
            }
            else if (hid.fullIndex() == 0x3A)
            {
                if (*chosen == 0x50)
                {
                    *chosen = oldValue; // Prevent accidental INT -> JP when inserting into a bank
                }
            }
        }
        else if (pkm.generation() == pksm::Generation::TWO)
        {
            if (hid.fullIndex() == 0x3)
            {
                if (!pkm.egg())
                {
                    pkm.rawData()[0x1] = *chosen;
                }
            }
            else if (hid.fullIndex() == 0x3E)
            {
                if (*chosen == 0x50)
                {
                    *chosen = oldValue; // Prevent accidental INT -> JP when inserting into a bank
                }
            }
        }
    }
    return true;
}

std::pair<const std::string*, HexEditScreen::SecurityLevel> HexEditScreen::describe(int i) const
{
    static const std::pair<const std::string*, HexEditScreen::SecurityLevel> UNKNOWN =
        std::make_pair(&i18n::localize("UNKNOWN"), UNRESTRICTED);
    static const std::pair<const std::string*, HexEditScreen::SecurityLevel> UNUSED =
        std::make_pair(&i18n::localize("UNUSED"), UNRESTRICTED);
    if (pkm.generation() == pksm::Generation::SIX || pkm.generation() == pksm::Generation::SEVEN ||
        pkm.generation() == pksm::Generation::LGPE)
    {
        switch (i)
        {
            case 0x00 ... 0x03:
                return std::make_pair(&i18n::localize("ENCRYPTION_KEY"), UNRESTRICTED);
            case 0x04 ... 0x05:
                return std::make_pair(&i18n::localize("SANITY_PLACEHOLDER"), UNRESTRICTED);
            case 0x06 ... 0x07:
                return std::make_pair(&i18n::localize("CHECKSUM"), UNRESTRICTED);
            case 0x08 ... 0x09:
                return std::make_pair(&i18n::localize("SPECIES"), NORMAL);
            case 0x0A ... 0x0B:
                return std::make_pair(&i18n::localize("ITEM"), NORMAL);
            case 0x0C ... 0x0D:
                return std::make_pair(&i18n::localize("OT_ID"), NORMAL);
            case 0x0E ... 0x0F:
                return std::make_pair(&i18n::localize("OT_SID"), NORMAL);
            case 0x10 ... 0x13:
                return std::make_pair(&i18n::localize("EXPERIENCE"), NORMAL);
            case 0x14:
                return std::make_pair(&i18n::localize("ABILITY"), NORMAL);
            case 0x15:
                return std::make_pair(&i18n::localize("ABILITY_NUMBER"), OPEN);
            case 0x16 ... 0x17:
                if (pkm.generation() == pksm::Generation::SIX)
                {
                    return std::make_pair(&i18n::localize("TRAINING_BAG_HITS_LEFT"), NORMAL);
                }
                else
                {
                    return std::make_pair(&i18n::localize("MARKINGS"), NORMAL);
                }
            case 0x18 ... 0x1B:
                return std::make_pair(&i18n::localize("PID"), NORMAL);
            case 0x1C:
                return std::make_pair(&i18n::localize("NATURE"), NORMAL);
            // Gender, fateful encounter, and form bits
            case 0x1D:
                return std::make_pair(&i18n::localize("GENDER_FATEFUL_ENCOUNTER_FORM"), NORMAL);
            case 0x1E:
                return std::make_pair(&i18n::localize("HP_EV"), NORMAL);
            case 0x1F:
                return std::make_pair(&i18n::localize("ATTACK_EV"), NORMAL);
            case 0x20:
                return std::make_pair(&i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x21:
                return std::make_pair(&i18n::localize("SPEED_EV"), NORMAL);
            case 0x22:
                return std::make_pair(&i18n::localize("SPATK_EV"), NORMAL);
            case 0x23:
                return std::make_pair(&i18n::localize("SPDEF_EV"), NORMAL);
            case 0x24:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("AWAKENED_HP"), NORMAL);
                }
                else
                {
                    return std::make_pair(&i18n::localize("CONTEST_VALUE_COOL"), NORMAL);
                }
            case 0x25:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("AWAKENED_ATTACK"), NORMAL);
                }
                else
                {
                    return std::make_pair(&i18n::localize("CONTEST_VALUE_BEAUTY"), NORMAL);
                }
            case 0x26:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("AWAKENED_DEFENSE"), NORMAL);
                }
                else
                {
                    return std::make_pair(&i18n::localize("CONTEST_VALUE_CUTE"), NORMAL);
                }
            case 0x27:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("AWAKENED_SPEED"), NORMAL);
                }
                else
                {
                    return std::make_pair(&i18n::localize("CONTEST_VALUE_SMART"), NORMAL);
                }
            case 0x28:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("AWAKENED_SPATK"), NORMAL);
                }
                else
                {
                    return std::make_pair(&i18n::localize("CONTEST_VALUE_TOUGH"), NORMAL);
                }
            case 0x29:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("AWAKENED_SPDEF"), NORMAL);
                }
                else
                {
                    return std::make_pair(&i18n::localize("CONTEST_VALUE_SHEEN"), NORMAL);
                }
            case 0x2A:
                if (pkm.generation() == pksm::Generation::SIX)
                {
                    return std::make_pair(&i18n::localize("MARKINGS"), NORMAL);
                }
                return UNUSED;
            case 0x2B:
                return std::make_pair(&i18n::localize("POKERUS"), NORMAL);
            case 0x2C ... 0x2F:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("HEIGHT_ABSOLUTE"), OPEN);
                }
                else
                {
                    return std::make_pair(&i18n::localize("SUPER_TRAINING_FLAGS"), NORMAL);
                }
            case 0x30 ... 0x36:
                return std::make_pair(&i18n::localize("RIBBONS"), NORMAL);
            case 0x37:
                return UNUSED;
            case 0x38:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return UNUSED;
                }
                else
                {
                    return std::make_pair(&i18n::localize("CONTEST_MEMORY_RIBBON_COUNT"), NORMAL);
                }
            case 0x39:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return UNUSED;
                }
                else
                {
                    return std::make_pair(&i18n::localize("BATTLE_MEMORY_RIBBON_COUNT"), NORMAL);
                }
            case 0x3A:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("HEIGHT"), NORMAL);
                }
                else
                {
                    return std::make_pair(
                        &i18n::localize("DISTRIBUTION_SUPER_TRAINING_FLAGS"), NORMAL);
                }
            case 0x3B:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("WEIGHT"), NORMAL);
                }
                return UNUSED;
            case 0x3C ... 0x3F:
                return UNUSED;
            case 0x40 ... 0x57:
                return std::make_pair(&i18n::localize("NICKNAME"), NORMAL);
            case 0x58 ... 0x59:
                return std::make_pair(&i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
            case 0x5A ... 0x5B:
                return std::make_pair(&i18n::localize("MOVE_1_ID"), NORMAL);
            case 0x5C ... 0x5D:
                return std::make_pair(&i18n::localize("MOVE_2_ID"), NORMAL);
            case 0x5E ... 0x5F:
                return std::make_pair(&i18n::localize("MOVE_3_ID"), NORMAL);
            case 0x60 ... 0x61:
                return std::make_pair(&i18n::localize("MOVE_4_ID"), NORMAL);
            case 0x62:
                return std::make_pair(&i18n::localize("MOVE_1_CURRENT_PP"), NORMAL);
            case 0x63:
                return std::make_pair(&i18n::localize("MOVE_2_CURRENT_PP"), NORMAL);
            case 0x64:
                return std::make_pair(&i18n::localize("MOVE_3_CURRENT_PP"), NORMAL);
            case 0x65:
                return std::make_pair(&i18n::localize("MOVE_4_CURRENT_PP"), NORMAL);
            case 0x66:
                return std::make_pair(&i18n::localize("MOVE_1_PP_UPS"), NORMAL);
            case 0x67:
                return std::make_pair(&i18n::localize("MOVE_2_PP_UPS"), NORMAL);
            case 0x68:
                return std::make_pair(&i18n::localize("MOVE_3_PP_UPS"), NORMAL);
            case 0x69:
                return std::make_pair(&i18n::localize("MOVE_4_PP_UPS"), NORMAL);
            case 0x6A ... 0x6B:
                return std::make_pair(&i18n::localize("RELEARN_MOVE_1_ID"), NORMAL);
            case 0x6C ... 0x6D:
                return std::make_pair(&i18n::localize("RELEARN_MOVE_2_ID"), NORMAL);
            case 0x6E ... 0x6F:
                return std::make_pair(&i18n::localize("RELEARN_MOVE_3_ID"), NORMAL);
            case 0x70 ... 0x71:
                return std::make_pair(&i18n::localize("RELEARN_MOVE_4_ID"), NORMAL);
            case 0x72:
                if (pkm.generation() != pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("SECRET_SUPER_TRAINING_FLAG"), NORMAL);
                }
                return UNUSED;
            case 0x73:
                return UNUSED;
            case 0x74 ... 0x76:
                return std::make_pair(&i18n::localize("IVS"), NORMAL);
            case 0x77:
                return std::make_pair(&i18n::localize("IVS_EGG_AND_NICKNAMED_FLAGS"), NORMAL);
            case 0x78 ... 0x8F:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_NAME"), NORMAL);
            case 0x90 ... 0x91:
                return std::make_pair(&i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
            case 0x92:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_GENDER"), NORMAL);
            case 0x93:
                return std::make_pair(&i18n::localize("CURRENT_HANDLER"), NORMAL);
            case 0x94 ... 0x95:
                return std::make_pair(&i18n::localize("GEOLOCATION_1"), NORMAL);
            case 0x96 ... 0x97:
                return std::make_pair(&i18n::localize("GEOLOCATION_2"), NORMAL);
            case 0x98 ... 0x99:
                return std::make_pair(&i18n::localize("GEOLOCATION_3"), NORMAL);
            case 0x9A ... 0x9B:
                return std::make_pair(&i18n::localize("GEOLOCATION_4"), NORMAL);
            case 0x9C ... 0x9D:
                return std::make_pair(&i18n::localize("GEOLOCATION_5"), NORMAL);
            case 0x9E ... 0xA1:
                return UNUSED;
            case 0xA2:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_FRIENDSHIP"), NORMAL);
            case 0xA3:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_AFFECTION"), NORMAL);
            case 0xA4:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_MEMORY_INTENSITY"), OPEN);
            case 0xA5:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_MEMORY_LINE"), OPEN);
            case 0xA6:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_MEMORY_FEELING"), OPEN);
            case 0xA7:
                return UNUSED;
            case 0xA8 ... 0xA9:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_MEMORY_TEXTVAR"), OPEN);
            case 0xAA ... 0xAD:
                return UNUSED;
            case 0xAE:
                return std::make_pair(&i18n::localize("FULLNESS"), NORMAL);
            case 0xAF:
                return std::make_pair(&i18n::localize("ENJOYMENT"), NORMAL);
            case 0xB0 ... 0xC7:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_NAME"), NORMAL);
            case 0xC8 ... 0xC9:
                return std::make_pair(&i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
            case 0xCA:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_FRIENDSHIP"), NORMAL);
            case 0xCB:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_AFFECTION"), NORMAL);
            case 0xCC:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_MEMORY_INTENSITY"), OPEN);
            case 0xCD:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_MEMORY_LINE"), OPEN);
            case 0xCE:
            case 0xCF:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_MEMORY_TEXTVAR"), OPEN);
            case 0xD0:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_MEMORY_FEELING"), OPEN);
            case 0xD1 ... 0xD3:
                return std::make_pair(&i18n::localize("EGG_RECEIVED_DATE"), NORMAL);
            case 0xD4 ... 0xD6:
                return std::make_pair(&i18n::localize("MET_DATE"), NORMAL);
            case 0xD7:
                return UNKNOWN;
            case 0xD8 ... 0xD9:
                return std::make_pair(&i18n::localize("EGG_LOCATION"), NORMAL);
            case 0xDA ... 0xDB:
                return std::make_pair(&i18n::localize("MET_LOCATION"), NORMAL);
            case 0xDC:
                return std::make_pair(&i18n::localize("POKEBALL"), NORMAL);
            case 0xDD:
                return std::make_pair(
                    &i18n::localize("MET_LEVEL_&_ORIGINAL_TRAINER_GENDER"), NORMAL);
            case 0xDE:
                if (pkm.generation() == pksm::Generation::SIX)
                {
                    return std::make_pair(&i18n::localize("GEN_4_ENCOUNTER_TYPE"), NORMAL);
                }
                else
                {
                    return std::make_pair(&i18n::localize("HYPER_TRAIN_FLAGS"), NORMAL);
                }
            case 0xDF:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_GAME_ID"), NORMAL);
            case 0xE0:
                return std::make_pair(&i18n::localize("COUNTRY_ID"), NORMAL);
            case 0xE1:
                return std::make_pair(&i18n::localize("REGION_ID"), NORMAL);
            case 0xE2:
                return std::make_pair(&i18n::localize("3DS_REGION_ID"), NORMAL);
            case 0xE3:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_LANGUAGE_ID"), NORMAL);
            case 0xE4 ... 0xE7:
                return UNUSED;
            case 0xE8 ... 0xEB:
                return std::make_pair(&i18n::localize("STATUS_CONDITIONS"), NORMAL);
            case 0xEC:
                return std::make_pair(&i18n::localize("LEVEL"), NORMAL); // TODO CHECK LGPE
            // Refresh dirt
            case 0xED:
                if (pkm.generation() != pksm::Generation::SIX)
                {
                    return std::make_pair(&i18n::localize("DIRT_TYPE"), OPEN); // TODO CHECK LGPE
                }
                return UNKNOWN;
            case 0xEE:
                if (pkm.generation() != pksm::Generation::SIX)
                {
                    return std::make_pair(
                        &i18n::localize("DIRT_LOCATION"), OPEN); // TODO CHECK LGPE
                }
                return UNKNOWN;
            case 0xEF:
                return UNKNOWN;
            case 0xF0 ... 0xF1:
                return std::make_pair(&i18n::localize("CURRENT_HP"), OPEN);
            case 0xF2 ... 0xF3:
                return std::make_pair(&i18n::localize("MAX_HP"), OPEN);
            case 0xF4 ... 0xF5:
                return std::make_pair(&i18n::localize("ATTACK"), OPEN);
            case 0xF6 ... 0xF7:
                return std::make_pair(&i18n::localize("DEFENSE"), OPEN);
            case 0xF8 ... 0xF9:
                return std::make_pair(&i18n::localize("SPEED"), OPEN);
            case 0xFA ... 0xFB:
                return std::make_pair(&i18n::localize("SPATK"), OPEN);
            case 0xFC ... 0xFD:
                return std::make_pair(&i18n::localize("SPDEF"), OPEN);
            case 0xFE ... 0xFF:
                if (pkm.generation() == pksm::Generation::LGPE)
                {
                    return std::make_pair(&i18n::localize("CP"), OPEN);
                }
                return UNKNOWN;
            case 0x100 ... 0x103:
                return UNKNOWN;
        }
    }
    else if (pkm.generation() == pksm::Generation::FIVE)
    {
        switch (i)
        {
            case 0x00 ... 0x03:
                return std::make_pair(&i18n::localize("PID"), NORMAL);
            case 0x04 ... 0x05:
                return UNUSED;
            case 0x06 ... 0x07:
                return std::make_pair(&i18n::localize("CHECKSUM"), UNRESTRICTED);
            case 0x08 ... 0x09:
                return std::make_pair(&i18n::localize("SPECIES"), NORMAL);
            case 0x0A ... 0x0B:
                return std::make_pair(&i18n::localize("ITEM"), NORMAL);
            case 0x0C ... 0x0D:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_ID"), NORMAL);
            case 0x0E ... 0x0F:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_SID"), NORMAL);
            case 0x10 ... 0x13:
                return std::make_pair(&i18n::localize("EXPERIENCE"), NORMAL);
            case 0x14:
                return std::make_pair(&i18n::localize("FRIENDSHIP"), NORMAL);
            case 0x15:
                return std::make_pair(&i18n::localize("ABILITY"), NORMAL);
            case 0x16:
                return std::make_pair(&i18n::localize("MARKINGS"), NORMAL);
            case 0x17:
                return std::make_pair(&i18n::localize("ORIGINAL_LANGUAGE"), NORMAL);
            case 0x18:
                return std::make_pair(&i18n::localize("HP_EV"), NORMAL);
            case 0x19:
                return std::make_pair(&i18n::localize("ATTACK_EV"), NORMAL);
            case 0x1A:
                return std::make_pair(&i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x1B:
                return std::make_pair(&i18n::localize("SPEED_EV"), NORMAL);
            case 0x1C:
                return std::make_pair(&i18n::localize("SPATK_EV"), NORMAL);
            case 0x1D:
                return std::make_pair(&i18n::localize("SPDEF_EV"), NORMAL);
            case 0x1E:
                return std::make_pair(&i18n::localize("COOL_CONTEST_VALUE"), NORMAL);
            case 0x1F:
                return std::make_pair(&i18n::localize("BEAUTY_CONTEST_VALUE"), NORMAL);
            case 0x20:
                return std::make_pair(&i18n::localize("CUTE_CONTEST_VALUE"), NORMAL);
            case 0x21:
                return std::make_pair(&i18n::localize("SMART_CONTEST_VALUE"), NORMAL);
            case 0x22:
                return std::make_pair(&i18n::localize("TOUGH_CONTEST_VALUE"), NORMAL);
            case 0x23:
                return std::make_pair(&i18n::localize("SHEEN_CONTEST_VALUE"), NORMAL);
            case 0x24 ... 0x27:
                return std::make_pair(&i18n::localize("RIBBONS"), NORMAL);
            case 0x28 ... 0x29:
                return std::make_pair(&i18n::localize("MOVE_1"), NORMAL);
            case 0x2A ... 0x2B:
                return std::make_pair(&i18n::localize("MOVE_2"), NORMAL);
            case 0x2C ... 0x2D:
                return std::make_pair(&i18n::localize("MOVE_3"), NORMAL);
            case 0x2E ... 0x2F:
                return std::make_pair(&i18n::localize("MOVE_4"), NORMAL);
            case 0x30:
                return std::make_pair(&i18n::localize("MOVE_1_PP"), NORMAL);
            case 0x31:
                return std::make_pair(&i18n::localize("MOVE_2_PP"), NORMAL);
            case 0x32:
                return std::make_pair(&i18n::localize("MOVE_3_PP"), NORMAL);
            case 0x33:
                return std::make_pair(&i18n::localize("MOVE_4_PP"), NORMAL);
            case 0x34:
                return std::make_pair(&i18n::localize("MOVE_1_PP_UPS"), NORMAL);
            case 0x35:
                return std::make_pair(&i18n::localize("MOVE_2_PP_UPS"), NORMAL);
            case 0x36:
                return std::make_pair(&i18n::localize("MOVE_3_PP_UPS"), NORMAL);
            case 0x37:
                return std::make_pair(&i18n::localize("MOVE_4_PP_UPS"), NORMAL);
            case 0x38 ... 0x3A:
                return std::make_pair(&i18n::localize("IVS"), NORMAL);
            case 0x3B:
                return std::make_pair(&i18n::localize("IVS_EGG_AND_NICKNAMED_FLAGS"), NORMAL);
            case 0x3C ... 0x3F:
                return std::make_pair(&i18n::localize("HOENN_RIBBONS"), NORMAL);
            case 0x40:
                return std::make_pair(&i18n::localize("GENDER_&_FORMS"), NORMAL);
            case 0x41:
                return std::make_pair(&i18n::localize("NATURE"), NORMAL);
            case 0x42:
                return std::make_pair(&i18n::localize("MISC_FLAGS"), NORMAL);
            case 0x43 ... 0x47:
                return UNUSED;
            case 0x48 ... 0x5D:
                return std::make_pair(&i18n::localize("NICKNAME"), NORMAL);
            case 0x5E:
                return UNKNOWN;
            case 0x5F:
                return std::make_pair(&i18n::localize("ORIGIN_GAME"), NORMAL);
            case 0x60 ... 0x63:
                return std::make_pair(&i18n::localize("RIBBONS"), NORMAL);
            case 0x64 ... 0x67:
                return UNUSED;
            case 0x68 ... 0x77:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_NAME"), NORMAL);
            case 0x78 ... 0x7A:
                return std::make_pair(&i18n::localize("EGG_DATE"), NORMAL);
            case 0x7B ... 0x7D:
                return std::make_pair(&i18n::localize("MET_DATE"), NORMAL);
            case 0x7E ... 0x7F:
                return std::make_pair(&i18n::localize("EGG_LOCATION"), NORMAL);
            case 0x80 ... 0x81:
                return std::make_pair(&i18n::localize("MET_LOCATION"), NORMAL);
            case 0x82:
                return std::make_pair(&i18n::localize("POKERUS"), NORMAL);
            case 0x83:
                return std::make_pair(&i18n::localize("POKEBALL"), NORMAL);
            case 0x84:
                return std::make_pair(
                    &i18n::localize("MET_LEVEL_&_ORIGINAL_TRAINER_GENDER"), NORMAL);
            case 0x85:
                return std::make_pair(&i18n::localize("ENCOUNTER_TYPE"), NORMAL);
            case 0x86 ... 0x87:
                return UNUSED;
            case 0x88:
                return std::make_pair(&i18n::localize("STATUS_CONDITIONS"), NORMAL);
            case 0x89:
                return std::make_pair(&i18n::localize("UNKNOWN_FLAGS"), UNRESTRICTED);
            case 0x8A ... 0x8B:
                return UNKNOWN;
            case 0x8C:
                return std::make_pair(&i18n::localize("LEVEL"), NORMAL);
            case 0x8D:
                return std::make_pair(&i18n::localize("CAPSULE_INDEX_(SEALS)"), OPEN);
            case 0x8E ... 0x8F:
                return std::make_pair(&i18n::localize("CURRENT_HP"), OPEN);
            case 0x90 ... 0x91:
                return std::make_pair(&i18n::localize("MAX_HP"), OPEN);
            case 0x92 ... 0x93:
                return std::make_pair(&i18n::localize("ATTACK"), OPEN);
            case 0x94 ... 0x95:
                return std::make_pair(&i18n::localize("DEFENSE"), OPEN);
            case 0x96 ... 0x97:
                return std::make_pair(&i18n::localize("SPEED"), OPEN);
            case 0x98 ... 0x99:
                return std::make_pair(&i18n::localize("SPATK"), OPEN);
            case 0x9A ... 0x9B:
                return std::make_pair(&i18n::localize("SPDEF"), OPEN);
            case 0x9C ... 0xD3:
                return std::make_pair(&i18n::localize("MAIL_MESSAGE_+_OT_NAME"), OPEN);
            case 0xD4 ... 0xDB:
                return UNKNOWN;
        }
    }
    else if (pkm.generation() == pksm::Generation::FOUR)
    {
        switch (i)
        {
            case 0x00 ... 0x03:
                return std::make_pair(&i18n::localize("PID"), NORMAL);
            case 0x04 ... 0x05:
                return UNUSED;
            case 0x06 ... 0x07:
                return std::make_pair(&i18n::localize("CHECKSUM"), UNRESTRICTED);
            case 0x08 ... 0x09:
                return std::make_pair(&i18n::localize("SPECIES"), NORMAL);
            case 0x0A ... 0x0B:
                return std::make_pair(&i18n::localize("ITEM"), NORMAL);
            case 0x0C ... 0x0D:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_ID"), NORMAL);
            case 0x0E ... 0x0F:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_SID"), NORMAL);
            case 0x10 ... 0x13:
                return std::make_pair(&i18n::localize("EXPERIENCE"), NORMAL);
            case 0x14:
                return std::make_pair(&i18n::localize("FRIENDSHIP"), NORMAL);
            case 0x15:
                return std::make_pair(&i18n::localize("ABILITY"), NORMAL);
            case 0x16:
                return std::make_pair(&i18n::localize("MARKINGS"), NORMAL);
            case 0x17:
                return std::make_pair(&i18n::localize("ORIGINAL_LANGUAGE"), NORMAL);
            case 0x18:
                return std::make_pair(&i18n::localize("HP_EV"), NORMAL);
            case 0x19:
                return std::make_pair(&i18n::localize("ATTACK_EV"), NORMAL);
            case 0x1A:
                return std::make_pair(&i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x1B:
                return std::make_pair(&i18n::localize("SPEED_EV"), NORMAL);
            case 0x1C:
                return std::make_pair(&i18n::localize("SPATK_EV"), NORMAL);
            case 0x1D:
                return std::make_pair(&i18n::localize("SPDEF_EV"), NORMAL);
            case 0x1E:
                return std::make_pair(&i18n::localize("COOL_CONTEST_VALUE"), NORMAL);
            case 0x1F:
                return std::make_pair(&i18n::localize("BEAUTY_CONTEST_VALUE"), NORMAL);
            case 0x20:
                return std::make_pair(&i18n::localize("CUTE_CONTEST_VALUE"), NORMAL);
            case 0x21:
                return std::make_pair(&i18n::localize("SMART_CONTEST_VALUE"), NORMAL);
            case 0x22:
                return std::make_pair(&i18n::localize("TOUGH_CONTEST_VALUE"), NORMAL);
            case 0x23:
                return std::make_pair(&i18n::localize("SHEEN_CONTEST_VALUE"), NORMAL);
            case 0x24 ... 0x27:
                return std::make_pair(&i18n::localize("RIBBONS"), NORMAL);
            case 0x28 ... 0x29:
                return std::make_pair(&i18n::localize("MOVE_1"), NORMAL);
            case 0x2A ... 0x2B:
                return std::make_pair(&i18n::localize("MOVE_2"), NORMAL);
            case 0x2C ... 0x2D:
                return std::make_pair(&i18n::localize("MOVE_3"), NORMAL);
            case 0x2E ... 0x2F:
                return std::make_pair(&i18n::localize("MOVE_4"), NORMAL);
            case 0x30:
                return std::make_pair(&i18n::localize("MOVE_1_PP"), NORMAL);
            case 0x31:
                return std::make_pair(&i18n::localize("MOVE_2_PP"), NORMAL);
            case 0x32:
                return std::make_pair(&i18n::localize("MOVE_3_PP"), NORMAL);
            case 0x33:
                return std::make_pair(&i18n::localize("MOVE_4_PP"), NORMAL);
            case 0x34:
                return std::make_pair(&i18n::localize("MOVE_1_PP_UPS"), NORMAL);
            case 0x35:
                return std::make_pair(&i18n::localize("MOVE_2_PP_UPS"), NORMAL);
            case 0x36:
                return std::make_pair(&i18n::localize("MOVE_3_PP_UPS"), NORMAL);
            case 0x37:
                return std::make_pair(&i18n::localize("MOVE_4_PP_UPS"), NORMAL);
            case 0x38 ... 0x3A:
                return std::make_pair(&i18n::localize("IVS"), NORMAL);
            case 0x3B:
                return std::make_pair(&i18n::localize("IVS_EGG_AND_NICKNAMED_FLAGS"), NORMAL);
            case 0x3C ... 0x3F:
                return std::make_pair(&i18n::localize("HOENN_RIBBONS"), NORMAL);
            case 0x40:
                return std::make_pair(&i18n::localize("GENDER_&_FORMS"), NORMAL);
            case 0x41:
                return std::make_pair(&i18n::localize("SHINY_LEAVES_(HGSS)"), NORMAL);
            case 0x42 ... 0x43:
                return UNUSED;
            case 0x44 ... 0x45:
                return std::make_pair(&i18n::localize("EGG_LOCATION_(PLATINUM)"), NORMAL);
            case 0x46 ... 0x47:
                return std::make_pair(&i18n::localize("MET_LOCATION_(PLATINUM)"), NORMAL);
            case 0x48 ... 0x5D:
                return std::make_pair(&i18n::localize("NICKNAME"), NORMAL);
            case 0x5E:
                return UNUSED;
            case 0x5F:
                return std::make_pair(&i18n::localize("ORIGIN_GAME"), NORMAL);
            case 0x60 ... 0x63:
                return std::make_pair(&i18n::localize("RIBBONS"), NORMAL);
            case 0x64 ... 0x67:
                return UNUSED;
            case 0x68 ... 0x77:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_NAME"), NORMAL);
            case 0x78 ... 0x7A:
                return std::make_pair(&i18n::localize("EGG_DATE"), NORMAL);
            case 0x7B ... 0x7D:
                return std::make_pair(&i18n::localize("MET_DATE"), NORMAL);
            case 0x7E ... 0x7F:
                return std::make_pair(&i18n::localize("EGG_LOCATION_(DIAMOND_&_PEARL)"), NORMAL);
            case 0x80 ... 0x81:
                return std::make_pair(&i18n::localize("MET_LOCATION_(DIAMOND_&_PEARL"), NORMAL);
            case 0x82:
                return std::make_pair(&i18n::localize("POKERUS"), NORMAL);
            case 0x83:
                return std::make_pair(&i18n::localize("POKEBALL"), NORMAL);
            case 0x84:
                return std::make_pair(
                    &i18n::localize("MET_LEVEL_&_ORIGINAL_TRAINER_GENDER"), NORMAL);
            case 0x85:
                return std::make_pair(&i18n::localize("ENCOUNTER_TYPE"), NORMAL);
            case 0x86:
                return std::make_pair(&i18n::localize("HGSS_POKEBALL"), NORMAL);
            case 0x87:
                return UNUSED;
            case 0x88:
                return std::make_pair(&i18n::localize("STATUS_CONDITIONS"), NORMAL);
            case 0x89:
                return std::make_pair(&i18n::localize("UNKNOWN_FLAGS"), UNRESTRICTED);
            case 0x8A ... 0x8B:
                return UNKNOWN;
            case 0x8C:
                return std::make_pair(&i18n::localize("LEVEL"), NORMAL);
            case 0x8D:
                return std::make_pair(&i18n::localize("CAPSULE_INDEX_(SEALS)"), OPEN);
            case 0x8E ... 0x8F:
                return std::make_pair(&i18n::localize("CURRENT_HP"), OPEN);
            case 0x90 ... 0x91:
                return std::make_pair(&i18n::localize("MAX_HP"), OPEN);
            case 0x92 ... 0x93:
                return std::make_pair(&i18n::localize("ATTACK"), OPEN);
            case 0x94 ... 0x95:
                return std::make_pair(&i18n::localize("DEFENSE"), OPEN);
            case 0x96 ... 0x97:
                return std::make_pair(&i18n::localize("SPEED"), OPEN);
            case 0x98 ... 0x99:
                return std::make_pair(&i18n::localize("SPATK"), OPEN);
            case 0x9A ... 0x9B:
                return std::make_pair(&i18n::localize("SPDEF"), OPEN);
            case 0x9C ... 0xD3:
                return std::make_pair(&i18n::localize("MAIL_MESSAGE_+_OT_NAME"), OPEN);
            case 0xD4 ... 0xEB:
                return std::make_pair(&i18n::localize("SEAL_COORDINATES"), OPEN);
        }
    }
    else if (pkm.generation() == pksm::Generation::THREE)
    {
        switch (i)
        {
            case 0x0 ... 0x3:
                return std::make_pair(&i18n::localize("PID"), NORMAL);
            case 0x4 ... 0x5:
                return std::make_pair(&i18n::localize("TID"), NORMAL);
            case 0x6 ... 0x7:
                return std::make_pair(&i18n::localize("SID"), NORMAL);
            case 0x8 ... 0x11:
                return std::make_pair(&i18n::localize("NICKNAME"), NORMAL);
            case 0x12:
                return std::make_pair(&i18n::localize("LANGUAGE"), NORMAL);
            case 0x13:
                return std::make_pair(&i18n::localize("BAD_EGG_HAS_SPECIES_IS_EGG"), NORMAL);
            case 0x14 ... 0x1A:
                return std::make_pair(&i18n::localize("OT_NAME"), NORMAL);
            case 0x1B:
                return std::make_pair(&i18n::localize("MARKINGS"), NORMAL);
            case 0x1C ... 0x1D:
                return std::make_pair(&i18n::localize("CHECKSUM"), UNRESTRICTED);
            case 0x1E ... 0x1F:
                return std::make_pair(&i18n::localize("SANITY_PLACEHOLDER"), UNRESTRICTED);
            case 0x20 ... 0x21:
                return std::make_pair(&i18n::localize("G3_SPECIES"), OPEN);
            case 0x22 ... 0x23:
                return std::make_pair(&i18n::localize("G3_ITEM"), OPEN);
            case 0x24 ... 0x27:
                return std::make_pair(&i18n::localize("EXPERIENCE"), NORMAL);
            case 0x28:
                return std::make_pair(&i18n::localize("PP_UPS"), NORMAL);
            case 0x29:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_FRIENDSHIP"), NORMAL);
            case 0x2A ... 0x2B:
                return UNUSED;
            case 0x2C ... 0x2D:
                return std::make_pair(&i18n::localize("MOVE_1"), NORMAL);
            case 0x2E ... 0x2F:
                return std::make_pair(&i18n::localize("MOVE_2"), NORMAL);
            case 0x30 ... 0x31:
                return std::make_pair(&i18n::localize("MOVE_3"), NORMAL);
            case 0x32 ... 0x33:
                return std::make_pair(&i18n::localize("MOVE_4"), NORMAL);
            case 0x34:
                return std::make_pair(&i18n::localize("MOVE_1_CURRENT_PP"), NORMAL);
            case 0x35:
                return std::make_pair(&i18n::localize("MOVE_2_CURRENT_PP"), NORMAL);
            case 0x36:
                return std::make_pair(&i18n::localize("MOVE_3_CURRENT_PP"), NORMAL);
            case 0x37:
                return std::make_pair(&i18n::localize("MOVE_4_CURRENT_PP"), NORMAL);
            case 0x38:
                return std::make_pair(&i18n::localize("HP_EV"), NORMAL);
            case 0x39:
                return std::make_pair(&i18n::localize("ATTACK_EV"), NORMAL);
            case 0x3A:
                return std::make_pair(&i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x3B:
                return std::make_pair(&i18n::localize("SPEED_EV"), NORMAL);
            case 0x3C:
                return std::make_pair(&i18n::localize("SPATK_EV"), NORMAL);
            case 0x3D:
                return std::make_pair(&i18n::localize("SPDEF_EV"), NORMAL);
            case 0x3E:
                return std::make_pair(&i18n::localize("COOL_CONTEST_VALUE"), NORMAL);
            case 0x3F:
                return std::make_pair(&i18n::localize("BEAUTY_CONTEST_VALUE"), NORMAL);
            case 0x40:
                return std::make_pair(&i18n::localize("CUTE_CONTEST_VALUE"), NORMAL);
            case 0x41:
                return std::make_pair(&i18n::localize("SMART_CONTEST_VALUE"), NORMAL);
            case 0x42:
                return std::make_pair(&i18n::localize("TOUGH_CONTEST_VALUE"), NORMAL);
            case 0x43:
                return std::make_pair(&i18n::localize("SHEEN_CONTEST_VALUE"), NORMAL);
            case 0x44:
                return std::make_pair(&i18n::localize("POKERUS"), NORMAL);
            case 0x45:
                return std::make_pair(&i18n::localize("MET_LOCATION"), NORMAL);
            case 0x46 ... 0x47:
                return std::make_pair(&i18n::localize("MET_LEVEL_VERSION_BALL_OT_GENDER"), NORMAL);
            case 0x48 ... 0x4B:
                return std::make_pair(&i18n::localize("IVS_EGG_ABILITY_BIT"), NORMAL);
            case 0x4C ... 0x4F:
                return std::make_pair(&i18n::localize("RIBBONS_FATEFUL_ENCOUNTER"), NORMAL);
            case 0x50 ... 0x53:
                return std::make_pair(&i18n::localize("STATUS_CONDITIONS"), OPEN);
            case 0x54:
                return std::make_pair(&i18n::localize("LEVEL"), NORMAL);
            case 0x55:
                return std::make_pair(&i18n::localize("MAIL_ID"), OPEN);
            case 0x56 ... 0x57:
                return std::make_pair(&i18n::localize("CURRENT_HP"), NORMAL);
            case 0x58 ... 0x59:
                return std::make_pair(&i18n::localize("HP"), NORMAL);
            case 0x5A ... 0x5B:
                return std::make_pair(&i18n::localize("ATTACK"), NORMAL);
            case 0x5C ... 0x5D:
                return std::make_pair(&i18n::localize("DEFENSE"), NORMAL);
            case 0x5E ... 0x5F:
                return std::make_pair(&i18n::localize("SPEED"), NORMAL);
            case 0x60 ... 0x61:
                return std::make_pair(&i18n::localize("SPATK"), NORMAL);
            case 0x62 ... 0x63:
                return std::make_pair(&i18n::localize("SPDEF"), NORMAL);
        }
    }
    else if (pkm.generation() == pksm::Generation::EIGHT)
    {
        switch (i)
        {
            case 0x00 ... 0x03:
                return std::make_pair(&i18n::localize("ENCRYPTION_KEY"), UNRESTRICTED);
            case 0x04 ... 0x05:
                return std::make_pair(&i18n::localize("SANITY_PLACEHOLDER"), UNRESTRICTED);
            case 0x06 ... 0x07:
                return std::make_pair(&i18n::localize("CHECKSUM"), UNRESTRICTED);
            case 0x08 ... 0x09:
                return std::make_pair(&i18n::localize("SPECIES"), NORMAL);
            case 0x0A ... 0x0B:
                return std::make_pair(&i18n::localize("ITEM"), NORMAL);
            case 0x0C ... 0x0D:
                return std::make_pair(&i18n::localize("OT_ID"), NORMAL);
            case 0x0E ... 0x0F:
                return std::make_pair(&i18n::localize("OT_SID"), NORMAL);
            case 0x10 ... 0x13:
                return std::make_pair(&i18n::localize("EXPERIENCE"), NORMAL);
            case 0x14 ... 0x15:
                return std::make_pair(&i18n::localize("ABILITY"), NORMAL);
            case 0x16:
                return std::make_pair(
                    &i18n::localize("ABILITY_NUMBER_FAVORITE_GIGANTAMAX_FACTOR"), NORMAL);
            case 0x17:
                return UNUSED;
            case 0x18 ... 0x19:
                return std::make_pair(&i18n::localize("MARKINGS"), NORMAL);
            case 0x1A ... 0x1B:
                return UNUSED;
            case 0x1C ... 0x1F:
                return std::make_pair(&i18n::localize("PID"), NORMAL);
            case 0x20:
                return std::make_pair(&i18n::localize("ORIGINAL_NATURE"), NORMAL);
            case 0x21:
                return std::make_pair(&i18n::localize("NATURE"), NORMAL);
            case 0x22:
                return std::make_pair(&i18n::localize("GENDER_FATEFUL_ENCOUNTER"), NORMAL);
            case 0x23:
                return UNUSED;
            case 0x24 ... 0x25:
                return std::make_pair(&i18n::localize("FORM"), NORMAL);
            case 0x26:
                return std::make_pair(&i18n::localize("HP_EV"), NORMAL);
            case 0x27:
                return std::make_pair(&i18n::localize("ATTACK_EV"), NORMAL);
            case 0x28:
                return std::make_pair(&i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x29:
                return std::make_pair(&i18n::localize("SPEED_EV"), NORMAL);
            case 0x2A:
                return std::make_pair(&i18n::localize("SPATK_EV"), NORMAL);
            case 0x2B:
                return std::make_pair(&i18n::localize("SPDEF_EV"), NORMAL);
            case 0x2C:
                return std::make_pair(&i18n::localize("COOL_CONTEST_VALUE"), NORMAL);
            case 0x2D:
                return std::make_pair(&i18n::localize("BEAUTY_CONTEST_VALUE"), NORMAL);
            case 0x2E:
                return std::make_pair(&i18n::localize("CUTE_CONTEST_VALUE"), NORMAL);
            case 0x2F:
                return std::make_pair(&i18n::localize("SMART_CONTEST_VALUE"), NORMAL);
            case 0x30:
                return std::make_pair(&i18n::localize("TOUGH_CONTEST_VALUE"), NORMAL);
            case 0x31:
                return std::make_pair(&i18n::localize("SHEEN_CONTEST_VALUE"), NORMAL);
            case 0x32:
                return std::make_pair(&i18n::localize("POKERUS"), NORMAL);
            case 0x33 ... 0x3B:
                return std::make_pair(&i18n::localize("RIBBONS"), NORMAL);
            case 0x3C:
                return std::make_pair(&i18n::localize("BATTLE_MEMORY_RIBBON_COUNT"), NORMAL);
            case 0x3D:
                return std::make_pair(&i18n::localize("CONTEST_MEMORY_RIBBON_COUNT"), NORMAL);
            case 0x3E ... 0x3F:
                return UNUSED;
            case 0x40 ... 0x47:
                return std::make_pair(&i18n::localize("RIBBONS"), NORMAL);
            case 0x48 ... 0x4B:
                return UNKNOWN; // Unknown u32
            case 0x4C ... 0x4F:
                return UNUSED; // Unused byte array
            case 0x50:
                return std::make_pair(&i18n::localize("HEIGHT"), NORMAL);
            case 0x51:
                return std::make_pair(&i18n::localize("WEIGHT"), NORMAL);
            case 0x52 ... 0x57:
                return UNKNOWN;
            case 0x58 ... 0x6F:
                return std::make_pair(&i18n::localize("NICKNAME"), NORMAL);
            case 0x70 ... 0x71:
                return std::make_pair(&i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
            case 0x72 ... 0x73:
                return std::make_pair(&i18n::localize("MOVE_1"), NORMAL);
            case 0x74 ... 0x75:
                return std::make_pair(&i18n::localize("MOVE_2"), NORMAL);
            case 0x76 ... 0x77:
                return std::make_pair(&i18n::localize("MOVE_3"), NORMAL);
            case 0x78 ... 0x79:
                return std::make_pair(&i18n::localize("MOVE_4"), NORMAL);
            case 0x7A:
                return std::make_pair(&i18n::localize("MOVE_1_CURRENT_PP"), NORMAL);
            case 0x7B:
                return std::make_pair(&i18n::localize("MOVE_2_CURRENT_PP"), NORMAL);
            case 0x7C:
                return std::make_pair(&i18n::localize("MOVE_3_CURRENT_PP"), NORMAL);
            case 0x7D:
                return std::make_pair(&i18n::localize("MOVE_4_CURRENT_PP"), NORMAL);
            case 0x7E:
                return std::make_pair(&i18n::localize("MOVE_1_PP_UPS"), NORMAL);
            case 0x7F:
                return std::make_pair(&i18n::localize("MOVE_2_PP_UPS"), NORMAL);
            case 0x80:
                return std::make_pair(&i18n::localize("MOVE_3_PP_UPS"), NORMAL);
            case 0x81:
                return std::make_pair(&i18n::localize("MOVE_4_PP_UPS"), NORMAL);
            case 0x82 ... 0x83:
                return std::make_pair(&i18n::localize("RELEARN_MOVE_1_ID"), NORMAL);
            case 0x84 ... 0x85:
                return std::make_pair(&i18n::localize("RELEARN_MOVE_2_ID"), NORMAL);
            case 0x86 ... 0x87:
                return std::make_pair(&i18n::localize("RELEARN_MOVE_3_ID"), NORMAL);
            case 0x88 ... 0x89:
                return std::make_pair(&i18n::localize("RELEARN_MOVE_4_ID"), NORMAL);
            case 0x8A ... 0x8B:
                return std::make_pair(&i18n::localize("CURRENT_HP"), NORMAL);
            case 0x8C ... 0x8F:
                return std::make_pair(&i18n::localize("IVS_EGG_AND_NICKNAMED_FLAGS"), NORMAL);
            case 0x90:
                return std::make_pair(&i18n::localize("DYNAMAX_LEVEL"), NORMAL);
            case 0x91 ... 0x93:
                return UNUSED;
            case 0x94 ... 0x97:
                return std::make_pair(&i18n::localize("STATUS_CONDITIONS"), NORMAL);
            case 0x98 ... 0x9B:
                return UNKNOWN;
            case 0x9C ... 0xA7:
                return UNUSED;
            case 0xA8 ... 0xBF:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_NAME"), NORMAL);
            case 0xC0 ... 0xC1:
                return std::make_pair(&i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
            case 0xC2:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_GENDER"), NORMAL);
            case 0xC3:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_LANGUAGE"), NORMAL);
            case 0xC4:
                return std::make_pair(&i18n::localize("CURRENT_HANDLER"), NORMAL);
            case 0xC5:
                return UNUSED;
            case 0xC6 ... 0xC7:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_ID"), NORMAL);
            case 0xC8:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_FRIENDSHIP"), NORMAL);
            case 0xC9:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_MEMORY_INTENSITY"), OPEN);
            case 0xCA:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_MEMORY_LINE"), OPEN);
            case 0xCB:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_MEMORY_FEELING"), OPEN);
            case 0xCC ... 0xCD:
                return std::make_pair(&i18n::localize("CURRENT_TRAINER_MEMORY_TEXTVAR"), OPEN);
            case 0xCE ... 0xDB:
                return UNUSED;
            case 0xDC:
                return std::make_pair(&i18n::localize("FULLNESS"), NORMAL);
            case 0xDD:
                return std::make_pair(&i18n::localize("ENJOYMENT"), NORMAL);
            case 0xDE:
                return std::make_pair(&i18n::localize("ORIGIN_GAME"), NORMAL);
            case 0xDF:
                return std::make_pair(&i18n::localize("BATTLE_VERSION"), NORMAL);
            case 0xE0 ... 0xE1:
                return UNUSED;
            case 0xE2:
                return std::make_pair(&i18n::localize("LANGUAGE"), NORMAL);
            case 0xE3:
                return UNKNOWN;
            case 0xE4 ... 0xE7:
                return std::make_pair(&i18n::localize("FORM_ARGUMENT"), NORMAL);
            case 0xE8:
                return std::make_pair(&i18n::localize("FAVORITE_RIBBON"), NORMAL);
            case 0xE9 ... 0xF7:
                return UNUSED;
            case 0xF8 ... 0x10F:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_NAME"), NORMAL);
            case 0x110 ... 0x111:
                return std::make_pair(&i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
            case 0x112:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_FRIENDSHIP"), NORMAL);
            case 0x113:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_MEMORY_INTENSITY"), OPEN);
            case 0x114:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_MEMORY_LINE"), OPEN);
            case 0x115:
                return UNUSED;
            case 0x116 ... 0x117:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_MEMORY_TEXTVAR"), OPEN);
            case 0x118:
                return std::make_pair(&i18n::localize("ORIGINAL_TRAINER_MEMORY_FEELING"), OPEN);
            case 0x119:
                return std::make_pair(&i18n::localize("EGG_YEAR"), NORMAL);
            case 0x11A:
                return std::make_pair(&i18n::localize("EGG_MONTH"), NORMAL);
            case 0x11B:
                return std::make_pair(&i18n::localize("EGG_DAY"), NORMAL);
            case 0x11C:
                return std::make_pair(&i18n::localize("MET_YEAR"), NORMAL);
            case 0x11D:
                return std::make_pair(&i18n::localize("MET_MONTH"), NORMAL);
            case 0x11E:
                return std::make_pair(&i18n::localize("MET_DAY"), NORMAL);
            case 0x11F:
                return UNUSED;
            case 0x120 ... 0x121:
                return std::make_pair(&i18n::localize("EGG_LOCATION"), NORMAL);
            case 0x122 ... 0x123:
                return std::make_pair(&i18n::localize("MET_LOCATION"), NORMAL);
            case 0x124:
                return std::make_pair(&i18n::localize("BALL"), NORMAL);
            case 0x125:
                return std::make_pair(
                    &i18n::localize("MET_LEVEL_&_ORIGINAL_TRAINER_GENDER"), NORMAL);
            case 0x126:
                return std::make_pair(&i18n::localize("HYPER_TRAIN_FLAGS"), NORMAL);
            case 0x127 ... 0x134:
                return std::make_pair(&i18n::localize("TECHNICAL_RECORD_FLAGS"), NORMAL);
            case 0x135 ... 0x13C:
                return std::make_pair(&i18n::localize("HOME_TRACKER"), OPEN);
            case 0x13D ... 0x147:
                return UNUSED;
            case 0x148:
                return std::make_pair(&i18n::localize("LEVEL"), NORMAL);
            case 0x149:
                return UNUSED;
            case 0x14A ... 0x14B:
                return std::make_pair(&i18n::localize("MAX_HP"), OPEN);
            case 0x14C ... 0x14D:
                return std::make_pair(&i18n::localize("ATTACK"), OPEN);
            case 0x14E ... 0x14F:
                return std::make_pair(&i18n::localize("DEFENSE"), OPEN);
            case 0x150 ... 0x151:
                return std::make_pair(&i18n::localize("SPEED"), OPEN);
            case 0x152 ... 0x153:
                return std::make_pair(&i18n::localize("SPATK"), OPEN);
            case 0x154 ... 0x155:
                return std::make_pair(&i18n::localize("SPDEF"), OPEN);
            case 0x156 ... 0x157:
                return std::make_pair(&i18n::localize("DYNAMAX_TYPE"), OPEN);
        }
    }
    else if (pkm.generation() == pksm::Generation::ONE)
    {
        switch (i)
        {
            case 0x0:
                return std::make_pair(&i18n::localize("GB_HEADER_COUNT"), UNRESTRICTED);
            case 0x1:
                return std::make_pair(&i18n::localize("GB_HEADER_SPECIES"), UNRESTRICTED);
            case 0x2:
                return std::make_pair(&i18n::localize("GB_HEADER_TERMINATOR"), UNRESTRICTED);
            case 0x3:
                return std::make_pair(&i18n::localize("SPECIES"), NORMAL);
            case 0x4 ... 0x5:
                return std::make_pair(&i18n::localize("CURRENT_HP"), NORMAL);
            case 0x6:
                return std::make_pair(&i18n::localize("PC_LEVEL"), NORMAL);
            case 0x7:
                return std::make_pair(&i18n::localize("STATUS_CONDITIONS"), NORMAL);
            case 0x8:
                return std::make_pair(&i18n::localize("TYPE1"), UNRESTRICTED);
            case 0x9:
                return std::make_pair(&i18n::localize("TYPE2"), UNRESTRICTED);
            case 0xA:
                return std::make_pair(&i18n::localize("CATCH_RATE"), NORMAL);
            case 0xB:
                return std::make_pair(&i18n::localize("MOVE_1"), NORMAL);
            case 0xC:
                return std::make_pair(&i18n::localize("MOVE_2"), NORMAL);
            case 0xD:
                return std::make_pair(&i18n::localize("MOVE_3"), NORMAL);
            case 0xE:
                return std::make_pair(&i18n::localize("MOVE_4"), NORMAL);
            case 0xF ... 0x10:
                return std::make_pair(&i18n::localize("TID"), NORMAL);
            case 0x11 ... 0x13:
                return std::make_pair(&i18n::localize("EXPERIENCE"), NORMAL);
            case 0x14 ... 0x15:
                return std::make_pair(&i18n::localize("HP_EV"), NORMAL);
            case 0x16 ... 0x17:
                return std::make_pair(&i18n::localize("ATTACK_EV"), NORMAL);
            case 0x18 ... 0x19:
                return std::make_pair(&i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x1A ... 0x1B:
                return std::make_pair(&i18n::localize("SPEED_EV"), NORMAL);
            case 0x1C ... 0x1D:
                return std::make_pair(&i18n::localize("SPECIAL_EV"), NORMAL);
            case 0x1E ... 0x1F:
                return std::make_pair(&i18n::localize("IVS"), NORMAL);
            case 0x20:
                return std::make_pair(&i18n::localize("MOVE_1_CURRENT_PP"), NORMAL);
            case 0x21:
                return std::make_pair(&i18n::localize("MOVE_2_CURRENT_PP"), NORMAL);
            case 0x22:
                return std::make_pair(&i18n::localize("MOVE_3_CURRENT_PP"), NORMAL);
            case 0x23:
                return std::make_pair(&i18n::localize("MOVE_4_CURRENT_PP"), NORMAL);
            case 0x24:
                return std::make_pair(&i18n::localize("LEVEL"), NORMAL);
            case 0x25 ... 0x26:
                return std::make_pair(&i18n::localize("MAX_HP"), OPEN);
            case 0x27 ... 0x28:
                return std::make_pair(&i18n::localize("ATTACK"), OPEN);
            case 0x29 ... 0x2A:
                return std::make_pair(&i18n::localize("DEFENSE"), OPEN);
            case 0x2B ... 0x2C:
                return std::make_pair(&i18n::localize("SPEED"), OPEN);
            case 0x2D ... 0x2E:
                return std::make_pair(&i18n::localize("SPECIAL"), OPEN);
            case 0x2F ... 0x44:
            {
                if (pkm.language() == pksm::Language::JPN)
                {
                    if (i <= 0x34)
                    {
                        return std::make_pair(
                            &i18n::localize("OT_NAME"), UNRESTRICTED); // floating terminator
                    }
                    return std::make_pair(
                        &i18n::localize("NICKNAME"), UNRESTRICTED); // floating terminator
                }
                if (i <= 0x39)
                {
                    return std::make_pair(
                        &i18n::localize("OT_NAME"), UNRESTRICTED); // floating terminator
                }
                return std::make_pair(
                    &i18n::localize("NICKNAME"), UNRESTRICTED); // floating terminator
            }
        }
    }
    else if (pkm.generation() == pksm::Generation::TWO)
    {
        switch (i)
        {
            case 0x0:
                return std::make_pair(&i18n::localize("GB_HEADER_COUNT"), UNRESTRICTED);
            case 0x1:
                return std::make_pair(&i18n::localize("GB_HEADER_SPECIES"), UNRESTRICTED);
            case 0x2:
                return std::make_pair(&i18n::localize("GB_HEADER_TERMINATOR"), UNRESTRICTED);
            case 0x3:
                return std::make_pair(&i18n::localize("SPECIES"), NORMAL);
            case 0x4:
                return std::make_pair(&i18n::localize("ITEM"), NORMAL);
            case 0x5:
                return std::make_pair(&i18n::localize("MOVE_1"), NORMAL);
            case 0x6:
                return std::make_pair(&i18n::localize("MOVE_2"), NORMAL);
            case 0x7:
                return std::make_pair(&i18n::localize("MOVE_3"), NORMAL);
            case 0x8:
                return std::make_pair(&i18n::localize("MOVE_4"), NORMAL);
            case 0x9 ... 0xA:
                return std::make_pair(&i18n::localize("TID"), NORMAL);
            case 0xB ... 0xD:
                return std::make_pair(&i18n::localize("EXPERIENCE"), NORMAL);
            case 0xE ... 0xF:
                return std::make_pair(&i18n::localize("HP_EV"), NORMAL);
            case 0x10 ... 0x11:
                return std::make_pair(&i18n::localize("ATTACK_EV"), NORMAL);
            case 0x12 ... 0x13:
                return std::make_pair(&i18n::localize("DEFENSE_EV"), NORMAL);
            case 0x14 ... 0x15:
                return std::make_pair(&i18n::localize("SPEED_EV"), NORMAL);
            case 0x16 ... 0x17:
                return std::make_pair(&i18n::localize("SPECIAL_EV"), NORMAL);
            case 0x18 ... 0x19:
                return std::make_pair(&i18n::localize("IVS"), NORMAL);
            case 0x1A:
                return std::make_pair(&i18n::localize("MOVE_1_CURRENT_PP"), NORMAL);
            case 0x1B:
                return std::make_pair(&i18n::localize("MOVE_2_CURRENT_PP"), NORMAL);
            case 0x1C:
                return std::make_pair(&i18n::localize("MOVE_3_CURRENT_PP"), NORMAL);
            case 0x1D:
                return std::make_pair(&i18n::localize("MOVE_4_CURRENT_PP"), NORMAL);
            case 0x1E:
                return std::make_pair(&i18n::localize("FRIENDSHIP"), NORMAL);
            case 0x1F:
                return std::make_pair(&i18n::localize("POKERUS"), NORMAL);
            case 0x20 ... 0x21:
                return std::make_pair(&i18n::localize("CAUGHT_DATA"), NORMAL);
            case 0x22:
                return std::make_pair(&i18n::localize("LEVEL"), NORMAL);
            case 0x23:
                return std::make_pair(&i18n::localize("STATUS_CONDITIONS"), NORMAL);
            case 0x24:
                return UNUSED;
            case 0x25 ... 0x26:
                return std::make_pair(&i18n::localize("CURRENT_HP"), NORMAL);
            case 0x27 ... 0x28:
                return std::make_pair(&i18n::localize("MAX_HP"), OPEN);
            case 0x29 ... 0x2A:
                return std::make_pair(&i18n::localize("ATTACK"), OPEN);
            case 0x2B ... 0x2C:
                return std::make_pair(&i18n::localize("DEFENSE"), OPEN);
            case 0x2D ... 0x2E:
                return std::make_pair(&i18n::localize("SPEED"), OPEN);
            case 0x2F ... 0x30:
                return std::make_pair(&i18n::localize("SPATK"), OPEN);
            case 0x31 ... 0x32:
                return std::make_pair(&i18n::localize("SPDEF"), OPEN);
            case 0x33 ... 0x48:
            {
                if (pkm.language() == pksm::Language::JPN)
                {
                    if (i <= 0x38)
                    {
                        return std::make_pair(
                            &i18n::localize("OT_NAME"), UNRESTRICTED); // floating terminator
                    }
                    if (i == 0x3E)
                    {
                        return std::make_pair(&i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
                    }
                    return std::make_pair(&i18n::localize("NICKNAME"), NORMAL);
                }
                if (i <= 0x3D)
                {
                    return std::make_pair(
                        &i18n::localize("OT_NAME"), UNRESTRICTED); // floating terminator
                }
                if (i == 0x48)
                {
                    return std::make_pair(&i18n::localize("NULL_TERMINATOR"), UNRESTRICTED);
                }
                return std::make_pair(&i18n::localize("NICKNAME"), NORMAL);
            }
        }
    }
    return std::make_pair(&i18n::localize("REPORT_THIS_TO_FLAGBREW"), UNRESTRICTED);
}

HexEditScreen::HexEditScreen(pksm::PKX& pkm) : pkm(pkm), hid(240, 16)
{
    // Set to fast mode
    hidSetRepeatParameters(5, 1);
    int currRibbon = 0;
    for (u32 i = 0; i < pkm.getLength(); i++)
    {
        buttons.push_back({});
        buttons[i].push_back(std::make_unique<HexEditButton>(
            145, 33, 13, 13, [this]() { return editNumber(true, true); },
            ui_sheet_button_plus_small_idx, "", false, 0));
        buttons[i].push_back(std::make_unique<HexEditButton>(
            161, 33, 13, 13, [this]() { return editNumber(false, true); },
            ui_sheet_button_plus_small_idx, "", false, 0));
        buttons[i].push_back(std::make_unique<HexEditButton>(
            145, 75, 13, 13, [this]() { return editNumber(true, false); },
            ui_sheet_button_minus_small_idx, "", false, 0));
        buttons[i].push_back(std::make_unique<HexEditButton>(
            161, 75, 13, 13, [this]() { return editNumber(false, false); },
            ui_sheet_button_minus_small_idx, "", false, 0));
        if (pkm.generation() == pksm::Generation::SIX ||
            pkm.generation() == pksm::Generation::SEVEN ||
            pkm.generation() == pksm::Generation::LGPE)
        {
            switch (i)
            {
                // Fateful Encounter
                case 0x1D:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 0); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FATEFUL_ENCOUNTER"),
                        true, 0));
                    buttons[i].back()->setToggled(pkm.rawData()[i] & 0x1);
                    break;
                // Markings
                case 0x16 ... 0x17:
                    if (pkm.generation() != pksm::Generation::SIX)
                    {
                        for (int j = 0; j < 4; j++)
                        {
                            buttons[i].pop_back();
                        }
                        for (int j = 0; j < (i == 0x16 ? 4 : 2); j++)
                        {
                            u8 currentMark = i == 0x16 ? j : j + 4;
                            buttons[i].push_back(std::make_unique<HexEditButton>(
                                30, 90 + j * 16, 13, 13,
                                [this, currentMark]() { return this->rotateMark(currentMark); },
                                ui_sheet_emulated_toggle_gray_idx,
                                i18n::localize(std::string(marks[currentMark])), false, currentMark,
                                true));
                            buttons[i].back()->setColor((pkm.rawData()[i] >> (j * 2)) & 0x3);
                        }
                    }
                    break;
                case 0x2A:
                    if (pkm.generation() == pksm::Generation::SIX)
                    {
                        for (int j = 0; j < 4; j++)
                        {
                            buttons[i].pop_back();
                        }
                        for (int j = 0; j < 6; j++)
                        {
                            buttons[i].push_back(std::make_unique<HexEditButton>(
                                30, 90 + j * 16, 13, 13,
                                [this, i, j]() { return this->toggleBit(i, j); },
                                ui_sheet_emulated_toggle_green_idx,
                                i18n::localize(std::string(marks[j])), true, j));
                            buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        }
                    }
                    break;
                case 0x36:
                    if (pkm.generation() == pksm::Generation::SIX)
                    {
                        for (size_t j = 0; j < 4; j++)
                        {
                            buttons[i].pop_back();
                        }
                        for (size_t j = 0; j < 8; j++)
                        {
                            buttons[i].push_back(std::make_unique<HexEditButton>(
                                30, 90 + j * 16, 13, 13,
                                [this, i, j]() { return this->toggleBit(i, j); },
                                ui_sheet_emulated_toggle_green_idx,
                                i18n::localize(std::string(gen67ToggleTexts[currRibbon + 4])), true,
                                j));
                            buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        }
                        currRibbon += 8;
                        break;
                    }
                // falls through
                // Super Training Flags
                case 0x2C ... 0x2F:
                // Ribbons
                case 0x30 ... 0x35:
                // Distribution Super Training (???)
                case 0x3A:
                    // LGPE height byte
                    if (pkm.generation() == pksm::Generation::LGPE && i == 0x3A)
                    {
                        currRibbon += 8;
                        break;
                    }
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        // Early exit to remove last two ribbons
                        if (pkm.generation() == pksm::Generation::SIX && i == 0x35 && j == 6)
                        {
                            currRibbon += 2;
                            buttons[i].push_back(std::make_unique<HexEditButton>(
                                30, 90 + j * 16, 13, 13,
                                [this, i, j]() { return this->toggleBit(i, j); },
                                ui_sheet_emulated_toggle_green_idx,
                                i18n::localize(std::string(gen67ToggleTexts[currRibbon + 4])), true,
                                j));
                            buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                            j++;
                            buttons[i].push_back(std::make_unique<HexEditButton>(
                                30, 90 + j * 16, 13, 13,
                                [this, i, j]() { return this->toggleBit(i, j); },
                                ui_sheet_emulated_toggle_green_idx,
                                i18n::localize(std::string(gen67ToggleTexts[currRibbon + 4])), true,
                                j));
                            buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                            break;
                        }
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(gen67ToggleTexts[currRibbon])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // Secret Super Training Flag
                case 0x72:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 0); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("SECRET_SUPER_TRAINING"),
                        true, 0));
                    buttons[i].back()->setToggled(pkm.rawData()[i] & 0x1);
                    break;
                // Egg, & Nicknamed Flag
                case 0x77:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("EGG"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("NICKNAMED"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                // OT Gender
                case 0xDD:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FEMALE_OT"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                case 0xDE:
                    if (pkm.generation() != pksm::Generation::SIX)
                    {
                        for (int j = 0; j < 4; j++)
                        {
                            buttons[i].pop_back();
                        }
                        for (int j = 0; j < 6; j++)
                        {
                            buttons[i].push_back(std::make_unique<HexEditButton>(
                                30, 90 + j * 16, 13, 13,
                                [this, i, j]() { return this->toggleBit(i, j); },
                                ui_sheet_emulated_toggle_green_idx,
                                i18n::localize(std::string(hyperVals[j])), true, j));
                            buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        }
                    }
                    break;
                // Status
                case 0xE8:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 3); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("POISONED"), true, 3));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 4); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("BURNED"), true, 4));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 122, 13, 13, [this, i]() { return this->toggleBit(i, 5); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FROZEN"), true, 5));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 138, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("PARALYZED"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 154, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("TOXIC"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
            }
        }
        else if (pkm.generation() == pksm::Generation::FIVE)
        {
            switch (i)
            {
                // Markings
                case 0x16:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 6; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(marks[j])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                    }
                    break;
                // Ribbons
                case 0x24 ... 0x27:
                case 0x3C ... 0x3F:
                case 0x60 ... 0x63:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(gen5ToggleTexts[currRibbon])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // Egg and Nicknamed Flags
                case 0x3B:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("EGG"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("NICKNAMED"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                // Fateful Encounter
                case 0x40:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 0); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FATEFUL_ENCOUNTER"),
                        true, 0));
                    buttons[i].back()->setToggled(pkm.rawData()[i] & 0x1);
                    break;
                // DreamWorldAbility & N's Pokemon Flags
                case 0x42:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 0); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("HIDDEN_ABILITY?"), true,
                        0));
                    buttons[i].back()->setToggled(pkm.rawData()[i] & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 1); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("NS_POKEMON?"), true,
                        1));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 1) & 0x1);
                    break;
                // OT Gender
                case 0x84:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FEMALE_OT"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                // Status
                case 0x88:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 3); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("POISONED"), true, 3));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 4); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("BURNED"), true, 4));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 122, 13, 13, [this, i]() { return this->toggleBit(i, 5); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FROZEN"), true, 5));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 138, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("PARALYZED"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 154, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("TOXIC"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
            }
        }
        else if (pkm.generation() == pksm::Generation::FOUR)
        {
            switch (i)
            {
                // Markings
                case 0x16:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 6; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(marks[j])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                    }
                    break;
                // Ribbons
                case 0x24 ... 0x27:
                case 0x3C ... 0x3F:
                case 0x60 ... 0x63:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(gen4ToggleTexts[currRibbon])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // Egg and Nicknamed Flags
                case 0x3B:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("EGG"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("NICKNAMED"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                // Fateful Encounter
                case 0x40:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 0); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FATEFUL_ENCOUNTER"),
                        true, 0));
                    buttons[i].back()->setToggled(pkm.rawData()[i] & 0x1);
                    break;
                // Gold Leaves & Crown
                case 0x41:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 5; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            (i18n::localize("SHINY_LEAF") + ' ') + (char)('A' + j), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                    }
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 170, 13, 13, [this, i]() { return this->toggleBit(i, 5); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("SHINY_CROWN"), true,
                        5));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 5) & 0x1);
                    break;
                // OT Gender
                case 0x84:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FEMALE_OT"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                // Status
                case 0x88:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 3); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("POISONED"), true, 3));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 4); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("BURNED"), true, 4));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 122, 13, 13, [this, i]() { return this->toggleBit(i, 5); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FROZEN"), true, 5));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 138, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("PARALYZED"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 154, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("TOXIC"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
            }
        }
        else if (pkm.generation() == pksm::Generation::THREE)
        {
            switch (i)
            {
                // Egg, bad egg, and has species flags
                case 0x13:
                    for (int j = 0; j < 3; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(gen3ToggleTexts[currRibbon])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // markings
                case 0x1B:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 6; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(marks[j])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                    }
                    break;
                // OT Gender
                case 0x47:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FEMALE_OT"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                // Egg and ability bit
                case 0x4B:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90 + 0 * 16, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("EGG"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90 + 1 * 16, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("ABILITY_NUMBER"), true,
                        7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                case 0x4D:
                    for (int j = 7; j < 8; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + (j - 7) * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(gen3ToggleTexts[currRibbon])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                case 0x4E ... 0x4F:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 8; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(gen3ToggleTexts[currRibbon])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
            }
        }
        else if (pkm.generation() == pksm::Generation::EIGHT)
        {
            switch (i)
            {
                // Favorite, can gigamax
                case 0x16:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 3); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FAVORITE"), true, 3));
                    buttons[i].back()->setToggled(pkm.rawData()[i] & 8);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90 + 16, 13, 13, [this, i]() { return this->toggleBit(i, 4); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("GIGATIMAX_FACTOR"),
                        true, 4));
                    buttons[i].back()->setToggled(pkm.rawData()[i] & 16);
                    break;
                // Markings
                case 0x18 ... 0x19:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < (i == 0x18 ? 4 : 2); j++)
                    {
                        u8 currentMark = i == 0x18 ? j : j + 4;
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, currentMark]() { return this->rotateMark(currentMark); },
                            ui_sheet_emulated_toggle_gray_idx,
                            i18n::localize(std::string(marks[currentMark])), false, currentMark,
                            true));
                        buttons[i].back()->setColor((pkm.rawData()[i] >> (j * 2)) & 0x3);
                    }
                    break;
                // Fateful Encounter
                case 0x22:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 0); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FATEFUL_ENCOUNTER"),
                        true, 0));
                    buttons[i].back()->setToggled(pkm.rawData()[i] & 0x1);
                    break;
                // Ribbons
                case 0x34 ... 0x3B:
                case 0x40 ... 0x47:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (size_t j = 0; j < 8; j++)
                    {
                        const std::string& text =
                            (size_t)currRibbon < gen8Ribbons.size()
                                ? i18n::ribbon(Configuration::getInstance().language(),
                                      gen8Ribbons[currRibbon])
                                : i18n::localize("UNUSED");
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx, text, true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
                // Egg, & Nicknamed Flag
                case 0x8F:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("EGG"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("NICKNAMED"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                // OT Gender
                case 0x125:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FEMALE_OT"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                // Hyper training
                case 0x126:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (int j = 0; j < 6; j++)
                    {
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::localize(std::string(hyperVals[j])), true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                    }
                    break;
                // technical records
                case 0x127:
                    currRibbon = 0; // Reset the counter so that I can reuse it
                    // falls through
                case 0x128 ... 0x134:
                    for (int j = 0; j < 4; j++)
                    {
                        buttons[i].pop_back();
                    }
                    for (size_t j = 0; j < 8; j++)
                    {
                        // Only 100 TRs
                        if (currRibbon > 99)
                        {
                            break;
                        }
                        // TR00 is item 1130
                        buttons[i].push_back(std::make_unique<HexEditButton>(
                            30, 90 + j * 16, 13, 13,
                            [this, i, j]() { return this->toggleBit(i, j); },
                            ui_sheet_emulated_toggle_green_idx,
                            i18n::item(Configuration::getInstance().language(), 1130 + currRibbon),
                            true, j));
                        buttons[i].back()->setToggled((pkm.rawData()[i] >> j) & 0x1);
                        currRibbon++;
                    }
                    break;
            }
        }
        else if (pkm.generation() == pksm::Generation::ONE)
        {
            switch (i)
            {
                case 0x7:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 3); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("ASLEEP"), true, 3));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 4); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("POISONED"), true, 4));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 122, 13, 13, [this, i]() { return this->toggleBit(i, 5); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("BURNED"), true, 5));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 138, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FROZEN"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 154, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("PARALYZED"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
            }
        }
        else if (pkm.generation() == pksm::Generation::TWO)
        {
            switch (i)
            {
                case 0x21:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 120, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FEMALE_OT"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
                case 0x23:
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 90, 13, 13, [this, i]() { return this->toggleBit(i, 3); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("ASLEEP"), true, 3));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 3) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 106, 13, 13, [this, i]() { return this->toggleBit(i, 4); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("POISONED"), true, 4));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 4) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 122, 13, 13, [this, i]() { return this->toggleBit(i, 5); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("BURNED"), true, 5));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 5) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 138, 13, 13, [this, i]() { return this->toggleBit(i, 6); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("FROZEN"), true, 6));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 6) & 0x1);
                    buttons[i].push_back(std::make_unique<HexEditButton>(
                        30, 154, 13, 13, [this, i]() { return this->toggleBit(i, 7); },
                        ui_sheet_emulated_toggle_green_idx, i18n::localize("PARALYZED"), true, 7));
                    buttons[i].back()->setToggled((pkm.rawData()[i] >> 7) & 0x1);
                    break;
            }
        }
    }
    hid.update(pkm.getLength());
    selectedDescription = describe(0);
}

void HexEditScreen::drawTop() const
{
    Gui::sprite(ui_sheet_part_mtx_15x16_idx, 0, 0);

    // Selected box
    Gui::drawSolidRect(
        (hid.index() % 16) * 25, (hid.index() / 16) * 15, 24, 14, PKSM_Color(15, 22, 89, 0));
    Gui::drawSolidRect((hid.index() % 16) * 25, (hid.index() / 16) * 15, 1, 14, COLOR_YELLOW);
    Gui::drawSolidRect((hid.index() % 16) * 25, (hid.index() / 16) * 15, 24, 1, COLOR_YELLOW);
    Gui::drawSolidRect((hid.index() % 16) * 25, (hid.index() / 16) * 15 + 13, 24, 1, COLOR_YELLOW);
    Gui::drawSolidRect((hid.index() % 16) * 25 + 23, (hid.index() / 16) * 15, 1, 14, COLOR_YELLOW);
    for (int y = 0; y < 15; y++)
    {
        for (int x = 0; x < 16; x++)
        {
            if (x + y * 16 + hid.page() * hid.maxVisibleEntries() < pkm.getLength())
            {
                std::pair<const std::string*, SecurityLevel> description =
                    describe(x + y * 16 + hid.page() * hid.maxVisibleEntries());
                PKSM_Color color = COLOR_WHITE;
                if (level < description.second)
                {
                    color = PKSM_Color(0, 0, 0, 120);
                }
                Gui::text(fmt::format(FMT_STRING("{:02X}"),
                              pkm.rawData()[x + y * 16 + hid.page() * hid.maxVisibleEntries()]),
                    x * 25 + 24 / 2, y * 15 + 1, FONT_SIZE_9, color, TextPosX::CENTER,
                    TextPosY::TOP);
            }
            else
            {
                break;
            }
        }
    }

    Gui::text(*selectedDescription.first, 200, 227, FONT_SIZE_9, COLOR_BLACK, TextPosX::CENTER,
        TextPosY::TOP);
}

void HexEditScreen::drawBottom() const
{
    Gui::backgroundBottom(false);
    Gui::text(fmt::format(FMT_STRING("{:s} 0x{:02X}"), i18n::localize("HEX_SELECTED_BYTE"),
                  hid.fullIndex()),
        160, 8, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);

    Gui::sprite(ui_sheet_emulated_button_selected_blue_idx, 140, 50);
    Gui::text(fmt::format(FMT_STRING("{:01X} {:01X}"), pkm.rawData()[hid.fullIndex()] >> 4,
                  pkm.rawData()[hid.fullIndex()] & 0x0F),
        160, 52, FONT_SIZE_14, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
    if (level >= selectedDescription.second)
    {
        for (const auto& button : buttons[hid.fullIndex()])
        {
            button->draw();
        }
    }
    drawMeaning();
}

void HexEditScreen::update(touchPosition* touch)
{
    u32 down   = hidKeysDown();
    u32 repeat = hidKeysDownRepeat();

    if (down & KEY_B)
    {
        // Reset back to normal repeat time
        hidSetRepeatParameters(10, 10);
        Gui::screenBack();
        return;
    }

    if (down & KEY_TOUCH && level < UNRESTRICTED)
    {
        if (touch->px > 0 && touch->px < 30 && touch->py > 0 && touch->py < 30)
        {
            if (superSecretCornersPressed[0])
            {
                superSecretCornersPressed[4] = true;
            }
            superSecretCornersPressed[0] = true;
            countDownSecretTimer         = true;
            superSecretTimer             = 600;
        }
        else if (touch->px > 290 && touch->px < 320 && touch->py < 30 && touch->py > 0)
        {
            if (superSecretCornersPressed[1])
            {
                superSecretCornersPressed[5] = true;
            }
            superSecretCornersPressed[1] = true;
            countDownSecretTimer         = true;
            superSecretTimer             = 600;
        }
        else if (touch->px > 0 && touch->px < 30 && touch->py > 210 && touch->py < 240)
        {
            if (superSecretCornersPressed[2])
            {
                superSecretCornersPressed[6] = true;
            }
            superSecretCornersPressed[2] = true;
            countDownSecretTimer         = true;
            superSecretTimer             = 600;
        }
        else if (touch->px > 290 && touch->px < 320 && touch->py > 210 && touch->py < 240)
        {
            if (superSecretCornersPressed[3])
            {
                superSecretCornersPressed[7] = true;
            }
            superSecretCornersPressed[3] = true;
            countDownSecretTimer         = true;
            superSecretTimer             = 600;
        }
        if (level == NORMAL)
        {
            if (superSecretCornersPressed[0] && superSecretCornersPressed[1] &&
                superSecretCornersPressed[2] && superSecretCornersPressed[3])
            {
                level            = OPEN;
                superSecretTimer = 0;
            }
        }
        else if (level == OPEN)
        {
            if (superSecretCornersPressed[4] && superSecretCornersPressed[5] &&
                superSecretCornersPressed[6] && superSecretCornersPressed[7])
            {
                level            = UNRESTRICTED;
                superSecretTimer = 0;
            }
        }
    }

    hid.update(pkm.getLength());

    selectedDescription = describe(hid.fullIndex());
    if (level >= selectedDescription.second)
    {
        for (size_t i = 0; i < buttons[hid.fullIndex()].size(); i++)
        {
            buttons[hid.fullIndex()][i]->update(touch);
        }

        if (repeat & KEY_A)
        {
            editNumber(false, true);
        }
        else if (repeat & KEY_X)
        {
            editNumber(false, false);
        }

        // Recolor buttons
        for (auto& button : buttons[hid.fullIndex()])
        {
            if (button->isToggle())
            {
                button->setToggled((this->pkm.rawData()[hid.fullIndex()] >> button->bit()) & 0x1);
            }
            // G6 buttons are toggles, so no need to contemplate them here.
            else if (button->isMark())
            {
                button->setColor((pkm.markValue() >> (button->bit() << 1)) & 0x3);
            }
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

void HexEditScreen::drawMeaning() const
{
    size_t i = hid.fullIndex();
    switch (pkm.generation())
    {
        case pksm::Generation::ONE:
            switch (i)
            {
                case 0x1:
                case 0x3:
                    Gui::text(pkm.species().localize(Configuration::getInstance().language()), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x8:
                    Gui::type(Configuration::getInstance().language(), pkm.type1(), 134, 90);
                    break;
                case 0x9:
                    Gui::type(Configuration::getInstance().language(), pkm.type2(), 134, 90);
                    break;
                case 0xA:
                    Gui::text(i18n::item2(Configuration::getInstance().language(),
                                  static_cast<pksm::PK2&>(pkm).heldItem2()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xB ... 0xE:
                    Gui::text(
                        i18n::move(Configuration::getInstance().language(), pkm.move(i - 0xB)), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xF ... 0x10:
                    Gui::text(fmt::format(i18n::localize("EDITOR_IDS"), pkm.formatTID(),
                                  pkm.formatSID(), pkm.TSV()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
            }
            break;
        case pksm::Generation::TWO:
            switch (i)
            {
                case 0x1:
                case 0x3:
                    Gui::text(pkm.species().localize(Configuration::getInstance().language()), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x4:
                    Gui::text(i18n::item2(Configuration::getInstance().language(),
                                  static_cast<pksm::PK2&>(pkm).heldItem2()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x5 ... 0x8:
                    Gui::text(
                        i18n::move(Configuration::getInstance().language(), pkm.move(i - 0x5)), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x9:
                    Gui::text(fmt::format(i18n::localize("EDITOR_IDS"), pkm.formatTID(),
                                  pkm.formatSID(), pkm.TSV()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x21:
                    Gui::text(i18n::location(Configuration::getInstance().language(),
                                  (pksm::Generation)pkm.version(), pkm.metLocation()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
            }
            break;
        case pksm::Generation::THREE:
            switch (i)
            {
                case 0x4 ... 0x7:
                    Gui::text(fmt::format(i18n::localize("EDITOR_IDS"), pkm.formatTID(),
                                  pkm.formatSID(), pkm.TSV()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x8 ... 0x11:
                    Gui::text(pkm.nickname(), 160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER,
                        TextPosY::TOP);
                    break;
                case 0x12:
                    Gui::text(i18n::language(pkm.language()), 160, 100, FONT_SIZE_12, COLOR_WHITE,
                        TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x20 ... 0x21:
                    Gui::text(pkm.species().localize(Configuration::getInstance().language()), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x22 ... 0x23:
                    Gui::text(i18n::item3(Configuration::getInstance().language(),
                                  static_cast<pksm::PK3&>(pkm).heldItem3()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x28:
                    Gui::text(i18n::localize("MOVE_1") + " - " + std::to_string((int)pkm.PPUp(0)),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    Gui::text(i18n::localize("MOVE_2") + " - " + std::to_string((int)pkm.PPUp(1)),
                        160, 115, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    Gui::text(i18n::localize("MOVE_3") + " - " + std::to_string((int)pkm.PPUp(2)),
                        160, 130, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    Gui::text(i18n::localize("MOVE_4") + " - " + std::to_string((int)pkm.PPUp(3)),
                        160, 145, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x2C ... 0x33:
                    Gui::text(i18n::move(Configuration::getInstance().language(),
                                  pkm.move((i - 0x2C) / 2)),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
            }
            break;
        case pksm::Generation::FOUR:
        case pksm::Generation::FIVE:
            switch (i)
            {
                case 0x8 ... 0x9:
                    Gui::text(pkm.species().localize(Configuration::getInstance().language()), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xA ... 0xB:
                    Gui::text(i18n::item(Configuration::getInstance().language(), pkm.heldItem()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xC ... 0xF:
                    Gui::text(fmt::format(i18n::localize("EDITOR_IDS"), pkm.formatTID(),
                                  pkm.formatSID(), pkm.TSV()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x15:
                    Gui::text(pkm.ability().localize(Configuration::getInstance().language()), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x17:
                    Gui::text(i18n::language(pkm.language()), 160, 100, FONT_SIZE_12, COLOR_WHITE,
                        TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x28 ... 0x2F:
                    Gui::text(i18n::move(Configuration::getInstance().language(),
                                  pkm.move((i - 0x28) / 2)),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x5F:
                    Gui::text(i18n::game(Configuration::getInstance().language(), pkm.version()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x44 ... 0x45:
                    if (pkm.generation() == pksm::Generation::FIVE)
                    {
                        break;
                    }
                    // falls through
                case 0x7E ... 0x7F:
                    Gui::text(i18n::location(Configuration::getInstance().language(),
                                  (pksm::Generation)pkm.version(), pkm.eggLocation()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x46 ... 0x47:
                    if (pkm.generation() == pksm::Generation::FIVE)
                    {
                        break;
                    }
                    // falls through
                case 0x80 ... 0x81:
                    Gui::text(i18n::location(Configuration::getInstance().language(),
                                  (pksm::Generation)pkm.version(), pkm.metLocation()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
            }
            break;
        case pksm::Generation::SIX:
        case pksm::Generation::SEVEN:
        case pksm::Generation::LGPE:
            switch (i)
            {
                case 0x8 ... 0x9:
                    Gui::text(pkm.species().localize(Configuration::getInstance().language()), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xA ... 0xB:
                    Gui::text(i18n::item(Configuration::getInstance().language(), pkm.heldItem()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xC ... 0xF:
                    Gui::text(fmt::format(i18n::localize("EDITOR_IDS"), pkm.formatTID(),
                                  pkm.formatSID(), pkm.TSV()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x14:
                    Gui::text(pkm.ability().localize(Configuration::getInstance().language()), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x5A ... 0x61:
                    Gui::text(i18n::move(Configuration::getInstance().language(),
                                  pkm.move((i - 0x5A) / 2)),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xDF:
                    Gui::text(i18n::game(Configuration::getInstance().language(), pkm.version()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x6A ... 0x71:
                    Gui::text(i18n::move(Configuration::getInstance().language(),
                                  pkm.relearnMove((i - 0x6A) / 2)),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xD8 ... 0xD9:
                    Gui::text(i18n::location(Configuration::getInstance().language(),
                                  (pksm::Generation)pkm.version(), pkm.eggLocation()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xDA ... 0xDB:
                    Gui::text(i18n::location(Configuration::getInstance().language(),
                                  (pksm::Generation)pkm.version(), pkm.metLocation()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xE0:
                {
                    if (pkm.generation() == pksm::Generation::LGPE)
                    {
                        break;
                    }
                    u8 country = 0;
                    switch (pkm.generation())
                    {
                        case pksm::Generation::SIX:
                            country = static_cast<pksm::PK6&>(pkm).country();
                            break;
                        case pksm::Generation::SEVEN:
                            country = static_cast<pksm::PK7&>(pkm).country();
                            break;
                        default:
                            break;
                    }
                    Gui::text(i18n::country(Configuration::getInstance().language(), country), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                }
                break;
                case 0xE1:
                {
                    if (pkm.generation() == pksm::Generation::LGPE)
                    {
                        break;
                    }
                    u8 country = 0;
                    u8 region  = 0;
                    switch (pkm.generation())
                    {
                        case pksm::Generation::SIX:
                            country = static_cast<pksm::PK6&>(pkm).country();
                            region  = static_cast<pksm::PK6&>(pkm).region();
                            break;
                        case pksm::Generation::SEVEN:
                            country = static_cast<pksm::PK7&>(pkm).country();
                            region  = static_cast<pksm::PK7&>(pkm).region();
                            break;
                        default:
                            break;
                    }
                    Gui::text(
                        i18n::subregion(Configuration::getInstance().language(), country, region),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                }
                break;
                case 0xE2:
                {
                    if (pkm.generation() == pksm::Generation::LGPE)
                    {
                        break;
                    }
                    u8 consoleRegion = 0;
                    switch (pkm.generation())
                    {
                        case pksm::Generation::SIX:
                            consoleRegion = static_cast<pksm::PK6&>(pkm).consoleRegion();
                            break;
                        case pksm::Generation::SEVEN:
                            consoleRegion = static_cast<pksm::PK7&>(pkm).consoleRegion();
                            break;
                        default:
                            break;
                    }
                    std::string data;
                    switch (consoleRegion)
                    {
                        case 0:
                            data = "JPN";
                            break;
                        case 1:
                            data = "USA";
                            break;
                        case 2:
                            data = "EUR";
                            break;
                        case 3:
                            data = "AUS";
                            break;
                        case 4:
                            data = "CHN";
                            break;
                        case 5:
                            data = "KOR";
                            break;
                        case 6:
                            data = "TWN";
                            break;
                        default:
                            data = "USA";
                            break;
                    }
                    Gui::text(
                        data, 160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                }
                break;
                case 0xE3:
                    Gui::text(i18n::language(pkm.language()), 160, 100, FONT_SIZE_12, COLOR_WHITE,
                        TextPosX::CENTER, TextPosY::TOP);
                    break;
            }
            break;
        case pksm::Generation::EIGHT:
            switch (i)
            {
                case 0x8 ... 0x9:
                    Gui::text(pkm.species().localize(Configuration::getInstance().language()), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xA ... 0xB:
                    Gui::text(i18n::item(Configuration::getInstance().language(), pkm.heldItem()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xC ... 0xF:
                    Gui::text(fmt::format(i18n::localize("EDITOR_IDS"), pkm.formatTID(),
                                  pkm.formatSID(), pkm.TSV()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x14 ... 0x15:
                    Gui::text(pkm.ability().localize(Configuration::getInstance().language()), 160,
                        100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x72 ... 0x79:
                    Gui::text(i18n::move(Configuration::getInstance().language(),
                                  pkm.move((i - 0x72) / 2)),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x82 ... 0x89:
                    Gui::text(i18n::move(Configuration::getInstance().language(),
                                  pkm.relearnMove((i - 0x82) / 2)),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xDE:
                    Gui::text(i18n::game(Configuration::getInstance().language(), pkm.version()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0xE2:
                    Gui::text(i18n::language(pkm.language()), 160, 100, FONT_SIZE_12, COLOR_WHITE,
                        TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x120 ... 0x121:
                    Gui::text(i18n::location(Configuration::getInstance().language(),
                                  (pksm::Generation)pkm.version(), pkm.eggLocation()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
                case 0x122 ... 0x123:
                    Gui::text(i18n::location(Configuration::getInstance().language(),
                                  (pksm::Generation)pkm.version(), pkm.metLocation()),
                        160, 100, FONT_SIZE_12, COLOR_WHITE, TextPosX::CENTER, TextPosY::TOP);
                    break;
            }
            break;
        case pksm::Generation::UNUSED:
            break;
    }
}

bool HexEditScreen::rotateMark(u8 mark)
{
    int offset = -1;
    if (pkm.generation() == pksm::Generation::SEVEN || pkm.generation() == pksm::Generation::LGPE)
    {
        offset = 0x16;
    }
    else if (pkm.generation() == pksm::Generation::EIGHT)
    {
        offset = 0x18;
    }

    if (offset == -1)
    {
        return false;
    }

    u16 markData = LittleEndian::convertTo<u16>(pkm.rawData().data() + offset);
    switch ((markData >> (mark * 2)) & 0x3)
    {
        case 0:
            markData &= (0xFFFF ^ (0x3 << (mark * 2)));
            markData |= 0x1 << (mark * 2);
            break;
        case 1:
            markData &= (0xFFFF ^ (0x3 << (mark * 2)));
            markData |= 0x2 << (mark * 2);
            break;
        case 2:
        default:
            markData &= (0xFFFF ^ (0x3 << (mark * 2)));
            break;
    }
    LittleEndian::convertFrom<u16>(pkm.rawData().data() + offset, markData);
    return false;
}
