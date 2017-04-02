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

static wchar_t* ACCENTED_CHAR_REPLACE_FROM = L"ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖ×ØÙÚÛÜÝÞßàáâãäåæçèéêëìíîïðñòóôõö÷øùúûüýþÿ";
static wchar_t* ACCENTED_CHAR_REPLACE_TO = L"AAAAAAECEEEEIIIIDNOOOOOx0UUUUYPsaaaaaaeceeeeiiiiOnooooo/0uuuuypy";

const int MAXLENGTH_LINE_TRANSLATION = 255;
const int DEFAULT_LANG = 1;
const int MAXLENGTH_PATH = 255;

static char* LANG_PREFIX[] = { "jp", "en", "fr", "de", "it", "es", "zh", "ko", "nl", "pt", "ru", "tw" };

/**
 * Generic path for Localization files
 */
struct i18n_files i18n_files_generic_paths = {
	"romfs:/i18n/%s/abilities.txt",
	"romfs:/i18n/%s/species.txt",
	"romfs:/i18n/%s/natures.txt",
	"romfs:/i18n/%s/moves.txt",
	"romfs:/i18n/%s/items.txt",
	"romfs:/i18n/%s/hp.txt",
	"romfs:/i18n/%s/forms.txt",
	"romfs:/i18n/%s/balls.txt",
	"romfs:/i18n/%s/types.png",
	"romfs:/i18n/%s/app.txt"
};

/**
 * Current localization files loaded
 */
struct i18n_files i18n_files_loaded;
ArrayUTF32 i18n_AppTexts;
u32 *text;

/**
 * Log debug with format
 */
