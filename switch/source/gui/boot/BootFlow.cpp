#include "gui/boot/BootFlow.hpp"

#include <atomic>
#include <switch.h>
#include <thread>

#include "input/ButtonInputHandler.hpp"
#include "utils/AssetDownloader.hpp"
#include "utils/Logger.hpp"

namespace pksm::boot {

BootProgress::BootProgress(pu::ui::render::Renderer::Ref& renderer) : renderer(renderer) {
    appName = pu::ui::elm::TextBlock::New(0, 430, "PKSM");
    appName->SetColor(ui::global::TEXT_WHITE);
    appName->SetFont(ui::global::MakeHeavyFontName(ui::global::FONT_SIZE_TITLE));
    appName->SetX((SCREEN_W - appName->GetWidth()) / 2);
    status = pu::ui::elm::TextBlock::New(0, 540, "");
    status->SetColor(ui::global::TEXT_WHITE);
    status->SetFont(ui::global::MakeMediumFontName(ui::global::FONT_SIZE_TRIGGER_BUTTON_NAVIGATION));
    bar = pu::ui::elm::ProgressBar::New((SCREEN_W - BAR_W) / 2, 610, BAR_W, 28, 1.0);
    bar->SetProgressColor(ui::global::OUTLINE_COLOR);
}

void BootProgress::ShowPhase(const std::string& text) {
    SetStatus(text);
    Draw(false, false);
}

void BootProgress::ShowDownload(const std::string& text, double progress) {
    SetStatus(text);
    bar->SetProgress(progress);
    Draw(true, false);
}

void BootProgress::ShowRetry(const std::string& text) {
    SetStatus(text);
    Draw(false, true);
}

void BootProgress::Release() {
    appName = nullptr;
    status = nullptr;
    bar = nullptr;
    retryAction.Release();
    quitAction.Release();
}

void BootProgress::SetRetryPressed(bool pressed) {
    retryAction.SetPressed(pressed);
}

void BootProgress::SetQuitPressed(bool pressed) {
    quitAction.SetPressed(pressed);
}

BootProgress::BootAction::BootAction(ui::global::ButtonGlyph button, const std::string& label) {
    glyphText = pu::ui::elm::TextBlock::New(0, 0, ui::global::GetButtonGlyphString(button));
    glyphText->SetFont(ui::global::MakeSwitchButtonFontName(ui::global::FONT_SIZE_ACCOUNT_NAME));
    labelText = pu::ui::elm::TextBlock::New(0, 0, label);
    labelText->SetFont(ui::global::MakeMediumFontName(ui::global::FONT_SIZE_ACCOUNT_NAME));
    SetPressed(false);
}

void BootProgress::BootAction::SetPressed(bool pressed) {
    const auto clr = pressed ? ui::global::OUTLINE_COLOR : ui::global::TEXT_WHITE;
    glyphText->SetColor(clr);
    labelText->SetColor(clr);
}

pu::i32 BootProgress::BootAction::GetWidth() {
    return glyphText->GetWidth() + GLYPH_GAP + labelText->GetWidth();
}

void BootProgress::BootAction::Release() {
    glyphText = nullptr;
    labelText = nullptr;
}

void BootProgress::BootAction::Render(pu::ui::render::Renderer::Ref& drawer, const pu::i32 x, const pu::i32 y) {
    glyphText->OnRender(drawer, x, y);
    labelText->OnRender(drawer, x + glyphText->GetWidth() + GLYPH_GAP, y);
}

// SetText rebuilds the TTF texture even for identical text; the retry screen redraws every frame
void BootProgress::SetStatus(const std::string& text) {
    if (text != currentStatus) {
        currentStatus = text;
        status->SetText(text);
    }
}

void BootProgress::Draw(bool withBar, bool withActions) {
    status->SetX((SCREEN_W - status->GetWidth()) / 2);
    renderer->InitializeRender(ui::global::BACKGROUND_BLUE);
    appName->OnRender(renderer, appName->GetX(), appName->GetY());
    status->OnRender(renderer, status->GetX(), status->GetY());
    if (withBar) {
        bar->OnRender(renderer, bar->GetX(), bar->GetY());
    }
    if (withActions) {
        const pu::i32 rowWidth = retryAction.GetWidth() + ACTION_GAP + quitAction.GetWidth();
        const pu::i32 rowX = (SCREEN_W - rowWidth) / 2;
        retryAction.Render(renderer, rowX, ACTION_ROW_Y);
        quitAction.Render(renderer, rowX + retryAction.GetWidth() + ACTION_GAP, ACTION_ROW_Y);
    }
    renderer->FinalizeRender();
}

bool RunAssetBootstrap(BootProgress& boot) {
    boot.ShowPhase("Checking sprites");
    utils::AssetDownloader::Refresh();

    PadState pad;
    padInitializeDefault(&pad);
    int retries = 0;
    while (utils::AssetDownloader::NeedsDownload()) {
        bool online = false;
        if (R_SUCCEEDED(nifmInitialize(NifmServiceType_User))) {
            NifmInternetConnectionType connType;
            u32 strength = 0;
            NifmInternetConnectionStatus connStatus;
            const Result rc = nifmGetInternetConnectionStatus(&connType, &strength, &connStatus);
            nifmExit();
            online = R_SUCCEEDED(rc) && connStatus == NifmInternetConnectionStatus_Connected;
        }

        if (online) {
            utils::AssetDownloader::Progress progress;
            std::atomic<bool> done{false};
            std::thread worker([&]() {
                utils::AssetDownloader::DownloadAll(progress);
                done.store(true);
            });
            while (!done.load()) {
                const size_t received = progress.received.load();
                const size_t total = progress.total.load();
                const size_t index = progress.fileIndex.load();
                std::string text = "Connecting...";
                if (index > 0) {
                    text = "Downloading sprites (" + std::to_string(index) + " of " +
                        std::to_string(progress.fileCount.load()) + ")  " + std::to_string(received / 1024) +
                        " KB";
                }
                boot.ShowDownload(text, total > 0 ? double(received) / double(total) : 0.0);
            }
            worker.join();
            if (!utils::AssetDownloader::NeedsDownload()) {
                LOG_INFO("Sprite assets downloaded and verified on SD");
                return true;
            }
        }

        std::string message;
        if (retries == 0) {
            message = online ? "The download failed - check your connection and try again"
                             : "PKSM needs to download its sprites on first launch - connect to the internet first";
        } else {
            message = online ? "The download failed again - check your connection and try again"
                             : "Still no connection - connect to the internet and retry";
        }
        retries++;

        input::ButtonInputHandler buttons;
        bool retryRequested = false;
        bool quitRequested = false;
        buttons.RegisterButton(
            HidNpadButton_A,
            [&]() { boot.SetRetryPressed(true); },
            [&]() {
                boot.SetRetryPressed(false);
                retryRequested = true;
            }
        );
        buttons.RegisterButton(
            HidNpadButton_Plus,
            [&]() { boot.SetQuitPressed(true); },
            [&]() {
                boot.SetQuitPressed(false);
                quitRequested = true;
            }
        );
        while (!retryRequested && !quitRequested) {
            padUpdate(&pad);
            buttons.HandleInput(padGetButtonsDown(&pad), padGetButtonsUp(&pad), padGetButtons(&pad));
            boot.ShowRetry(message);
        }
        if (quitRequested) {
            LOG_INFO("User quit from the asset retry screen");
            return false;
        }
    }
    LOG_DEBUG("Sprite assets verified on SD");
    return true;
}

}  // namespace pksm::boot
