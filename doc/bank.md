# Pokémon storage

The Pokémon storage function serves as an open source alternative to
[Pokémon Bank](https://www.nintendo.com/games/detail/pokemon-bank-3ds), that
is, a Pokémon mass storage solution that can be accessed from multiple
games and used to transfer Pokémon between generations.

Unlike the Nintendo implementation, PKSM does not store Pokémon in the
cloud, but on your own device. Therefore no internet access is
required to access the service, although a minimal amount of SD card
storage will be used. The service is of course entirely free of charge
:)

Currently, only generations VI-VII are supported (yes, this means that
you unfortunately cannot yet transfer Pokémon with PKSM).

## Managing Pokémon with Pokémon storage

> You will always be prompted to confirm the changes you made to your
> game and bank when you exit PKSM, pressing "B" on either prompt will
> undo all changes - in case you accidentally released your shiny
> Mewto.

The interface for this feature is quite simplistic:

![](https://i.imgur.com/2Vt1dYE.png)

The boxes in the top screen represent the "Bank", Pokémon stored in
the space that is shared between games, the boxes in the lower screen
represent the boxes of the game currently managed by PKSM.

Pokémon can be selected using either the direction pad or the touch
screen, and can be picked up and placed down using the `A` button.

The blue button to the top right will swap the contents of the boxes.

The menu items to the right of the game boxes work as follows:

| Menu Item | Description                                                                |
| :-------: | -------------------------------------------------------------------------- |
|  X: View  | Toggle a detailed view of the currently selected Pokémon                   |
| Clear Box | Release all Pokémon in the current box (you will be prompted to confirm)   |
|  Release  | Release the currently selected Pokémon                                     |
|    Dump   | Write the currently selected Pokémon to `/3ds/PKSM/dump/[date]/[name].pk7` |
|  PokéDex  | This was removed in version 5.1.0, but used to view the PokéDex            |

## Files created by Pokémon storage

> If you're coming from a PKSM version < 5.0.0, you'll notice your
> `bank.bin` file isn't recognized anymore. That's because the working
> path changed from 5.0.0 onwards, so you'll need to manually move
> your `bank.bin` file to the new location, from
> `/3ds/data/PKSM/bank/` to `/3ds/PKSM/bank`.

Pokémon storage will create files in two sub-directories of
`/3ds/PKSM/`:

#### `/3ds/PKSM/bank/`

This directory contains the "Bank" shared between your games. The
current Bank will be stored as `bank.bin`, and a backup of the last
time you opened the menu item (in case you accidentally released your
shiny Mewto) will be stored as `bank_[DATE].bak`.

Should you need to restore an old version of your bank, simply rename
the appropriate `bank_[DATE].bak` to `bank.bin` (this will overwrite
the contents of your current Bank with the old version, so you may
want to rename the newest version first).

#### `/3ds/PKSM/dump`

This directory will contain `pk7` files that describe any Pokémon you
used the "Dump" feature on - you can copy these files to your PC and
open them with [PKHeX](https://github.com/kwsch/PKHeX), which may
allow for more fine-grained editing.
