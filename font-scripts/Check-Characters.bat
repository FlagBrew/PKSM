@echo off
rem ###########################################################
rem ### This script adds the Japanese and Chinese characters used in Pokémon nicknames and i18n files
rem ### so the font can be the smallest size possible
rem ### 
rem ### The font used with all Japanese and Chinese characters is DroidSansFallback-Full.ttf
rem ### The characters are small, it's why we are scaling by 4x (the 400 value)
rem ###
rem ###########################################################

rem ### Variables
rem #############

SET FONTPATH=..\assets\romfs\res\Bold.ttf

rem ### Setting the Program Files x86 folder for FontForge
rem #############

SET ProgFiles86Root=%ProgramFiles(x86)%
IF NOT "%ProgFiles86Root%"=="" GOTO win64
SET ProgFiles86Root=%ProgramFiles%
:win64

rem ### Script beginning
rem #############

rem ### Check if all characters are present in the font
echo Checking Pokemon Nickname chars...
echo ==================================
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -script pe-scripts\check-pokemon-nickname-characters.pe %FONTPATH%
echo ==================================
pause

