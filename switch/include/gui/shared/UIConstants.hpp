#pragma once

#include <pu/Plutonium>
#include <string>

namespace pksm::ui::global {
// Colors
static constexpr pu::ui::Color BACKGROUND_BLUE = pu::ui::Color(10, 15, 75, 255);
static constexpr pu::ui::Color TEXT_WHITE = pu::ui::Color(255, 255, 255, 255);
static constexpr pu::ui::Color OUTLINE_COLOR = pu::ui::Color(0, 150, 255, 255);

// Font Sizes - Custom
static constexpr u32 FONT_SIZE_TITLE = 60;
static constexpr u32 FONT_SIZE_HEADER = 40;
static constexpr u32 FONT_SIZE_BUTTON = 40;
static constexpr u32 FONT_SIZE_ACCOUNT_NAME = 28;
static constexpr u32 FONT_SIZE_TRIGGER_BUTTON_NAVIGATION = 32;
static constexpr u32 FONT_SIZE_BUTTON_GLYPH_Y_OFFSET = -4;
static constexpr u32 FONT_SIZE_TRAINER_INFO_STATS = 36;

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

// Switch Button Glyphs
enum class ButtonGlyph {
    A,
    B,
    X,
    Y,
    L,
    R,
    ZL,
    ZR,
    Plus,
    Minus,
    Home,
    AnalogStick,
    LeftAnalogStick,
    RightAnalogStick,
    DPad,
    TouchScreen,
    ChevronLeft,
    ChevronRight,
    TriggerLeft,
    TriggerRight
};

inline std::string GetButtonGlyphString(ButtonGlyph button) {
    switch (button) {
        case ButtonGlyph::A:
            return "\uE0E0";
        case ButtonGlyph::B:
            return "\uE0E1";
        case ButtonGlyph::X:
            return "\uE0E2";
        case ButtonGlyph::Y:
            return "\uE0E3";
        case ButtonGlyph::L:
            return "\uE0E4";
        case ButtonGlyph::R:
            return "\uE0E5";
        case ButtonGlyph::ZL:
            return "\uE0E6";
        case ButtonGlyph::ZR:
            return "\uE0E7";
        case ButtonGlyph::Plus:
            return "\uE0EF";
        case ButtonGlyph::Minus:
            return "\uE0F0";
        case ButtonGlyph::Home:
            return "\uE0F4";
        case ButtonGlyph::AnalogStick:
            return "\uE100";
        case ButtonGlyph::LeftAnalogStick:
            return "\uE101";
        case ButtonGlyph::RightAnalogStick:
            return "\uE102";
        case ButtonGlyph::DPad:
            return "\uE110";
        case ButtonGlyph::TouchScreen:
            return "\uE121";
        case ButtonGlyph::ChevronLeft:
            return "\uE149";
        case ButtonGlyph::ChevronRight:
            return "\uE14A";
        case ButtonGlyph::TriggerLeft:
            return "\uE0F6";
        case ButtonGlyph::TriggerRight:
            return "\uE0F7";
        default:
            return "?";
    }
}
}  // namespace pksm::ui::global