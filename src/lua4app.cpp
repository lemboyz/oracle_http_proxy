#include "lua4app.hpp"

#define THROW(L,errtxt) lua_pushstring((L),(errtxt));\
                        lua_error(L);
#define LUA_REGISTER(L,funcnametxt,func,p) lua_pushlightuserdata((L),(void*)p);\
                                       lua_pushcclosure((L),func,1);\
                                          lua_setglobal((L),funcnametxt);

void CLua4app::initfunctions()
{
    lua_register(L_, "log"       , log);
    lua_register(L_, "log2"      , log2);
    lua_register(L_, "bcdzip", bcdzip);
    lua_register(L_, "bcdunzip", bcdunzip);
    lua_register(L_, "getfield", getfield);
	LUA_REGISTER(L_, "httpGetRequestBody" , httpGetRequestBody , httpReq_ );
	LUA_REGISTER(L_, "httpSetResponseBody", httpSetResponseBody, httpResp_);
	lua_register(L_, "parseUriParam"      , parseUriParam);
	lua_register(L_, "sql_select", sql_select);
	lua_register(L_, "sql_update", sql_update);
	lua_register(L_, "chunEscape", chunEscape);
	lua_register(L_, "chEscape", chEscape);	
}

int CLua4app::call(const string& func)
{
    lua_getglobal(L_,func.c_str());
    string err;
    if(lua_type(L_,-1) != LUA_TFUNCTION) {
        err = "function " + func + " not defined !!";
        return -1;
    }
    if(lua_pcall(L_,0,1,0)) {
        err = "lua_pcall() error," + string(lua_tostring(L_,-1));
        cout << err << endl;
        lua_pop(L_,1);
		if( err.find("error8145") != string::npos )
		{
			throw runtime_error("8145");
		}
        return -1;
    }
    lua_pop(L_,1);

    return 0;  // 0 ��ʾ�ɹ�
}

///////////////////////////////////////////
// ����ĺ����Ǹ�lua�ű��е��õ�

int CLua4app::log(lua_State *L)
{
	int n = lua_gettop(L);
	if(n!=1) return 0;
	try
	{
		lua_Debug ar;
		lua_getstack(L, 1, &ar);
		lua_getinfo(L, "nSl", &ar);
		string filename = ar.short_src;
		char* bname = basename((char*)filename.c_str());
		int line = ar.currentline;
		string info = getstring(L, 1);

		cout << "(" << bname << "," << line << ") " << info << endl;
	}
	catch(exception& e)
	{
		cout << "exception: " << e.what() << endl;
	}
	catch(...)
	{
		cout << "erro: " << strerror(errno) << endl;
	}
	return 0;
}


int CLua4app::log2(lua_State *L)
{
	int n = lua_gettop(L);
	if(n <= 0) return 0;
	try
	{
		int l = 2;
		if(n > 1)
		{
			int itemp = (int)lua_tointeger(L, 2);
			if(itemp > 2)
			{
				l = itemp;
			}
		}
		lua_Debug ar;
		lua_getstack(L, l, &ar);
		lua_getinfo(L, "nSl", &ar);
		string filename = ar.short_src;
		int line = ar.currentline;
		string info = getstring(L, 1);

		cout << "(" << filename << "," << line << ") " << info << endl;
	}
	catch(exception& e)
	{
		cout << "exception: " << e.what() << endl;
	}
	catch(...)
	{
		cout << "erro: " << strerror(errno) << endl;
	}
	return 0;
}

/**
@brief ��BCD��ķ�ʽ���ַ���ѹ��Ϊһ�볤��, ����"1234AF"ѹ��Ϊ0x12 0x34 0xAF
@param str ��Ҫѹ�����ַ���,�ַ�����Ӧ������0123456789ABCDEF��Χ�ڵ��ַ���, ���ҳ��ȱ���Ϊż��
@return zipedString �ѱ�ѹ�����ַ���
@note str = bcdzip("1234EF"); str: 0x12 0x34 0xEF
*/
int CLua4app::bcdzip(lua_State *L)
{
    int n = lua_gettop(L);
    if(n!=1) return 0;

    string in= getstring(L,1);
	string out;
	try
	{
		out = tools::zip(in);
	}
	catch(exception& ex)
	{
		cout<< "CLua4app::bcdzip exception[" << ex.what() <<"]" <<endl;
	}
	catch(...)
	{
		cout<< "CLua4app::bcdzip unknown exception"<<endl;
	}
    lua_pushlstring(L,out.c_str(),out.length());

    return 1;
}

// bcdunzip(in) return out
/**
@brief bcdunzip(str)��bcd��ķ�ʽ��ѹ���ַ���,��bcdzip(str)���෴�Ĳ���
@param str ��ѹ��ǰ���ַ���
@return unzipedString ����ѹ������ַ���
@note str = bcdunzip("123"); -- str: "313233"
*/
int CLua4app::bcdunzip(lua_State *L)
{
    int n = lua_gettop(L);
    if(n!=1) return 0;

    string in = getstring(L,1);
	string out;
	try
	{
		out = tools::unzip(in);
	}
	catch(exception& ex)
	{
		cout<< "CLua4app::bcdunzip exception[" << ex.what() <<"]" <<endl;
	}
	catch(...)
	{
		cout<< "CLua4app::bcdunzip unknown exception"<<endl;
	}
    lua_pushlstring(L,out.c_str(),out.length());
    return 1;
}

