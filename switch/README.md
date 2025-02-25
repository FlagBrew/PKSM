# PKSM Switch Port

This is the Nintendo Switch port of PKSM, a Pokémon save manager and editor.

## Application Architecture

PKSM for Switch is built using the Plutonium UI framework, which is based on SDL2. The application follows a modular architecture with clear separation of concerns:

### Core Components

1. **PKSMApplication**: The main application class that initializes the framework, manages screens, and handles global state.

2. **Screens**: Full-screen UI layouts that represent different parts of the application:

   - `TitleLoadScreen`: The game selection screen (shown in the screenshot)
   - `MainMenu`: The main menu screen with various tools and options

3. **UI Components**: Reusable UI elements that make up the screens:

   - `GameList`: Manages the display of available Pokémon games
   - `SaveList`: Shows available save files for the selected game
   - `FocusableButton`: Interactive button with focus and touch support
   - `UserIconButton`: Button for selecting the current user account

4. **Data Providers**: Abstract interfaces for accessing game and save data:
   - `ITitleDataProvider`: Provides access to game titles (installed, custom, emulator)
   - `ISaveDataProvider`: Provides access to save files for each title
   - `AccountManager`: Manages Switch user accounts and selection

### Component Hierarchy

```
PKSMApplication
├── TitleLoadScreen
│   ├── GameList
│   │   ├── ConsoleGameList (installed games)
│   │   ├── CustomGameList (custom games)
│   │   └── EmulatorGameList (emulator games)
│   ├── SaveList (saves for selected game)
│   ├── LoadButton
│   ├── WirelessButton
│   └── UserIconButton
└── MainMenu
    └── (Various tools and options)
```

### Focus Management Hierarchy

```
TitleLoadScreen FocusManager
├── GameList FocusManager
│   ├── ConsoleGameList FocusManager
│   ├── CustomGameList FocusManager
│   └── EmulatorGameList FocusManager
├── SaveList
├── LoadButton
├── WirelessButton
└── UserIconButton
```

### Navigation Flow

1. The application starts with the `TitleLoadScreen`
2. Users can navigate between different game lists using L/R triggers
3. Users can select a game from the grid using directional inputs or touch
4. After selecting a game, focus moves to the save list at the bottom
5. Users can select a save file and then choose to load it or use wireless features
6. After loading a save, the application transitions to the `MainMenu`

### Data Flow

1. `ITitleDataProvider` provides game titles to the `GameList`
2. When a game is selected, `ISaveDataProvider` provides save files to the `SaveList`
3. When a save is loaded, the application transitions to the `MainMenu` with the loaded save data

### Input Handling

1. Touch input is processed by each component to handle selection and focus
2. Directional input is managed through the focus system for navigation
3. Button inputs (A, B, etc.) are handled by the focused component
4. Special inputs (L/R triggers, ZL/ZR) are handled at the screen level

## Most Sections Still Under Construction

## Font Management

PKSM uses a custom font management system that supports multiple font weights, styles, and sizes. The system is built around the `FontManager` class which handles font registration and retrieval.

### Font System Architecture

1. **FontManager Class**:

   - Centralized management of all application fonts
   - Automatic registration of fonts at all defined sizes
   - Consistent naming convention for font retrieval

2. **Font Naming Convention**:
   - Fonts are identified by `{style}Font@{size}` (e.g., `HeavyFont@60`)
   - Default font is accessed via `pu::ui::MakeDefaultFontName(size)`
   - Custom fonts have dedicated name generators (e.g., `MakeHeavyFontName`)

### Adding a New Font Size

1. Add your new font size to `include/gui/shared/UIConstants.hpp`:

   ```cpp
   namespace pksm::ui::global {
       // Custom font sizes
       static constexpr u32 FONT_SIZE_TITLE = 60;
       static constexpr u32 FONT_SIZE_HEADER = 45;
       static constexpr u32 MY_NEW_SIZE = 32;  // Add your new size here
   }
   ```

2. The `FontManager` will automatically register all existing fonts at this new size during initialization.

### Adding a New Font Style

1. Add your font file (TTF format) to `romfs/gfx/fonts/`

