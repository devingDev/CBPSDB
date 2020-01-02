#include "draw_font.hpp"

vita2d_font * commiesans;

int loadFonts(){
	commiesans = vita2d_load_font_file("app0:assets/LDFCOMMIUNISMSANS.ttf");
    return 0;
}

int unloadFonts(){
	vita2d_free_font(commiesans);
}

vita2d_font * GetCommieFont(){
    return commiesans;
}
