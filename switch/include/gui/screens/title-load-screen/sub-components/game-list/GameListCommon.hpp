#pragma once

#include <pu/Plutonium>
#include <string>

namespace pksm::ui {

enum class GameListType { Console, Custom, Emulator };

struct GameListInfo {
    GameListType type;
    std::string navigationTitle;
};

struct GameListLayoutConfig {
    pu::i32 paddingLeft;
    pu::i32 paddingRight;
    pu::i32 paddingTop;
    pu::i32 paddingBottom;
    pu::i32 sectionTitleSpacing;
    pu::i32 gameOutlinePadding;
};
}  // namespace pksm::ui