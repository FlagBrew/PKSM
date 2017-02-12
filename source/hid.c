#include <3ds.h>
#include "hid.h"

int calcCurrentEntryOneScreen(int currentEntry, int max, int columns) {
	if (hidKeysDown() & KEY_DRIGHT)
		if (currentEntry < max)
			currentEntry++;
	
	if (hidKeysDown() & KEY_DLEFT)
		if (currentEntry > 0) 
			currentEntry--;
	
	if (hidKeysDown() & KEY_DUP)
		if (currentEntry >= columns) 
			currentEntry -= columns;
	
	if (hidKeysDown() & KEY_DDOWN)
		if (currentEntry <= max - columns)
			currentEntry += columns;
		
	return currentEntry;
}

int calcCurrentEntryOneScreenReversed(int currentEntry, int max, int rows) {
	if (hidKeysDown() & KEY_DDOWN)
		if (currentEntry < max)
			currentEntry++;
	
	if (hidKeysDown() & KEY_DUP)
		if (currentEntry > 0) 
			currentEntry--;
	
	if (hidKeysDown() & KEY_DLEFT)
		if (currentEntry >= rows) 
			currentEntry -= rows;
	
	if (hidKeysDown() & KEY_DRIGHT)
		if (currentEntry <= max - rows)
			currentEntry += rows;
		
	return currentEntry;
}