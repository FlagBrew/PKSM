/*  This file is part of PKSM
>	Copyright (C) 2016/2017 Bernardo Giordano
>
>   Multi-Language support added by Naxann
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
#include "i18n.h"

static char* ACCENTED_CHAR_REPLACE_FROM = "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
static char* ACCENTED_CHAR_REPLACE_TO = "AAAAAAECEEEEIIIIDNOOOOOx0UUUUYPsaaaaaaeceeeeiiiiOnooooo/0uuuuypy";

/**
 * English Localization files
 */
struct i18n_files i18n_files_en = {
	"romfs:/i18n/en/abilities.txt",
	"romfs:/i18n/en/species.txt",
	"romfs:/i18n/en/natures.txt",
	"romfs:/i18n/en/moves.txt",
	"romfs:/i18n/en/items.txt",
	"romfs:/i18n/en/hp.txt",
	"romfs:/i18n/en/app.txt"
};

/**
 * French Localization files
 */
struct i18n_files i18n_files_fr = {
	"romfs:/i18n/fr/abilities.txt",
	"romfs:/i18n/fr/species.txt",
	"romfs:/i18n/fr/natures.txt",
	"romfs:/i18n/fr/moves.txt",
	"romfs:/i18n/fr/items.txt",
	"romfs:/i18n/fr/hp.txt",
	"romfs:/i18n/fr/app.txt"
};

struct ArrayUTF32 i18n_AppTexts;

/**
 * Log debug with format
 */
void debuglogf(const char* format, ...) {
#if DEBUG_I18N
	va_list args;
    va_start(args, format);
	#ifdef VERSION
		char* str = malloc(255*sizeof(char));
		vsprintf(str, format, args);
		consoleMsg(str);
		//free(str);
	#else
		vprintf(format, args);
	#endif
	va_end(args);
#endif
}

/**
 * Get the length of a string in UTF8
 */
int utf8_strlen(char* s) {
   int i = 0, j = 0;
   while (s[i]) {
     if ((s[i] & 0xc0) != 0x80) j++;
     i++;
   }
   return j;
}

/**
 * Transform a string from UTF8 (char*) to UTF32 (wchar_t*)
 */
wchar_t* s_utf32(char* str) { return ss_utf32(str, 0); }

/**
 * Transform a string from UTF8 (char*) to UTF32 with a specified memory size
 */
wchar_t* ss_utf32(char* str, int size) {
	int length = utf8_strlen(str)+1;
	if (size == 0) {
		size = length;
	}
	u32 *text = malloc(size*sizeof(u32));
	utf8_to_utf32(text, (unsigned char*)str, length-1);
	text[length-1] = '\0';
	return (wchar_t*)text;
}



/**
 * File information : lines and chars
 */
struct i18n_FileInfo {
    int numberOfLines;
    int maxCharPerLine;
    int totalChars;
};

/**
 * Get information about a file
 */
struct i18n_FileInfo i18n_getInfoFile(char* filepath) {
    int lines = 0;
    int maxChar = 0;
    int currentMaxChar = 0;
    int totalChars = 0;
    char ch = ' ';
    FILE *fp = fopen(filepath, "r");
    while(!feof(fp)) {
        ch = fgetc(fp);
        currentMaxChar++;
        if(ch == '\n') {
            lines++;
            if (currentMaxChar > maxChar) {
                maxChar = currentMaxChar;
            }
            currentMaxChar = 0;
        } else if (ch != '\r' && ch != '\n') {
            totalChars++;
        }
    }
    if (currentMaxChar > maxChar) {
        maxChar = currentMaxChar;
    }
	// The last line
	if (totalChars > 0 && ch != '\r' && ch != '\n') {
		lines++;
	}
    fclose(fp);

    struct i18n_FileInfo fileinfo;
    fileinfo.numberOfLines = lines;
    fileinfo.maxCharPerLine = maxChar;
    fileinfo.totalChars = totalChars;

    return fileinfo;
}

/**
 * Remove endline characters from a string
 */
void i18n_removeEndline(char* str) {
	if (str[strlen(str)-2] == '\r') {
		str[strlen(str)-2] = '\0';
	} else if (str[strlen(str)-1] == '\n') {
		str[strlen(str)-1] = '\0';
	}
}

/**
 * Get an ArrayUTF32 from a File
 * Items are the lines of the file
 */
