#pragma once

#include "gui/screens/title-load-screen/sub-components/game-list/FullWidthGameList.hpp"

namespace pksm::ui {

class CustomGameList : public FullWidthGameList {
public:
    CustomGameList(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const pu::i32 height,
        const GameListLayoutConfig& config,
        input::FocusManager::Ref parentFocusManager
    )
      : FullWidthGameList(x, y, width, height, config, parentFocusManager, "Custom Games") {}

    PU_SMART_CTOR(CustomGameList)
};

}  // namespace pksm::ui