2. Define a font name generator in `include/gui/shared/UIConstants.hpp`:

   ```cpp
   namespace pksm::ui::global {
       // Example for a new italic font
       inline std::string MakeItalicFontName(const u32 font_size) {
           return "ItalicFont@" + std::to_string(font_size);
       }
   }
   ```

3. Register your font in `PKSMApplication::RegisterAdditionalFonts`:

   ```cpp
   void PKSMApplication::RegisterAdditionalFonts() {
       // Register italic font for all custom sizes
       pksm::ui::FontManager::RegisterFont(
           "romfs:/gfx/fonts/my_italic_font.ttf",
           pksm::ui::global::MakeItalicFontName
       );
   }
   ```

   The `FontManager` will automatically register your font for all custom sizes defined in `UIConstants`.

### Font Registration Process

1. **Default Font**:

   - Registered in `PKSMApplication::ConfigureFonts`
   - Available at all sizes through Plutonium's font system
   - Used as fallback when custom fonts aren't available

   ```cpp
   void PKSMApplication::ConfigureFonts(pu::ui::render::RendererInitOptions& renderer_opts) {
       // Register default (light) font
       renderer_opts.AddDefaultFontPath("romfs:/gfx/fonts/dinnextw1g_light.ttf");

       // Configure font sizes
       pksm::ui::FontManager::ConfigureRendererFontSizes(renderer_opts);
   }
   ```

2. **Custom Fonts**:

   - Registered after romfs is mounted in `PKSMApplication::RegisterAdditionalFonts`
   - Each font is registered with a name generator function
   - The `FontManager` creates instances at all defined sizes

   ```cpp
   void PKSMApplication::RegisterAdditionalFonts() {
       // Register heavy font for all custom sizes
       pksm::ui::FontManager::RegisterFont(
           "romfs:/gfx/fonts/dinnextw1g_heavy.ttf",
           pksm::ui::global::MakeHeavyFontName
       );

       // Register medium font for all custom sizes
       pksm::ui::FontManager::RegisterFont(
           "romfs:/gfx/fonts/dinnextw1g_medium.ttf",
           pksm::ui::global::MakeMediumFontName
       );
   }
   ```

### Using Fonts in UI Elements

To use a font in a UI element:

```cpp
// Using the default (light) font
textBlock->SetFont(pu::ui::MakeDefaultFontName(pksm::ui::global::FONT_SIZE_MEDIUM));

// Using the heavy font
textBlock->SetFont(pksm::ui::global::MakeHeavyFontName(pksm::ui::global::FONT_SIZE_TITLE));

// Using the medium font
textBlock->SetFont(pksm::ui::global::MakeMediumFontName(pksm::ui::global::FONT_SIZE_BUTTON));

// Using the Switch button font (for controller button glyphs)
textBlock->SetFont(pksm::ui::global::MakeSwitchButtonFontName(pksm::ui::global::FONT_SIZE_SMALL));
```

### Font Usage Guidelines

1. **Semantic Usage**:

   - Use `FONT_SIZE_TITLE` for main screen titles
   - Use `FONT_SIZE_HEADER` for section headers
   - Use `FONT_SIZE_BUTTON` for button text
   - Use `FONT_SIZE_BODY` for regular content text

2. **Weight Selection**:

   - Use heavy font for emphasis and titles
   - Use medium font for buttons and important UI elements
   - Use light font (default) for most text content
   - Use Switch button font only for controller button glyphs

3. **Performance Considerations**:
   - Font rendering is expensive, especially at large sizes
   - Minimize the number of unique font/size combinations
   - Reuse text elements when possible instead of creating new ones
   - Consider using texture caching for frequently used text

## Touch, Focus, and Directional Input

PKSM uses a comprehensive input system that combines touch, focus management, and directional input. Here's how to implement these features in your UI components:

### Focus Management System

PKSM implements a hierarchical focus management system that handles focus transitions between components:

1. **Focus Manager Hierarchy**:

   - The application uses a tree of `FocusManager` instances
   - Each screen has a root focus manager
   - Complex components have their own child focus managers
   - Focus managers handle registration and focus transitions between components

2. **Implementing a Focusable Component**:

```cpp
class MyComponent : public pu::ui::elm::Element, public IFocusable {
private:
    bool focused = false;
    std::shared_ptr<input::FocusManager> focusManager;

public:
    // IFocusable implementation
    void SetFocused(bool focus) override {
        focused = focus;
        // Update visual state based on focus
    }

    bool IsFocused() const override {
        return focused;
    }

    void SetFocusManager(std::shared_ptr<input::FocusManager> manager) override {
        focusManager = manager;
    }

    // Request focus through the focus manager
    void RequestFocus() {
        if (focusManager) {
            focusManager->RequestFocus(shared_from_this());
        }
    }
}
```

3. **Setting Up Focus Managers**:

```cpp
// Create focus managers
rootFocusManager = pksm::input::FocusManager::New("Root Manager");
rootFocusManager->SetActive(true);  // Root manager should be active
childFocusManager = pksm::input::FocusManager::New("Child Manager");

// Set up hierarchy
rootFocusManager->RegisterChildManager(childFocusManager);

// Register focusable components
rootFocusManager->RegisterFocusable(myComponent);
childFocusManager->RegisterFocusable(myChildComponent);
```

4. **Focus Transitions**:

```cpp
// Request focus on a component
myComponent->RequestFocus();

// Handle focus resignation
void OnInput(u64 down, u64 up, u64 held) {
    if (myComponent->IsFocused()) {
        if (down & HidNpadButton_Down) {
            // Focus will be automatically resigned from current component
            myOtherComponent->RequestFocus();
        }
    }
}
```

### Adding Touch Support

1. Override the `OnInput` method to handle touch events:

```cpp
void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override {
    if (!touch_pos.IsEmpty() &&
        touch_pos.HitsRegion(this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight())) {
        // Request focus when touched
        this->RequestFocus();

        // Handle touch selection
        if (onTouchSelectCallback) {
            onTouchSelectCallback();
        }
    }
}
```

### Implementing Directional Input

1. Create a `DirectionalInputHandler` for your component:

```cpp
private:
    DirectionalInputHandler inputHandler;
```

2. Set up movement callbacks in your constructor:

```cpp
// Set up directional movement with focus transitions
inputHandler.SetOnMoveLeft([this]() {
    // Either handle internal movement or resign focus
    if (atLeftEdge) {
        leftComponent->RequestFocus();
    } else {
        // Handle internal movement
    }
});

inputHandler.SetOnMoveRight([this]() { /* Similar logic */ });
inputHandler.SetOnMoveUp([this]() { /* Similar logic */ });
inputHandler.SetOnMoveDown([this]() { /* Similar logic */ });
```

3. Handle input in your `OnInput` method:

```cpp
void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override {
    if (IsFocused()) {
        // Let the directional handler process input first
        if (inputHandler.HandleInput(keys_down, keys_held)) {
            // Input was handled by directional handler
            return;
        }

        // Handle other button inputs
        if (keys_down & HidNpadButton_A) {
            // Handle A button
        }
    }

    // Handle touch input regardless of focus state
    if (!touch_pos.IsEmpty()) {
        // Process touch input
    }
}
```

### Best Practices

1. **Focus Management**:

   - Use the `FocusManager` hierarchy for all focusable components
   - Call `RequestFocus()` instead of directly setting focus
   - Implement clear focus transitions between components
   - Use visual feedback to indicate focus state
   - Consider both selection and focus states for complex components

2. **Focus Resignation**:

   - Components should know when to resign focus (e.g., at edges)
   - Implement methods like `ShouldResignUpFocus()` for edge detection
   - Let parent components handle focus transitions between siblings

3. **Touch Input**:

   - Process touch input regardless of focus state to allow touch-to-focus
   - Request focus when a component is touched
   - Provide immediate visual feedback for touch interactions
   - Consider both tap and drag gestures if needed

4. **Directional Input**:

   - Use the `DirectionalInputHandler` for consistent behavior
   - Handle both d-pad and analog stick input
   - Implement logical navigation patterns
   - Consider edge cases (e.g., moving beyond boundaries)

