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

#include "SavSWSH.hpp"
#include "PK8.hpp"
#include "WC8.hpp"
#include "endian.hpp"
#include "i18n.hpp"
#include "utils.hpp"

namespace
{
    struct DexEntry
    {
        u64 seenNonShinyMale : 63;
        u64 seenNonShinyMaleGiga : 1;
        u64 seenNonShinyFemale : 63;
        u64 seenNonShinyFemaleGiga : 1;
        u64 seenShinyMale : 63;
        u64 seenShinyMaleGiga : 1;
        u64 seenShinyFemale : 63;
        u64 seenShinyFemaleGiga : 1;
        // Obtained info
        u32 owned : 1;
        u32 ownedGiga : 1;
        u32 languages : 13;     // flags
        u32 displayFormID : 13; // value
        u32 displayGiga : 1;
        u32 displayGender : 2;
        u32 displayShiny : 1;

        u32 battled : 32;
        u32 unk1 : 32;
        u32 unk2 : 32;
    };

    void setProperLocation(DexEntry& entry, u16 form, bool shiny, u8 gender)
    {
        if (gender == 1)
        {
            if (shiny)
            {
                entry.seenShinyFemale |= 1 << form;
            }
            else
            {
                entry.seenNonShinyFemale |= 1 << form;
            }
        }
        else
        {
            if (shiny)
            {
                entry.seenShinyMale |= 1 << form;
            }
            else
            {
                entry.seenNonShinyMale |= 1 << form;
            }
        }
    }

    void setProperGiga(DexEntry& entry, bool giga, bool shiny, u8 gender)
    {
        if (gender == 1)
        {
            if (shiny)
            {
                entry.seenShinyFemaleGiga = giga ? 1 : 0;
            }
            else
            {
                entry.seenNonShinyFemaleGiga = giga ? 1 : 0;
            }
        }
        else
        {
            if (shiny)
            {
                entry.seenShinyMaleGiga = giga ? 1 : 0;
            }
            else
            {
                entry.seenNonShinyMaleGiga = giga ? 1 : 0;
            }
        }
    }
}

template <>
DexEntry Endian::convertTo<DexEntry>(const u8* data)
{
    DexEntry ret;
    u64 v64                  = Endian::convertTo<u64>(data);
    ret.seenNonShinyMale     = v64;
    ret.seenNonShinyMaleGiga = v64 >> 63;

    v64                        = Endian::convertTo<u64>(data + 8);
    ret.seenNonShinyFemale     = v64;
    ret.seenNonShinyFemaleGiga = v64 >> 63;

    v64                   = Endian::convertTo<u64>(data + 16);
    ret.seenShinyMale     = v64;
    ret.seenShinyMaleGiga = v64 >> 63;

    v64                     = Endian::convertTo<u64>(data + 24);
    ret.seenShinyFemale     = v64;
    ret.seenShinyFemaleGiga = v64 >> 63;

    u32 v32           = Endian::convertTo<u32>(data + 32);
    ret.owned         = v32 & 1;
    ret.ownedGiga     = (v32 >> 1) & 1;
    ret.languages     = (v32 >> 2) & 0x1FFF;
    ret.displayFormID = (v32 >> 15) & 0x1FFF;
    ret.displayGiga   = (v32 >> 28) & 1;
    ret.displayGender = (v32 >> 29) & 3;
    ret.displayShiny  = v32 >> 31;

    ret.battled = Endian::convertTo<u32>(data + 36);
    ret.unk1    = Endian::convertTo<u32>(data + 40);
    ret.unk2    = Endian::convertTo<u32>(data + 44);

    return ret;
}

template <>
void Endian::convertFrom<DexEntry>(u8* data, const DexEntry& entry)
{
    u64 w64 = entry.seenNonShinyMale | (u64(entry.seenNonShinyMaleGiga) << 63);
    Endian::convertFrom<u64>(data, w64);

    w64 = entry.seenNonShinyFemale | (u64(entry.seenNonShinyFemaleGiga) << 63);
    Endian::convertFrom<u64>(data + 8, w64);

    w64 = entry.seenShinyMale | (u64(entry.seenShinyMaleGiga) << 63);
    Endian::convertFrom<u64>(data + 16, w64);

    w64 = entry.seenShinyFemale | (u64(entry.seenShinyFemaleGiga) << 63);
    Endian::convertFrom<u64>(data + 24, w64);

    u32 w32 = entry.owned | (entry.ownedGiga << 1) | (entry.languages << 2) | (entry.displayFormID << 15) | (entry.displayGiga << 28) |
              (entry.displayGender << 29) | (entry.displayShiny << 31);
    Endian::convertFrom<u32>(data + 32, w32);

    Endian::convertFrom<u32>(data + 36, entry.battled);
    Endian::convertFrom<u32>(data + 40, entry.unk1);
    Endian::convertFrom<u32>(data + 44, entry.unk2);
}

