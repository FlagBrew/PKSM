#include "gui/screens/storage-screen/StorageScreen.hpp"

#include <ctime>

#include "gui/screens/main-menu/sub-components/menu-grid/MenuButtonGrid.hpp"
#include "utils/Logger.hpp"
#include "utils/SoftwareKeyboard.hpp"

namespace pksm::layout {

StorageScreen::StorageScreen(
    std::function<void()> onBack,
    std::function<void(pu::ui::Overlay::Ref)> onShowOverlay,
    std::function<void()> onHideOverlay,
    std::function<bool(const std::string& title, const std::string& message, const std::string& confirmLabel)>
        requestConfirmation,
    ISaveDataAccessor::Ref saveDataAccessor,
    IBoxDataProvider::Ref boxDataProvider,
    IStorageHand::Ref storageHand,
    IBoxNameEditor::Ref boxNameEditor
)
  : BaseLayout(onShowOverlay, onHideOverlay),
    onBack(onBack),
    requestConfirmation(requestConfirmation),
    saveDataAccessor(saveDataAccessor),
    boxDataProvider(boxDataProvider),
    storageHand(storageHand),
    boxNameEditor(boxNameEditor) {
    LOG_DEBUG("Initializing StorageScreen...");

    this->SetBackgroundColor(bgColor);
    background = ui::AnimatedBackground::New();
    this->Add(background);

    // Initialize focus management
    InitializeFocusManagement();

    // Initialize BoxGrid
    InitializePokemonBox();

    // Initialize help footer
    InitializeHelpFooter();

    // Set up button input handler for Back button
    buttonHandler.RegisterButton(HidNpadButton_B, nullptr, [this]() { HandleBackButton(); });
    // Clone/Release act on the selected slot, so they only apply while focus
    // is on the grid itself (same gating as the slots' own A handler)
    buttonHandler.RegisterButton(
        HidNpadButton_X,
        nullptr,
        [this]() { HandleCloneButton(); },
        [this]() { return pokemonBox->IsGridFocused(); }
    );
    buttonHandler.RegisterButton(
        HidNpadButton_Y,
        nullptr,
        [this]() { HandleReleaseButton(); },
        [this]() { return pokemonBox->IsGridFocused(); }
    );
    buttonHandler.RegisterButton(
        HidNpadButton_Plus,
        nullptr,
        [this]() { HandleSummaryButton(); },
        [this]() { return pokemonBox->IsGridFocused(); }
    );

    // Set initial help items
    UpdateHelpItems();

    // Set up input handling
    this->SetOnInput(
        std::bind(&StorageScreen::OnInput, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)
    );

    LOG_DEBUG("StorageScreen initialization complete");
    PreRender();
}

void StorageScreen::InitializeFocusManagement() {
    LOG_DEBUG("Initializing focus and selection management...");

    // Initialize focus managers
    storageScreenFocusManager = pksm::input::FocusManager::New("StorageScreen Manager");
    storageScreenFocusManager->SetActive(true);  // since this is the root manager
    pokemonBoxFocusManager = pksm::input::FocusManager::New("PokemonBox Manager");

    // Initialize selection managers
    storageScreenSelectionManager = pksm::input::SelectionManager::New("StorageScreen Manager");
    storageScreenSelectionManager->SetActive(true);  // since this is the root manager
    pokemonBoxSelectionManager = pksm::input::SelectionManager::New("PokemonBox Manager");

    storageScreenFocusManager->RegisterChildManager(pokemonBoxFocusManager);
    storageScreenSelectionManager->RegisterChildManager(pokemonBoxSelectionManager);

    // Set up directional input handlers
    pokemonBoxDirectionalHandler.SetOnMoveUp([this]() {
        // Handle up navigation from box grid if needed
    });

    LOG_DEBUG("Focus and selection management initialization complete");
}

void StorageScreen::InitializePokemonBox() {
    LOG_DEBUG("Initializing PokemonBox...");

    // Create the PokemonBox with the same parameters as we would BoxGrid
    pokemonBox = pksm::ui::PokemonBox::New(
        BOX_GRID_SIDE_MARGIN,
        BOX_GRID_TOP_MARGIN,
        BOX_ITEM_SIZE,
        pokemonBoxFocusManager,
        pokemonBoxSelectionManager
    );
    this->Add(pokemonBox);
    pokemonBox->SetName("PokemonBox Element");
    pokemonBox->EstablishOwningRelationship();

    // Load box data for the current save
    LoadBoxData();

    // Set up selection changed callback
    pokemonBox->SetOnSelectionChanged([this](int boxIndex, int slotIndex) {
        LOG_DEBUG("Box selection changed: Box " + std::to_string(boxIndex) + ", Slot " + std::to_string(slotIndex));
        // Entering a box re-reads it from the provider: edits made while it
        // was off screen leave its stored copy stale (LGPE party moves write
        // into distant boxes - the box list backs the party there)
        if (boxIndex != lastDisplayedBox) {
            lastDisplayedBox = boxIndex;
            RefreshBox(boxIndex);
        }
        UpdateHeldVisual();
    });

    pokemonBox->SetOnSlotActivated([this](int boxIndex, int slotIndex) {
        HandleSlotActivated(boxIndex, slotIndex);
    });

    pokemonBox->SetOnBoxNameActivated([this](int boxIndex) { HandleBoxNameActivated(boxIndex); });

    // The slot and hand verbs only apply while the grid itself is focused
    pokemonBox->SetOnFocusZoneChanged([this]() { UpdateHelpItems(); });

    // The held sprite renders above the box, following the cursor
    heldSprite = pksm::ui::SpriteImage::New(0, 0, BOX_ITEM_SIZE, BOX_ITEM_SIZE, 0);
    heldSprite->SetVisible(false);
    this->Add(heldSprite);

    // Set initial focus
    pokemonBox->RequestFocus();

    LOG_DEBUG("PokemonBox initialization complete");
}

void StorageScreen::HandleSlotActivated(int boxIndex, int slotIndex) {
    auto saveData = saveDataAccessor->getCurrentSaveData();
    if (!saveData) {
        return;
    }

    // This runs inside the slot's own A-press callback, so the grid may only
    // be updated in place - a rebuild would destroy the calling BoxItem
    if (!storageHand->HasHeldPokemon()) {
        if (storageHand->PickUpPokemon(saveData, boxIndex, slotIndex)) {
            LOG_DEBUG("Picked up from box " + std::to_string(boxIndex) + " slot " + std::to_string(slotIndex));
            RefreshAffectedSlots();
            UpdateHeldVisual();
        }
    } else {
        if (storageHand->PlaceDownPokemon(saveData, boxIndex, slotIndex)) {
            LOG_DEBUG("Placed at box " + std::to_string(boxIndex) + " slot " + std::to_string(slotIndex));
            RefreshAffectedSlots();
            UpdateHeldVisual();
        }
    }
}

void StorageScreen::HandleBoxNameActivated(int boxIndex) {
    auto saveData = saveDataAccessor->getCurrentSaveData();
    if (!saveData || !boxNameEditor->CanRenameBox(saveData, boxIndex)) {
        return;
    }
    const std::string currentName = boxDataProvider->GetBoxData(saveData, boxIndex).name;
    const auto entered =
        pksm::utils::ShowKeyboard("Box Name", currentName, boxNameEditor->GetBoxNameMaxLength(saveData));
    if (!entered) {
        return;
    }
    if (boxNameEditor->RenameBox(saveData, boxIndex, *entered)) {
        LOG_DEBUG("Renamed box " + std::to_string(boxIndex));
        // Re-read the name the save actually stored (its encoding may have
        // truncated or transliterated the input)
        pokemonBox->SetBoxName(boxIndex, boxDataProvider->GetBoxData(saveData, boxIndex).name);
    }
}

void StorageScreen::HandleBackButton() {
    if (storageHand->HasHeldPokemon()) {
        auto saveData = saveDataAccessor->getCurrentSaveData();
        // Read before the cancel: a successful CancelHold clears the hand
        const int originBox = storageHand->GetHeldOriginBox();
        if (storageHand->CancelHold(saveData)) {
            LOG_DEBUG("Cancelled pick-up, restored to origin");
            const int currentBox = pokemonBox->GetCurrentBox();
            RefreshBox(currentBox);
            if (originBox >= 0 && originBox != currentBox) {
                RefreshBox(originBox);
            }
            // A cancelled carry can also restore party membership badges
            // when the party box is neither current nor origin
            constexpr int partyBox = IBoxDataProvider::PARTY_BOX_INDEX;
            if (partyBox != currentBox && partyBox != originBox) {
                RefreshBox(partyBox);
            }
            UpdateHeldVisual();
        }
        return;
    }

    LOG_DEBUG("B button pressed, returning to main menu");
    if (onBack) {
        onBack();
    }
}

void StorageScreen::HandleCloneButton() {
    if (storageHand->HasHeldPokemon()) {
        return;
    }
    auto saveData = saveDataAccessor->getCurrentSaveData();
    if (!saveData) {
        return;
    }
    const int boxIndex = pokemonBox->GetCurrentBox();
    const int slotIndex = pokemonBox->GetSelectedSlot();
    if (storageHand->ClonePokemon(saveData, boxIndex, slotIndex)) {
        LOG_DEBUG("Cloned box " + std::to_string(boxIndex) + " slot " + std::to_string(slotIndex) + " into hand");
        UpdateHeldVisual();
    }
}

void StorageScreen::HandleReleaseButton() {
    auto saveData = saveDataAccessor->getCurrentSaveData();
    if (!saveData) {
        return;
    }

    if (storageHand->HasHeldPokemon()) {
        // A cloned hand has no Release verb - discarding the copy (B)
        // already covers it
        if (storageHand->IsHeldPokemonClone() || !storageHand->CanReleaseHeldPokemon(saveData)) {
            return;
        }
        if (requestConfirmation("Release Pokémon", "Release the held Pokémon? It will be gone for good.", "Release")) {
            if (storageHand->ReleaseHeldPokemon(saveData)) {
                LOG_DEBUG("Released held Pokémon");
                RefreshAffectedSlots();
                UpdateHeldVisual();
            }
        }
        return;
    }

    const int boxIndex = pokemonBox->GetCurrentBox();
    const int slotIndex = pokemonBox->GetSelectedSlot();
    // Covers empty slots and a Pokémon the party cannot spare
    if (!storageHand->CanReleasePokemon(saveData, boxIndex, slotIndex)) {
        return;
    }
    if (!requestConfirmation("Release Pokémon", "Release this Pokémon? It will be gone for good.", "Release")) {
        return;
    }
    if (storageHand->ReleasePokemon(saveData, boxIndex, slotIndex)) {
        LOG_DEBUG("Released box " + std::to_string(boxIndex) + " slot " + std::to_string(slotIndex));
        RefreshAffectedSlots();
        // The cursor now sits on an empty slot
        UpdateHelpItems();
    }
}

void StorageScreen::HandleSummaryButton() {
    if (storageHand->HasHeldPokemon()) {
        return;
    }
    auto saveData = saveDataAccessor->getCurrentSaveData();
    if (!saveData) {
        return;
    }
    auto summaryData =
        boxDataProvider->GetPokemonSummary(saveData, pokemonBox->GetCurrentBox(), pokemonBox->GetSelectedSlot());
    if (!summaryData) {
        return;
    }

    if (!summaryOverlay) {
        summaryOverlay = pksm::ui::PokemonSummaryOverlay::New(0, 0, GetWidth(), GetHeight());
    }
    summaryOverlay->SetData(std::move(*summaryData));
    onShowOverlay(summaryOverlay);
    isSummaryVisible = true;
    pokemonBox->SetDisabled(true);
}

void StorageScreen::HideSummary() {
    onHideOverlay();
    isSummaryVisible = false;
    pokemonBox->SetDisabled(false);
}

void StorageScreen::RefreshBox(int boxIndex) {
    auto saveData = saveDataAccessor->getCurrentSaveData();
    if (!saveData) {
        return;
    }
    // SetBoxData rebuilds the grid, which resets the cursor; keep it in place
    const int selectedSlot = pokemonBox->GetSelectedSlot();
    pokemonBox->SetBoxData(boxIndex, boxDataProvider->GetBoxData(saveData, boxIndex));
    if (boxIndex == pokemonBox->GetCurrentBox()) {
        pokemonBox->SetSelectedSlot(selectedSlot);
    }
}

void StorageScreen::RefreshAffectedSlots() {
    // In-place resync after an edit: party compaction and party-reference
    // moves can change slots and badges beyond the touched one, and this can
    // run inside a slot's own callback, where a grid rebuild would destroy
    // the caller - so update per slot, never rebuild
    auto saveData = saveDataAccessor->getCurrentSaveData();
    if (!saveData) {
        return;
    }
    constexpr int partyBox = IBoxDataProvider::PARTY_BOX_INDEX;
    const auto partyData = boxDataProvider->GetBoxData(saveData, partyBox);
    for (size_t slot = 0; slot < partyData.size(); slot++) {
        pokemonBox->SetPokemonData(partyBox, static_cast<int>(slot), partyData[slot]);
    }
    const int currentBox = pokemonBox->GetCurrentBox();
    if (currentBox != partyBox) {
        const auto boxData = boxDataProvider->GetBoxData(saveData, currentBox);
        for (size_t slot = 0; slot < boxData.size(); slot++) {
            pokemonBox->SetPokemonData(currentBox, static_cast<int>(slot), boxData[slot]);
        }
    }
}

void StorageScreen::UpdateHeldVisual() {
    if (storageHand->HasHeldPokemon()) {
        const auto held = storageHand->GetHeldPokemon();
        heldSprite->SetPokemonSprite(held.species, held.form, held.shiny);
        heldSprite->SetX(pokemonBox->GetSelectedItemX() + BOX_ITEM_SIZE / 3);
        heldSprite->SetY(pokemonBox->GetSelectedItemY() - BOX_ITEM_SIZE / 3);
        heldSprite->SetVisible(true);
    } else {
        heldSprite->SetVisible(false);
    }
    UpdateHelpItems();
}

void StorageScreen::UpdateHelpItems() {
    const auto saveData = saveDataAccessor->getCurrentSaveData();
    const bool held = storageHand->HasHeldPokemon();
    const bool clone = held && storageHand->IsHeldPokemonClone();

    // The screen contributes the verbs it handles; the box then appends its
    // own (box switching, navigation). A held Pokémon drops on any usable
    // slot; the empty-hand slot verbs also need an occupied one. A cloned
    // hand gets no Release (discarding the copy already covers it), and
    // neither hand nor slot get one for the party's last member.
    std::vector<pksm::ui::HelpItem> helpItems;
    if (pokemonBox->IsGridFocused() && held && pokemonBox->IsSelectedSlotUsable()) {
        helpItems = {
            {{{pksm::ui::global::ButtonGlyph::A}}, "Place / Swap"},
            {{{pksm::ui::global::ButtonGlyph::B}}, clone ? "Discard Copy" : "Put Back"},
        };
        if (!clone && storageHand->CanReleaseHeldPokemon(saveData)) {
            helpItems.push_back({{{pksm::ui::global::ButtonGlyph::Y}}, "Release"});
        }
    } else if (pokemonBox->IsGridFocused() && !held && pokemonBox->IsSelectedSlotOccupied()) {
        helpItems = {
            {{{pksm::ui::global::ButtonGlyph::A}}, "Pick Up"},
            {{{pksm::ui::global::ButtonGlyph::X}}, "Clone"},
        };
        if (storageHand->CanReleasePokemon(saveData, pokemonBox->GetCurrentBox(), pokemonBox->GetSelectedSlot())) {
            helpItems.push_back({{{pksm::ui::global::ButtonGlyph::Y}}, "Release"});
        }
        helpItems.push_back({{{pksm::ui::global::ButtonGlyph::Plus}}, "Summary"});
        helpItems.push_back({{{pksm::ui::global::ButtonGlyph::B}}, "Back"});
    } else {
        // No slot verbs apply here (header pill, Box Spaces button, empty or
        // unusable slot); the pill offers Rename where the save supports it
        if (pokemonBox->IsBoxNameFocused() &&
            boxNameEditor->CanRenameBox(saveData, pokemonBox->GetCurrentBox())) {
            helpItems.push_back({{{pksm::ui::global::ButtonGlyph::A}}, "Rename"});
        }
        helpItems.push_back(
            {{{pksm::ui::global::ButtonGlyph::B}}, held ? (clone ? "Discard Copy" : "Put Back") : "Back"}
        );
    }
    for (const auto& item : pokemonBox->GetHelpItems()) {
        helpItems.push_back(item);
    }
    helpFooter->SetHelpItems(helpItems);
}

void StorageScreen::LoadBoxData() {
    LOG_DEBUG("Loading box data from provider...");

    auto currentSave = saveDataAccessor->getCurrentSaveData();
    if (!currentSave) {
        LOG_DEBUG("No save data available, using fallback box data");
        // Set a default box count if no save data available
        pokemonBox->SetBoxCount(1);
        // Start at box 0
        pokemonBox->SetCurrentBox(0);
        LOG_DEBUG("Fallback box data loaded");
        return;
    }

    // Get box count from the provider
    size_t boxCount = boxDataProvider->GetBoxCount(currentSave);
    LOG_DEBUG("Setting box count to " + std::to_string(boxCount));
    pokemonBox->SetBoxCount(boxCount);

    // Load all boxes at once to ensure the box data provider knows about them
    for (size_t i = 0; i < boxCount; ++i) {
        auto boxData = boxDataProvider->GetBoxData(currentSave, i);
        pokemonBox->SetBoxData(i, boxData);
    }

    // Start at box 0
    pokemonBox->SetCurrentBox(0);

    LOG_DEBUG("Box data loaded successfully");
}

StorageScreen::~StorageScreen() = default;

void StorageScreen::OnInput(u64 down, u64 up, u64 held) {
    // The summary overlay swallows all input until dismissed
    if (isSummaryVisible) {
        if (down & (HidNpadButton_B | HidNpadButton_Plus)) {
            HideSummary();
        }
        return;
    }

    // First handle help-related input
    if (HandleHelpInput(down)) {
        return;  // Input was handled by help system
    }

    // Process button inputs
    pokemonBoxDirectionalHandler.HandleInput(down, held);
    buttonHandler.HandleInput(down, up, held);
}

std::vector<pksm::ui::HelpItem> StorageScreen::GetHelpOverlayItems() const {
    const auto saveData = saveDataAccessor->getCurrentSaveData();
    const bool held = storageHand->HasHeldPokemon();
    const bool clone = held && storageHand->IsHeldPokemonClone();

    // Composed like the footer: the screen's verbs, then the box's own
    std::vector<pksm::ui::HelpItem> items;
    if (pokemonBox->IsGridFocused() && held && pokemonBox->IsSelectedSlotUsable()) {
        items = {
            {{{pksm::ui::global::ButtonGlyph::A}}, "Place / Swap Pokémon"},
            {{{pksm::ui::global::ButtonGlyph::B}}, clone ? "Discard the Copy" : "Put Back"},
        };
        if (!clone && storageHand->CanReleaseHeldPokemon(saveData)) {
            items.push_back({{{pksm::ui::global::ButtonGlyph::Y}}, "Release Held Pokémon"});
        }
    } else if (pokemonBox->IsGridFocused() && !held && pokemonBox->IsSelectedSlotOccupied()) {
        items = {
            {{{pksm::ui::global::ButtonGlyph::A}}, "Pick Up Pokémon"},
            {{{pksm::ui::global::ButtonGlyph::X}}, "Clone into Hand"},
        };
        if (storageHand->CanReleasePokemon(saveData, pokemonBox->GetCurrentBox(), pokemonBox->GetSelectedSlot())) {
            items.push_back({{{pksm::ui::global::ButtonGlyph::Y}}, "Release Pokémon"});
        }
        items.push_back({{{pksm::ui::global::ButtonGlyph::Plus}}, "View Summary"});
        items.push_back({{{pksm::ui::global::ButtonGlyph::B}}, "Back to Main Menu"});
    } else {
        if (pokemonBox->IsBoxNameFocused() &&
            boxNameEditor->CanRenameBox(saveData, pokemonBox->GetCurrentBox())) {
            items.push_back({{{pksm::ui::global::ButtonGlyph::A}}, "Rename Box"});
        }
        items.push_back(
            {{{pksm::ui::global::ButtonGlyph::B}},
             held ? (clone ? "Discard the Copy" : "Put Back") : "Back to Main Menu"}
        );
    }
    for (const auto& item : pokemonBox->GetHelpItems()) {
        items.push_back(item);
    }
    items.push_back({{{pksm::ui::global::ButtonGlyph::Minus}}, "Close Help"});
    return items;
}

void StorageScreen::OnHelpOverlayShown() {
    LOG_DEBUG("Help overlay shown, disabling UI elements");
    pokemonBox->SetDisabled(true);
}

void StorageScreen::OnHelpOverlayHidden() {
    LOG_DEBUG("Help overlay hidden, re-enabling UI elements");
    pokemonBox->SetDisabled(false);
}

}  // namespace pksm::layout