![PKSM](https://raw.githubusercontent.com/FlagBrew/PKSM/master/assets/banner.png)

## Notice

PKSM is in the process of being rewritten to a different language. The repo won't be actively updated for the time being and pull requests may get ignored. If anyone wishes to help with the rewrite they may join the Discord server and provide proof of coding experience(pokemon and homebrew related would be highly appreciated).

![License](https://img.shields.io/badge/License-GPLv3-blue.svg)

Multipurpose and portable save manager for Generations IV to VII, programmed in C.

* Supports original cartridges and digital copies of games from DPPt to USUM
* Allows on-the-fly modifications to all of your data
* Allows internal and resizable offline storage to store your Pokémon
* Capable of running custom scripts to allow injection of arbitrary data into your saves
* Interfaces with multiple ad-hoc programs running on your computer, web browser or smartphone
* Capable of scanning QR codes to inject both .pkx files or event wondercards
* Contains an offline wondercard database to get events from old distributions

**I do not support or condone cheating at the expense of others. Do not use significantly edited Pokémon in battle or in trades with those who are unaware edited Pokémon are in use.**

## Screenshots

![](https://i.imgur.com/Rj3OwBZ.png) ![](https://i.imgur.com/Nl5z9Yx.png)
![](https://i.imgur.com/2Vt1dYE.png) ![](https://i.imgur.com/2Vt1dYE.png)
![](https://i.imgur.com/a355RYx.png) ![](https://i.imgur.com/EyTwxIc.png)
![](https://i.imgur.com/i3dLv54.png) ![](https://i.imgur.com/Oyr5zsF.png)
![](https://i.imgur.com/e9I6UoK.png) ![](https://i.imgur.com/8yYEfnX.png)
![](https://i.imgur.com/qVz2vba.png) ![](https://i.imgur.com/Wg1m4Ws.png)
![](https://i.imgur.com/8bPKwNe.png) ![](https://i.imgur.com/OdjkZwG.png)

## Installation

This software works on CFW and *hax/Rosalina-based Homebrew Launchers. An internet connection may be required at the first launch.

| Entrypoint | Instructions |
| :--------: | :----------- |
| CFW | Install the `PKSM.cia` file provided in the release page with your favourite installer. You can now launch the application from the Home Menu |
| Rosalina HBL | Move `PKSM.3dsx` from the rosalina folder in the `PKSM.zip` file. You can now launch PKSM from a [Rosalina-based Homebrew Launcher](https://github.com/fincs/new-hbmenu) |
| *hax HBL | Move `PKSM.3dsx` and `PKSM.xml` from the standard folder in the `PKSM.zip` file. You can now launch PKSM from a [*hax-based Homebrew Launcher](https://smealum.github.io/ninjhax2/starter.zip) |

Initial launch will require your system to be connected to the internet. This is needed to download the additional assets required for the application to launch. If your system is not connected to the internet, you will get a **Failed to receive a status code** error.

You need internet access to download the additional assets. If you're unable to download them through the application, download and extract the [additionalassets](https://github.com/dsoldier/PKResources/releases/download/final/additionalassets.zip) folder in the `.zip` file to your SD card, at `/3ds/PKSM/`. The final result should be a folder filled with assets, located at `/3ds/PKSM/additionalassets/`.

### DS cartridges (*hax)

To let PKSM load your DS save correctly, you need to select a target title that has DS save access when selecting PKSM from the homebrew launcher.

A title satisfying those specifics is, for example, Poké Transporter.

### Storage changes from 5.0.0+

If you're coming from a PKSM version < 5.0.0, you'll notice your `bank.bin` file isn't recognized anymore. That's because the working path changed from 5.0.0 onwards, so you'll need to **manually move** your `bank.bin` file to the new location, from `/3ds/data/PKSM/bank/` to `/3ds/PKSM/bank`. 

## Applications compatible with PKSM

* **[serveLegality](https://github.com/FlagBrew/serveLegality)**: PC tool to check and fix a pkmn's legality. (Development on hold)
* **[PKHeX](https://github.com/kwsch/PKHeX)**: Generates .pk7, .pk6, .wc7, .wc6 QR codes scannable from PKSM. Runs on PC.
* **[MysteryGiftBot](https://twitter.com/mysterygiftbot)**: Generates .wc7 QR codes scannable from PKSM. Runs on Twitter.
* **[servepkx](https://github.com/FlagBrew/servepkx)**: multiplatform tool to send .pk7, .pk6, .wc7full, .wc6full, .wc7, .wc6 files to PKSM. Requires a web browser or JRE.
* **[phbank2pksm](https://github.com/BernardoGiordano/PKSM-Tools)**: tool to convert a PHBank bank file to a PKSM storage file. Requires a web browser.
* **[PKSMScript](https://github.com/FlagBrew/PKSM-Scripts)**: python tool to compile .pksm scripts.

## Working paths

* Additional assets are located at `/3ds/PKSM/additionalassets/`
* Automatic save backups are located at `/3ds/PKSM/backup/[GAME_DATE]/main`
* Extra storage backups are located at `/3ds/PKSM/bank/bank_[DATE].bak`

## Troubleshooting

Before submitting an issue, have a look through the issues page, because your question may have already been answered in the past.

Please only submit consistent issues (submitting your environment and which version of PKSM you're running), without asking for the moon: I'm working for free here. Duplicate issues will be closed without any reply.

I'll not reply to issues related to versions of PKSM different from the latest stable release currently available.

You can get real-time support by joining PKSM's discord server.

[![Discord](https://discordapp.com/api/guilds/278222834633801728/widget.png?style=banner3&time-)](https://discord.gg/bGKEyfY)

## Building

PKSM has the following dependencies:

- The latest version of ctrulib, citro3d, citro2d, 3ds-curl and [3dstool](https://github.com/dnasdw/3dstool/releases), [bannertool](https://github.com/Steveice10/bannertool/releases) and [makerom](https://github.com/profi200/Project_CTR/releases). (All of these except for bannertool, makerom and 3dstool should be installed from the [devKitPro pacman repository](https://devkitpro.org/wiki/devkitPro_pacman). ctrulib, citro3d and citro2d should be installed with `3ds-dev` and then selecting yes on the entire group instead of installing the packages directly).
- The latest version of python 3.x and the GitPython library.

To compile, clone the repository with all submodules (`git clone --recursive https://github.com/FlagBrew/PKSM.git` if initially cloning, `git submodule init` and `git submodule update` if running from an existing clone) and run `make all`.

## Credits

* dsoldier for the gorgeous graphic work
* Naxann and Anty-Lemon for their contributions to the project
* Smealum for ctrulib
* Kaphotics and SciresM for PKHeX and memecrypto
* J-K-D for direct save import/export
* Astronautlevel for QR codes support
* Slownic and zaksabeast for servepkx
* Slashcash for PCHex++
* TuxSH for TWLSaveTool
* ProjectPokemon.org for most of the wondercards
* Simona for being my best supporter
* all the people contributing to the scripts
* all the countless translators who helped with the multilanguage feature
* everyone who helped during the development process
* all the supporters

**If you appreciate my work, I appreciate [a coffee](https://www.patreon.com/bernardogiordano) :)** 

## License

> This file is part of PKSM
> 
> Copyright (C) 2016/2017 Bernardo Giordano
>
>    This program is free software: you can redistribute it and/or modify
>    it under the terms of the GNU General Public License as published by
>    the Free Software Foundation, either version 3 of the License, or
>    (at your option) any later version.
>
>    This program is distributed in the hope that it will be useful,
>    but WITHOUT ANY WARRANTY; without even the implied warranty of
>    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>    GNU General Public License for more details.
>
>    You should have received a copy of the GNU General Public License
>    along with this program.  If not, see <http://www.gnu.org/licenses/>.
>    See LICENSE for information.
