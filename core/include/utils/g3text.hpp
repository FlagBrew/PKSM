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

#ifndef G3TEXT_HPP
#define G3TEXT_HPP

#include <array>

constexpr std::array<char16_t, 247> G3_EN = {u' ', u'À', u'Á', u'Â', u'Ç', u'È', u'É', u'Ê', u'Ë', u'Ì', u'こ', u'Î', u'Ï', u'Ò', u'Ó', u'Ô', u'Œ',
    u'Ù', u'Ú', u'Û', u'Ñ', u'ß', u'à', u'á', u'ね', u'Ç', u'È', u'é', u'ê', u'ë', u'ì', u'í', u'î', u'ï', u'ò', u'ó', u'ô', u'œ', u'ù', u'ú', u'û',
    u'ñ', u'º', u'ª', u'⒅', u'&', u'+', u'あ', u'ぃ', u'ぅ', u'ぇ', u'ぉ', u'ゃ', u'=', u'ょ', u'が', u'ぎ', u'ぐ', u'げ', u'ご', u'ざ', u'じ', u'ず',
    u'ぜ', u'ぞ', u'だ', u'ぢ', u'づ', u'で', u'ど', u'ば', u'び', u'ぶ', u'べ', u'ぼ', u'ぱ', u'ぴ', u'ぷ', u'ぺ', u'ぽ', u'っ', u'¿', u'¡', u'⒆',
    u'⒇', u'オ', u'カ', u'キ', u'ク', u'ケ', u'Í', u'コ', u'サ', u'ス', u'セ', u'ソ', u'タ', u'チ', u'ツ', u'テ', u'ト', u'ナ', u'ニ', u'ヌ', u'â',
    u'ノ', u'ハ', u'ヒ', u'フ', u'ヘ', u'ホ', u'í', u'ミ', u'ム', u'メ', u'モ', u'ヤ', u'ユ', u'ヨ', u'ラ', u'リ', u'ル', u'レ', u'ロ', u'ワ', u'ヲ',
    u'ン', u'ァ', u'ィ', u'ゥ', u'ェ', u'ォ', u'ャ', u'ュ', u'ョ', u'ガ', u'ギ', u'グ', u'ゲ', u'ゴ', u'ザ', u'ジ', u'ズ', u'ゼ', u'ゾ', u'ダ', u'ヂ',
    u'ヅ', u'デ', u'ド', u'バ', u'ビ', u'ブ', u'ベ', u'ボ', u'パ', u'ピ', u'プ', u'ペ', u'ポ', u'ッ', u'0', u'1', u'2', u'3', u'4', u'5', u'6', u'7',
    u'8', u'9', u'!', u'?', u'.', u'-', u'・', u'⑬', u'“', u'”', u'‘', u'’', u'♂', u'♀', u'$', u',', u'⑧', u'/', u'A', u'B', u'C', u'D', u'E', u'F',
    u'G', u'H', u'I', u'J', u'K', u'L', u'M', u'N', u'O', u'P', u'Q', u'R', u'S', u'T', u'U', u'V', u'W', u'X', u'Y', u'Z', u'a', u'b', u'c', u'd',
    u'e', u'f', u'g', u'h', u'i', u'j', u'k', u'l', u'm', u'n', u'o', u'p', u'q', u'r', u's', u't', u'u', u'v', u'w', u'x', u'y', u'z', u'0', u':',
    u'Ä', u'Ö', u'Ü', u'ä', u'ö', u'ü'};

constexpr std::array<char16_t, 247> G3_JP = {u'　', u'あ', u'い', u'う', u'え', u'お', u'か', u'き', u'く', u'け', u'こ', u'さ', u'し', u'す', u'せ',
    u'そ', u'た', u'ち', u'つ', u'て', u'と', u'な', u'に', u'ぬ', u'ね', u'の', u'は', u'ひ', u'ふ', u'へ', u'ほ', u'ま', u'み', u'む', u'め', u'も',
    u'や', u'ゆ', u'よ', u'ら', u'り', u'る', u'れ', u'ろ', u'わ', u'を', u'ん', u'ぁ', u'ぃ', u'ぅ', u'ぇ', u'ぉ', u'ゃ', u'ゅ', u'ょ', u'が', u'ぎ',
    u'ぐ', u'げ', u'ご', u'ざ', u'じ', u'ず', u'ぜ', u'ぞ', u'だ', u'ぢ', u'づ', u'で', u'ど', u'ば', u'び', u'ぶ', u'べ', u'ぼ', u'ぱ', u'ぴ', u'ぷ',
    u'ぺ', u'ぽ', u'っ', u'ア', u'イ', u'ウ', u'エ', u'オ', u'カ', u'キ', u'ク', u'ケ', u'コ', u'サ', u'シ', u'ス', u'セ', u'ソ', u'タ', u'チ', u'ツ',
    u'テ', u'ト', u'ナ', u'ニ', u'ヌ', u'ネ', u'ノ', u'ハ', u'ヒ', u'フ', u'ヘ', u'ホ', u'マ', u'ミ', u'ム', u'メ', u'モ', u'ヤ', u'ユ', u'ヨ', u'ラ',
    u'リ', u'ル', u'レ', u'ロ', u'ワ', u'ヲ', u'ン', u'ァ', u'ィ', u'ゥ', u'ェ', u'ォ', u'ャ', u'ュ', u'ョ', u'ガ', u'ギ', u'グ', u'ゲ', u'ゴ', u'ザ',
    u'ジ', u'ズ', u'ゼ', u'ゾ', u'ダ', u'ヂ', u'ヅ', u'デ', u'ド', u'バ', u'ビ', u'ブ', u'ベ', u'ボ', u'パ', u'ピ', u'プ', u'ペ', u'ポ', u'ッ', u'０',
    u'１', u'２', u'３', u'４', u'５', u'６', u'７', u'８', u'９', u'！', u'？', u'。', u'ー', u'・', u'⋯', u'『', u'』', u'「', u'」', u'♂', u'♀',
    u'$', u'.', u'⑧', u'/', u'Ａ', u'Ｂ', u'Ｃ', u'Ｄ', u'Ｅ', u'Ｆ', u'Ｇ', u'Ｈ', u'Ｉ', u'Ｊ', u'Ｋ', u'Ｌ', u'Ｍ', u'Ｎ', u'Ｏ', u'Ｐ', u'Ｑ',
    u'Ｒ', u'Ｓ', u'Ｔ', u'Ｕ', u'Ｖ', u'Ｗ', u'Ｘ', u'Ｙ', u'Ｚ', u'ａ', u'ｂ', u'ｃ', u'ｄ', u'ｅ', u'ｆ', u'ｇ', u'ｈ', u'ｉ', u'ｊ', u'ｋ', u'ｌ',
    u'ｍ', u'ｎ', u'ｏ', u'ｐ', u'ｑ', u'ｒ', u'ｓ', u'ｔ', u'ｕ', u'ｖ', u'ｗ', u'ｘ', u'ｙ', u'ｚ', u'0', u':', u'Ä', u'Ö', u'Ü', u'ä', u'ö', u'ü'};

#endif
