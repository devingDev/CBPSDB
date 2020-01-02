#include "draw_side_menu.hpp"
#include <vita2d.h>
#include "app_defines.hpp"

vita2d_texture *category;

void setup_side_menu(){
    category = vita2d_load_PNG_file("app0:assets/HB_CATAGORY.png");
	
}

void draw_side_menu(){
    vita2d_draw_rectangle(0, 0, 211, 515, RGBA8(0x49, 0x49, 0x49, 0xFF));
    vita2d_draw_texture(category, 22, 128);
    vita2d_draw_texture(category, 22, 206);
    vita2d_draw_texture(category, 22, 284);
    vita2d_draw_texture(category, 22, 364);
    vita2d_draw_texture(category, 22, 438);
}

void end_side_menu(){
	vita2d_free_texture(category);
}