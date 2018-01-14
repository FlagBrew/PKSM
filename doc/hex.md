# Hex editing

While PKSM supports graphical user interfaces for the most common
operations, sometimes you will need to delve into binary files to get
exactly what you want.

PKSM has a built-in hex editor to help with these scenarios.

## Binary files

> This is only an introduction to help with the basics of editing. If
> you struggle, you may fare better doing this with PKHeX.

In computers, anything is represented by a series of numbers. The
letter A in ASCII encoding, for example, is represented by the number
41.

A binary file is a file in which there is no obvious encoding. The
file is therefore not human-readable, but may take less storage since
we can put all kinds of information together however we like.

Since Pokémon are stored using this format, and not every detail is
translated to a human-readable format by PKSM yet, sometimes you will
need to read and modify this kind of data.

Hexadecimal is a number system often used to represent numbers in
binary files, since they are easier to read by humans and map
perfectly to binary digits.

The numbers can be translated as follows (The 0x prefix is generally
used to state 'this number is hexadecimal', since it might be
ambiguous otherwise):

| Hex  | Decimal |
| :--: | :-----: |
| 0x00 | 0       |
| 0x01 | 1       |
| 0x02 | 2       |
| ...  | ...     |
| 0x0a | 10      |
| 0x0b | 11      |
| 0x0c | 12      |
| 0x0d | 13      |
| 0x0e | 14      |
| 0x0f | 15      |
| 0x10 | 16      |
| 0x11 | 17      |
| 0x12 | 18      |
| ...  | ...     |

The letter 'A' (41 decimal) in ASCII encoding can therefore be
expressed as '0x29' in hexadecimal. Should you need to translate a
number to hexadecimal, Google responds with the correct number to a
search such as '41 in hex'.

Individual numbers are stored in these binary files as 'bytes', that
is, portions of memory that can store exactly 0xff (255) as its
maximum value. If you are directed to "byte 0x02", this means that you
should look at the second number in the hex editor.

## Using the editor

![](https://i.imgur.com/EyTwxIc.png)

In the hex editor, the upper screen shows the contents of the file
being edited, and the bottom screen shows a menu that helps
translating the values to something a human might understand.

The screenshot shows the 0x30 byte of a Pokémon file, which, as the
title suggests, is the first byte of several which dictate the ribbons
a Pokémon might have.

The values of a byte may be increased using the `A` button, or
decreased using the `X` button, or modified using the touchscreen
buttons shown on the bottom screen.

## God mode

> Be especially careful when using god mode, you may be able to break
> your save file beyond repair

When you open a Pokémon file, you may notice that some bytes are
grayed out and will not save when you attempt to edit them. This is to
ensure that you don't accidentally modify values that might cause
issues.

In some cases you may still need to modify these values, and for this
there is god mode.

To enable god mode, tap all four corners of the bottom screen when
using the hex editor.
