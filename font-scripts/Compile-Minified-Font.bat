@echo off
rem ###########################################################
rem ### This script adds the Japanese and Chinese characters used in Pokémon nicknames and i18n files
rem ### so the font can be the smallest size possible
rem ### 
rem ### The font used with all Japanese and Chinese characters is DroidSansFallback-Full.ttf
rem ### The characters are small, it's why we are scaling by 4x (the 400 value)
rem ### 
rem ### The font used with all Korean chars characters is NanumGothicBold.ttf
rem ### 
rem ###########################################################


rem ### Variables
rem #############

SET FONTPATH=..\assets\romfs\res\Bold.ttf
SET KORFONT=fonts\KOR.ttf
SET ZHJPFONT=fonts\ZH-JP.ttf
SET TEMPFONT=Bold-Modified.ttf

rem ### Setting the Program Files x86 folder for FontForge
rem #############

SET ProgFiles86Root=%ProgramFiles(x86)%
IF NOT "%ProgFiles86Root%"=="" GOTO win64
SET ProgFiles86Root=%ProgramFiles%
:win64

rem ### Script beginning
rem #############

rem ### Generating FontForge PE scripts from JP, ZH and KO folders

GeneratePEImportUnicodeScript.exe ..\assets\romfs\i18n\jp 0x4e00 0x9fff pe-scripts\add-chars.pe.tpl > pe-scripts\add-jp-chars.pe
GeneratePEImportUnicodeScript.exe ..\assets\romfs\i18n\zh 0x4e00 0x9fff pe-scripts\add-chars.pe.tpl > pe-scripts\add-zh-chars.pe
GeneratePEImportUnicodeScript.exe ..\assets\romfs\i18n\ko 0x3131 0x318e pe-scripts\add-chars.pe.tpl > pe-scripts\add-ko-base-chars.pe
GeneratePEImportUnicodeScript.exe ..\assets\romfs\i18n\ko 0xac00 0xd7a3 pe-scripts\add-chars.pe.tpl > pe-scripts\add-ko-chars.pe

rem ### Removing all Chinese and Japanese characters present in the Font so we can start at 0
echo.
echo =================================
echo Removing all Kanji (0x4e00 to 0x9fff)...
echo =================================
echo.
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\remove-chars.pe %FONTPATH% 4e00 9fff %TEMPFONT%
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\remove-chars.pe %TEMPFONT% e800 ee1d %TEMPFONT%

rem ### Removing all Korean characters present in the Font so we can start at 0
echo.
echo =================================
echo Removing all Hangul (0xac00 to d7a3, 0x3131 to 0x318e)...
echo =================================
echo.
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\remove-chars.pe %TEMPFONT% ac00 d7a3 %TEMPFONT%
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\remove-chars.pe %TEMPFONT% 3131 318e %TEMPFONT%

rem ### Adding Pokémon Chinese characters and reference
echo.
echo =================================
echo Adding Pokemon Nicknames chars...
echo =================================
echo.
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\add-all-zh-chars-pkmn.pe %ZHJPFONT% %TEMPFONT% %TEMPFONT% 400

rem ### Adding Pokémon Korean characters
echo.
echo =================================
echo Adding Pokemon Nicknames chars...
echo =================================
echo.
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\add-all-ko-chars-pkmn.pe %KORFONT% %TEMPFONT% %TEMPFONT% 100 50

pause

rem ### Adding the Japanese chars to the font
echo.
echo =================================
echo Adding JP chars...
echo =================================
echo.
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\add-jp-chars.pe %ZHJPFONT% %TEMPFONT% %TEMPFONT% 400

rem ### Adding the Chinese chars to the font
echo.
echo =================================
echo Adding ZH chars...
echo =================================
echo.
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\add-zh-chars.pe %ZHJPFONT% %TEMPFONT% %TEMPFONT% 400

rem ### Adding the Korean chars to the font
echo.
echo =================================
echo Adding KOR chars...
echo =================================
echo.
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\add-ko-base-chars.pe %KORFONT% %TEMPFONT% %TEMPFONT% 100 50
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\add-ko-chars.pe %KORFONT% %TEMPFONT% %TEMPFONT% 100 50
echo =================================

Del pe-scripts\add-jp-chars.pe
Del pe-scripts\add-zh-chars.pe
Del pe-scripts\add-ko-chars.pe
Del pe-scripts\add-ko-base-chars.pe

echo.
echo.
set /p replace= Do you want to open the font in FontForge (Y/N) ? 
IF /I "%replace%"=="Y" (
	"%ProgFiles86Root%\FontForgeBuilds\run_fontforge.exe" %TEMPFONT%
)
echo.
set /p replace= Do you want to replace the font in the assets (Y/N) ? 
IF /I "%replace%"=="Y" (
	copy %TEMPFONT% %FONTPATH%
	Del %TEMPFONT%
)
pause