struct ArrayUTF32 i18n_FileToArrayUTF32(char* filepath) {
    struct i18n_FileInfo fileinfo = i18n_getInfoFile(filepath);
    wchar_t **arrwc = malloc(fileinfo.numberOfLines * sizeof(wchar_t*));

    FILE* fp = fopen(filepath, "rt");
    char *line = malloc((fileinfo.maxCharPerLine+1)*sizeof(char));
	int index = 0;
	// debuglogf("Reading file [%s]...\n", filepath);
    while (fgets(line, (fileinfo.maxCharPerLine+1)*sizeof(char), fp) != 0) {
		i18n_removeEndline(line);
		wchar_t* lineutf32 = s_utf32(line);
		arrwc[index] = lineutf32;
		index++;
    }
	rewind(fp);
	// debuglogf("Last ID: %i / %s.\n", index, line);
	// debuglogf("Reading file ended, closing file.\n");
    if (fclose(fp)) {
		// debuglogf("Error closing file !\n");
	}

	// debuglogf("Creating array...\n");
	struct ArrayUTF32 arr;
	arr.length = fileinfo.numberOfLines;
	arr.items = arrwc;
	arr.sorted = false;

	// debuglogf("Freeing memory line...\n");
	free(line);
	return arr;
}
/**
 * Copy an array
 */
/*
struct ArrayUTF32 ArrayUTF32_copy(struct ArrayUTF32 from) {
	struct ArrayUTF32 to;
	to.length = from.length;
	wchar_t **arrwc = malloc(from.length * sizeof(wchar_t*));

	for (int i = 0; i < from.length; i++) {
		int strlen = wcslen(from.items[i]);
		wchar_t *strcp = malloc((strlen+1)*sizeof(wchar_t));
		wcscpy(strcp, from.items[i]);
		strcp[strlen] = '\0';
		arrwc[i] = strcp;
	}
	to.items = arrwc;
	return to;
}
*/


/**
 * Replace the character Œ by Oe for string comparison
 */
wchar_t* UTF32_ReplaceOE(const wchar_t *str) {
	bool replaced = true;
	wchar_t *newstr = malloc(wcslen(str)*sizeof(wchar_t)+1);
	wcscpy(newstr, str);
	newstr[wcslen(str)] = '\0';

	while (replaced) {
		replaced = false;
		for (int i = 0; i < wcslen(newstr); i++) {
			wchar_t* newstr2;
			if (newstr[i] == L'Œ') {
				newstr2 = malloc((wcslen(newstr)+2)*sizeof(wchar_t));
				wcsncpy(newstr2, newstr, i);
				newstr2[i] = L'O';
				newstr2[i+1] = L'e';
				wcscpy(newstr2+i+2, newstr+i+1);
				newstr2[wcslen(newstr)+1] = '\0';
				replaced = true;
			} else if (newstr[i] == L'œ') {
				newstr2 = malloc((wcslen(newstr)+2)*sizeof(wchar_t));
				wcsncpy(newstr2, newstr, i);
				newstr2[i] = L'o';
				newstr2[i+1] = L'e';
				wcscpy(newstr2+i+2, newstr+i+1);
				newstr2[wcslen(newstr)+1] = '\0';
				replaced = true;
			}
			if (replaced) {
				free(newstr);
				newstr = newstr2;
				break;
			}
		}
	}
	return newstr;
}

/**
 * Replace the accented characters by the normal ones for string comparison
 */
wchar_t* UTF32_ReplaceAccentedChar(const wchar_t *str) {
	wchar_t *from = s_utf32(ACCENTED_CHAR_REPLACE_FROM);
	wchar_t *to   = s_utf32(ACCENTED_CHAR_REPLACE_TO);
	int totalChars = 64;

	wchar_t *newstr = malloc(wcslen(str)*sizeof(wchar_t)+1);
	wcscpy(newstr, str);
	newstr[wcslen(str)] = '\0';

	for (int i = 0; i < totalChars; i++) {
		for (int j = 0; j < wcslen(newstr); j++) {
			if (newstr[j] == from[i]) {
				newstr[j] = to[i];
			}
		}
	}
	return newstr;
}

/**
 * Replace Œ and accented characters
 */
wchar_t* UTF32_ReplaceAllComplexChars(const wchar_t *str) {
	return UTF32_ReplaceOE(UTF32_ReplaceAccentedChar(str));
}

/**
 * Comparison function used for sorting
 */
