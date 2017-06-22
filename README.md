<p align="center">
  <br><br>
  <img src="https://raw.githubusercontent.com/BernardoGiordano/PKSM/master/assets/banner.png">
</p>

**Editor**, **wondercard injector**, **offline extra storage** and **OTA injector**.

If you want a non-invasive homebrew to see your save's infos, check out **PKSV** in the releases page.

| Downloads | Links |
| :-------: | :---: |
| Latest 3dsx build | [direct](https://github.com/BernardoGiordano/PKSM/releases/download/4.3.0/PKSM.zip) |
| Latest cfw build | [direct](https://github.com/BernardoGiordano/PKSM/releases/download/4.3.0/PKSM.cia) |
| Latest QR code | [qr](https://chart.googleapis.com/chart?chs=300x300&cht=qr&chl=https://github.com/BernardoGiordano/PKSM/releases/download/4.3.0/PKSM.cia&choe=UTF-8.png) |

## Installation

Download the latest release (4.3.0) and install it with your favourite installation method. You should only copy/paste the whole content of the release zip into the root of your sd-card, most of the time. When booted, the application will automatically download the required additional assets from an external source. You can provide your own assets, if you want.

PKSM works with Rosalina-based Homebrew Launchers, too.

If you're using Homebrew Launcher with a *hax exploit, make sure you have it updated to the [latest version](https://smealum.github.io/ninjhax2/starter.zip).

You need internet access to download the additional assets. If you can't still download them through the application, you can put them manually in the SD card, putting the external [additionalassets](https://github.com/Naxann/PKRessources/releases/tag/PKSM-4.3.0) folder in your SD card, located at ` /3ds/data/PKSM/ `. To avoid troubles, the final result should be a folder filled with assets, located at ` /3ds/data/PKSM/additionalassets/ `.

## Usage

GenVI to GenVII games: trigger the right game when prompted (you will do it twice if using a *hax-based homebrew launcher).

GenIV to GenV games: if using a *hax-based homebrew launcher, you must trigger Poketrasporter. Otherwise, use as always.

## Backups

Automatic save backups are located at ` /3ds/data/PKSM/backup/[GAME_DATE]/main `

Extra storage backups are located at ` /3ds/data/PKSM/bank/bank_[DATE].bak `

## OTA injection

You can use servepkx to inject things from your PC to PKSM directly. Use your favourite version between the ones proposed here:

| servepkx | Links |
| :-------: | :---: |
| **Java** | [Usage](https://github.com/BernardoGiordano/PKSM/tree/master/servepkx/java) |
| **Python** | [Usage](https://github.com/BernardoGiordano/PKSM/tree/master/servepkx/python) |
| **Web Browser** | [Usage](https://github.com/zaksabeast/PKSMBrowserInject) |

## Utilities

| Application | Description |
| :---------: | :---------: |
| **[badsectors](https://github.com/BernardoGiordano/PKSM/tree/master/extrastorage/badsectors)** | This hb application cleans occasional wrong bytes into your extra storage file |
| **[phbank2pksm](https://github.com/zaksabeast/phbank2pksm)** | This web application converts your PHBank extra storage file to a PKSM compatible one |

## Issues

Before submitting an issue, look for it into the issues page, because it could have been already answered in the past.

Please only do consistent issues (submitting your environment and which version of PKSM you're running), without asking for the moon: we're working for free here. Duplicate issues will be closed without answer by our part.

We'll not reply to issues related to versions of PKSM different from the latest stable release currently available.

## Contributing

Pull Requests are greatly appreciated. If you're planning to add features that requires a good bunch of work, please tell us before starting, in order to avoid wasting your time if the feature you're planning to add will not be possible to merge.

## Compiling

You will need:

* devKitArm r46
* ctrulib 1.2.1
* citro3D (commit 3ae31ad)
* libpng 1.6.19
* libJPEGTurbo 1.5.1
* freetype 2.7.1
* sf2dlib
* sftdlib
* sfillib

When cloning the repo make sure to use `git clone --recursive` in order to also get buildtools and the other dependencies.

Lastly in case you're compiling for *hax-based homebrew launchers ensure you also create an xml file with `<targets selectable="true"></targets>` and put it along side the `.3dsx` file with matching names.

## Screenshots

<img src="assets/page/01.png" width="350"> <img src="assets/page/02.png" width="350">
<img src="assets/page/03.png" width="350"> <img src="assets/page/04.png" width="350">
<img src="assets/page/05.png" width="350"> <img src="assets/page/06.png" width="350">
<img src="assets/page/07.png" width="350"> <img src="assets/page/08.png" width="350">
<img src="assets/page/09.png" width="350"> <img src="assets/page/10.png" width="350">
<img src="assets/page/11.png" width="350"> <img src="assets/page/12.png" width="350">
 
## Credits

* dsoldier for the gorgeous graphic work
* Naxann and Anty-Lemon for their contributions to the project
* Smealum for ctrulib, Xerpi for sf2d, sftd and sfil
* Kaphotics and SciresM for PKHeX and memecrypto
* J-K-D for direct save import/export
* Slownic for java servepkx
* Slashcash for PCHex++
* Gocario for PKBrew
* TuxSH for TWLSaveTool
* PPorg for most of the wcx included here
* Simona, Carlo, Matteo for fill.c work

**If you appreciate my work, I appreciate [a coffee](https://www.paypal.me/BernardoGiordano) :)** 

## License

This file is part of PKSM

Copyright (C) 2016/2017 Bernardo Giordano

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
