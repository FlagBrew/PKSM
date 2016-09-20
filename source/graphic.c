#include <sf2d.h>
#include <sftd.h>
#include <sfil.h>
#include "graphic.h"

int trasp = 255;
bool vanish[1] = {true};

void refreshMenu(sftd_font *font, sftd_font *bold, char *menu[], int option, int n) {
	for (int i = 0; i < n; i++) {
		if (i == option) 
			sftd_draw_text(bold, 1, 15 + i * 10, RED, 10, menu[i]);
		else 
			sftd_draw_text(font, 1, 15 + i * 10, BLACK, 10, menu[i]);
	}
}

int giveTransparence(int trasp, bool vanish[]) {
	if (vanish[0] && trasp > 80)
		trasp--;
	if (trasp == 80)
		vanish[0] = false;
	if (!(vanish[0]) && trasp < 255)
		trasp++;
	if (trasp == 255)
		vanish[0] = true;
	
	return trasp;
}

void autoupdaterMenu(sftd_font *fontRegular, sftd_font *fontBold, sf2d_texture *button) {
	int button_pos = (400 - FILL - button->width - sftd_get_text_width(fontRegular, SIZE, "Check")) / 2;
	int scarto = (button->width - sftd_get_text_width(fontRegular, 10, "A")) / 2;
	trasp = giveTransparence(trasp, vanish);
	
	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	sftd_draw_text(fontBold, (400 - sftd_get_text_width(fontBold, SIZE, "Do you want to check for updates?")) / 2, (240 - 20) / 2, RGBA8(0, 0, 0, trasp), SIZE, "Do you want to check for updates?");
	sf2d_draw_texture(button, button_pos, 240/2 - button->height/2 + 20);
	sftd_draw_text(fontRegular, button_pos + scarto, (240 - textHeight) / 2 + 20 + scarto, DARKGREY, 10, "A");	
	sf2d_draw_texture(button, button_pos, 240/2 - button->height/2 + 38);
	sftd_draw_text(fontRegular, button_pos + scarto, (240 - textHeight) / 2 + 38 + scarto, DARKGREY, 10, "B");	
	sftd_draw_text(fontRegular, button_pos + button->width + FILL, (240 - textHeight) / 2 + 20 + (textHeight - button->height) / 2, BLACK, SIZE - 1, "Check");	
	sftd_draw_text(fontRegular, button_pos + button->width + FILL, (240 - textHeight) / 2 + 38 + (textHeight - button->height) / 2, BLACK, SIZE - 1, "Skip");
	sf2d_end_frame();
	sf2d_swapbuffers();
}

void gameSelectorMenu(sftd_font *fontRegular, sftd_font *fontBold, sf2d_texture *button, sf2d_texture *banner, char* gamesList[], int game, int N) {
	int scarto = (button->width - sftd_get_text_width(fontRegular, 10, "A")) / 2;
	int button_pos = (320 - FILL - button->width - sftd_get_text_width(fontRegular, SIZE, "Continue")) / 2;

	sf2d_start_frame(GFX_TOP, GFX_LEFT);
	sftd_draw_text(fontBold, 1, 0, BLACK, SIZE - 1, "Choose game.   Cart has priority over digital copy.");
	refreshMenu(fontRegular, fontBold, gamesList, game, N);
	sf2d_draw_texture(banner, 185, 75);
	sf2d_end_frame();
	
	sf2d_start_frame(GFX_BOTTOM, GFX_LEFT);
	sf2d_draw_texture(button, button_pos, 240/2 - button->height/2);
	sftd_draw_text(fontRegular, button_pos + scarto, (240 - textHeight) / 2 + scarto, DARKGREY, 10, "A");	
	sf2d_draw_texture(button, button_pos, 240/2 - button->height/2 + 18);
	sftd_draw_text(fontRegular, button_pos + scarto, (240 - textHeight) / 2 + 18 + scarto, DARKGREY, 10, "B");	
	sftd_draw_text(fontRegular, button_pos + button->width + FILL, (240 - textHeight) / 2 + (textHeight - button->height) / 2, BLACK, SIZE - 1, "Continue");	
	sftd_draw_text(fontRegular, button_pos + button->width + FILL, (240 - textHeight) / 2 + 18 + (textHeight - button->height) / 2, BLACK, SIZE - 1, "Exit");		
	sf2d_end_frame();
	sf2d_swapbuffers();
}