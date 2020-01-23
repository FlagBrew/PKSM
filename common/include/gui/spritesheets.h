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

#ifndef SPRITESHEETS_H
#define SPRITESHEETS_H

// These must exist for all valid targets
#include "pkm_spritesheet.h"
#include "types_spritesheet.h"
#include "ui_sheet.h"

#if defined(_3DS)
// emulated sprites
#define ui_sheet_res_null_idx 500
#define ui_sheet_emulated_pointer_horizontal_flipped_idx 501
#define ui_sheet_emulated_bg_top_red 502
#define ui_sheet_emulated_bg_top_blue 503
#define ui_sheet_emulated_bg_top_green 504
#define ui_sheet_emulated_bg_bottom_red 505
#define ui_sheet_emulated_bg_bottom_blue 506
#define ui_sheet_emulated_bg_bottom_green 507
#define ui_sheet_emulated_eventmenu_bar_selected_flipped_horizontal_idx 508
#define ui_sheet_emulated_eventmenu_bar_unselected_flipped_horizontal_idx 509
#define ui_sheet_emulated_eventmenu_bar_selected_flipped_vertical_idx 510
#define ui_sheet_emulated_eventmenu_bar_unselected_flipped_vertical_idx 511
#define ui_sheet_emulated_eventmenu_bar_selected_flipped_both_idx 512
#define ui_sheet_emulated_eventmenu_bar_unselected_flipped_both_idx 513
#define ui_sheet_emulated_storage_box_corner_flipped_horizontal_idx 514
#define ui_sheet_emulated_storage_box_corner_flipped_vertical_idx 515
#define ui_sheet_emulated_storage_box_corner_flipped_both_idx 516
#define ui_sheet_emulated_toggle_green_idx 517
#define ui_sheet_emulated_toggle_red_idx 518
#define ui_sheet_emulated_gameselector_bg_idx 519
#define ui_sheet_emulated_button_qr_idx 520
#define ui_sheet_emulated_button_item_idx 521
#define ui_sheet_emulated_button_plus_small_black_idx 522
#define ui_sheet_emulated_button_minus_small_black_idx 523
#define ui_sheet_emulated_box_search_idx 524
#define ui_sheet_emulated_toggle_gray_idx 525
#define ui_sheet_emulated_toggle_blue_idx 526
#define ui_sheet_emulated_party_indicator_1_idx 527
#define ui_sheet_emulated_party_indicator_2_idx 528
#define ui_sheet_emulated_party_indicator_3_idx 529
#define ui_sheet_emulated_party_indicator_4_idx 530
#define ui_sheet_emulated_party_indicator_5_idx 531
#define ui_sheet_emulated_party_indicator_6_idx 532
#define ui_sheet_emulated_button_selected_blue_idx 533
#define ui_sheet_emulated_button_unselected_blue_idx 534
#define ui_sheet_emulated_button_unavailable_blue_idx 535
#define ui_sheet_emulated_button_selected_red_idx 536
#define ui_sheet_emulated_button_unselected_red_idx 537
#define ui_sheet_emulated_button_unavailable_red_idx 538
#define ui_sheet_emulated_button_pouch_idx 539
#define ui_sheet_emulated_textbox_illegal_idx 540
#define ui_sheet_emulated_bg_top_yellow_idx 541
#define ui_sheet_emulated_bg_bottom_yellow_idx 542
#define ui_sheet_emulated_button_lang_enabled_idx 543
#define ui_sheet_emulated_button_lang_disabled_idx 544
#define ui_sheet_emulated_stripe_move_grey_idx 545
// These are both about 18x18
#define ui_sheet_emulated_button_filter_positive_idx 546
#define ui_sheet_emulated_button_filter_negative_idx 547
#define ui_sheet_emulated_button_tab_unselected_idx 548

#elif defined(__SWITCH__)
// No emulated sprites because no sprites yet :P
#endif

#endif
