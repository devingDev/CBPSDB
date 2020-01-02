#include "draw_app_list.hpp"
#include <vita2d.h>
#include "app_defines.hpp"
#include "apps.hpp"
#include "draw_font.hpp"

vita2d_texture *bar;
vita2d_texture *scroll;
vita2d_texture *scrollbar;
vita2d_texture *dl;
vita2d_texture *icon;
vita2d_texture *info;

float y_offset = 0.0f;
App * app;
std::string nameOfAppTest = "TEST";
std::string descOfAppTest = "TESTDESC";

void setup_app_list(){
    bar = vita2d_load_PNG_file("app0:assets/HB_BAR.png");
	scroll = vita2d_load_PNG_file("app0:assets/HB_SCROL.png");
	scrollbar = vita2d_load_PNG_file("app0:assets/HB_SCROL_BAR.png");
	dl = vita2d_load_PNG_file("app0:assets/HB_DL_.png");
	icon = vita2d_load_PNG_file("app0:assets/HB_ICON.png");
	info = vita2d_load_PNG_file("app0:assets/HB_INFO.png");

	app = GetApp(0);
	if(app != NULL){
		nameOfAppTest = app->app_name;
		descOfAppTest = app->short_description;
	}
	
}

void draw_app_list(){
    vita2d_draw_rectangle(217, 113, 743, 408, RGBA8(0x49, 0x49, 0x49, 0xFF));
    vita2d_draw_texture(scrollbar, 915, 120);
    vita2d_draw_texture(scroll, 904, 144);
    vita2d_draw_texture(bar, 236, 134 + y_offset);
    vita2d_draw_texture(bar, 236, 258 + y_offset);
    vita2d_draw_texture(bar, 236, 386 + y_offset);

	//vita2d_font_draw_text(commiesans, 460, 160, RGBA8(255,255,255,255), 20.0f, nameOfAppTest.c_str());
	//vita2d_font_draw_text(commiesans, 460, 180, RGBA8(255,255,255,255), 16.0f, descOfAppTest.c_str());

	y_offset -= 0.16f;
}

void end_app_list(){
	vita2d_free_texture(bar);
	vita2d_free_texture(scroll);
	vita2d_free_texture(scrollbar);
	vita2d_free_texture(dl);
	vita2d_free_texture(icon);
	vita2d_free_texture(info);
}