void debuglogf(const char* format, ...) {
#if DEBUG_I18N
	va_list args;
    va_start(args, format);
	#ifdef VERSION
		char str[255];
		vsprintf(str, format, args);
		consoleMsg(str);
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
 * Transform a string from UTF8 (char*) to UTF32 with a specified memory size
 */
wchar_t* ss_utf32(char* str, int size) {
	int length = utf8_strlen(str)+1;
	if (size == 0) {
		size = length;
	}
	text = malloc(size*sizeof(u32));
	utf8_to_utf32(text, (unsigned char*)str, length-1);
	text[length-1] = '\0';
	return (wchar_t*)text;
}

/**
 * Transform a string from UTF8 (char*) to UTF32 (wchar_t*)
 */
wchar_t* s_utf32(char* str) { return ss_utf32(str, 0); }


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
    char ch = ' ', lastch = ch;
	if( access( filepath, F_OK ) != -1 ) {
		FILE *fp = fopen(filepath, "r");
		while((ch = fgetc(fp)) && !feof(fp)) {
			currentMaxChar++;
			if(ch == '\n') {
				lines++;
				if (currentMaxChar > maxChar) {
					maxChar = currentMaxChar;
				}
				currentMaxChar = 0;
			} else if (ch != '\r' && ch != '\n' && ch > 0) {
				totalChars++;
			}
			lastch = ch;
		}
		if (currentMaxChar > maxChar) {
			maxChar = currentMaxChar;
		}
		// The last line
		if (totalChars > 0 && lastch != '\r' && lastch != '\n') {
			lines++;
		}
		fclose(fp);
	}

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
ArrayUTF32 i18n_FileToArrayUTF32(char* filepath) {
    struct i18n_FileInfo fileinfo = i18n_getInfoFile(filepath);
	int BUFFER_SIZE = MAXLENGTH_LINE_TRANSLATION;
    wchar_t **arrwc = malloc(fileinfo.numberOfLines * sizeof(wchar_t*));
	if( access( filepath, F_OK ) != -1 ) {
		FILE* fp = fopen(filepath, "rt");
		char line[BUFFER_SIZE];
		int index = 0;

		while (fgets(line, BUFFER_SIZE, fp) != 0) {
			if (index < fileinfo.numberOfLines) {
				i18n_removeEndline(line);
				wchar_t* lineutf32 = s_utf32(line);
				arrwc[index] = lineutf32;
				index++;
			}

		}

		if (fclose(fp)) {
			// debuglogf("Error closing file !\n");
		}
	}

	// debuglogf("Creating array...\n");
	ArrayUTF32 arr;
	arr.length = fileinfo.numberOfLines;
	arr.items = arrwc;
	arr.sorted = false;

	return arr;
}
/**
 * Free an arrayUTF32
 */
void i18n_free_ArrayUTF32(ArrayUTF32 *arr) {
    for (int i = 0; i < arr->length; i++) {
		free(arr->items[i]);
	}
	free(arr->items);
}

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
	wchar_t *from = ACCENTED_CHAR_REPLACE_FROM;
	wchar_t *to   = ACCENTED_CHAR_REPLACE_TO;
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
	wchar_t* stringReplacedChar = UTF32_ReplaceAccentedChar(str);
	wchar_t* stringReplacedCharAndOE = UTF32_ReplaceOE(stringReplacedChar);
	free(stringReplacedChar);
	return stringReplacedCharAndOE;
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
 * You can specify a custom function to compare 2 items
 */
void ArrayUTF32_sort_starting_index_with_sort_func(ArrayUTF32 *arr, int index, int (*fsort)(const wchar_t *a,const wchar_t *b)) {
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
			while (fsort(item1, item2) > 0) {
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
 * Sort an ArrayUTF32 and starting at an index
 * This function set the sortedItems and dont change the items attribute
 * Original IDs of sortedItems are stored in sortedItemsID
 * Use the default function of sorting UTF32 strings
 */
void ArrayUTF32_sort_starting_index(ArrayUTF32 *arr, int index) {
	ArrayUTF32_sort_starting_index_with_sort_func(arr, index, ArrayUTF32_sort_cmp);
}

/**
 * Sort an ArrayUTF32 from the beginning
 */
void ArrayUTF32_sort(ArrayUTF32 *arr) { ArrayUTF32_sort_starting_index(arr, 0); }


/**
 * Get the list of the localization files
 * If a file don't exist, the english one is used
 *
 * Use the internal system language to detect the correct one
 */
struct i18n_files i18n_getFilesPath() {
	return i18n_files_loaded;
}

void i18n_set_language_filepath(u8 language, const char* generic_path, char* path) {
	int BUFFER_SIZE = 255;
	char defaultlang_path[BUFFER_SIZE];
	char *correctlang_path;

	sprintf(defaultlang_path, generic_path, LANG_PREFIX[DEFAULT_LANG]);
	if (language == DEFAULT_LANG) {
		correctlang_path = defaultlang_path;
	} else {
		char lang_path[BUFFER_SIZE];
		sprintf(lang_path, generic_path, LANG_PREFIX[language]);
		struct i18n_FileInfo finfoLang = i18n_getInfoFile(lang_path);
		struct i18n_FileInfo finfoDefaultLang = i18n_getInfoFile(defaultlang_path);

		if (finfoLang.numberOfLines != finfoDefaultLang.numberOfLines) {
			// debuglogf("File [%s] haven't the correct number of lines : %i != %i!", lang_path, finfoDefaultLang.numberOfLines, finfoLang.numberOfLines);
			correctlang_path = defaultlang_path;
		} else {
			correctlang_path = lang_path;
		}
	}
	strcpy(path, correctlang_path);
	path[strlen(correctlang_path)] = '\0';
}


void i18n_load(u8 language) {
	struct i18n_files files = i18n_files_generic_paths;
	i18n_set_language_filepath(language, files.abilities, i18n_files_loaded.abilities);
	i18n_set_language_filepath(language, files.species, i18n_files_loaded.species);
	i18n_set_language_filepath(language, files.natures, i18n_files_loaded.natures);
	i18n_set_language_filepath(language, files.moves, i18n_files_loaded.moves);
	i18n_set_language_filepath(language, files.items, i18n_files_loaded.items);
	i18n_set_language_filepath(language, files.hp, i18n_files_loaded.hp);
	i18n_set_language_filepath(language, files.forms, i18n_files_loaded.forms);
	i18n_set_language_filepath(language, files.balls, i18n_files_loaded.balls);
	i18n_set_language_filepath(language, files.types, i18n_files_loaded.types);
	i18n_set_language_filepath(language, files.app, i18n_files_loaded.app);

	i18n_AppTexts = i18n_FileToArrayUTF32(i18n_files_loaded.app);
}

void i18n_init() {
	u8 language = 0;
	if (!checkFile("sdmc:/3ds/data/PKSM/i18n.bin")) {
		CFGU_GetSystemLanguage(&language);
		u8 localeConfig[1];
		localeConfig[0] = language;
		
		FILE *conf = fopen("sdmc:/3ds/data/PKSM/i18n.bin", "wb");
		fwrite(localeConfig, 1, 1, conf);
		fclose(conf);
	} else {
		FILE *conf = fopen("sdmc:/3ds/data/PKSM/i18n.bin", "rt");
		fseek(conf, 0, SEEK_END);
		u8 localeConfig[1];
		rewind(conf);
		fread(localeConfig, 1, 1, conf);
		fclose(conf);
	
		language = localeConfig[0];
	}
	
	#ifdef DEBUG_I18N_LANG
		language = (u8)DEBUG_I18N_LANG;
	#endif
	int sizefilepath = sizeof(char)*MAXLENGTH_PATH;
	i18n_files_loaded.abilities = malloc(sizefilepath);
	i18n_files_loaded.species = malloc(sizefilepath);
	i18n_files_loaded.natures = malloc(sizefilepath);
	i18n_files_loaded.moves = malloc(sizefilepath);
	i18n_files_loaded.items = malloc(sizefilepath);
	i18n_files_loaded.hp = malloc(sizefilepath);
	i18n_files_loaded.forms = malloc(sizefilepath);
	i18n_files_loaded.balls = malloc(sizefilepath);
	i18n_files_loaded.types = malloc(sizefilepath);
	i18n_files_loaded.app = malloc(sizefilepath);
	i18n_load(language);
}


wchar_t* i18n(AppTextCode code) {
	wchar_t *s;
	if (code >= i18n_AppTexts.length) {
		s = L"???";
	} else {
		s = i18n_AppTexts.items[code];
	}
	return s;
}

void i18n_initTextSwkbd(SwkbdState* swkbd, AppTextCode leftButtonTextCode, AppTextCode rightButtonTextCode, AppTextCode hintTextCode) {
	wchar_t* leftButtonWText = i18n(leftButtonTextCode);
	wchar_t* rightButtonWText = i18n(rightButtonTextCode);
	wchar_t* hintWText = i18n(hintTextCode);
	static unsigned char leftButtonText[SWKBD_MAX_BUTTON_TEXT_LEN+1];
	static unsigned char rightButtonText[SWKBD_MAX_BUTTON_TEXT_LEN+1];
	static unsigned char hintText[SWKBD_MAX_HINT_TEXT_LEN+1];
	leftButtonText[SWKBD_MAX_BUTTON_TEXT_LEN] = '\0';
	rightButtonText[SWKBD_MAX_BUTTON_TEXT_LEN] = '\0';
	hintText[SWKBD_MAX_HINT_TEXT_LEN] = '\0';

	int sizeLeftButtonText = (int)utf32_to_utf8(leftButtonText, (uint32_t*)leftButtonWText, SWKBD_MAX_BUTTON_TEXT_LEN+1);
	int sizeRightButtonText = (int)utf32_to_utf8(rightButtonText, (uint32_t*)rightButtonWText, SWKBD_MAX_BUTTON_TEXT_LEN+1);
	int sizeHintText = (int)utf32_to_utf8(hintText, (uint32_t*)hintWText, SWKBD_MAX_HINT_TEXT_LEN+1);

	leftButtonText[sizeLeftButtonText] = '\0';
	rightButtonText[sizeRightButtonText] = '\0';
	hintText[sizeHintText] = '\0';

	swkbdSetButton(swkbd, SWKBD_BUTTON_LEFT, (char*)leftButtonText, false);
	swkbdSetButton(swkbd, SWKBD_BUTTON_RIGHT, (char*)rightButtonText, true);
	swkbdSetHintText(swkbd, (char*)hintText);
}

void i18n_exit() {
	free(text);
	
	free(i18n_files_loaded.abilities);
	free(i18n_files_loaded.species);
	free(i18n_files_loaded.natures);
	free(i18n_files_loaded.moves);
	free(i18n_files_loaded.items);
	free(i18n_files_loaded.hp);
	free(i18n_files_loaded.forms);
	free(i18n_files_loaded.balls);
	free(i18n_files_loaded.types);
	free(i18n_files_loaded.app);

	i18n_free_ArrayUTF32(&i18n_AppTexts);

}
