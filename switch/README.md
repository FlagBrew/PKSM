# PKSM Switch Port

This is the Nintendo Switch port of PKSM, a Pokémon save manager and editor.

## Most Sections Still Under Construction

## Font Management

PKSM uses a custom font system that supports multiple font weights and sizes. Here's how to manage fonts in the application:

### Adding a New Font

1. Add your font file (TTF format) to `romfs/gfx/ui/`

2. Define a font name generator in `include/gui/UIConstants.hpp`:

   ```cpp
   // Example for a new italic font
   inline std::string MakeItalicFontName(const u32 font_size) {
       return "ItalicFont@" + std::to_string(font_size);
   }
   ```

3. If you need a new font size, add it to `UIConstants.hpp`:

   ```cpp
   // Custom font sizes
   static constexpr u32 FONT_SIZE_TITLE = 60;
   static constexpr u32 FONT_SIZE_HEADER = 45;
   static constexpr u32 MY_NEW_SIZE = 32;  // Add your new size here
   ```

4. Register your font in one of two ways:

   a. For fonts that should be available at all sizes (default font):

   ```cpp
   // In PKSMApplication::ConfigureFonts
   renderer_opts.AddDefaultFontPath("romfs:/gfx/ui/my_font.ttf");
   ```

   b. For fonts that need specific sizes or weights:

   ```cpp
   // In PKSMApplication::RegisterAdditionalFonts
   auto my_font = std::make_shared<pu::ttf::Font>(UIConstants::MY_NEW_SIZE);
   my_font->LoadFromFile("romfs:/gfx/ui/my_font.ttf");
   pu::ui::render::AddFont(UIConstants::MakeItalicFontName(UIConstants::MY_NEW_SIZE), my_font);
   ```

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
3. Register specific weight/style fonts only for the sizes you need
4. Keep font files in the `romfs/gfx/ui/` directory
5. Follow the naming convention: `{style}Font@{size}` for font names
