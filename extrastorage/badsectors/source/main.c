#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <3ds.h>

u8* buffer;
u32 size = 0;
u32 boxes = 0;
u32 k = 0;

bool loadBuffer() {
	FILE *fptr = fopen("sdmc:/3ds/data/PKSM/bank/bank.bin", "rt");
	fseek(fptr, 0, SEEK_END);
	size = ftell(fptr);
	
	if (size % (30*232)) {
		fclose(fptr);
		printf("Extra storage has bad size!\n");
		return false;
	}
	buffer = (u8*)malloc(size * sizeof(u8));
	if (buffer == NULL) {
		fclose(fptr);
		free(buffer);
		printf("Cannot allocate buffer!\n");
		return false;
	}
	rewind(fptr);
	fread(buffer, size, 1, fptr);
	fclose(fptr);
	
	boxes = size/(30*232) - 1;
	
	return true;
}

int main() {
	gfxInitDefault(); 
	consoleInit(GFX_TOP, NULL);
	
	if (loadBuffer()) {
		for (u32 i = 0, n = 30*boxes; i < n; i++) {
			u16 tempSpecies = 0;
			memcpy(&tempSpecies, &buffer[i*232 + 0x8], 2);
			
			if (tempSpecies == 0 || tempSpecies > 802) {
				for (u32 j = 0; j < 232; j++)
					buffer[i*232 + j] = 0;
				printf("Sector #%lu [box %lu slot %lu] restored\n", ++k, i/30 + 1, i%30 + 1);
			}
		}

		FILE *new = fopen("sdmc:/3ds/data/PKSM/bank/bank.bin", "wb");
		fwrite(buffer, 1, size, new);
		fclose(new);
	}
	
	printf("\nTotal sectors replaced: %lu\nPress B to exit.", k);
	gfxFlushBuffers();
	gfxSwapBuffers();
	while(aptMainLoop() && !(hidKeysHeld() & KEY_B)) {
		hidScanInput();
	}

	free(buffer);
	gfxExit();
	return 0;
}