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

#include "SavPT.hpp"
#include "PGT.hpp"

SavPT::SavPT(std::shared_ptr<u8[]> dt)
{
    length = 0x80000;
    boxes  = 18;
    game   = Game::Pt;
    data   = dt;

    GBOOffset = 0xCF1C;
    SBOOffset = 0x1F100;
    GBO();
    SBO();

    Trainer1        = 0x68 + gbo;
    Party           = 0xA0 + gbo;
    PokeDex         = 0x1328 + gbo;
    WondercardFlags = 0xB4C0 + gbo;
    WondercardData  = 0xB5C0 + gbo;
    PouchHeldItem   = 0x630 + gbo;
    PouchKeyItem    = 0x8C4 + gbo;
    PouchTMHM       = 0x98C + gbo;
    MailItems       = 0xB1C + gbo;
    PouchMedicine   = 0xB4C + gbo;
    PouchBerry      = 0xBEC + gbo;
    PouchBalls      = 0xCEC + gbo;
    BattleItems     = 0xD28 + gbo;
    Box             = 0xCF30 + sbo;
}

std::map<Sav::Pouch, std::vector<int>> SavPT::validItems() const
{
    return {{NormalItem,
                {68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100,
                    101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 135, 136, 213, 214, 215, 216, 217, 218, 219, 220, 221, 222, 223, 224,
                    225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 235, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250,
                    251, 252, 253, 254, 255, 256, 257, 258, 259, 260, 261, 262, 263, 264, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276,
                    277, 278, 279, 280, 281, 282, 283, 284, 285, 286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302,
                    303, 304, 305, 306, 307, 308, 309, 310, 311, 312, 313, 314, 315, 316, 317, 318, 319, 320, 321, 322, 323, 324, 325, 326, 327}},
        {KeyItem, {428, 429, 430, 431, 432, 433, 434, 435, 436, 437, 438, 439, 440, 441, 442, 443, 444, 445, 446, 447, 448, 449, 450, 451, 452, 453,
                      454, 455, 456, 457, 458, 459, 460, 461, 462, 463, 464, 465, 466, 467}},
        {TM, {328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354,
                 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380,
                 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 400, 401, 402, 403, 404, 405, 406,
                 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 420, 421, 422, 423, 424, 425, 426, 427}},
        {Mail, {137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148}},
        {Medicine, {17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                       49, 50, 51, 52, 53, 54}},
        {Berry, {149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174,
                    175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 200,
                    201, 202, 203, 204, 205, 206, 207, 208, 209, 210, 211, 212}},
        {Ball, {1, 2, 3, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}}, {Battle, {55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67}}};
}
