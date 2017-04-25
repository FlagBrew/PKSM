/*  This file is part of PKSM
>	Copyright (C) 2016/2017 Bernardo Giordano
>
>   This program is free software: you can redistribute it and/or modify
>   it under the terms of the GNU General Public License as published by
>   the Free Software Foundation, either version 3 of the License, or
>   (at your option) any later version.
>
>   This program is distributed in the hope that it will be useful,
>   but WITHOUT ANY WARRANTY; without even the implied warranty of
>   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>   GNU General Public License for more details.
>
>   You should have received a copy of the GNU General Public License
>   along with this program.  If not, see <http://www.gnu.org/licenses/>.
>   See LICENSE for information.
*/

#include "hid.h"

int calcCurrentEntryOneScreen(int currentEntry, int max, int columns) {
	if (hidKeysDown() & KEY_RIGHT)
		if (currentEntry < max)
			currentEntry++;
	
	if (hidKeysDown() & KEY_LEFT)
		if (currentEntry > 0) 
			currentEntry--;
	
	if (hidKeysDown() & KEY_UP)
		if (currentEntry >= columns) 
			currentEntry -= columns;
	
	if (hidKeysDown() & KEY_DOWN)
		if (currentEntry <= max - columns)
			currentEntry += columns;
		
	return currentEntry;
}

int calcCurrentEntryOneScreenReversed(int currentEntry, int max, int rows) {
	if (hidKeysDown() & KEY_DOWN)
		if (currentEntry < max)
			currentEntry++;
	
	if (hidKeysDown() & KEY_UP)
		if (currentEntry > 0) 
			currentEntry--;
	
	if (hidKeysDown() & KEY_LEFT)
		if (currentEntry >= rows) 
			currentEntry -= rows;
	
	if (hidKeysDown() & KEY_RIGHT)
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
	
	if (hidKeysDown() & KEY_LEFT) {
		if (currentEntry > 0) 
			currentEntry--;
		else if (currentEntry == 0) 
			currentEntry = maxentries;
	}
	
	if (hidKeysDown() & KEY_RIGHT) {
		if (currentEntry < maxentries) 
			currentEntry++;
		else if (currentEntry == maxentries) 
			currentEntry = 0;
	}
	
	if (hidKeysDown() & KEY_UP) {
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
	
	if (hidKeysDown() & KEY_UP) {
		if (currentEntry > 0) 
			currentEntry--;
		else if (currentEntry == 0) 
			currentEntry = maxentries;
	}
	
	if (hidKeysDown() & KEY_DOWN) {
		if (currentEntry < maxentries) 
			currentEntry++;
		else if (currentEntry == maxentries) 
			currentEntry = 0;
	}
	
	if (hidKeysDown() & KEY_LEFT) {
		if (currentEntry <= maxentries - rows)	{
			page--;
			if (page < 0) 
				page = maxpages - 1;
		}
		else if (currentEntry >= rows) 
			currentEntry -= rows;
	}
	
	if (hidKeysDown() & KEY_RIGHT) {
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