5. **Input Debouncing**:

   - **Why Debounce?**

     - Prevents multiple actions from being triggered by a single input
     - Improves user experience by avoiding unintended double-activations
     - Ensures consistent behavior across different input methods

   - **Touch Input Debouncing**:

     - Always implement debouncing for touch input in interactive components
     - Use a time-based approach with `SDL_GetTicks64()`
     - Example implementation:

       ```cpp
       // In class definition
       u64 lastTouchTime = 0;
       static constexpr u64 TOUCH_DEBOUNCE_TIME = 160;

       // In OnInput method
       if (!touch_pos.IsEmpty() && touch_pos.HitsRegion(...)) {
           u64 currentTime = SDL_GetTicks64();
           if (currentTime - lastTouchTime >= TOUCH_DEBOUNCE_TIME) {
               lastTouchTime = currentTime;
               // Process the touch input
               if (onTouchCallback) {
                   onTouchCallback();
               }
           }
       }
       ```

   - **Directional Input Debouncing**:

     - The `DirectionalInputHandler` already implements debouncing
     - It uses a state machine approach with timing for both D-pad and analog sticks
     - Different wait times are used for initial and repeated movements
     - Simply use the handler in your components:

       ```cpp
       // In class definition
       DirectionalInputHandler inputHandler;

       // In OnInput method
       if (IsFocused() && inputHandler.HandleInput(keys_down, keys_held)) {
           return;  // Input was handled
       }
       ```

   - **When to Implement Custom Debouncing**:
     - When a component handles multiple sequential touch actions (like TriggerButton)
     - For components that need to distinguish between different types of touches
     - When implementing custom input handling not covered by existing handlers
     - For any input that could trigger significant state changes or screen transitions

### Example Components

For reference implementations, see:

- `TitleLoadScreen`: Example of a screen with multiple focusable regions
- `GameList`: Complex component with focus manager hierarchy
- `FocusableButton`: Simple focusable component with touch support
- `UserIconButton`: Component that handles focus and touch selection
- `TriggerButton`: Example of touch debouncing for multiple callbacks

## Contributing Guidelines

If you're interested in contributing to the PKSM Switch port, please follow these guidelines to ensure your code integrates well with the existing codebase.

### Code Style and Organization

1. **Namespaces**:

   - Use the `pksm` namespace for all code
   - Use sub-namespaces to organize code by functionality:
     - `pksm::ui` for UI components
     - `pksm::layout` for screen layouts
     - `pksm::data` for data providers and models
     - `pksm::input` for input handling
     - `pksm::utils` for utility functions

2. **Focus and Input**:

   - Follow the focus management patterns described earlier
   - Handle both touch and directional input
   - Provide clear visual feedback for focus and selection states

3. **Input Debouncing**:

   - **Touch Input Debouncing**:

     - Always implement debouncing for touch input in interactive components
     - Use `SDL_GetTicks64()` to track time between touches
     - Store the last touch time as a member variable in your component
     - Only process touch input if enough time has passed since the last touch
     - Example pattern:

       ```cpp
       // Member variables
       u64 lastTouchTime = 0;
       static constexpr u64 TOUCH_DEBOUNCE_TIME = 160;  // About 10 frames at 60fps

       // In OnInput method
       u64 currentTime = SDL_GetTicks64();
       if (currentTime - lastTouchTime >= TOUCH_DEBOUNCE_TIME) {
           lastTouchTime = currentTime;
           // Process the touch input
       }
       ```

   - **Directional Input Debouncing**:

     - Use the `DirectionalInputHandler` class for consistent directional input handling
     - The handler already implements debouncing for both D-pad and analog stick input
     - Set up movement callbacks using the handler's methods:
       ```cpp
       directionalHandler.SetOnMoveLeft([this]() { /* Handle left movement */ });
       directionalHandler.SetOnMoveRight([this]() { /* Handle right movement */ });
       directionalHandler.SetOnMoveUp([this]() { /* Handle up movement */ });
       directionalHandler.SetOnMoveDown([this]() { /* Handle down movement */ });
       ```
     - In your `OnInput` method, delegate directional input handling:
       ```cpp
       if (IsFocused() && directionalHandler.HandleInput(keys_down, keys_held)) {
           return;  // Input was handled by the directional handler
       }
       ```

   - **Button Input Debouncing**:
     - For button inputs that trigger significant actions, consider implementing debouncing
     - Use the same time-based approach as touch input debouncing
     - This is especially important for buttons that change screens or modify data

4. **Performance**:
   - Minimize UI updates and redraws
   - Cache rendered elements when possible
   - Be mindful of texture memory usage
