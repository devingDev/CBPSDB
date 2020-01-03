#include "json.hpp"
#include "apps.hpp"
#include "main.hpp"


App::App(){}
App::~App(){}

nlohmann::json appsJSON;
std::vector<App> allApps;
int appsAmount = 0;

int ex(const char *fname) {
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

int loadAppsJson(){
	int fhAppsJson;
    if (ex("ux0:data/cbps/apps.json")) {
        fhAppsJson = sceIoOpen("ux0:data/cbps/apps.json" , SCE_O_RDONLY , 0777);
    } else if (ex("ux0:data/cbps/db.json")) {
        fhAppsJson = sceIoOpen("ux0:data/cbps/db.json" , SCE_O_RDONLY , 0777); 
    } else {
        fhAppsJson = sceIoOpen("app0:assets/default_db.json" , SCE_O_RDONLY , 0777); 
    }

	if(fhAppsJson < 0){
		return -1;
	}

	int fileSize = sceIoLseek ( fhAppsJson, 0, SCE_SEEK_END );
	sceIoLseek ( fhAppsJson, 0, SCE_SEEK_SET ); 
	std::string appsJSONString(fileSize , '\0');
	sceIoRead(fhAppsJson , &appsJSONString[0] , fileSize );
	sceIoClose(fhAppsJson);
	
	appsJSON = nlohmann::json::parse(appsJSONString);
	
	return 0;
}

int LoadAppsSmart(){
    allApps.clear();
    if(appsJSON.is_null()){
        return -1;
    }
    if(appsJSON["apps"].is_null()){
        return -1;
    }
    appsAmount = appsJSON["apps"].size();

    for(int i = 0; i < appsAmount; i++){
        App app;

        app.id = std::to_string(i);
        app.thumbnailUrl = std::string(BASE_URL);
        app.thumbnailUrl.append("homebrews/");
        app.thumbnailUrl.append(std::to_string(i));
        app.thumbnailUrl.append("/icon.png");


        if(!appsJSON["apps"][i]["name"].is_null()){
            app.app_name = appsJSON["apps"][i]["name"].get<std::string>();
        }else{
            app.app_name = "No name available.";
        }
        if(!appsJSON["apps"][i]["description"].is_null()){
            app.short_description = appsJSON["apps"][i]["description"].get<std::string>();
        }else{
            app.short_description = "No short description available.";
        }

        if(!appsJSON["apps"][i]["id"].is_null()){
            app.legacy_id = appsJSON["apps"][i]["id"].get<std::string>();
        }

        allApps.push_back(app);
    }
    return 0;
}

App * GetApp(int i){
    if(allApps.empty()){
        return NULL;
    }
    else if(i > allApps.size()){
        return NULL;
    }
    return &allApps[i];
}

std::string GetAppName(int i){
    std::string appName = "App Name : ";
    if(allApps.empty()){
        appName.append("All apps vector is empty!");
    }
    else if(allApps.size() <= 0){
        appName.append("All apps vector has 0 or no entries!");
    }
    else if(i > allApps.size()){
        appName.append("Requested index is out of bounds (in all apps vector)!");
    }
    else{
        appName.append(allApps[i].app_name);
    }
    return appName;
}

int GetAppsAmount(){
    return appsAmount;
}
