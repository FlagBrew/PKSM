#pragma once

#include <pu/Plutonium>
#include <string>

#include "gui/shared/UIConstants.hpp"

namespace pksm::boot {

// Hand-pumped boot frames: each phase renders one labeled frame and holds it
// while the work runs; downloads animate from the worker thread's byte counts.
class BootProgress {
public:
    explicit BootProgress(pu::ui::render::Renderer::Ref& renderer);

    void ShowPhase(const std::string& text);
    void ShowDownload(const std::string& text, double progress);
    void ShowRetry(const std::string& text);

    // Drop the SDL textures before Renderer::Finalize - destruction after it reads freed memory
    void Release();

    void SetRetryPressed(bool pressed);
    void SetQuitPressed(bool pressed);

private:
    static constexpr pu::i32 SCREEN_W = pu::ui::render::ScreenWidth;
    static constexpr pu::i32 BAR_W = 640;

    // Highlighted while held, so a retry that lands on the same screen still visibly registered
    class BootAction {
    public:
        BootAction(ui::global::ButtonGlyph button, const std::string& label);

        void SetPressed(bool pressed);
        pu::i32 GetWidth();
        void Release();
        void Render(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y);

    private:
        static constexpr pu::i32 GLYPH_GAP = 14;
        pu::ui::elm::TextBlock::Ref glyphText;
        pu::ui::elm::TextBlock::Ref labelText;
    };

    void SetStatus(const std::string& text);
    void Draw(bool withBar, bool withActions);

    static constexpr pu::i32 ACTION_ROW_Y = 640;
    static constexpr pu::i32 ACTION_GAP = 72;

    pu::ui::render::Renderer::Ref& renderer;
    pu::ui::elm::TextBlock::Ref appName;
    pu::ui::elm::TextBlock::Ref status;
    pu::ui::elm::ProgressBar::Ref bar;
    std::string currentStatus;
    BootAction retryAction{ui::global::ButtonGlyph::A, "Retry"};
    BootAction quitAction{ui::global::ButtonGlyph::Plus, "Quit"};
};

// romfs ships no sprite art: boot gates on every asset verified on SD.
// Returns false only when the user quits from the retry screen.
bool RunAssetBootstrap(BootProgress& boot);

}  // namespace pksm::boot
