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

void calcCurrentEntryMorePages(int *currentEntryP, int *pageP, int maxpages, int maxentries, int columns) {
	int page = *pageP;
	int currentEntry = *currentEntryP;
	
	if (hidKeysDown() & KEY_L) {
		if (page > 0) 
			page--;
		else if (page == 0) 
			page = maxpages - 1;
	}
	
	if (hidKeysDown() & KEY_R) {
		if (page < maxpages - 1) 
			page++;
		else if (page == maxpages - 1) 
			page = 0;
	}
	
	if (hidKeysDown() & KEY_DLEFT) {
		if (currentEntry > 0) 
			currentEntry--;
		else if (currentEntry == 0) 
			currentEntry = maxentries;
	}
	
	if (hidKeysDown() & KEY_DRIGHT) {
		if (currentEntry < maxentries) 
			currentEntry++;
		else if (currentEntry == maxentries) 
			currentEntry = 0;
	}
	
	if (hidKeysDown() & KEY_DUP) {
		if (currentEntry <= columns - 1)	{
			page--;
			if (page < 0) 
				page = maxpages - 1;
		}
		else if (currentEntry >= columns) 
			currentEntry -= columns;
	}
	
	if (hidKeysDown() & KEY_DOWN) {
		if (currentEntry <= maxentries - columns) 
			currentEntry += columns;
		else if (currentEntry >= maxentries - columns + 1) {
			page++;
			if (page > maxpages - 1)
				page = 0;
		}
	}
	
	*pageP = page;
	*currentEntryP = currentEntry;
}

void calcCurrentEntryMorePagesReversed(int *currentEntryP, int *pageP, int maxpages, int maxentries, int rows) {
	int page = *pageP;
	int currentEntry = *currentEntryP;
	
	if (hidKeysDown() & KEY_L) {
		if (page > 0) 
			page--;
		else if (page == 0) 
			page = maxpages - 1;
	}
	
	if (hidKeysDown() & KEY_R) {
		if (page < maxpages - 1) 
			page++;
		else if (page == maxpages - 1) 
			page = 0;
	}
	
	if (hidKeysDown() & KEY_DUP) {
		if (currentEntry > 0) 
			currentEntry--;
		else if (currentEntry == 0) 
			currentEntry = maxentries;
	}
	
	if (hidKeysDown() & KEY_DDOWN) {
		if (currentEntry < maxentries) 
			currentEntry++;
		else if (currentEntry == maxentries) 
			currentEntry = 0;
	}
	
	if (hidKeysDown() & KEY_DLEFT) {
		if (currentEntry <= maxentries - rows)	{
			page--;
			if (page < 0) 
				page = maxpages - 1;
		}
		else if (currentEntry >= rows) 
			currentEntry -= rows;
	}
	
	if (hidKeysDown() & KEY_DRIGHT) {
		if (currentEntry <= maxentries - rows) 
			currentEntry += rows;
		else if (currentEntry >= rows) {
			page++;
			if (page > maxpages - 1)
				page = 0;
		}
	}
	
	*pageP = page;
	*currentEntryP = currentEntry;
}