int ArrayUTF32_sort_cmp(const wchar_t *a,const wchar_t *b) {
	return (wcscmp(a,b));
}


/**
 * Sort an ArrayUTF32 and starting at an index
 * This function set the sortedItems and dont change the items attribute
 * Original IDs of sortedItems are stored in sortedItemsID
 */
void ArrayUTF32_sort_starting_index(struct ArrayUTF32 *arr, int index) {
	int nitems = arr->length;

	// On initialise un nouveau tableau de pointeur, ainsi qu'un nouveau tableau d'ID si aucun sort n'a été effectué
	if (!arr->sorted) {
		wchar_t** sortedItems = malloc(arr->length*sizeof(wchar_t*));
		int* sortedItemsID = malloc(arr->length*sizeof(int));
		for (int i = 0; i < arr->length; i++) {
			wchar_t* item = arr->items[i];
			sortedItems[i] = item;
			sortedItemsID[i] = i;
		}
		arr->sortedItems = sortedItems;
		arr->sortedItemsID = sortedItemsID;
		arr->sorted = true;
	}


	// On crée un tableau avec les caractères convertis, pour le trier par la suite
	wchar_t** convertedItems = malloc(arr->length*sizeof(wchar_t*));
	for (int i = 0; i < arr->length; i++) {
		wchar_t* item = arr->items[i];
		convertedItems[i] = UTF32_ReplaceAllComplexChars(item);
	}

	bool asort = true;
	int iteration = 0;
	int lastIndex = index;
	while (asort) {
		asort = false;
		iteration = 0;
		for (int i = lastIndex; i < nitems-1; i++) {
			wchar_t *item1 = convertedItems[i];
			wchar_t *item2 = convertedItems[i+1];
			int idItem1 = arr->sortedItemsID[i];
			int idItem2 = arr->sortedItemsID[i+1];
			while (ArrayUTF32_sort_cmp(item1, item2) > 0) {
				asort = true;
				convertedItems[i] = item2;
				convertedItems[i+1] = item1;
				arr->sortedItemsID[i] = idItem2;
				arr->sortedItemsID[i+1] = idItem1;
				i--;
				if (i < index) {
					break;
				}
				iteration++;

				item1 = convertedItems[i];
				item2 = convertedItems[i+1];
				idItem1 = arr->sortedItemsID[i];
				idItem2 = arr->sortedItemsID[i+1];

			}
			if (asort) {
				lastIndex++;
				// debuglogf("%i / %i (it: %i)...\n", lastIndex, nitems, iteration);
				break;
			}
		}
	}
	// debuglogf("Setting sortedItems and freeing memory...\n");
	for (int i = 0; i < arr->length; i++) {
		int idSortedItem = arr->sortedItemsID[i];
		wchar_t* item = arr->items[idSortedItem];
		arr->sortedItems[i] = item;
		free(convertedItems[i]);
	}

	free(convertedItems);
}

/**
 * Sort an ArrayUTF32 from the beginning
 */
void ArrayUTF32_sort(struct ArrayUTF32 *arr) { ArrayUTF32_sort_starting_index(arr, 0); }


/**
 * Get the list of the localization files
 * If a file don't exist, the english one is used
 *
 * Use the internal system language to detect the correct one
 */
struct i18n_files i18n_getFilesPath() {
	u8 language = 0;
	CFGU_GetSystemLanguage(&language);

	struct i18n_files files = i18n_files_en;

	switch(language) {
		case CFG_LANGUAGE_FR:
			files = i18n_files_fr;
			break;
	}
	// If a file don't exist, we use the english localization to avoid bugs
	if((access( files.abilities, F_OK ) == -1) ||
	   (access( files.species, F_OK ) == -1) ||
	   (access( files.natures, F_OK ) == -1) ||
	   (access( files.moves, F_OK ) == -1) ||
	   (access( files.items, F_OK ) == -1) ||
	   (access( files.hp, F_OK ) == -1) ||
	   (access( files.app, F_OK ) == -1)
	   ){
		files = i18n_files_en;
	}
	return files;
}

void i18n_init() {
	struct i18n_files files = i18n_getFilesPath();
	i18n_AppTexts = i18n_FileToArrayUTF32(files.app);
}

wchar_t* i18n(AppTextCode code) {
	wchar_t *s;
	if (code >= i18n_AppTexts.length) {
		s = L"??? (TnF)";
	} else {
		s = i18n_AppTexts.items[code];
	}
	return s;
}



