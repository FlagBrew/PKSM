#include "gui/screens/main-menu/sub-components/TrainerInfo.hpp"

#include "gui/shared/UIConstants.hpp"
#include "utils/Logger.hpp"

namespace pksm::ui {

TrainerInfo::TrainerInfo(
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
)
  : Element(),
    x(x),
    y(y),
    width(width),
    otName(otName),
    badges(badges),
    wonderCards(wonderCards),
    nationalDexSeen(nationalDexSeen),
    nationalDexCaught(nationalDexCaught),
    completion(completion),
    timePlayed(timePlayed) {
    LOG_DEBUG("Initializing TrainerInfo component...");

    // Initialize container with temporary height, will be updated after layout
    container = pu::ui::Container::New(x, y, width, 1);

    // Create circular outline for OT letter
    circleOutline = pksm::ui::CircularOutline::New(
        x + CIRCLE_MARGIN,
        y + CIRCLE_MARGIN,
        CIRCLE_DIAMETER,
        CIRCLE_OUTLINE_COLOR,
        4  // Border width
    );
    container->Add(circleOutline);

    // Create OT letter text
    otLetterText = pu::ui::elm::TextBlock::New(0, 0, otName.substr(0, 1));  // First letter only
    otLetterText->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_TITLE));
    otLetterText->SetColor(TEXT_COLOR);
    container->Add(otLetterText);

    // Create trainer name text with separate label and value
    trainerLabel = pu::ui::elm::TextBlock::New(0, 0, "Trainer:");
    trainerLabel->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_HEADER));
    trainerLabel->SetColor(TEXT_COLOR);
    container->Add(trainerLabel);

    trainerValue = pu::ui::elm::TextBlock::New(0, 0, otName);
    trainerValue->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_HEADER));
    trainerValue->SetColor(TEXT_COLOR);
    container->Add(trainerValue);

    // Create generation text with separate label and value
    generationLabel = pu::ui::elm::TextBlock::New(0, 0, "Generation:");
    generationLabel->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_HEADER));
    generationLabel->SetColor(TEXT_COLOR);
    container->Add(generationLabel);

    generationValue = pu::ui::elm::TextBlock::New(0, 0, "2");
    generationValue->SetFont(pu::ui::MakeDefaultFontName(pksm::ui::global::FONT_SIZE_HEADER));
    generationValue->SetColor(TEXT_COLOR);
    container->Add(generationValue);

    // Create stats text blocks with separate labels and values
    badgesLabel = pu::ui::elm::TextBlock::New(0, 0, "Badges:");
    badgesLabel->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    badgesLabel->SetColor(TEXT_COLOR);
    container->Add(badgesLabel);

    badgesValue = pu::ui::elm::TextBlock::New(0, 0, "");
    badgesValue->SetFont(pu::ui::MakeDefaultFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    badgesValue->SetColor(TEXT_COLOR);
    container->Add(badgesValue);

    wonderCardsLabel = pu::ui::elm::TextBlock::New(0, 0, "Wonder Cards:");
    wonderCardsLabel->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    wonderCardsLabel->SetColor(TEXT_COLOR);
    container->Add(wonderCardsLabel);

    wonderCardsValue = pu::ui::elm::TextBlock::New(0, 0, "");
    wonderCardsValue->SetFont(pu::ui::MakeDefaultFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    wonderCardsValue->SetColor(TEXT_COLOR);
    container->Add(wonderCardsValue);

    nationalDexSeenLabel = pu::ui::elm::TextBlock::New(0, 0, "National Dex Seen:");
    nationalDexSeenLabel->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    nationalDexSeenLabel->SetColor(TEXT_COLOR);
    container->Add(nationalDexSeenLabel);

    nationalDexSeenValue = pu::ui::elm::TextBlock::New(0, 0, "");
    nationalDexSeenValue->SetFont(pu::ui::MakeDefaultFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    nationalDexSeenValue->SetColor(TEXT_COLOR);
    container->Add(nationalDexSeenValue);

    nationalDexCaughtLabel = pu::ui::elm::TextBlock::New(0, 0, "National Dex Caught:");
    nationalDexCaughtLabel->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS)
    );
    nationalDexCaughtLabel->SetColor(TEXT_COLOR);
    container->Add(nationalDexCaughtLabel);

    nationalDexCaughtValue = pu::ui::elm::TextBlock::New(0, 0, "");
    nationalDexCaughtValue->SetFont(pu::ui::MakeDefaultFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    nationalDexCaughtValue->SetColor(TEXT_COLOR);
    container->Add(nationalDexCaughtValue);

    completionLabel = pu::ui::elm::TextBlock::New(0, 0, "Completion:");
    completionLabel->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    completionLabel->SetColor(TEXT_COLOR);
    container->Add(completionLabel);

    completionValue = pu::ui::elm::TextBlock::New(0, 0, "");
    completionValue->SetFont(pu::ui::MakeDefaultFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    completionValue->SetColor(TEXT_COLOR);
    container->Add(completionValue);

    timePlayedLabel = pu::ui::elm::TextBlock::New(0, 0, "Time Played:");
    timePlayedLabel->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    timePlayedLabel->SetColor(TEXT_COLOR);
    container->Add(timePlayedLabel);

    timePlayedValue = pu::ui::elm::TextBlock::New(0, 0, "");
    timePlayedValue->SetFont(pu::ui::MakeDefaultFontName(pksm::ui::global::FONT_SIZE_TRAINER_INFO_STATS));
    timePlayedValue->SetColor(TEXT_COLOR);
    container->Add(timePlayedValue);

    // Create completion progress bar
    completionBar = pu::ui::elm::ProgressBar::New(0, 0, width - (PROGRESS_BAR_MARGIN * 2), PROGRESS_BAR_HEIGHT, 100.0);
    completionBar->SetBackgroundColor(PROGRESS_BAR_BG_COLOR);
    completionBar->SetProgressColor(PROGRESS_BAR_FILL_COLOR);
    container->Add(completionBar);

    // Update texts
    UpdateTexts();

    // Let container prepare elements
    container->PreRender();

    // Now layout elements and calculate final height
    LayoutElements();

    // Update container height
    container->SetHeight(height);

    LOG_DEBUG("TrainerInfo component initialization complete");
}

void TrainerInfo::UpdateTexts() {
    badgesValue->SetText(std::to_string(badges));
    wonderCardsValue->SetText(std::to_string(wonderCards));
    nationalDexSeenValue->SetText(std::to_string(nationalDexSeen));
    nationalDexCaughtValue->SetText(std::to_string(nationalDexCaught));
    completionValue->SetText(std::to_string(static_cast<int>(completion * 100)) + "%");
    timePlayedValue->SetText(timePlayed);
    completionBar->SetProgress(completion * 100.0);
}

void TrainerInfo::LayoutElements() {
    const pu::i32 borderWidth = circleOutline->GetBorderWidth();

    // Calculate the total diameter including the border
    const pu::i32 totalDiameter = CIRCLE_DIAMETER + (borderWidth * 2);

    // Position the circle including border space
    const pu::i32 circleX = x + CIRCLE_MARGIN;
    const pu::i32 circleY = y + CIRCLE_MARGIN;

    // Set circle position
    circleOutline->SetX(circleX);
    circleOutline->SetY(circleY);

    // Calculate the center point accounting for asymmetric border growth
    // X uses CIRCLE_DIAMETER since border grows evenly left/right
    // Y uses totalDiameter since border grows more downward
    const pu::i32 centerX = circleX + (CIRCLE_DIAMETER / 2);
    const pu::i32 centerY = circleY + (totalDiameter / 2);

    // Center text relative to the true center point
    const pu::i32 letterX = centerX - (otLetterText->GetWidth() / 2);
    const pu::i32 letterY = centerY - (otLetterText->GetHeight() / 2);

    otLetterText->SetX(letterX);
    otLetterText->SetY(letterY);

    // Position trainer name and generation next to circle accounting for total diameter
    const pu::i32 headerStartX = x + CIRCLE_MARGIN + totalDiameter + HEADER_TEXT_MARGIN;

    // Calculate total height of header text section
    const pu::i32 headerTextTotalHeight = trainerLabel->GetHeight() + HEADER_TEXT_VERTICAL_SPACING +
        generationLabel->GetHeight();

    // Calculate starting Y to center header text with circle
    const pu::i32 headerStartY = y + CIRCLE_MARGIN + (CIRCLE_DIAMETER - headerTextTotalHeight) / 2;

    trainerLabel->SetX(headerStartX);
    trainerLabel->SetY(headerStartY);
    trainerValue->SetX(headerStartX + trainerLabel->GetWidth() + TEXT_SPACING);
    trainerValue->SetY(headerStartY);

    generationLabel->SetX(headerStartX);
    generationLabel->SetY(headerStartY + trainerLabel->GetHeight() + HEADER_TEXT_VERTICAL_SPACING);
    generationValue->SetX(headerStartX + generationLabel->GetWidth() + TEXT_SPACING);
    generationValue->SetY(headerStartY + trainerLabel->GetHeight() + HEADER_TEXT_VERTICAL_SPACING);

    // Calculate stats container position and size
    const pu::i32 statsStartY = y + CIRCLE_MARGIN + CIRCLE_DIAMETER + SECTION_SPACING;
    const pu::i32 statsWidth = width - (STATS_CONTAINER_MARGIN * 2);
    const pu::i32 statsX = x + STATS_CONTAINER_MARGIN;
    statsContainerX = statsX;
    statsContainerY = statsStartY;
    statsContainerWidth = statsWidth;

    // Find the longest label width to align all values
    const pu::i32 longestLabelWidth = std::max(
        {badgesLabel->GetWidth(),
         wonderCardsLabel->GetWidth(),
         nationalDexSeenLabel->GetWidth(),
         nationalDexCaughtLabel->GetWidth(),
         completionLabel->GetWidth(),
         timePlayedLabel->GetWidth()}
    );

    // Position stats vertically with proper spacing
    pu::i32 currentY = statsStartY + STATS_INTERNAL_MARGIN;
    const pu::i32 statsTextX = statsX + STATS_INTERNAL_MARGIN;
    const pu::i32 valueX = statsTextX + longestLabelWidth + TEXT_SPACING;  // Consistent value position

    // Position each stat with proper spacing based on actual text height
    badgesLabel->SetX(statsTextX);
    badgesLabel->SetY(currentY);
    badgesValue->SetX(valueX);
    badgesValue->SetY(currentY);
    currentY += std::max(badgesLabel->GetHeight(), badgesValue->GetHeight()) + TEXT_SPACING;

    wonderCardsLabel->SetX(statsTextX);
    wonderCardsLabel->SetY(currentY);
    wonderCardsValue->SetX(valueX);
    wonderCardsValue->SetY(currentY);
    currentY += std::max(wonderCardsLabel->GetHeight(), wonderCardsValue->GetHeight()) + TEXT_SPACING;

    nationalDexSeenLabel->SetX(statsTextX);
    nationalDexSeenLabel->SetY(currentY);
    nationalDexSeenValue->SetX(valueX);
    nationalDexSeenValue->SetY(currentY);
    currentY += std::max(nationalDexSeenLabel->GetHeight(), nationalDexSeenValue->GetHeight()) + TEXT_SPACING;

    nationalDexCaughtLabel->SetX(statsTextX);
    nationalDexCaughtLabel->SetY(currentY);
    nationalDexCaughtValue->SetX(valueX);
    nationalDexCaughtValue->SetY(currentY);
    currentY += std::max(nationalDexCaughtLabel->GetHeight(), nationalDexCaughtValue->GetHeight()) + TEXT_SPACING;

    completionLabel->SetX(statsTextX);
    completionLabel->SetY(currentY);
    completionValue->SetX(valueX);
    completionValue->SetY(currentY);
    currentY += std::max(completionLabel->GetHeight(), completionValue->GetHeight()) + TEXT_SPACING;

    // Update progress bar width to match stats container
    const pu::i32 progressBarWidth = statsWidth - (STATS_INTERNAL_MARGIN * 2);
    completionBar->SetWidth(progressBarWidth);
    completionBar->SetX(statsTextX);
    completionBar->SetY(currentY);
    currentY += PROGRESS_BAR_HEIGHT + TEXT_SPACING;

    timePlayedLabel->SetX(statsTextX);
    timePlayedLabel->SetY(currentY);
    timePlayedValue->SetX(valueX);
    timePlayedValue->SetY(currentY);

    const pu::i32 lastElementHeight = std::max(timePlayedLabel->GetHeight(), timePlayedValue->GetHeight());
    statsContainerHeight = (currentY + lastElementHeight + STATS_INTERNAL_MARGIN) - statsStartY;

    // The total height should be the distance from the top (y) to the bottom of the stats container
    height = (statsContainerY + statsContainerHeight + BOTTOM_MARGIN) - y;
}

pu::i32 TrainerInfo::GetX() {
    return x;
}

pu::i32 TrainerInfo::GetY() {
    return y;
}

pu::i32 TrainerInfo::GetWidth() {
    return width;
}

pu::i32 TrainerInfo::GetHeight() {
    return height;
}

void TrainerInfo::OnRender(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    // Render rounded rectangles for backgrounds
    drawer->RenderRoundedRectangleFill(MAIN_BACKGROUND_COLOR, x, y, width, height, 16);

    // Render stats background with rounded corners
    drawer->RenderRoundedRectangleFill(
        STATS_BACKGROUND_COLOR,
        statsContainerX,
        statsContainerY,
        statsContainerWidth,
        statsContainerHeight,
        12
    );

    // Render all elements in the container
    for (auto& element : container->GetElements()) {
        element->OnRender(drawer, element->GetX(), element->GetY());
    }
}

void TrainerInfo::SetTrainerInfo(
    const std::string& otName,
    const u32 badges,
    const u32 wonderCards,
    const u32 nationalDexSeen,
    const u32 nationalDexCaught,
    const float completion,
    const std::string& timePlayed
) {
    this->otName = otName;
    this->badges = badges;
    this->wonderCards = wonderCards;
    this->nationalDexSeen = nationalDexSeen;
    this->nationalDexCaught = nationalDexCaught;
    this->completion = completion;
    this->timePlayed = timePlayed;

    otLetterText->SetText(otName.substr(0, 1));
    trainerValue->SetText(otName);  // Update trainer name value
    UpdateTexts();
    LayoutElements();
}

}  // namespace pksm::ui