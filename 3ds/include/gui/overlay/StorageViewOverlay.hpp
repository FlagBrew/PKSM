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

#ifndef STORAGEVIEWOVERLAY_HPP
#define STORAGEVIEWOVERLAY_HPP

#include "ViewOverlay.hpp"
#include <bitset>
#include <vector>

class StorageViewOverlay : public ViewOverlay
{
public:
    StorageViewOverlay(Screen& screen, std::shared_ptr<PKX>& pkm, std::vector<std::shared_ptr<PKX>>& clone, std::vector<int>& partyNum,
        std::pair<int, int>& cloneDims, bool& currentlySelecting, std::pair<int, int> emergencyInfo)
        : ViewOverlay(screen, pkm, true, i18n::localize("A_SELECT") + '\n' + i18n::localize("X_CLONE") + '\n' + i18n::localize("B_BACK")),
          clone(clone),
          partyNum(partyNum),
          cloneDims(cloneDims),
          currentlySelecting(currentlySelecting),
          emergencyInfo(emergencyInfo)
    {
    }
    StorageViewOverlay(Overlay& ovly, std::shared_ptr<PKX>& pkm, std::vector<std::shared_ptr<PKX>>& clone, std::vector<int>& partyNum,
        std::pair<int, int>& cloneDims, bool& currentlySelecting, std::pair<int, int> emergencyInfo)
        : ViewOverlay(ovly, pkm, true, i18n::localize("A_SELECT") + '\n' + i18n::localize("X_CLONE") + '\n' + i18n::localize("B_BACK")),
          clone(clone),
          partyNum(partyNum),
          cloneDims(cloneDims),
          currentlySelecting(currentlySelecting),
          emergencyInfo(emergencyInfo)
    {
    }
    virtual ~StorageViewOverlay() {}
    void update(touchPosition* touch) override;
    void drawBottom() const override;
    bool handlesUpdate() const override { return true; }

private:
    std::vector<std::shared_ptr<PKX>>& clone;
    std::vector<int>& partyNum;
    std::pair<int, int>& cloneDims;
    bool& currentlySelecting;
    std::pair<int, int> emergencyInfo;
    std::bitset<9> emergencyMode;
};

#endif