SavSWSH::SavSWSH(std::shared_ptr<u8[]> dt) : Sav8(dt, 0x17195E)
{
    game = Game::SWSH;

    Box            = 0x0d66012c;
    WondercardData = 0x112d5141;
    Party          = 0x2985fe5d;
    PokeDex        = 0x4716c404;
    Items          = 0x1177c2c4;
    BoxLayout      = 0x19722c89;
    Misc           = 0x1b882b09;
    TrainerCard    = 0x874da6fa;
    PlayTime       = 0x8cbbfd90;
    Status         = 0xf25c070e;
}

u16 SavSWSH::TID(void) const
{
    return Endian::convertTo<u16>(getBlock(Status)->decryptedData() + 0xA0);
}
void SavSWSH::TID(u16 v)
{
    Endian::convertFrom<u16>(getBlock(Status)->decryptedData() + 0xA0, v);
    Endian::convertFrom<u32>(getBlock(TrainerCard)->decryptedData() + 0x1C, displayTID());
}

u16 SavSWSH::SID(void) const
{
    return Endian::convertTo<u16>(getBlock(Status)->decryptedData() + 0xA2);
}
void SavSWSH::SID(u16 v)
{
    Endian::convertFrom<u16>(getBlock(Status)->decryptedData() + 0xA2, v);
    Endian::convertFrom<u32>(getBlock(TrainerCard)->decryptedData() + 0x1C, displayTID());
}

u8 SavSWSH::version(void) const
{
    return getBlock(Status)->decryptedData()[0xA4];
}
void SavSWSH::version(u8 v)
{
    getBlock(Status)->decryptedData()[0xA4] = v;
}

u8 SavSWSH::gender(void) const
{
    return getBlock(Status)->decryptedData()[0xA5];
}
void SavSWSH::gender(u8 v)
{
    getBlock(Status)->decryptedData()[0xA5] = v;
}

Language SavSWSH::language(void) const
{
    return Language(getBlock(Status)->decryptedData()[0xA7]);
}
void SavSWSH::language(Language v)
{
    getBlock(Status)->decryptedData()[0xA7] = u8(v);
}

std::string SavSWSH::otName(void) const
{
    return StringUtils::getString(getBlock(Status)->decryptedData(), 0xB0, 13);
}
void SavSWSH::otName(const std::string& v)
{
    StringUtils::setString(getBlock(Status)->decryptedData(), v, 0xB0, 13);
    StringUtils::setString(getBlock(TrainerCard)->decryptedData(), v, 0, 13);
}

std::string SavSWSH::jerseyNum(void) const
{
    return std::string((char*)getBlock(TrainerCard)->decryptedData() + 0x39, 3);
}
void SavSWSH::jerseyNum(const std::string& v)
{
    for (size_t i = 0; i < std::min(v.size(), (size_t)3); i++)
    {
        getBlock(TrainerCard)->decryptedData()[0x39 + i] = v[i];
    }
}

u32 SavSWSH::money(void) const
{
    return Endian::convertTo<u32>(getBlock(Misc)->decryptedData());
}
void SavSWSH::money(u32 v)
{
    Endian::convertFrom<u32>(getBlock(Misc)->decryptedData(), v);
}

u32 SavSWSH::BP(void) const
{
    return Endian::convertTo<u32>(getBlock(Misc)->decryptedData() + 4);
}
void SavSWSH::BP(u32 v)
{
    Endian::convertFrom<u32>(getBlock(Misc)->decryptedData() + 4, v);
}

u8 SavSWSH::badges(void) const
{
    return getBlock(Misc)->decryptedData()[0x11C];
}

u16 SavSWSH::playedHours(void) const
{
    return Endian::convertTo<u16>(getBlock(PlayTime)->decryptedData());
}
void SavSWSH::playedHours(u16 v)
{
    Endian::convertFrom<u16>(getBlock(PlayTime)->decryptedData(), v);
}

