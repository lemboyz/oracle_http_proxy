#ifndef _LUA4APP_HPP_
#define _LUA4APP_HPP_

#include <iostream>
#include <algorithm>
#include <vector>
#include <fstream>
#include <stdexcept>

#include <errno.h>
#include <regex.h>
#include <sys/types.h>
#include <libgen.h>
#include <iconv.h>
extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "DBPool.hpp"   
#include "DBData.hpp"
#include "httpparser.hpp"
#include "tools.hpp"

using namespace std;

class CLua4app
{
private:
	lua_State* L_;
	string luafile_;
	HttpRequest* httpReq_;
	HttpResponse* httpResp_;

	void initfunctions();

public:
	CLua4app(const string& luafile, HttpRequest* req, HttpResponse* resp)
	{
		httpReq_ = req;
		httpResp_ = resp;
		luafile_ = luafile;
		L_ = luaL_newstate();
		luaL_openlibs(L_);
		initfunctions();
		if (luaL_dofile(L_, luafile_.c_str()))
		{
			string err = "luaL_dofile(L,"+luafile_+") error: " + lua_tostring(L_,-1);
			lua_pop(L_, 1);
			lua_close(L_);
			throw runtime_error(err);
		}
	}
	~CLua4app()
	{
		cout << "~CLua4app()" << endl;
		lua_close(L_);
	}
	int call(const string& func);
	static string getstring(lua_State *L, int i)
	{
		size_t len = 0;
		const char * czStr = luaL_checklstring(L,i,&len);
		string out = string(czStr,len);
		return out;
	}

	// 下面的函数是给lua脚本中调用的
	static int log(lua_State *L);  
	static int log2(lua_State *L);
	static int bcdzip(lua_State *L);
	static int bcdunzip(lua_State *L);
	static int profilestring(lua_State *L);
	static int getfield(lua_State *L);

	static int sql_select(lua_State *L);
	static int sql_update(lua_State *L);
	
	static int chEscape(lua_State *L);
	static int chunEscape(lua_State *L);				
	
	static int httpGetRequestBody(lua_State* L);
	static int httpSetResponseBody(lua_State* L);
	static int parseUriParam(lua_State* L);
};

#endif
