#include "draw_app_list.hpp"
#include <vita2d.h>
#include "app_defines.hpp"
#include "apps.hpp"
#include "main.hpp"

class AppEntry {
	public:
		int index = 0;
		vita2d_texture * icon;
		bool failedIconLoad;
		bool triedDownload;
		
		AppEntry();
		~AppEntry();
};
AppEntry::AppEntry(){}
AppEntry::~AppEntry(){}


vita2d_texture *bar;
vita2d_texture *scroll;
vita2d_texture *scrollbar;
vita2d_texture *dl;
vita2d_texture *icon;
vita2d_texture *info;

vita2d_font *commieSansL;

float y_offset = 0.0f;
std::string nameOfAppTest = "TEST";
std::string descOfAppTest = "TESTDESC";

std::vector<AppEntry> appEntries;

const int entries_y_start = 130;
const int entries_y_diff= 125;

VitaNet vitaNet;

vita2d_texture * loadIcon(const char * path){
	return vita2d_load_PNG_file(path);
}
std::string createIconPath(std::string id){
	std::string path = "ux0:data/cbps/icons/";
	path.append(id);
	path.append("/");
  	sceIoMkdir( path.c_str() , 0777);
	path.append("icon.png");
	return path;
}
vita2d_texture * loadIcon(std::string id){
	std::string path = createIconPath(id);
	return vita2d_load_PNG_file(path.c_str());
}

void DownloadAppIcon(std::string url, std::string id, AppEntry * appEntry){
	std::string path = createIconPath(id);
	VitaNet::http_response resp = vitaNet.curlDownloadFile(url, "", path);

	if(resp.httpcode == 200){
		appEntry->icon = loadIcon(path.c_str());
		if(appEntry->icon == NULL){
			appEntry->failedIconLoad = true;
			appEntry->triedDownload = true;
		}
	}else{
	}
}

void downloadJson(){
	std::string dbJsonPath = "ux0:data/cbps/db.json";
	std::string jsonUrl = std::string(BASE_URL);
	jsonUrl.append("default_db.json");
	VitaNet::http_response resp = vitaNet.curlDownloadFile(jsonUrl, "", dbJsonPath);
	if(resp.httpcode != 200){
		//sceIoRemove(dbJsonPath.c_str());
	}
}

void setup_app_list(){
    bar = vita2d_load_PNG_file("app0:assets/HB_BAR.png");
	scroll = vita2d_load_PNG_file("app0:assets/HB_SCROL.png");
	scrollbar = vita2d_load_PNG_file("app0:assets/HB_SCROL_BAR.png");
	dl = vita2d_load_PNG_file("app0:assets/HB_DL_.png");
	icon = vita2d_load_PNG_file("app0:assets/HB_ICON.png");
	info = vita2d_load_PNG_file("app0:assets/HB_INFO.png");
	commieSansL = vita2d_load_font_file("app0:assets/LDFCOMMIUNISMSANS.ttf");

	appEntries.clear();

	int currentY = entries_y_start;
	for(int i = 0; i < GetAppsAmount(); i++){
		App * app = GetApp(i);
		if(app == NULL){
			continue;
		}
		AppEntry appEntry;
		appEntry.index = i;
		
		appEntries.push_back(appEntry);


		if(currentY < 1000){
			appEntries[i].icon = loadIcon(app->id);
			if(appEntries[i].icon == NULL){
				appEntries[i].failedIconLoad = true;
				DownloadAppIcon(app->thumbnailUrl, app->id, &appEntries[i]);
			}else{
				appEntries[i].failedIconLoad = false;
			}
		}

		currentY += entries_y_diff;
	}
	
}

void do_checks_before_draw(){
	int currentY = entries_y_start + y_offset;
	for(int i = 0; i < appEntries.size(); i++){

		if(currentY < -500){
			if(appEntries[i].icon != NULL){
				vita2d_free_texture(appEntries[i].icon);
			}
			continue;
		}
		
		if(currentY > 1000){
			if(appEntries[i].icon != NULL){
				vita2d_free_texture(appEntries[i].icon);
			}
			continue;
		}

		if(appEntries[i].icon == NULL){
			if(appEntries[i].triedDownload == true && appEntries[i].failedIconLoad == true){
				continue;
			}
			appEntries[i].icon = loadIcon(GetApp(i)->id);
			if(appEntries[i].icon == NULL){
				appEntries[i].failedIconLoad = true;
				DownloadAppIcon(GetApp(i)->thumbnailUrl, GetApp(i)->id, &appEntries[i]);
			}else{
				appEntries[i].failedIconLoad = false;
			}
		}
		
		currentY += entries_y_diff;
	}
}

void draw_app_list(){
    vita2d_draw_rectangle(217, 113, 743, 408, RGBA8(0x49, 0x49, 0x49, 0xFF));
    vita2d_draw_texture(scrollbar, 915, 120);
    vita2d_draw_texture(scroll, 904, 144);

	int currentY = entries_y_start + y_offset;
	for(int i = 0; i < appEntries.size(); i++){

		if(currentY < -500){
			continue;
		}
		
		if(currentY > 1000){
			break;
		}

    	vita2d_draw_texture(bar, 236, currentY + y_offset);
		vita2d_font_draw_text(commieSansL, 460, currentY + 30 + y_offset, RGBA8(255,255,255,255), 26.0f, GetApp(i)->app_name.c_str());
		vita2d_font_draw_text(commieSansL, 460, currentY + 50 + y_offset, RGBA8(255,255,255,255), 18.0f, GetApp(i)->short_description.c_str());
		if(appEntries[i].icon != NULL){
			vita2d_draw_texture_scale(appEntries[i].icon, 250, currentY + 20 + y_offset, 0.5f, 0.5f);
		}
		
		currentY += entries_y_diff;
	}


	y_offset -= 0.5f;
}

void end_app_list(){
	vita2d_free_texture(bar);
	vita2d_free_texture(scroll);
	vita2d_free_texture(scrollbar);
	vita2d_free_texture(dl);
	vita2d_free_texture(icon);
	vita2d_free_texture(info);
	vita2d_free_font(commieSansL);
}