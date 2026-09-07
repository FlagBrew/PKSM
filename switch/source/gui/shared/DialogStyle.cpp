#include "gui/shared/DialogStyle.hpp"

#include "gui/shared/UIConstants.hpp"

namespace pksm::ui {

void StyleListDialog(pu::ui::Dialog::Ref& dialog, const std::vector<std::string>& notes) {
    dialog->SetVerticalOptions(true);
    dialog->SetTitleFontName(global::MakeHeavyFontName(global::FONT_SIZE_HEADER));
    dialog->SetContentFontName(global::MakeMediumFontName(global::FONT_SIZE_ACCOUNT_NAME));
    dialog->SetOptionFontName(global::MakeMediumFontName(global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION));
    dialog->SetOptionNoteFontName(global::MakeMediumFontName(global::FONT_SIZE_ACCOUNT_NAME));
    dialog->SetDialogColor(pu::ui::Color(235, 235, 235, 255));
    dialog->SetDialogBorderRadius(12);
    dialog->SetMinimumWidth(820);
    dialog->SetTitleX(40);
    dialog->SetTitleY(36);
    dialog->SetContentX(40);
    dialog->SetVerticalTitleContentGap(18);
    dialog->SetSpaceBetweenContentAndOptions(24);
    dialog->SetOptionsBaseHorizontalMargin(40);
    dialog->SetOptionHorizontalMargin(22);
    dialog->SetOptionHeight(72);
    dialog->SetOptionBorderRadius(8);
    dialog->SetSpaceBetweenOptionRows(10);
    dialog->SetOptionBottomMargin(32);
    dialog->SetOptionColor(pu::ui::Color(26, 26, 26, 255));
    dialog->SetOptionRowColor(pu::ui::Color(248, 248, 248, 255));
    dialog->SetOptionNoteColor(pu::ui::Color(102, 102, 102, 255));
    dialog->SetOptionOutlineColor(global::OUTLINE_COLOR);
    dialog->SetOptionOutlineWidth(4);
    for (size_t i = 0; i < notes.size(); i++) {
        dialog->SetOptionNote(static_cast<u32>(i), notes[i]);
    }
}

}  // namespace pksm::ui
