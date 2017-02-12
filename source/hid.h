#pragma once

int calcCurrentEntryOneScreen(int currentEntry, int max, int columns);
int calcCurrentEntryOneScreenReversed(int currentEntry, int max, int columns);
void calcCurrentEntryMorePages(int *currentEntry, int *page, int maxpages, int maxentries, int rows);
void calcCurrentEntryMorePagesReversed(int *currentEntry, int *page, int maxpages, int maxentries, int rows);