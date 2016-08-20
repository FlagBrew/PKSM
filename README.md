# EventAssistant [![Github latest downloads](https://img.shields.io/github/downloads/BernardoGiordano/EventAssistant/latest/total.svg?maxAge=86400)](https://github.com/BernardoGiordano/EventAssistant/releases/latest)

*A general purpose Pokémon save file editor... and more.

---

**[EventAssistant](https://github.com/BernardoGiordano/EventAssistant/releases): what is this?**

[**EventAssistant**](https://github.com/BernardoGiordano/EventAssistant/releases) is a general purpose Pokémon save editor and information viewer that can be run on your console (you need *Homebrew Launcher* to launch the *.3dsx* version or *CFW* to install the *.cia* version on the console and run it from the home menu, or Citra to run the *.3dsx* version on your PC).

This application allows you to make some cool things, like:

* Check **Gen VI's Event Database**;
* **Download and inject WC6s** into a XY or ORAS save file. Multiple injection is allowed;
* **Edit Pokémon stats**, like friendship, IVs, EVs, Hidden Power, shinyness, and **clone** easily single pokémons or entire boxes;
* **Edit you save** like Powersaves does, directly from your 3DS. You can edit *money*, *Battle Points*, *items* and much more;
* Seeing **current event distributions** directly from your 3DS. It provides infos about start and end dates of local and wifi distributions, with related codes if there is a general code for the distribution;
* **Capture probability calculation**: you can now calculate it directly on your 3DS, for every generation starting by the third. You can also calculate how much tries are needed to reach a certain probability of success;
* Check **Powersaves dates** for common Hacked Events.
 
---

**Important things about Gen VI's Event Database**

Event Database links are hardcoded in the program, so it knows where to look for stuff. The point is that writing this stuff is very difficult to do alone, so lots of event cards are unavailable at the moment. They'll be available when uploaded on the server I use, and you'll be able to see them without any update of the app. Updates are needed if new events are released, basically to update the description of the event.

---

**Where gamesaves have to be?**

Saves are directly taken/saved from your cartridge/digital copy of the game (thanks @J-K-D). Just select the proper game in the application.

**The application will look first for a cartridge, then for a digital copy of the game!**

---

**What will come next?**

* More Powersaves-like features;
* Capture probability calculation support for I-II gens;
* Regular updates to the Event Database.

Tell me if you like this work and what you would have featured in the next update! :)

---
 
Thank you to:

* Smealum for ctrulib
* Kaphotics for PKHeX and useful documentation
* J-K-D for direct import/export
* Slashcash for PCHex++ and lots of source code
* Nba_Yoh for the received flag issue
* Gocario for ccitt CHK algorithms
* Hamcha for cybertrust.h and digicert.h
* LiquidFenrir for useful http source code 
* PPorg for most of wc6s
* all the guys on the 3dshacks discord
* Simona Mastroianni for fill.c work
* Federico, YodaDaCoda and /u/SatansRoommate for testing builds
* Cosimo Vivoli for some database entries
* Kian Josh King for some wc6s
* Shai for the icon and Fabrizio for the name

---

# How to compile it?

Use latest **ctrulib** and devKitArm. 

---

# DISCLAIMER

**I'm NOT RESPONSIBLE for data losses or shits that could remotely happen using this application!**

---

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.me/BernardoGiordano)