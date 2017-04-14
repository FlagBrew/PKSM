@echo off
rem ###########################################################
rem ### This script get all Korean characters used in Bold.ttf
rem ### File korean-chars.txt is created
rem ### 
rem ### Unicode are written in decimal and not in hexadecimal
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
echo Exporting all korean characters...
echo ==================================
"%ProgFiles86Root%\FontForgeBuilds\bin\fontforge.exe" -nosplash -script pe-scripts\get-korean-characters-used.pe %FONTPATH% > korean-chars.txt
echo ==================================
echo Characters exported to korean-chars.txt !
pause