u8 SavSWSH::playedMinutes(void) const
{
    return getBlock(PlayTime)->decryptedData()[2];
}
void SavSWSH::playedMinutes(u8 v)
{
    getBlock(PlayTime)->decryptedData()[2] = v;
}

u8 SavSWSH::playedSeconds(void) const
{
    return getBlock(PlayTime)->decryptedData()[3];
}
void SavSWSH::playedSeconds(u8 v)
{
    getBlock(PlayTime)->decryptedData()[3] = v;
}

void SavSWSH::item(const Item& item, Pouch pouch, u16 slot)
{
    auto bytes = item.bytes();
    switch (pouch)
    {
        case Medicine:
            std::copy(bytes.first, bytes.first + bytes.second, getBlock(Items)->decryptedData() + 4 * slot);
            break;
        case Ball:
            std::copy(bytes.first, bytes.first + bytes.second, getBlock(Items)->decryptedData() + 0xF0 + 4 * slot);
            break;
        case Battle:
            std::copy(bytes.first, bytes.first + bytes.second, getBlock(Items)->decryptedData() + 0x168 + 4 * slot);
            break;
        case Berry:
            std::copy(bytes.first, bytes.first + bytes.second, getBlock(Items)->decryptedData() + 0x1B8 + 4 * slot);
            break;
        case NormalItem:
            std::copy(bytes.first, bytes.first + bytes.second, getBlock(Items)->decryptedData() + 0x2F8 + 4 * slot);
            break;
        case TM:
            std::copy(bytes.first, bytes.first + bytes.second, getBlock(Items)->decryptedData() + 0xB90 + 4 * slot);
            break;
        case Treasure:
            std::copy(bytes.first, bytes.first + bytes.second, getBlock(Items)->decryptedData() + 0xED8 + 4 * slot);
            break;
        case Ingredient:
            std::copy(bytes.first, bytes.first + bytes.second, getBlock(Items)->decryptedData() + 0x1068 + 4 * slot);
            break;
        case KeyItem:
            std::copy(bytes.first, bytes.first + bytes.second, getBlock(Items)->decryptedData() + 0x11F8 + 4 * slot);
            break;
        default:
            break;
    }
}
std::unique_ptr<Item> SavSWSH::item(Pouch pouch, u16 slot) const
{
    switch (pouch)
    {
        case Medicine:
            return std::make_unique<Item8>(getBlock(Items)->decryptedData() + 4 * slot);
        case Ball:
            return std::make_unique<Item8>(getBlock(Items)->decryptedData() + 0xF0 + 4 * slot);
        case Battle:
            return std::make_unique<Item8>(getBlock(Items)->decryptedData() + 0x168 + 4 * slot);
        case Berry:
            return std::make_unique<Item8>(getBlock(Items)->decryptedData() + 0x1B8 + 4 * slot);
        case NormalItem:
            return std::make_unique<Item8>(getBlock(Items)->decryptedData() + 0x2F8 + 4 * slot);
        case TM:
            return std::make_unique<Item8>(getBlock(Items)->decryptedData() + 0xB90 + 4 * slot);
        case Treasure:
            return std::make_unique<Item8>(getBlock(Items)->decryptedData() + 0xED8 + 4 * slot);
        case Ingredient:
            return std::make_unique<Item8>(getBlock(Items)->decryptedData() + 0x1068 + 4 * slot);
        case KeyItem:
            return std::make_unique<Item8>(getBlock(Items)->decryptedData() + 0x11F8 + 4 * slot);
        default:
            return std::make_unique<Item8>();
    }
}
std::vector<std::pair<Sav::Pouch, int>> SavSWSH::pouches(void) const
{
    return {{Medicine, 60}, {Ball, 30}, {Battle, 20}, {Berry, 80}, {NormalItem, 550}, {TM, 210}, {Treasure, 100}, {Ingredient, 100}, {KeyItem, 64}};
}
std::map<Sav::Pouch, std::vector<int>> SavSWSH::validItems(void) const
{
    return {{Medicine, {17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 54, 134, 54, 591,
                           78, 79, 852, 93}},
        {Ball, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 492, 493, 494, 495, 496, 497, 498, 499, 50, 576, 851}},
        {Battle, {55, 56, 57, 58, 59, 60, 61, 62, 63}},
        {Berry, {149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 169, 170, 171, 172, 173, 174, 184, 185, 186, 187, 188,
                    189, 190, 191, 192, 193, 194, 195, 196, 197, 198, 199, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 210, 211, 212, 686, 687, 688}},
        {NormalItem,
            {45, 46, 47, 48, 49, 50, 51, 52, 53, 76, 77, 79, 80, 81, 82, 83, 84, 85, 107, 108, 109, 116, 117, 118, 119, 213, 214, 215, 217, 218, 219,
                220, 221, 222, 223, 224, 228, 229, 230, 231, 232, 233, 234, 236, 237, 238, 239, 240, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250,
                251, 253, 254, 255, 257, 259, 265, 266, 267, 268, 269, 270, 271, 272, 273, 274, 275, 276, 277, 278, 279, 280, 281, 282, 283, 284, 285,
                286, 287, 288, 289, 290, 291, 292, 293, 294, 295, 296, 297, 298, 299, 300, 301, 302, 303, 304, 305, 306, 307, 308, 309, 310, 311, 312,
                313, 314, 315, 316, 317, 318, 319, 320, 321, 325, 326, 537, 538, 539, 540, 541, 542, 543, 544, 545, 546, 547, 564, 565, 566, 567, 568,
                569, 570, 639, 640, 644, 645, 646, 647, 648, 649, 650, 846, 849, 879, 880, 881, 882, 883, 884, 904, 905, 906, 907, 908, 909, 910, 911,
                912, 913, 914, 915, 916, 917, 918, 919, 920, 1103, 1104, 1109, 1110, 1111, 1112, 1113, 1114, 1115, 1116, 1117, 1118, 1119, 1120, 1121,
                1122, 1123, 1124, 1125, 1126, 1127, 1128, 1129, 1231, 1232, 1233, 1234, 1235, 1236, 1237, 1238, 1239, 1240, 1241, 1242, 1243, 1244,
                1245, 1246, 1247, 1248, 1249, 1250, 1251, 1252, 1253, 1254, 1279, 1280, 1281, 1282, 1283, 1284, 1285, 1286, 1287, 1288, 1289, 1290,
                1291, 1292, 1293, 1294, 1295, 1296, 1297, 1298, 1299, 1300, 1301, 1302, 1303, 1304, 1305, 1306, 1307, 1308, 1309, 1310, 1311, 1312,
                1313, 1314, 1315, 1316, 1317, 1318, 1319, 1320, 1321, 1322, 1323, 1324, 1325, 1326, 1327, 1328, 1329, 1330, 1331, 1332, 1333, 1334,
                1335, 1336, 1337, 1338, 1339, 1340, 1341, 1342, 1343, 1344, 1345, 1346, 1347, 1348, 1349, 1350, 1351, 1352, 1353, 1354, 1355, 1356,
                1357, 1358, 1359, 1360, 1361, 1362, 1363, 1364, 1365, 1366, 1367, 1368, 1369, 1370, 1371, 1372, 1373, 1374, 1375, 1376, 1377, 1378,
                1379, 1380, 1381, 1382, 1383, 1384, 1385, 1386, 1387, 1388, 1389, 1390, 1391, 1392, 1393, 1394, 1395, 1396, 1397, 1398, 1399, 1400,
                1401, 1402, 1403, 1404, 1405, 1406, 1407, 1408, 1409, 1410, 1411, 1412, 1413, 1414, 1415, 1416, 1417, 1418, 1419, 1420, 1421, 1422,
                1423, 1424, 1425, 1426, 1427, 1428, 1429, 1430, 1431, 1432, 1433, 1434, 1435, 1436, 1437, 1438, 1439, 1440, 1441, 1442, 1443, 1444,
                1445, 1446, 1447, 1448, 1449, 1450, 1451, 1452, 1453, 1454, 1455, 1456, 1457, 1458, 1459, 1460, 1461, 1462, 1463, 1464, 1465, 1466,
                1467, 1468, 1469, 1470, 1471, 1472, 1473, 1474, 1475, 1476, 1477, 1478, 1479, 1480, 1481, 1482, 1483, 1484, 1485, 1486, 1487, 1488,
                1489, 1490, 1491, 1492, 1493, 1494, 1495, 1496, 1497, 1498, 1499, 1500, 1501, 1502, 1503, 1504, 1505, 1506, 1507, 1508, 1509, 1510,
                1511, 1512, 1513, 1514, 1515, 1516, 1517, 1518, 1519, 1520, 1521, 1522, 1523, 1524, 1525, 1526, 1527, 1528, 1529, 1530, 1531, 1532,
                1533, 1534, 1535, 1536, 1537, 1538, 1539, 1540, 1541, 1542, 1543, 1544, 1545, 1546, 1547, 1548, 1549, 1550, 1551, 1552, 1553, 1554,
                1555, 1556, 1557, 1558, 1559, 1560, 1561, 1562, 1563, 1564, 1565, 1566, 1567, 1568, 1569, 1570, 1571, 1572, 1573, 1574, 1575, 1576,
                1577, 1578}},
        {TM, {328, 329, 330, 331, 332, 333, 334, 335, 336, 337, 338, 339, 340, 341, 342, 343, 344, 345, 346, 347, 348, 349, 350, 351, 352, 353, 354,
                 355, 356, 357, 358, 359, 360, 361, 362, 363, 364, 365, 366, 367, 368, 369, 370, 371, 372, 373, 374, 375, 376, 377, 378, 379, 380,
                 381, 382, 383, 384, 385, 386, 387, 388, 389, 390, 391, 392, 393, 394, 395, 396, 397, 398, 399, 40, 41, 42, 43, 44, 45, 46, 47, 48,
                 49, 410, 411, 412, 413, 414, 415, 416, 417, 418, 419, 618, 619, 620, 690, 691, 692, 693, 1230, 1130, 1131, 1132, 1133, 1134, 1135,
                 1136, 1137, 1138, 1139, 1140, 1141, 1142, 1143, 1144, 1145, 1146, 1147, 1148, 1149, 1150, 1151, 1152, 1153, 1154, 1155, 1156, 1157,
                 1158, 1159, 1160, 1161, 1162, 1163, 1164, 1165, 1166, 1167, 1168, 1169, 1170, 1171, 1172, 1173, 1174, 1175, 1176, 1177, 1178, 1179,
                 1180, 1181, 1182, 1183, 1184, 1185, 1186, 1187, 1188, 1189, 1190, 1191, 1192, 1193, 1194, 1195, 1196, 1197, 1198, 1199, 1200, 1201,
                 1202, 1203, 1204, 1205, 1206, 1207, 1208, 1209, 1210, 1211, 1212, 1213, 1214, 1215, 1216, 1217, 1218, 1219, 1220, 1221, 1222, 1223,
                 1224, 1225, 1226, 1227, 1228, 1229}},
        {Treasure, {86, 87, 88, 89, 90, 91, 92, 94, 16, 571, 580, 581, 582, 583, 795, 796, 115, 116, 117, 118}},
        {Ingredient, {1084, 1085, 1086, 1087, 1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1256, 1257, 1258, 1259, 1260,
                         1261, 1262, 1263, 1264}},
        {KeyItem, {78, 628, 629, 631, 632, 73, 943, 944, 945, 946, 174, 175, 176, 177, 180, 181, 110, 1255, 1266, 1267, 1269, 1270, 1271, 1278}}};
}
std::string SavSWSH::pouchName(Language lang, Pouch pouch) const
{
    switch (pouch)
    {
        case Medicine:
            return i18n::localize(lang, "MEDICINE");
        case Ball:
            return i18n::localize(lang, "BALLS");
        case Battle:
            return i18n::localize(lang, "BATTLE_ITEMS");
        case Berry:
            return i18n::localize(lang, "BERRIES");
        case NormalItem:
            return i18n::localize(lang, "ITEMS");
        case TM:
            return i18n::localize(lang, "TMHM");
        case Treasure:
            return i18n::localize(lang, "TREASURES");
        case Ingredient:
            return i18n::localize(lang, "INGREDIENTS");
        case KeyItem:
            return i18n::localize(lang, "KEY_ITEMS");
        default:
            return "";
    }
}

