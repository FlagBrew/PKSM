#pragma once

#include <pu/Plutonium>

#include "gui/shared/components/StaticOutline.hpp"

namespace pksm::ui {

class TrainerInfo : public pu::ui::elm::Element {
private:
    // Layout constants
    static constexpr pu::i32 CIRCLE_DIAMETER = 140;  // Circle size
    static constexpr pu::i32 CIRCLE_MARGIN = 32;  // Margin around circle
    static constexpr pu::i32 BOTTOM_MARGIN = 24;  // Margin around bottom of container
    static constexpr pu::i32 TEXT_SPACING = 32;  // Vertical spacing between text elements
    static constexpr pu::i32 HEADER_TEXT_VERTICAL_SPACING = 12;  // Vertical spacing between header text elements
    static constexpr pu::i32 PROGRESS_BAR_HEIGHT = 16;  // Progress bar height
    static constexpr pu::i32 PROGRESS_BAR_MARGIN = 24;  // Margin around progress bar
    static constexpr pu::i32 HEADER_TEXT_MARGIN = 48;  // Margin between circle and header text
    static constexpr pu::i32 SECTION_SPACING = 48;  // Spacing between header and stats sections
    static constexpr pu::i32 STATS_CONTAINER_MARGIN = 24;  // Margin around stats container
    static constexpr pu::i32 STATS_INTERNAL_MARGIN = 20;  // Internal margin within stats container

    // Colors - all with appropriate alpha for translucency
    static constexpr pu::ui::Color MAIN_BACKGROUND_COLOR = pu::ui::Color(39, 66, 164, 140);  // Main panel background
    static constexpr pu::ui::Color STATS_BACKGROUND_COLOR = pu::ui::Color(30, 50, 120, 160);  // Stats container
                                                                                              // background
    static constexpr pu::ui::Color TEXT_COLOR = pu::ui::Color(255, 255, 255, 180);  // All text
    static constexpr pu::ui::Color CIRCLE_OUTLINE_COLOR = pu::ui::Color(255, 255, 255, 180);  // Circle border
    static constexpr pu::ui::Color PROGRESS_BAR_BG_COLOR = pu::ui::Color(30, 50, 120, 180);  // Progress bar background
    static constexpr pu::ui::Color PROGRESS_BAR_FILL_COLOR = pu::ui::Color(0, 150, 255, 180);  // Progress bar fill

    // Position and size
    pu::i32 x;
    pu::i32 y;
    pu::i32 width;
    pu::i32 height;

    // Stats container dimensions
    pu::i32 statsContainerX;
    pu::i32 statsContainerY;
    pu::i32 statsContainerWidth;
    pu::i32 statsContainerHeight;

    // State
    std::string otName;
    u32 badges;
    u32 wonderCards;
    u32 nationalDexSeen;
    u32 nationalDexCaught;
    float completion;
    std::string timePlayed;

    // UI Elements
    pu::ui::Container::Ref container;
    pksm::ui::CircularOutline::Ref circleOutline;
    pu::ui::elm::TextBlock::Ref otLetterText;
    pu::ui::elm::TextBlock::Ref trainerLabel;
    pu::ui::elm::TextBlock::Ref trainerValue;
    pu::ui::elm::TextBlock::Ref generationLabel;
    pu::ui::elm::TextBlock::Ref generationValue;
    pu::ui::elm::TextBlock::Ref badgesLabel;
    pu::ui::elm::TextBlock::Ref badgesValue;
    pu::ui::elm::TextBlock::Ref wonderCardsLabel;
    pu::ui::elm::TextBlock::Ref wonderCardsValue;
    pu::ui::elm::TextBlock::Ref nationalDexSeenLabel;
    pu::ui::elm::TextBlock::Ref nationalDexSeenValue;
    pu::ui::elm::TextBlock::Ref nationalDexCaughtLabel;
    pu::ui::elm::TextBlock::Ref nationalDexCaughtValue;
    pu::ui::elm::TextBlock::Ref completionLabel;
    pu::ui::elm::TextBlock::Ref completionValue;
    pu::ui::elm::TextBlock::Ref timePlayedLabel;
    pu::ui::elm::TextBlock::Ref timePlayedValue;
    pu::ui::elm::ProgressBar::Ref completionBar;

    // Helper methods
    void UpdateTexts();
    void LayoutElements();

public:
    TrainerInfo(
        const pu::i32 x,
        const pu::i32 y,
        const pu::i32 width,
        const std::string& otName,
        const u32 badges,
        const u32 wonderCards,
        const u32 nationalDexSeen,
        const u32 nationalDexCaught,
        const float completion,
        const std::string& timePlayed
    );
    PU_SMART_CTOR(TrainerInfo)

    // Element implementation
    pu::i32 GetX() override;
    pu::i32 GetY() override;
    pu::i32 GetWidth() override;
    pu::i32 GetHeight() override;
    void OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) override;
    void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos)
        override {}

    // Update method
    void SetTrainerInfo(
        const std::string& otName,
        const u32 badges,
        const u32 wonderCards,
        const u32 nationalDexSeen,
        const u32 nationalDexCaught,
        const float completion,
        const std::string& timePlayed
    );
};

}  // namespace pksm::ui