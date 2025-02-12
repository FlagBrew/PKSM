#pragma once

#include <pu/Plutonium>

namespace pksm::ui::global {
// Colors
static constexpr pu::ui::Color BACKGROUND_BLUE = pu::ui::Color(10, 15, 75, 255);
static constexpr pu::ui::Color TEXT_WHITE = pu::ui::Color(255, 255, 255, 255);

// Font Sizes - Predefined
static constexpr auto FONT_SIZE_SMALL = static_cast<u32>(pu::ui::DefaultFontSize::Small);
static constexpr auto FONT_SIZE_MEDIUM = static_cast<u32>(pu::ui::DefaultFontSize::Medium);
static constexpr auto FONT_SIZE_MEDIUM_LARGE = static_cast<u32>(pu::ui::DefaultFontSize::MediumLarge);
static constexpr auto FONT_SIZE_LARGE = static_cast<u32>(pu::ui::DefaultFontSize::Large);

// Font Sizes - Custom
static constexpr u32 FONT_SIZE_TITLE = 60;
static constexpr u32 FONT_SIZE_HEADER = 40;
static constexpr u32 FONT_SIZE_BUTTON = 40;
static constexpr u32 FONT_SIZE_TRIGGER_BUTTON_NAVIGATION = 36;

// Font Names
inline std::string MakeHeavyFontName(const u32 font_size) {
    return "HeavyFont@" + std::to_string(font_size);
}

inline std::string MakeMediumFontName(const u32 font_size) {
    return "MediumFont@" + std::to_string(font_size);
}

inline std::string MakeSwitchButtonFontName(const u32 font_size) {
    return "SwitchButtonFont@" + std::to_string(font_size);
}
}  // namespace pksm::ui::global