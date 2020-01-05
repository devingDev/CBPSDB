#include <vita2d.h>
#include <queue>
#include <mutex>
#include <psp2/kernel/threadmgr.h>
#include "draw_app_list.hpp"
#include "app_defines.hpp"
#include "apps.hpp"
#include "VitaNet.hpp"

class AppEntry {
	public:
		int index = 0;
		std::string id = 0;
		
};

VitaNet vitaNet;


vita2d_texture *bar;
vita2d_texture *scroll;
vita2d_texture *scrollbar;
vita2d_texture *dl;
vita2d_texture *icon;
vita2d_texture *info;
vita2d_font *commieSansL;


std::mutex * lockloading;
SceUID dlThreadId = -1;
SceUID pngloadThreadId = -1;
std::vector<bool> appIconsDownloading;
std::vector<bool> appIconsDownloaded;


std::vector<AppEntry> appEntries;

float y_offset = 0;

const int entries_y_start = 130;
const int entries_y_diff= 125;
const int MAX_ICONS_LOADED = 30;
const int y_extra_load = entries_y_diff * MAX_ICONS_LOADED / 2;
const int y_extra_vis_top = 0 - entries_y_diff;
const int y_extra_vis_bot = DISPLAY_HEIGHT;

vita2d_texture * icons[MAX_ICONS_LOADED];
int icons_indexes[MAX_ICONS_LOADED];


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


SceUID fd;
void openLog(){
	fd = sceIoOpen("ux0:data/cbps/log.txt", SCE_O_WRONLY|SCE_O_APPEND|SCE_O_CREAT, 0777);
}
void writeLog(std::string data){
	const char * nl = "\r\n";
	const char * datac = data.c_str();
	sceIoWrite(fd, datac, strlen(datac));
	sceIoWrite(fd, nl, strlen(nl));
}
void closeLog(){
	sceIoClose(fd);
}

