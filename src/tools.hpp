#ifndef __TOOLS_HPP__
#define __TOOLS_HPP__

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <stdexcept>

#include <errno.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <regex.h>
#include <sys/syscall.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

#define cout cout <<  "("<< syscall(SYS_gettid) <<" " << tools::currentTime() <<" " <<__FILE__<<","<<__LINE__<<") "
#define endl endl;fflush(stdout)

class tools
{
public:
	static string currentTime(); //return: hh:mi:ss.microsecond
	static string now();         //return: YYYYMMDDhhmiss (14)
	static string getfield(const string& str, const int& n, const char delimiter);
	static string upper(const string& str);
	static string lower(const string& str);
	static string ProfileString(string filename, string section, string key, string default_str);
	static string trim(const string& s,char c=' ');

	static int zip  (const char *czIn, int nInLen, char *czOut);
	static int unzip(const char *czIn, int nInLen, char *czOut);
	static string zip  (const string& czIn);
	static string unzip(const string& czIn);

	static string urlUnescape(const string& value);
	static map<string,string> parseBodyKeyValue(const string& body_);
	static void mkDir(const string& dirname);

	static void daemon_init();
};

#endif
