# PKSM Switch Port

This is the Nintendo Switch port of PKSM, a Pokémon save manager and editor.

## Most Sections Still Under Construction

## Font Management

PKSM uses a custom font system that supports multiple font weights and sizes. Here's how to manage fonts in the application:

### Adding a New Font Size

1. Add your new font size to `include/gui/UIConstants.hpp`:

   ```cpp
   // Custom font sizes
   static constexpr u32 FONT_SIZE_TITLE = 60;
   static constexpr u32 FONT_SIZE_HEADER = 45;
   static constexpr u32 MY_NEW_SIZE = 32;  // Add your new size here
   ```

   The size will automatically be registered for all custom fonts through the `FontManager`.

### Adding a New Font Style

1. Add your font file (TTF format) to `romfs/gfx/ui/`

2. Define a font name generator in `include/gui/UIConstants.hpp`:

   ```cpp
   // Example for a new italic font
   inline std::string MakeItalicFontName(const u32 font_size) {
       return "ItalicFont@" + std::to_string(font_size);
   }
   ```

3. Register your font in one of two ways:

   a. For fonts that should be available at all sizes (default font):

   ```cpp
   // In PKSMApplication::ConfigureFonts
   renderer_opts.AddDefaultFontPath("romfs:/gfx/ui/my_font.ttf");
   ```

   b. For custom weight/style fonts:

   ```cpp
   // In PKSMApplication::RegisterAdditionalFonts
   gui::FontManager::RegisterFont(
       "romfs:/gfx/ui/my_font.ttf",
       UIConstants::MakeItalicFontName  // Your font name generator
   );
   ```

   The `FontManager` will automatically register your font for all custom sizes defined in `UIConstants`.

### Using Fonts in UI Elements

To use a font in a UI element:

```cpp
// Using the default (light) font
textBlock->SetFont(pu::ui::MakeDefaultFontName(UIConstants::FONT_SIZE_MEDIUM));

// Using the heavy font
textBlock->SetFont(UIConstants::MakeHeavyFontName(UIConstants::FONT_SIZE_TITLE));

// Using your custom font
textBlock->SetFont(UIConstants::MakeItalicFontName(UIConstants::MY_NEW_SIZE));
```

### Best Practices

1. Use predefined font sizes from `UIConstants.hpp` whenever possible
2. Register commonly used fonts as default fonts
3. Keep font files in the `romfs/gfx/ui/` directory
4. Follow the naming convention: `{style}Font@{size}` for font names
5. Use the `FontManager` for registering custom weight/style fonts to ensure consistency across all sizes

## Touch, Focus, and Directional Input

PKSM uses a comprehensive input system that combines touch, focus management, and directional input. Here's how to implement these features in your UI components:

### Making a Component Focusable

1. Implement the `IFocusable` interface:

```cpp
class MyComponent : public pu::ui::elm::Element, public IFocusable {
private:
    bool focused;

public:
    // IFocusable implementation
    void SetFocused(bool focus) override {
        focused = focus;
        // Update visual state based on focus
    }

    bool IsFocused() const override {
        return focused;
    }
};
```

2. Add visual feedback for focus state:
   - Use `PulsingOutline` for a pulsing selection border
   - Adjust colors, opacity, or other visual properties

### Adding Touch Support

1. Override the `OnInput` method to handle touch events:

```cpp
void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override {
    if (!touch_pos.IsEmpty() &&
        touch_pos.HitsRegion(this->GetX(), this->GetY(), this->GetWidth(), this->GetHeight())) {
        // Handle touch input
        if (onTouchSelectCallback) {
            onTouchSelectCallback();
        }
    }
}
```

2. Add a touch selection callback:

```cpp
private:
    std::function<void()> onTouchSelectCallback;

public:
    void SetOnTouchSelect(std::function<void()> callback) {
        onTouchSelectCallback = callback;
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
inputHandler.SetOnMoveLeft([this]() { /* Handle left movement */ });
inputHandler.SetOnMoveRight([this]() { /* Handle right movement */ });
inputHandler.SetOnMoveUp([this]() { /* Handle up movement */ });
inputHandler.SetOnMoveDown([this]() { /* Handle down movement */ });
```

3. Handle input in your `OnInput` method:

```cpp
void OnInput(const u64 keys_down, const u64 keys_up, const u64 keys_held, const pu::ui::TouchPoint touch_pos) override {
    if (focused) {
        inputHandler.HandleInput(keys_down, keys_held);
    }
    // Handle touch input regardless of focus state
    if (!touch_pos.IsEmpty()) {
        // Process touch input even when not focused
        // This allows for touch-to-focus behavior
    }
}
```

### Best Practices

1. Focus Management:

   - Only handle directional input when the component is focused
   - Implement clear focus transitions between components
   - Use visual feedback to indicate focus state
   - Consider both selection and focus states for complex components

2. Touch Input:

   - Process touch input regardless of focus state to allow touch-to-focus
   - Check touch position against component bounds
   - Provide immediate visual feedback for touch interactions
   - Consider both tap and drag gestures if needed
   - Avoid duplicate notifications when handling touch events

3. Directional Input:

   - Use the `DirectionalInputHandler` for consistent behavior
   - Handle both d-pad and analog stick input
   - Implement logical navigation patterns
   - Consider edge cases (e.g., moving beyond boundaries)

4. Component Integration:
   - Combine focus, touch, and directional input cohesively
   - Use callbacks to notify parent components of input events
   - Follow existing patterns in the codebase (see `GameList`, `FocusableMenu`, etc.)
   - Maintain clear parent-child relationships for input propagation

### Example Components

For reference implementations, see:

- `FocusableButton`: Simple focusable component with touch support
- `FocusableMenu`: List-based component with focus and touch
- `GameList`: Complex component with multi-item focus and touch handling
- `GameGrid`: Grid-based component with advanced focus and selection states
