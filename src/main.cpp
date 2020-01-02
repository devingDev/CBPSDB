#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <psp2/kernel/processmgr.h>
#include <vita2d.h>
#include "app_defines.hpp"
#include "draw_side_menu.hpp"
#include "draw_app_list.hpp"
#include "apps.hpp"
#include "draw_font.hpp"

const char* TITLE = "CBPS HOMEBREW BROWSER";
const char* COPYRIGHT = " 2020 by CBPS";


vita2d_font* testSans;

int main(int argc, char *argv[]) {
	int loadedFonts = loadFonts();
	loadAppsJson();
	LoadAppsSmart();


	vita2d_init_advanced(8 * 1024 * 1024);
	vita2d_set_clear_color(RGBA8(0xC3, 0xC3, 0xC3, 0xFF));
	
	vita2d_texture *appicon = vita2d_load_PNG_file("app0:assets/APP_ICON.png");
	vita2d_texture *search = vita2d_load_PNG_file("app0:assets/System_search_icon.png");
	vita2d_texture *copyright = vita2d_load_PNG_file("app0:assets/Copyright.png");
	vita2d_texture *rtrigger = vita2d_load_PNG_file("app0:assets/PSVita/Vita_Bumper_Right.png");
	vita2d_texture *dpad = vita2d_load_PNG_file("app0:assets/PSVita/Vita_Dpad.png");
	vita2d_texture *cross = vita2d_load_PNG_file("app0:assets/PSVita/Vita_Cross.png");
	vita2d_texture *circle = vita2d_load_PNG_file("app0:assets/PSVita/Vita_Circle.png");

	testSans = vita2d_load_font_file("app0:assets/ldfsans.ttf");

	setup_side_menu();
	setup_app_list();
	

	//vita2d_font * commiesans = vita2d_load_font_file("app0:assets/LDFCOMMIUNISMSANS.ttf");

	while(1){
		vita2d_start_drawing();
		vita2d_clear_screen();
		

		//vita2d_draw_rectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, RGBA8(0xC3, 0xC3, 0xC3, 0xFF));
		

		draw_app_list();
		vita2d_draw_rectangle(0, 521, DISPLAY_WIDTH, 23, RGBA8(0x49, 0x49, 0x49, 0xFF));
		draw_side_menu();
		vita2d_draw_rectangle(220, 1, 740, 107, RGBA8(0x49, 0x49, 0x49, 0xFF));
		vita2d_draw_rectangle(218,108, 742, 5, RGBA8(0xC3, 0xC3, 0xC3, 0xFF));


		vita2d_draw_texture(appicon, 55, 13);

		vita2d_draw_texture(search, DISPLAY_WIDTH - 80, 8);
		vita2d_draw_texture_scale(rtrigger, DISPLAY_WIDTH - 75, 50, 0.6f, 0.6f);

		vita2d_draw_texture_scale(copyright, 15, DISPLAY_HEIGHT - 23, 0.115f, 0.115f);
		vita2d_draw_texture_scale(dpad, 220, DISPLAY_HEIGHT - 23, 0.23f, 0.23f);
		vita2d_draw_texture_scale(cross, 450, DISPLAY_HEIGHT - 23, 0.23f, 0.23f);
		vita2d_draw_texture_scale(circle, 700, DISPLAY_HEIGHT - 23, 0.23f, 0.23f);

		

		
		//vita2d_draw_line(0, 107, DISPLAY_WIDTH, 107, RGBA8(255, 255, 255, 255));
		
		if(loadedFonts < 0){
			vita2d_draw_rectangle(0,0,48,48, RGBA8(255,0,0,255));
		}

		if(commiesans == NULL){
			vita2d_draw_rectangle(0,48,48,48, RGBA8(255,255,0,255));
		}

		vita2d_font_draw_text(testSans, 273, 42, RGBA8(255,0,50,255), 31, TITLE );
/* These don't work when using the included one : 
	They work on vita3k thou 
		vita2d_font_draw_text(commiesans, 45, DISPLAY_HEIGHT - 6, RGBA8(0xED,0x3E,0x19,255), 12.0f, COPYRIGHT);

		vita2d_font_draw_text(commiesans, 250, DISPLAY_HEIGHT - 6, RGBA8(0xED,0x3E,0x19,255), 13.0f, "Move around (list/categories)");
		vita2d_font_draw_text(commiesans, 480, DISPLAY_HEIGHT - 6, RGBA8(0xED,0x3E,0x19,255), 13.0f, "Submit / Enter");
		vita2d_font_draw_text(commiesans, 730, DISPLAY_HEIGHT - 6, RGBA8(0xED,0x3E,0x19,255), 13.0f, "Go back / Cancel");
*/

		vita2d_end_drawing();
		vita2d_swap_buffers();
		vita2d_wait_rendering_done();
	}
	
	vita2d_fini();

	vita2d_free_texture(appicon);
	vita2d_free_texture(search);
	vita2d_free_texture(copyright);
	vita2d_free_texture(rtrigger);
	vita2d_free_texture(dpad);
	vita2d_free_texture(cross);
	vita2d_free_texture(circle);
	vita2d_free_font(testSans);
	unloadFonts();

	end_side_menu();
	end_app_list();

	sceKernelExitProcess(0);
    return 0;
}
