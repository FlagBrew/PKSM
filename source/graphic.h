#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>

#define BLACK RGBA8(0, 0, 0, 255)
#define RED RGBA8(255, 0, 0, 255)
#define GREY RGBA8(0xE3, 0xE3, 0xE3, 0xFF)
#define DARKGREY RGBA8(0x60, 0x60, 0x60, 0xFF)

#define FILL 5
#define textHeight 20
#define SIZE 13

void autoupdaterMenu(sftd_font *fontRegular, sftd_font *fontBold, sf2d_texture *button);
void refreshMenu(sftd_font *font, sftd_font *bold, char *menu[], int option, int n);
int giveTransparence(int trasp, bool vanish[]);
void gameSelectorMenu(sftd_font *fontRegular, sftd_font *fontBold, sf2d_texture *button, sf2d_texture *banner, char* gamesList[], int game, int N);