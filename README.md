# EventAssistant

*A general purpose Pokémon toolkit*.

---

**[EventAssistant](https://github.com/BernardoGiordano/EventAssistant/releases): what is this?**

[**EventAssistant**](https://github.com/BernardoGiordano/EventAssistant/releases) is a general purpose Pokémon application that can be run on your console or PC (you need *Homebrew Launcher* to launch the *.3dsx* version or *CFW* to install the *.cia* version on the console and run it from the home menu, or Citra to run the *.3dsx* version on your PC).

This application allows you to make some cool things, like:

* Check **Gen VI's Event Database**;
* **Download and inject WC6s** into an OR/AS or XY save file. Multiple injection is allowed;
* **Edit you save** like Powersaves does, directly from your 3DS. You can edit *money*, *Battle Points*, *items* and much more;
* Seeing **current event distributions** directly from your 3DS. It provides infos about start and end dates of local and wifi distributions, with related codes if there is a general code for the distribution;
* **Capture probability calculation**: you can now calculate it directly on your 3DS, for every generation starting by the third. You can also calculate how much tries are needed to reach a certain probability of success;
* Checking **PID** of a pokémon. Pretty useful to check very old events, that may be manipulated with old tools and could have leftovers of manipulation. You can see if calculations done by the app are coherent with the pokémon;
* Check **Powersaves dates** for common Hacked Events.
 
---

**Important things about Gen VI's Event Database**

Event Database links are hardcoded in the program, so it knows where to look for stuff. The point is that writing this stuff is very difficult to do alone, so lots of event cards are unavailable at the moment. They'll be available when uploaded on the server I use, and you'll be able to see them without any update of the app. Updates are needed if new events are released, basically to update the description of the event.

---

**How to use it on my PC instead of console?**

You can use Citra to run the *.3dsx* version of the app. I made a few changes to have a better usability with Citra. As for now, you can only use properly PID Checker and Capture Probability Calculator, because http functions are unimplemented on Citra. To scroll properly values on Citra, just set the program in **SLOW mode**.

**Where gamesaves have to be?**

Saves are directly taken/saved from your cartridge/digital copy of the game (thanks @J-K-D).

**What will come next?**

* More Powersaves-like features;
* Much more informations related to PID checking;
* Capture probability calculation support for I-II gens;
* Regular updates to the Event Database.

Tell me if you like this work and what you would have featured in the next update! :)

---
 
Thank you to:

* smea for ctrulib
* Kaphotics for PKHeX and useful documentation
* J-K-D for direct import/export
* Nba_Yoh for the received flag issue
* Gocario for ccitt CHK algorithms
* Hamcha for cybertrust.h and digicert.h
* LiquidFenrir for useful http source code 
* all the guys on the 3dshacks discord
* Simona Mastroianni for fill.c work
* Federico for testing builds
* Cosimo Vivoli for some database entries
* Kian Josh King for some wc6s
* Shai for the icon and Fabrizio for the name

---

# How to compile it?

Use latest **ctrulib** and devKitArm. 

---

# DISCLAIMER

**I'm NOT RESPONSIBLE for data losses or shits that could remotely happen using the wc6 injection!**

