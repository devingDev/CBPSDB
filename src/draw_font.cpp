#include "draw_font.hpp"

vita2d_font *commiesans;

int loadFonts(){
	commiesans = vita2d_load_font_file("app0:assets/ldfsans.ttf");
    if(commiesans == NULL){
        return -1;
    }
    return 0;
}

int unloadFonts(){
	vita2d_free_font(commiesans);
    return 0;
}

vita2d_font * GetCommieFont(){
    return commiesans;
}
