<p align="center"><img src="https://raw.githubusercontent.com/FlagBrew/PKSM/master/assets/banner.png" /></p>
<p align="center"><img src="https://img.shields.io/badge/License-GPLv3-blue.svg" /></p>
<p align="center"><img src="https://github.com/FlagBrew/PKSM/workflows/CI/badge.svg" /></p>

Multipurpose and portable Pokemon save manager and editor for generations III to VIII, programmed in
C++.

* Supports original cartridges and digital copies of games from FrLgRSE to SwSh
* Allows on-the-fly modifications to all of your data
* Allows internal and resizable offline storage to store your Pokémon
* Capable of running custom scripts to allow injection of arbitrary data into
  your saves
* Capable of scanning QR codes to inject both .pkx files or event wondercards
* Contains an offline wondercard database to get events from old distributions
* Capable of automatic verification and legalization of all your data (internet connection required)
  * Optional interfacing with an Android app to handle verification and legalization without a network connection

**We do not support or condone cheating at the expense of others. Do not use
significantly edited Pokémon in battle or in trades with those who are unaware
edited Pokémon are in use.**

## Screenshots

![](https://i.imgur.com/HeRfuyl.png) ![](https://i.imgur.com/1GQsSbl.png)
![](https://i.imgur.com/2nePNbY.png) ![](https://i.imgur.com/nLSknIq.png)
![](https://i.imgur.com/2G7zbBH.png) ![](https://i.imgur.com/KPMIoHa.png)
![](https://i.imgur.com/LKnAcHI.png) ![](https://i.imgur.com/NaWoUIa.png)
![](https://i.imgur.com/0VSTcgA.png) ![](https://i.imgur.com/0g7O9y7.png)

## Documentation

Please refer to the [PKSM wiki](https://github.com/FlagBrew/PKSM/wiki) for
detailed documentation.

## Installation

This software works on CFW and Rosalina-based Homebrew Launchers. An internet
connection is required when first launching PKSM.

| Entrypoint | Instructions |
| :--------: | :----------- |
| CFW | Install the `PKSM.cia` file provided in the release page with your favourite installer. You can now launch the application from the Home Menu |
| Rosalina HBL | Copy the `PKSM.3dsx` file provided in the release page into your SD card. You can now launch PKSM from a [Rosalina-based Homebrew Launcher](https://github.com/fincs/new-hbmenu) |

Initial launch will require your system to be connected to the internet. This is
needed to download the additional assets required for the application to launch.
If your system is not connected to the internet, PKSM will close.

## Applications compatible with PKSM

* **[PKHeX](https://github.com/kwsch/PKHeX)**: Generates .pk7, .pk6, .wc7, .wc6
  QR codes scannable from PKSM. Runs on PC.
* **[MysteryGiftBot](https://twitter.com/mysterygiftbot)**: Generates .wc7 QR
  codes scannable from PKSM. Runs on Twitter.
* **[PKSM-Scripts](https://github.com/FlagBrew/PKSM-Scripts)**: A toolkit to
  develop and compile .pksm and .c scripts.
* **[The GPSS](https://flagbrew.org/gpss)**: An online Pokémon sharing platform.
* **[GPSS Mobile](https://play.google.com/store/apps/details?id=com.flagbrew.gpss_mobile)**: Mobile interface for the GPSS and related features. Android only.

## Working path

* Additional assets are located at `/3ds/PKSM/assets`
* Automatic save backups are located at `/3ds/PKSM/backups`
* Extra storage data is located at `/3ds/PKSM/banks`
* .pkx and .wcx dumps are located in `/3ds/PKSM/dumps`
* Custom scripts are located in `/3ds/PKSM/scripts`
* Custom background songs are located in `/3ds/PKSM/songs`

## Troubleshooting

Before submitting an issue, have a look through the [issue tracker](https://github.com/FlagBrew/PKSM/issues), as your
question or bug request may have already been answered in the past.

Please only submit consistent issues (submitting your environment and which
version of PKSM you're running, for example). Duplicate issues will be closed.

Issues that are opened concerning releases other than the latest stable release will also be closed. Please do not report bugs with old software.

You can get real-time support by joining FlagBrew's discord server:

[![Discord](https://discordapp.com/api/guilds/278222834633801728/widget.png?style=banner3&time-)](https://discord.gg/bGKEyfY)

## Building

PKSM has the following dependencies:

- The latest version of libctru, citro3d, citro2d, 3ds-curl, 3ds-pkg-config, 3ds-bzip2, 3ds-mpg123, 3dstools, and tex3ds with
  their dependancies. All of these should be installed from [devKitPro
  pacman](https://devkitpro.org/wiki/devkitPro_pacman).
- rsync
- Your system's pkg-config
- [3dstool](https://github.com/dnasdw/3dstool/releases),
  [bannertool](https://github.com/Steveice10/bannertool/releases) and
  [makerom](https://github.com/profi200/Project_CTR/releases), if you want to be
  able to compile a `.cia` build of PKSM.
- The latest version of Python 3.x and the GitPython library.
- node.js, [pandoc](https://pandoc.org/), [wkhtmltopdf](https://wkhtmltopdf.org)
  and
  [github-wikito-converter](https://www.npmjs.com/package/github-wikito-converter)
  to be able to build the HTML documentation.

To compile, clone the repository with all submodules (`git clone --recursive
https://github.com/FlagBrew/PKSM.git` if initially cloning, `git submodule init`
and `git submodule update` if running from an existing clone) and run `make
all`.

## Credits

* [Bernardo](https://github.com/BernardoGiordano/) for creating PKSM
* [piepie62](https://github.com/piepie62) and
  [Admiral-Fish](https://github.com/Admiral-Fish) for the immense amount of
  dedication they put into the project
* dsoldier for the gorgeous graphic work
* [SpiredMoth](https://github.com/SpiredMoth),
  [trainboy2019](https://github.com/trainboy2019) and all the scripters for
  making PKSM-Scripts great
* [Archit Date](https://github.com/architdate) for CoreConsole and PKHeX AutoLegalityMod
* [Allen](https://github.com/FM1337) for the GPSS, CoreConsole's successor, CoreAPI and GPSS Mobile
* [LiquidFenrir](https://github.com/LiquidFenrir) for the PoC of session stealing that led to Gen 3 support and GPSS Mobile
* The whole [FlagBrew](https://github.com/FlagBrew) team for collaborating with
  us
* [kwsch](https://github.com/kwsch) and [SciresM](https://github.com/SciresM)
  for PKHeX, memecrypto and documentation
* [fincs](https://github.com/fincs) and
  [WinterMute](https://github.com/WinterMute) for citro2d and devkitARM
* [kamronbatman](https://github.com/kamronbatman) and ProjectPokemon.org for
  EventsGallery
* [nayuki](https://github.com/nayuki) for [QR-Code-generator](https://github.com/nayuki/QR-Code-generator)
* [jpoirier](https://github.com/jpoirier), [zsaleeba](https://gitlab.com/zsaleeba), and [Jan Švejda](https://gitlab.com/jenda.svejda) for the base and most improvements in [our picoC fork](https://github.com/FlagBrew/picoc)
* [dlbeer](https://github.com/dlbeer) for [quirc](https://github.com/dlbeer/quirc)
* [Griffin](https://github.com/GriffinG1) For FlagBot and all the help it has been on the server.
* All the translators
* Subject21_J and all the submitters for PKSM's icon
* Allen, piepie62, SpiredMoth and all the contributors for the documentation
* All the past contributors to the project
* All the patrons and the huge amount of supporters that this project has gained
  over the year
  
Without you, this project wouldn't have existed. Thank you.

## Support us

If you appreciate our work, you can support us on
[Patreon](https://www.patreon.com/FlagBrew)!

## License

This project is licensed under the GNU GPLv3. Additional Terms 7.b and 7.c of
GPLv3 apply to this. See
[LICENSE](https://github.com/FlagBrew/PKSM/blob/master/LICENSE) for details.

Visit us on [flagbrew.org](https://flagbrew.org/)!