bool DownloadAppIconLocking(std::string url, std::string path, int vecIndex){
	VitaNet::http_response resp = vitaNet.curlDownloadFile(url, "", path);
	bool downloaded = false;
	if(resp.httpcode == 200){
		downloaded = true;
	}else{
		downloaded = false;
	}
	return downloaded;
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

std::mutex loadpnglock;
int loadPngThread(SceSize args, void * argp){
	while(1){
		
		sceKernelDelayThread(100000);
	}
}

int downloadThread(SceSize args, void * argp){
	for(int i = 0; i < appEntries.size(); i++){
		App * a = GetApp(i);
		std::string iconPath = createIconPath(a->id);
		const char * iconPathC = iconPath.c_str();
		SceUID fd = -1;
		if(fd = sceIoOpen(iconPathC, SCE_O_RDONLY, 0777) < 0){

			lockloading[i].lock();
			appIconsDownloading[i] = true;
			appIconsDownloaded[i] = false;
			lockloading[i].unlock();

			bool downloaded = DownloadAppIconLocking(a->thumbnailUrl, iconPath, i);
			
			lockloading[i].lock();
			appIconsDownloaded[i] = downloaded;
			lockloading[i].unlock();

		}else{
			sceIoClose(fd);

			lockloading[i].lock();
			appIconsDownloading[i] = false;
			appIconsDownloaded[i] = true;
			lockloading[i].unlock();

			continue;
		}
	}
	sceKernelDelayThread(1000);
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
	appIconsDownloaded.clear();
	appIconsDownloading.clear();

	lockloading = new std::mutex[1000];

	for(int i = 0; i < MAX_ICONS_LOADED; i++){
		icons_indexes[i] = i;
	}

	for(int i = 0; i < GetAppsAmount(); i++){
		App * app = GetApp(i);
		if(app == NULL){
			continue;
		}
		
		AppEntry appEntry;
		appEntry.index = i;
		appEntry.id = app->id;
		
		appEntries.push_back(appEntry);

		appIconsDownloading.push_back(false);
		appIconsDownloaded.push_back(false);
	}

	dlThreadId = sceKernelCreateThread("dlthread", downloadThread, 0x10000100, 0x10000, 0, 0, NULL);
	sceKernelStartThread(dlThreadId, 0, NULL);

	pngloadThreadId = sceKernelCreateThread("pngloadthread", loadPngThread, 0x10000100, 0x10000, 0, 0, NULL);
	sceKernelStartThread(pngloadThreadId, 0, NULL);
	
}

int lastTop = 0;
int currentTop = 0;
int lastBottom = MAX_ICONS_LOADED - 1;
int currentBottom = MAX_ICONS_LOADED - 1;

void do_checks_after_draw(int move){
	y_offset -= move;

	if(y_offset > 0){
		y_offset = 0;
	}

	bool shouldBeLoaded = false;
	bool foundCurrentTop = false;
	bool foundCurrentBottom = false;

	for(int i = 0; i < appEntries.size() + 1; i++){
		float currentY = entries_y_start + y_offset + entries_y_diff * i;

		if(currentY < -y_extra_load || currentY > y_extra_load){
			shouldBeLoaded = false;
			if(foundCurrentTop && !foundCurrentBottom){
				foundCurrentBottom = true;
				currentBottom = i - 1;
				break;
			}
		}else{
			shouldBeLoaded  = true;
			if(!foundCurrentTop){
				foundCurrentTop = true;
				currentTop = i;
			}
		}
	}

	// Free necessary icons

	if(move < 0){
		int top = 0;
		for(int i = lastTop; i < currentTop; i+=1){
			int indexicon = icons_indexes[top];
			if(icons[indexicon] != NULL){
				vita2d_free_texture(icons[indexicon]);
				icons[indexicon] = NULL;
			}
			icons_indexes[top] = -1;
			top++;
		}
	}else if(move > 0){
		int bot = MAX_ICONS_LOADED - 1;
		for(int i = lastBottom; i > currentBottom; i-=1){
			int indexicon = icons_indexes[bot];
			if(icons[indexicon] != NULL){
				vita2d_free_texture(icons[indexicon]);
				icons[indexicon] = NULL;
			}
			icons_indexes[bot] = -1;
			bot--;
		}
	}


	// LOAD ICONS
	if(move < 0){
		int top = 0;
		for(int i = lastTop; i < currentTop; i+=1){
			for(int f = 0; f < MAX_ICONS_LOADED; f++){
				if(icons_indexes[f] < 0){
					icons_indexes[f] = top;
					break;
				}
			}
			int indexicon = icons_indexes[top];
			if(icons[indexicon] != NULL){
				vita2d_free_texture(icons[indexicon]);
				icons[indexicon] = NULL;
			}
			icons_indexes[top] = -1;
			top++;
		}
	}else if(move > 0){
		
	}

	

}

void draw_app_list(){
    vita2d_draw_rectangle(217, 113, 743, 408, RGBA8(0x49, 0x49, 0x49, 0xFF));
    vita2d_draw_texture(scrollbar, 915, 120);
    vita2d_draw_texture(scroll, 904, 144);

	for(int i = 0; i < appEntries.size(); i++){

		float currentY = entries_y_start + y_offset + entries_y_diff * i;

		if(currentY < -300){
			continue;
		}
		
		if(currentY > 600){
			i = appEntries.size();
			continue;
		}

    	vita2d_draw_texture(bar, 236, currentY );
		vita2d_font_draw_text(commieSansL, 460, currentY + 30 , RGBA8(255,255,255,255), 26.0f, GetApp(i)->app_name.c_str());
		vita2d_font_draw_text(commieSansL, 460, currentY + 50 , RGBA8(255,255,255,255), 18.0f, GetApp(i)->short_description.c_str());
		
		bool trylock = lockloading[i].try_lock();
		if(trylock == false){				
			vita2d_draw_texture_scale(icon, 250, currentY + 20, 0.6f, 0.6f);
		}else{
			if(appEntries[i].icon != NULL){
				vita2d_draw_texture_scale(appEntries[i].icon, 250, currentY + 20, 0.5f, 0.5f);
			}else{
				vita2d_draw_texture_scale(icon, 250, currentY + 20, 0.6f, 0.6f);
			}
			lockloading[i].unlock();
		}
	}

}

void end_app_list(){
	for(int i = 0; i < appEntries.size(); i++){
		if(icons[i] != NULL){
			vita2d_free_texture(icons[i]);
			icons[i] = NULL;
		}
	}
	vita2d_free_texture(bar);
	vita2d_free_texture(scroll);
	vita2d_free_texture(scrollbar);
	vita2d_free_texture(dl);
	vita2d_free_texture(icon);
	vita2d_free_texture(info);
	vita2d_free_font(commieSansL);
}