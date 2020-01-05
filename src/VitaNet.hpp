#ifndef VITANET_HPP
#define VITANET_HPP

#include <psp2/sysmodule.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>

#include <curl/curl.h>
#include <string>
#include <string.h>

#define BASE_URL "http://217.61.5.187/"

class VitaNet{
	
	
	public:
		typedef struct{
			long httpcode;
			std::string header;
			std::string body;
		} http_response;
		
		typedef struct{
			std::string url;
			std::string postdata;
			std::string authtoken;
		} thread_request;
		
		VitaNet();
		~VitaNet();
		http_response curlGet(std::string url , std::string authtoken);
		http_response curlDelete(std::string url , std::string authtoken);
		http_response curlPost(std::string url , std::string postData , std::string authtoken);
		http_response curlPatch(std::string url , std::string patchData , std::string authtoken);
		
		http_response curlDownloadFile(std::string url , std::string authtoken , std::string file);


 
	
	private:
		void init(); 
		void terminate();
		
		
};



#endif