// getfield(str,i,div)
/**
@brief getfield(str, i, delimiter)ͨ��һ���ֽڵķָ�����ȡ�ַ����е�ĳһ�ֶ�
@param str �ܵ��ַ���
@param i �ָ����ָ�ĵ�i���ֶ�, ��1��ʼ����
@param delimiter �ָ���, ����^|,;�����ⵥ���ַ�
@return �ֶ��Ӵ�
@note s = getfield("123|456|789", 2, "|"); s���Ϊ"456"
*/
int CLua4app::getfield(lua_State *L)
{
    int n = lua_gettop(L);
    if(n!=3) return 0;

    string str(lua_tostring(L,1));
    int    i(lua_tointeger(L,2));
    string ch(lua_tostring(L,3));

	string out;
	try
	{
		out = tools::getfield(str,i,ch[0]);
	}
	catch(exception& ex)
	{
		cout<< "CLua4app::getfield exception[" << ex.what() <<"]" <<endl;
	}
	catch(...)
	{
		cout<< "CLua4app::getfield unknown exception"<<endl;
	}
    lua_pushlstring(L,out.c_str(),out.length());
    return 1;
}

#define THROW_LUA(L,errtxt) do{luaL_error(L,(errtxt)); return 0;} while(0)

/**
 * @brief sql_select(sql, v1, v2, v3, ...) ��֧�־�̬�����󶨵�sql��ѯ, ��������ֻ��Ϊstring��int,��֧��float
 * @param sql
 * @param v1, v2, v3 ��Ҫ���õľ�̬����ֵ
 * @return ResultSet
 * @note local rs = sql_select(sql, 1,'123')
 */ 
int CLua4app::sql_select(lua_State *L)
{
	int n = lua_gettop(L);
	if (n < 1)
	{
		THROW_LUA(L, "parameter error");
	}

	Statement* stmt = NULL;
	Connection* conn = NULL;
	lua_newtable(L);
	try
	{
		string sql = getstring(L,1);
		conn = DBPOOL.getConn();
		stmt = conn->createStatement(sql);
		if (n > 1)
		{
			for (int i=2; i<=n; ++i)
			{
				if(lua_type(L,i)==LUA_TSTRING){
					string str = getstring(L, i);
					cout << "setString(" << i-1 << "," << str << ")" << endl;
					stmt->setString(i-1, str);
				}else if(lua_type(L,i)==LUA_TNUMBER){
					int num = lua_tointeger(L,i);
					cout << "setInt(" << i-1 << "," << num << ")" << endl;
					stmt->setInt(i-1, num);
				}else{
					throw runtime_error("Unsupported value type");
				}
			}
		}
		
		ResultSet *rset = NULL;
		rset = stmt->executeQuery();

		if (rset != NULL)
		{
			vector<MetaData> listOfColumns = rset->getColumnListMetaData();
			int colNum                     = listOfColumns.size(); // �и���
			cout << "column size:" << colNum <<endl;
			int j=0;	// ����
			while (rset->next())
			{
				lua_newtable(L);
				for (int i=0; i<colNum; ++i)	// ÿһ�ж�����
				{
					string columnName  = listOfColumns[i].getString(MetaData::ATTR_NAME);
					string columnVal   = rset->getString(i+1);
					std::transform(columnName.begin(), columnName.end(), columnName.begin(), ::tolower);
					lua_pushlstring(L, columnName.c_str()  , columnName.length());
					lua_pushlstring(L, columnVal.c_str()   , columnVal.length());
					lua_rawset(L, -3);
				}
				lua_rawseti(L, -2, j+1);
				j++;
			}
			cout << "row size:" << j << endl;
			stmt->closeResultSet(rset);
		}
	}
	catch(exception &e)
	{
		cout << "sql execute fail:" << e.what() << endl;
	}

	try
	{
		if((conn!=NULL) && (stmt!=NULL))
		{
			conn->terminateStatement(stmt);
			stmt = NULL;
		}
		if (conn != NULL)
		{
			DBPOOL.ReleaseOne(conn);
			conn = NULL;
		}
	}
	catch(exception &e)
	{
		cout << "release oracle resource error:" << e.what() << endl;
	}

	return 1;
}

/**
 * @brief sql_update(sql, v1, v2, v3, ...) ��֧�־�̬�����󶨵�sql insert/update, ��������ֻ��Ϊstring��int,��֧��float
 * @param sql
 * @param v1, v2, v3 ��Ҫ���õľ�̬����ֵ
 * @return affectNum Ӱ������
 * @note local rs = sql_update(sql, 1,'123')
 */ 