std::string SavSWSH::boxName(u8 box) const
{
    return StringUtils::getString(getBlock(BoxLayout)->decryptedData(), box * 0x22, 17);
}
void SavSWSH::boxName(u8 box, const std::string& name)
{
    StringUtils::setString(getBlock(BoxLayout)->decryptedData(), name, box * 0x22, 17);
}

u32 SavSWSH::boxOffset(u8 box, u8 slot) const
{
    return 0x158 * slot + 0x158 * 30 * box;
}
u32 SavSWSH::partyOffset(u8 slot) const
{
    return 0x158 * slot;
}

u8 SavSWSH::partyCount(void) const
{
    return getBlock(Party)->decryptedData()[0x158 * 6];
}
void SavSWSH::partyCount(u8 count)
{
    getBlock(Party)->decryptedData()[0x158 * 6] = count;
}

std::shared_ptr<PKX> SavSWSH::pkm(u8 slot) const
{
    u32 offset = partyOffset(slot);
    return std::make_shared<PK8>(getBlock(Party)->decryptedData() + offset, true, true);
}
std::shared_ptr<PKX> SavSWSH::pkm(u8 box, u8 slot, bool ekx) const
{
    u32 offset = boxOffset(box, slot);
    return std::make_shared<PK8>(getBlock(Box)->decryptedData() + offset, ekx, true);
}

