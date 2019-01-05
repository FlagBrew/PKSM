# Changelog
## 6.0.1
+ **Fix crash when a flashcard was inserted**. You can now plug your cartridge
  reader with whatever you want.
+ **Fix various offsets** used here and there in the application. This is
  **critical** and **updating** to this version against v6.0.0 **is strongly
  suggested**.
+ **Fix storage renaming bug**, which caused your storage to not be opened
  again.
+ **Fix crashes on unsuccessfull app exit**.
+ Fix party generation.
+ Fix event injection for Pokemon Diamond/Pearl.
+ Fix transfer code from gen4 upwards and a couple backwards transfer issues.
+ Disable nature edit screen for Gen4, as it is linked to PID.
+ Added a console displaying why PKSM doesn't boot up, with explanation and
  possible fixes.
+ Fix various Hex Editor offsets (thanks @SadisticMystic ).
+ Storage transfers through boxes now automatically set dex data.
+ DS save recognition simplified.
+ Added JP translations (thanks @pass0418 ).
+ Added form changing for Arceus and Genesect, disable form changing for
  Xerneas.
+ Added Hyper Training flags to the hex editor.
+ Enable Hex Editor value changing with `A`/`X` buttons.
+ Fix empty ability after generating a pokemon from scratch.
+ Added a search bar for the moves editor, item editor and bag editor screens.
+ Hex Editor now enabled for LGPE.
+ Backup bridged save to the SD card in case the bridge disconnects, so you can
  reinject it manually later.
## 6.0.0
+ **Game support has been extended to cover every functionality from Generation
  4 to LGPE**. This includes, among the others, *Storage*, *Editor* and
  *Scripts*.
  + This means that Editor and Storage are now available for DS games. More on
    that later.
  + This also means that Editor is available for LGPE.
