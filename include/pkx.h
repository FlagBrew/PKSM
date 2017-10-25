/*  This file is part of PKSM
>	Copyright (C) 2016/2017 Bernardo Giordano
>
>   This program is free software: you can redistribute it and/or modify
>   it under the terms of the GNU General Public License as published by
>   the Free Software Foundation, either version 3 of the License, or
>   (at your option) any later version.
>
>   This program is distributed in the hope that it will be useful,
>   but WITHOUT ANY WARRANTY; without even the implied warranty of
>   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>   GNU General Public License for more details.
>
>   You should have received a copy of the GNU General Public License
>   along with this program.  If not, see <http://www.gnu.org/licenses/>.
>   See LICENSE for information.
*/

#ifndef PKX_H
#define PKX_H

#include "common.h"

typedef struct FormData {
	int spriteNum;
	int stringNum;
	int min;
	int max;
	bool editable;
} FormData;

struct {
	u8 pkmData[961][15];
} personal;

FormData *pkx_get_legal_form_data(const u16 species, const int game);

void pkx_set(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn);
void pkx_set_as_it_is(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn);
void pkx_get(u8* mainbuf, const int boxnumber, const int indexnumber, u8* pkmn);

u32 pkx_lcrng(u32 seed);
u32 pkx_get_save_address(const int boxnumber, const int indexnumber);

u8 pkx_get_HT(u8* pkmn);
u8 pkx_get_gender(u8* pkmn);
u8 pkx_get_language(u8* pkmn);
u8 pkx_get_ability(u8* pkmn);
u8 pkx_get_ability_number(u8* pkmn);
u8 pkx_get_form(u8* pkmn);
u16 pkx_get_item(u8* pkmn);
u8 pkx_get_hp_type(u8* pkmn);
u8 pkx_get_ot_gender(u8* pkmn);
bool pkx_is_egg(u8* pkmn);
void pkx_reroll_encryption_key(u8* pkmn);

void pkx_reroll_pid(u8* pkmn);
u32 pkx_get_pid(u8* pkmn);

bool pkx_get_nickname_flag(u8* pkmn);
u16 pkx_get_egg_move(u8 *pkmn, const int nmove);
bool pkx_get_pokerus(u8* pkmn);
u16 pkx_get_move(u8* pkmn, const int nmove);
u32 *pkx_get_ot(u8* pkmn, u32* dst);
u32 *pkx_get_nickname(u8* pkmn, u32* dst);
u8 *pkx_get_nickname_u8(u8* pkmn, u8* dst);
u32 *pkx_get_ht(u8* pkmn, u32* dst);
u16 pkx_get_tid(u8* pkmn);
u16 pkx_get_sid(u8* pkmn);
u16 pkx_get_tsv(u8* pkmn);
u16 pkx_get_psv(u8* pkmn);
u16 pkx_get_species(u8* pkmn);
u8 pkx_get_level(u8* pkmn);
void pkx_set_level(u8* pkmn, const int lv);

bool pkx_is_shiny(u8* pkmn);
u16 pkx_get_stat(u8* pkmn, const int stat);
u16 pkx_get_form_species_number(u8 *pkmn);
u8 pkx_get_friendship(u8* pkmn);
u8 pkx_get_ht_friendship(u8* pkmn);
u8 pkx_get_ot_friendship(u8* pkmn);
u8 pkx_get_nature(u8* pkmn);
u8 pkx_get_ev(u8* pkmn, const int stat);
u8 pkx_get_iv(u8* pkmn, const int stat);
u8 pkx_get_ball(u8* pkmn);
u8 pkx_get_version(u8* pkmn);

void pkx_set_item(u8* pkmn, const u16 item);
void pkx_set_gender(u8* pkmn, const u8 val);
void pkx_set_form(u8* pkmn, const u8 val);
void pkx_set_ball(u8* pkmn, const u8 val);
void pkx_set_ot_gender(u8* pkmn, const u8 val);

void pkx_set_tid(u8* pkmn, const u16 tid);
void pkx_set_sid(u8* pkmn, const u16 sid);

void pkx_set_ability(u8* pkmn, const u8 ability);
void pkx_set_move(u8* pkmn, const u16 move, const int nmove);
void pkx_set_egg_move(u8* pkmn, const u16 move, const int nmove);
void pkx_set_ht(u8* pkmn, char* nick);
void pkx_set_ht_gender(u8* pkmn, const u8 gender);
void pkx_set_nature(u8* pkmn, const u8 nature);
void pkx_set_friendship(u8* pkmn, const int val);
void pkx_set_ot_friendship(u8* pkmn, const int val);
void pkx_set_ev(u8* pkmn, u8 val, const int stat);
void pkx_set_iv(u8* pkmn, u8 val, const int stat);
void pkx_set_hp_type(u8* pkmn, const int val);
void pkx_set_shiny(u8* pkmn, const bool shiny);
void pkx_set_nickname(u8* pkmn, char* nick, const int dst);
void pkx_set_ribbons(u8* pkmn, const int ribcat, const int ribnumber, const bool value);
bool pkx_get_ribbons(u8* pkmn, const int ribcat, const int ribnumber);
void pkx_set_hti(u8* pkmn, const int htnumber, const bool value);
bool pkx_get_hti(u8* pkmn, const int htnumber);
void pkx_set_flag(u8* pkmn, const int flgaddr, const int flgshift, const bool value);
void pkx_set_pokerus(u8* pkmn);
void pkx_set_nickname_flag(u8* pkmn);

#endif