void SavSWSH::pkm(std::shared_ptr<PKX> pk, u8 box, u8 slot, bool applyTrade)
{
    if (pk->generation() == Generation::EIGHT)
    {
        if (applyTrade)
        {
            trade(pk);
        }

        u8 buf[0x158] = {0};
        std::copy(pk->rawData(), pk->rawData() + pk->getLength(), buf);
        std::unique_ptr<PK8> pk8 = std::make_unique<PK8>(buf, false, true, true);

        if (pk->getLength() != 0x158)
        {
            for (int i = 0; i < 6; i++)
            {
                pk8->partyStat(Stat(i), pk8->stat(Stat(i)));
            }
            pk8->partyLevel(pk8->level());
            pk8->partyCurrHP(pk8->stat(Stat::HP));
        }

        std::copy(pk8->rawData(), pk8->rawData() + pk8->getLength(), getBlock(Box)->decryptedData() + boxOffset(box, slot));
    }
}
void SavSWSH::pkm(std::shared_ptr<PKX> pk, u8 slot)
{
    if (pk->generation() == Generation::EIGHT)
    {
        u8 buf[0x158] = {0};
        std::copy(pk->rawData(), pk->rawData() + pk->getLength(), buf);
        std::unique_ptr<PK8> pk8 = std::make_unique<PK8>(buf, false, true, true);

        if (pk->getLength() != 0x158)
        {
            for (int i = 0; i < 6; i++)
            {
                pk8->partyStat(Stat(i), pk8->stat(Stat(i)));
            }
            pk8->partyLevel(pk8->level());
            pk8->partyCurrHP(pk8->stat(Stat::HP));
        }

        pk8->encrypt();
        std::copy(pk8->rawData(), pk8->rawData() + pk8->getLength(), getBlock(Party)->decryptedData() + partyOffset(slot));
    }
}