+ The **title loader** has been completely redesigned to highly improve the user
  experience.
  + The user interface is now more *Checkpoint-esque*. PKSM will automatically
    retrieve any valid title (cartridge and digital) that is currently available
    in your console.
  + Title loader now supports **cartridge hotswapping**. You can now remove the
    cartridge while in it and put another one. The title list will automatically
    reflect the changes in the interface.
  + Title loader is now **able to load the save file** for a certain title
    **from** both the actual save archive and **the SD card**.
    + This means that you're now able to load *any* save file for a certain
      game, provided they're located in a folder specified in the configuration
      file (more about this later).
    + PKSM automatically adds Checkpoint's working path to the list of valid
      folders to look for save files. This means you'll be able to directly edit
      your save backups you made with Checkpoint.
    + While the main title loader interface lets you only work with save files
      you have games for, *how do you work with save file you don't actually
      have games in your console*? You're welcome, we thought about this too.
      Title loader now has an additional tab in which you can choose save files
      for not directly available titles, giving you the opportunity to edit
      whatever you want. What has already been said about SD card save backup
      loading also applies to this menu.
  + It's now possible to return to the title loader once you're done making
    changes to a save file, instead of having to close PKSM and boot it back up.
  + You can edit PKSM's configurations by pressing `SELECT`, which will let you
    enter the configuration menu before the main menu is even loaded. More about
    configurations will be said later.
  + The title loader has now a **wireless loading** feature, with which you can
    load a save file from your local network, that can be sent back when you're
    done applying your changes. 
    + This directly allows you to load LGPE saves over the network using
      [Checkpoint](https://github.com/FlagBrew/Checkpoint/releases) for Switch.
    + **Warning**: once you enter the wireless loader (you'll be prompted before
      entering it, to make sure you don't misclick on it) you won't be able to
      return back without receiving a save file. If you're in this situation and
      don't know what to do, rebooting your console will be mandatory.
+ The **Storage** functionality has been completely redesigned. It's now
  possible to browse Storage for every game, from DPPt to USUM.
  + It's now possible, among the already available functionalities, to clone
    from the Storage itself.
  + **Transfer through generations** is available. This means you can perform a
    complete Gen4 <-> Gen7 swap in freedom.
    + Transfer through generations needs to be specifically allowed from the
      configurations, because it changes the original pokemon data. The option
      to allow is `Edit during transfers` (or the equivalent in the language
      you're familiar to).
    + Please note that conversions are still experimental and there will be edge
      cases not covered. Please always make a save/storage backup using
      Checkpoint.
  + **Box renaming** is available for both save and storage boxes.
  + Storage size expansion has been enabled once again. Default Storage size is
    still 150 boxes, but the upper limit is now set to **2000**.
+ The **Editor** has been completely redesigned as well.
  + Supports every game from DPPt to LGPE.
  + Hex Editor and QR Code scanner are also available from DPPt to USUM.
  + **Party editing is now possible**.
  + Cloning is possible by pressing `X`.
  + Box renaming is available here, too.
  + You're now prompted if you're exiting a pkm edit session without saving.
+ A **Bag editor** has been included. You can now edit most sections of your
  bag, for every game supported by PKSM.
  + You're allowed to change the item by tapping on the dedicated button, and
    increase or decrease the amount using the touch screen.
  + Dynamic addition and removal of items is also available.
+ The **Event database** has been finally updated to include all the latest
  wondercards. 
  + New wondercards will be automatically fetched from the EventsGallery when
    you compile PKSM from scratch, or we push a new release. This means manual
    work is not required anymore to include wondercards.
  + Dumping wondercards you own is possible by pressing `X` in the event list.
  + QR code wondercard injection is available as well, and has been extended to
    support all wondercard formats from Gen4 to Gen7.
+ **Scripts** have been greatly enhanced as well. We'll be short but this
  describing the latest changes would require a dedicate changelog.
  + Default scripts are now built-in. Thanks to the recent work by many
    scripters, there are now more than **800** scripts available for everyone to
    use.
    + This means every new PKSM update will always contain default scripts.
      Placing default ones in the SD card is not required anymore.
  + Support for **scripts written in C**. PKSM now has a built-in **C
    interpreter** which will allow you to write more complex scripts directly in
    C.
    + This directly means you're allowed to use standard library functions in
      scripts, create variables, doing math operations and work with pointers.
      Good stuff.
    + Some APIs you can use in your scripts are directly provided by PKSM, and
      they focus on user interaction, keyboard access and utility functions.
    + You can always request some more if you're about to develop a new script
      which requires a functionality not yet available from PKSM.
  + Support for folders. Scripts are now organized in folders, to make the UI
    more elegant and accessible.
  + You're allowed to place your own scripts into the SD card and switch between
    built-in and your own from the UI itself.
+ The **QR code** scanner has been vastly improved to allow for a smoother
  experience. This will allow you to scan big QR codes faster then ever before.
+ The **Configuration** menu has been vastly redesigned to get rid of the
  unintuitive *hex editor-like* user interface.
  + Interface language can be changed in real time.
    + This version of PKSM ships with English, Italian, Spanish, French and
      German already supported. Other languages haven't been contributed by
      native speakers yet, and we hope to support more languages in the next
      updates.
  + Default values like the `OT Name` can now be edited through the keyboard.
  + Some more options that affect all the other section of the application are
    available.
+ **PKSM now relies on extdata** to store important data, like the storage and
  configurations.
  + This means you can now use Checkpoint to make a proper and easy backup of
    your relevant PKSM data, like the whole storage file and configurations.
    Sharing your data is now more convenient, too.
  + You're also allowed to move the storage data to the SD card.
  + **Warning**: proper extdata support for PKSM is only granted from Checkpoint
    **3.6.0** and above. Don't try restoring a PKSM save backup with a version
    inferior to the one suggested.
+ **Support for \*hax** (also known as *just homebrew*) has been dropped. PKSM
  only works under a Luma3DS environment.
  + Specifically, PKSM will check for the `hb:ldr` port, which is available, for
    example, through the Rosalina system module.
  + If you try booting PKSM under *hax, it will return to the homebrew launcher.
  + This also means PKSM will not work in Citra.
+ **PKSM now supports audio playback**. Audio will start once boot is completed.
  + There actually is no way to stop the audio playback, so turn down the volume
    in case it starts getting annoying.
  + It's possible to provide your own audio soundtracks by placing `.mp3` files
    in the `sdmc:/3ds/PKSM/songs` folder.
  + It's suggested to provide relatively small sized files. We generally suggest
    tracks with a length of no more than 4 minutes, single channel, with a
    sampling frequency of 44100Hz and a bitrate of 96kbps.
    + These characteristics are not actual constraints: you can throw whatever
      you want into the SD card and PKSM will mostly be able to work with it
      fine.
    + You can convert a track to get these characteristics through
      [ffmpeg](https://stackoverflow.com/questions/3255674/convert-audio-files-to-mp3-using-ffmpeg).
+ A *search* function for species has been implemented so you're now able to
  search a Pokemon with your keyboard rather than scrolling the entire list
  while generating one or changing the species.
+ Tons of other stuff. Really *everything* changed from the latest release.
+ Last but not least, a whole **documentation** in `.html` format explaining in
  details how PKSM works! The documentation will be bundled with every release,
  and the most up-to-date version will always be accessible through the *Github
  wiki for this project.