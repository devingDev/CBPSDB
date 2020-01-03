#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include <psp2/kernel/processmgr.h>
#include <vita2d.h>
#include <psp2/display.h>
#include <psp2/promoterutil.h>
#include <psp2/sysmodule.h>
#include <psp2/io/stat.h>
#include <psp2/io/fcntl.h>
#include <psp2/ctrl.h>

#include "app_defines.hpp"
#include "draw_side_menu.hpp"
#include "draw_app_list.hpp"
#include "apps.hpp"
#include "VitaNet.hpp"
#include "main.hpp"

#include "./myfile.h"
#include "./headgen.h"
#include "./zip.hpp"

int _newlib_heap_size_user = 128 * 1024 * 1024;


static int loadScePaf() {
  static uint32_t argp[] = { 0x180000, 0xFFFFFFFF, 0xFFFFFFFF, 1, 0xFFFFFFFF, 0xFFFFFFFF };

  int result = -1;

  uint32_t buf[4];
  buf[0] = sizeof(buf);
  buf[1] = (uint32_t)&result;
  buf[2] = -1;
  buf[3] = -1;

  return sceSysmoduleLoadModuleInternalWithArg(SCE_SYSMODULE_INTERNAL_PAF, sizeof(argp), argp, buf);
}

static int unloadScePaf() {
  uint32_t buf = 0;
  return sceSysmoduleUnloadModuleInternalWithArg(SCE_SYSMODULE_INTERNAL_PAF, 0, NULL, &buf);
}

// Name is used for temporary folder so don't put any weird strings in there !
void installApp(std::string srcFile , const char* name ){

	removePath(PACKAGE_DIR);

	Zipfile z = Zipfile(srcFile);
	z.Unzip(std::string(PACKAGE_DIR "/"));

	generateHeadBin(PACKAGE_DIR);

	// Start promoter stuff
	loadScePaf();
	sceSysmoduleLoadModuleInternal(SCE_SYSMODULE_INTERNAL_PROMOTER_UTIL);
	scePromoterUtilityInit();

	if (scePromoterUtilityPromotePkgWithRif(PACKAGE_DIR , 1) == 0)
	{
	//debugNetPrinf(DEBUG,"Successful install of %s \n" , name);
	}
	else
	{
	//debugNetPrinf(DEBUG,"Failed to install %s \n" , name);
	}

	// End promoter stuff
	scePromoterUtilityExit();
	sceSysmoduleUnloadModuleInternal(SCE_SYSMODULE_INTERNAL_PROMOTER_UTIL);
	unloadScePaf();

	// Remove Vpk
	sceIoRemove(srcFile.c_str());
}

const char* TITLE = "CBPS HOMEBREW BROWSER";
const char* COPYRIGHT = " 2020 by CBPS";



int main(int argc, char *argv[]) {
	//int loadedFonts = loadFonts();
  	sceIoMkdir( "ux0:data/cbps/" , 0777);
  	sceIoMkdir( "ux0:data/cbps/icons/" , 0777);
  	sceIoMkdir( "ux0:data/cbps/pkg/" , 0777);
	
	downloadJson();
	loadAppsJson();
	LoadAppsSmart();

	/*
	std::string vpkPath = "ux0:data/cbps/vita_cord.vpk";
	VitaNet::http_response resp = vitaNet.curlDownloadFile("https://github.com/devingDev/VitaCord/releases/download/1.5fix1/vita_cord.vpk",
	 "",
	 vpkPath);
	if(resp.httpcode == 200){
		installApp(vpkPath, "testinstall");
	}
	*/

	vita2d_init_advanced(8 * 1024 * 1024);
	vita2d_set_clear_color(RGBA8(0xC3, 0xC3, 0xC3, 0xFF));
	
	vita2d_texture *appicon = vita2d_load_PNG_file("app0:assets/APP_ICON.png");
	vita2d_texture *search = vita2d_load_PNG_file("app0:assets/System_search_icon.png");
	vita2d_texture *copyright = vita2d_load_PNG_file("app0:assets/Copyright.png");
	vita2d_texture *rtrigger = vita2d_load_PNG_file("app0:assets/PSVita/Vita_Bumper_Right.png");
	vita2d_texture *dpad = vita2d_load_PNG_file("app0:assets/PSVita/Vita_Dpad.png");
	vita2d_texture *cross = vita2d_load_PNG_file("app0:assets/PSVita/Vita_Cross.png");
	vita2d_texture *circle = vita2d_load_PNG_file("app0:assets/PSVita/Vita_Circle.png");

	setup_side_menu();
	setup_app_list();
	

	vita2d_font * commieSans = vita2d_load_font_file("app0:assets/LDFCOMMIUNISMSANS.ttf");

	while(1){

		do_checks_before_draw();

		vita2d_start_drawing();
		vita2d_clear_screen();
		

		//vita2d_draw_rectangle(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, RGBA8(0xC3, 0xC3, 0xC3, 0xFF));
		

		draw_app_list();
		vita2d_draw_rectangle(0, 521, DISPLAY_WIDTH, 23, RGBA8(0x49, 0x49, 0x49, 0xFF));
		draw_side_menu();
		vita2d_draw_rectangle(220, 0, 740, 108, RGBA8(0x49, 0x49, 0x49, 0xFF));
		vita2d_draw_rectangle(218,108, 742, 5, RGBA8(0xC3, 0xC3, 0xC3, 0xFF));


		vita2d_draw_texture(appicon, 55, 13);

		vita2d_draw_texture(search, DISPLAY_WIDTH - 80, 8);
		vita2d_draw_texture_scale(rtrigger, DISPLAY_WIDTH - 75, 50, 0.6f, 0.6f);

		vita2d_draw_texture_scale(copyright, 15, DISPLAY_HEIGHT - 23, 0.115f, 0.115f);
		vita2d_draw_texture_scale(dpad, 220, DISPLAY_HEIGHT - 23, 0.23f, 0.23f);
		vita2d_draw_texture_scale(cross, 450, DISPLAY_HEIGHT - 23, 0.23f, 0.23f);
		vita2d_draw_texture_scale(circle, 700, DISPLAY_HEIGHT - 23, 0.23f, 0.23f);


		vita2d_font_draw_text(commieSans, 273, 42, RGBA8(255,0,50,255), 31, TITLE );
		vita2d_font_draw_text(commieSans, 45, DISPLAY_HEIGHT - 6, RGBA8(0xED,0x3E,0x19,255), 12.0f, COPYRIGHT);
		vita2d_font_draw_text(commieSans, 250, DISPLAY_HEIGHT - 6, RGBA8(0xED,0x3E,0x19,255), 13.0f, "Move around (list/categories)");
		vita2d_font_draw_text(commieSans, 480, DISPLAY_HEIGHT - 6, RGBA8(0xED,0x3E,0x19,255), 13.0f, "Submit / Enter");
		vita2d_font_draw_text(commieSans, 730, DISPLAY_HEIGHT - 6, RGBA8(0xED,0x3E,0x19,255), 13.0f, "Go back / Cancel");


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
	vita2d_free_font(commieSans);
	//unloadFonts();

	end_side_menu();
	end_app_list();

	sceKernelExitProcess(0);
    return 0;
}
