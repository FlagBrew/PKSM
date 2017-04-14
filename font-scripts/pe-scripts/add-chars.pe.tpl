#!/usr/local/bin/fontforge

# Add characters from a font to another font
#
# This script needs FontForge installed
# How to use :
#
# [Path to fontforge executable]fontforge -script add-chars.pe font-from.ttf font-to.ttf font-merged.ttf scaling padding-left


fontContainingCharacters = $1
fontTo = $2

scaling = 100
if ($argc >= 5)
	scaling = Strtol($argv[4])
endif

padding = 0
if ($argc >= 6)
	padding = Strtol($argv[5])*100
endif

totalCharacters = {totalCharacters}

if (totalCharacters > 0)
	characters = Array({totalCharacters})

	{characters}
	Print("Scaling: "+scaling+", padding: "+padding)
	Print("Opening " + fontTo +" file...")
	Open(fontTo)
	Print("Opening " + fontContainingCharacters +" file...")
	Open(fontContainingCharacters)
	i = 0
	while (i < {totalCharacters})
		if (totalCharacters >= 500 && i%100 == 0)
			Print("Adding char "+i+" / "+totalCharacters)
		endif
		if (totalCharacters < 500 && i%10 == 0)
			Print("Adding char "+i+" / "+totalCharacters)
		endif
		Open(fontTo)
		Select(characters[i])
		ttfIndex = GlyphInfo("GlyphIndex")

		# We copy only if character is not existing
		if (ttfIndex == 65535)
			Open(fontContainingCharacters)
			Select(characters[i])
			CopyUnlinked()
			SelectNone()
			Open(fontTo)
			Select(characters[i])
			Paste()
			if (scaling != 100)
				Select(characters[i])
				Transform(scaling, 0, 0, scaling, padding, 0)
			endif
			if (padding != 0)
				Select(characters[i])
				Transform(100, 0, 0, 100, padding, 0)
			endif

			SelectNone()
		endif
		++i
	endloop

	Open(fontContainingCharacters)
	Close()
	Open(fontTo)
	Generate($3)
	Print("The file " +$3+" has been generated !")
	Close()
else
	Print("No characters to add.")
endif
