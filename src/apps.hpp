#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>
#include <psp2/io/dirent.h>
#include <psp2/types.h>
#include <vector>
#include <string>

class App {
	public:
		std::string developer = "";
		std::string app_name = "";
		std::string title_id = "";
		std::string short_description = "";
		std::string long_description = "";
		std::string link = "";
        bool has_extra_data = false;
		std::string data_link = "";
		std::string thumbnailUrl = "";
		std::string size = "";
		std::string id = "";
		
		App();
		~App();
};

int ex(const char *fname);
int loadAppsJson();
int LoadAppsSmart();
std::string GetAppName(int i);
App * GetApp(int i);
int GetAppsAmount();