int CLua4app::sql_update(lua_State *L)
{
	int n = lua_gettop(L);
	if(n<1){
    	THROW_LUA(L, "parameter error");
	}
    
	Statement* stmt = NULL;
	Connection* conn = NULL;
	int affect_num=0;

	try{
		string sql = getstring(L,1);

		conn = DBPOOL.getConn();
		stmt = conn->createStatement(sql);
		if (n > 1)
		{
			for (int i=2; i<=n; ++i)
			{
				if(lua_type(L,i)==LUA_TSTRING){
					string str = getstring(L, i);
					cout << "setString(" << i-1 << "," << str << ")" << endl;
					stmt->setString(i-1, str);
				}else if(lua_type(L,i)==LUA_TNUMBER){
					int num = lua_tointeger(L,i);
					cout << "setInt(" << i-1 << "," << num << ")" << endl;
					stmt->setInt(i-1, num);
				}else{
					throw runtime_error("Unsupported value type");
				}
			}
		}

		affect_num = stmt->executeUpdate();
		conn->commit();
  
		cout << "�з��������ļ�¼" << affect_num << "������" << endl;   
	}catch(exception &e){
		conn->rollback();
		cout << "SQL fail:" << e.what() << endl;    
	}
  
	try{
    	if(conn!=NULL && stmt!=NULL){
			conn->terminateStatement(stmt);
			stmt = NULL;
    	}
		if(conn!=NULL){
			DBPOOL.ReleaseOne(conn);
			conn = NULL;
    	}
	}catch(exception &e){
		cout << "[" << __FILE__<< ":" <<__LINE__<<"]" << "SQL ERROR " << e.what() << endl;    
	}
  
	lua_pushnumber(L,affect_num);
	return 1;
}

/**
@brief  chEscape(str )
        ת���ַ���,�Ը������ַ�������URL����
        
@param  str        ������� �ַ���
@return ת����ַ���
@note   str = chEscape(str )
*/
int CLua4app::chEscape(lua_State *L)
{
	int n = lua_gettop(L);
	if (n!=1) return 0;
  
  string req = getstring(L, 1);
  char *p = curl_escape((char *)req.c_str(), (int)req.size());    
  lua_pushlstring(L, p, strlen(p));  
  curl_free(p);
  return 1;

}

/**
@brief  chunEscape(str )
        ����URL�������ַ���
        
@param  str        ������� �ַ���
@return ������ַ���
@note   str = chunEscape(str )
*/
int CLua4app::chunEscape(lua_State *L)
{
	int n = lua_gettop(L);
	if (n!=1) return 0;

	string outstr;
	string req = getstring(L, 1);
	CURL *curl = curl_easy_init();
	if(NULL != curl)
	{
		int outlen = 0;
		char* p = curl_easy_unescape(curl, req.c_str(), req.length(), &outlen);
		outstr  = string(p, outlen);
		curl_easy_cleanup(curl);
	}

	lua_pushlstring(L, outstr.c_str(), outstr.length());
	return 1;
}

int CLua4app::httpGetRequestBody(lua_State* L)
{
	string strReqBody;
	HttpRequest* request = NULL;
	try
	{
		request = (HttpRequest*)lua_touserdata(L, lua_upvalueindex(1));
		if(NULL != request)
		{
			strReqBody = request->body_;
		}
	}
	catch(exception& ex)
	{
        cout<<ex.what()<<endl;
	}
	catch(...)
	{
        cout<< "unknown exception caught:" << strerror(errno) <<endl;
	}
	lua_pushlstring(L, strReqBody.c_str(), strReqBody.length());
	return 1;
}

int CLua4app::httpSetResponseBody(lua_State* L)
{
	string strRespBody = getstring(L, 1);
	try
	{
		HttpResponse* response = NULL;
		response = (HttpResponse*)lua_touserdata(L, lua_upvalueindex(1));
		if(NULL != response)
		{
			response->setBody(strRespBody);
		}
	}
	catch(exception& ex)
	{
        cout<<ex.what()<<endl;
	}
	catch(...)
	{
        cout<< "unknown exception caught:" << strerror(errno) <<endl;
	}
	return 0;
}

static void map2LuaTable(lua_State* L, const map<string, string>& aMap)
{
	lua_newtable(L);
	map<string, string>::const_iterator it;
	string key, value;
	for(it = aMap.begin(); it != aMap.end(); ++it)
	{
		key   = it->first;
		value = it->second;
		lua_pushlstring(L, key.c_str(), key.length());
		lua_pushlstring(L, value.c_str(), value.length());
		lua_rawset(L, -3);
	}
}

int CLua4app::parseUriParam(lua_State* L)
{
	string param = getstring(L, 1);
	try
	{
		map<string, string> mapParam = tools::parseBodyKeyValue(param);
		map2LuaTable(L, mapParam);
	}
	catch(exception& ex)
	{
        cout<<ex.what()<<endl;
	}
	catch(...)
	{
        cout<< "unknown exception caught:" << strerror(errno) <<endl;
	}
	return 1;
}