void SavSWSH::cryptBoxData(bool crypted)
{
    for (u8 box = 0; box < maxBoxes(); box++)
    {
        for (u8 slot = 0; slot < 30; slot++)
        {
            std::unique_ptr<PKX> pk8 = std::make_unique<PK8>(getBlock(Box)->decryptedData() + boxOffset(box, slot), crypted, true, true);
            if (!crypted)
            {
                pk8->encrypt();
            }
        }
    }
}

std::unique_ptr<WCX> SavSWSH::mysteryGift(int pos) const
{
    return nullptr;
}

void SavSWSH::dex(std::shared_ptr<PKX> pk)
{
    if (u16 index = ((PK8*)pk.get())->pokedexIndex())
    {
        u8* entryAddr  = getBlock(PokeDex)->decryptedData() + sizeof(DexEntry) * index;
        DexEntry entry = Endian::convertTo<DexEntry>(entryAddr);

        u16 form = pk->alternativeForm();
        if (pk->species() == 869) // Alcremie
        {
            form *= 7;
            form += ((PK8*)pk.get())->formDuration();
        }
        else if (pk->species() == 890) // Eternatus
        {
            form = 0;
            setProperGiga(entry, true, pk->shiny(), pk->gender());
        }

        setProperLocation(entry, form, pk->shiny(), pk->gender());
        entry.owned = 1;
        entry.languages |= 1 << u8(pk->language());
        entry.displayFormID = form;
        entry.displayShiny  = pk->shiny() ? 1 : 0;

        if (entry.battled == 0)
        {
            entry.battled++;
        }

        Endian::convertFrom<DexEntry>(entryAddr, entry);
    }
}

int SavSWSH::dexSeen() const
{
    int ret = 0;
    for (auto i : availableSpecies())
    {
        u16 index       = PersonalSWSH::pokedexIndex(i);
        u8* entryOffset = getBlock(PokeDex)->decryptedData() + index * sizeof(DexEntry);
        for (size_t j = 0; j < 0x20; j++) // Entire seen region size
        {
            if (entryOffset[j])
            {
                ret++;
                break;
            }
        }
    }
    return ret;
}

int SavSWSH::dexCaught() const
{
    int ret = 0;
    for (auto i : availableSpecies())
    {
        u16 index       = PersonalSWSH::pokedexIndex(i);
        u8* entryOffset = getBlock(PokeDex)->decryptedData() + index * sizeof(DexEntry);
        if (entryOffset[0x20] & 3)
        {
            ret++;
        }
    }
    return ret;
}

const std::set<int>& SavSWSH::availableSpecies() const
{
    if (species.empty())
    {
        species = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 25, 26, 35, 36, 37, 38, 43, 44, 45, 50, 51, 52, 53, 58, 59, 66, 67, 68, 77, 78, 83, 90, 91,
            92, 93, 94, 95, 98, 99, 106, 107, 109, 110, 111, 112, 118, 119, 122, 129, 130, 131, 132, 133, 134, 135, 136, 143, 150, 151, 163, 164, 170,
            171, 172, 173, 175, 176, 177, 178, 182, 185, 194, 195, 196, 197, 202, 208, 211, 213, 215, 220, 221, 222, 223, 224, 225, 226, 236, 237,
            246, 247, 248, 251, 263, 264, 270, 271, 272, 273, 274, 275, 278, 279, 280, 281, 282, 290, 291, 292, 302, 303, 309, 310, 315, 320, 321,
            324, 328, 329, 330, 337, 338, 339, 340, 341, 342, 343, 344, 349, 350, 355, 356, 360, 361, 362, 385, 406, 407, 415, 416, 420, 421, 422,
            423, 425, 426, 434, 435, 436, 437, 438, 439, 446, 447, 448, 449, 450, 451, 452, 453, 454, 458, 459, 460, 461, 464, 468, 470, 471, 473,
            475, 477, 478, 479, 509, 510, 517, 518, 519, 520, 521, 524, 525, 526, 527, 528, 529, 530, 532, 533, 534, 535, 536, 537, 538, 539, 546,
            547, 550, 554, 555, 556, 557, 558, 559, 560, 561, 562, 563, 568, 569, 572, 573, 574, 575, 576, 577, 578, 579, 582, 583, 584, 588, 589,
            592, 593, 595, 596, 597, 598, 599, 600, 601, 605, 606, 607, 608, 609, 610, 611, 612, 613, 614, 616, 617, 618, 622, 623, 624, 625, 627,
            628, 629, 630, 631, 632, 633, 634, 635, 638, 639, 640, 643, 644, 646, 647, 659, 660, 674, 675, 677, 678, 679, 680, 681, 682, 683, 684,
            685, 686, 687, 688, 689, 694, 695, 700, 701, 704, 705, 706, 708, 709, 710, 711, 712, 713, 714, 715, 722, 723, 724, 725, 726, 727, 728,
            729, 730, 736, 737, 738, 742, 743, 746, 747, 748, 749, 750, 751, 752, 755, 756, 757, 758, 759, 760, 761, 762, 763, 765, 766, 767, 768,
            771, 772, 773, 776, 777, 778, 780, 781, 782, 783, 784, 789, 790, 791, 792, 800, 802, 807, 808, 809, 810, 811, 812, 813, 814, 815, 816,
            817, 818, 819, 820, 821, 822, 823, 824, 825, 826, 827, 828, 829, 830, 831, 832, 833, 834, 835, 836, 837, 838, 839, 840, 841, 842, 843,
            844, 845, 846, 847, 848, 849, 850, 851, 852, 853, 854, 855, 856, 857, 858, 859, 860, 861, 862, 863, 864, 865, 866, 867, 868, 869, 870,
            871, 872, 873, 874, 875, 876, 877, 878, 879, 880, 881, 882, 883, 884, 885, 886, 887, 888, 889, 890};
    }
    return species;
}
