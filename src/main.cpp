#include <iostream>
#include <fstream>
#include <stdexcept>

#include <unistd.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>

#include "tools.hpp"
#include "lua4app.hpp"
#include "httpparser.hpp"
#include "pbexception.hpp"
#include "DBPool.hpp"   
#include "DBData.hpp"
#include "CoutRedirector.hpp"
#include "listener.hpp"

#define THROW_EXCEPTION(errcode, errmsg)\
do{\
    throw PBException((errcode), (errmsg), __FILE__, __LINE__);\
}while(0)

using namespace std;

void *thread(void *arg);
void initDB(string inifile);
int transTryHttp(int fd, const string& inifile);
string getScriptName(const string& inifile, const string& url);
void executeLuaFile(const string& luafile, HttpRequest& req, HttpResponse& resp);
int transaction(const string& inifile, HttpRequest& httpRequest, HttpResponse& httpResponse);
int sendn(int fd, const string& str);

string inifile;
string g_orclconnstr = "";
int g_orclminconncnt = 0;
int g_orclmaxconncnt = 0;  

int main(int argc,char** argv)
{
	if (argc != 3) 	
	{
    	cout << "Usage: " << argv[0] << " ip:port inifile" << endl;
		exit(-1);
	}
    
	inifile = argv[2];
    if( inifile.empty() ) exit(-1);

    tools::daemon_init();
    string ip = tools::getfield(argv[1], 1, ':');
	int port = atoi(tools::getfield(argv[1], 2, ':').c_str());
    cout << "ip: " << ip<< " port:" << port << endl;
	listener srvlistener;
	srvlistener.open(ip, port);
    
	initDB(inifile);

    tools::mkDir("./log/");
	CoutRedirector redir("./log/detail");

    int i = 0;
    while(1)
    {
    	try
    	{
			redir.redirectByHour();
        	int fd = srvlistener.acceptConnection();
			int *sock = new int;
			*sock = fd;
	        cout << "get a connection:conn[" << i << "] " << fd << endl;
        	pthread_t tid;
			int err = pthread_create(&tid, NULL, thread, (void*)sock);
			if (err != 0) 
			{
				cout << "can't create thread: [" << strerror(err) << "]" << endl;
				sleep(1);
			}
    	}
		catch(exception& ex)
		{
			cout << "exception [" << ex.what() <<"]" <<endl;
		}
		catch(...)
		{
			cout << "error [" << strerror(errno) <<"]"<< endl;
		}
    }
}

void *thread(void *arg)
{
	pthread_detach(pthread_self());

	int fd = *((int*)arg);
	delete (int*)arg;

	try
	{
		transTryHttp(fd, inifile);
	}
	catch(exception& ex)
	{
		cout << "exception[" << ex.what() <<"]" <<endl;
	}
	catch(...)
	{
		cout << "error[" << strerror(errno) <<"]"<< endl;
	}

//	char * str = "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Length: 29\r\nContent-Type: text/html\r\nDate: 2016-02-17 16:00:18\r\nServer: C++ HttpResponse\r\n\r\n{tftxcode:'2016012110309945'}\r\n";
//	sendn(fd, str);

	close(fd);
	pthread_exit(NULL);
	return NULL;
}

void initDB(string inifile)
{
    g_orclconnstr = tools::ProfileString(inifile,"DATABASE","ORCL_CONNSTR","");
    if (g_orclconnstr.size() > 0) 
	{
		g_orclminconncnt = atoi(tools::ProfileString(inifile,"DATABASE","ORCL_MINCONNCNT","1").c_str());
		g_orclmaxconncnt = atoi(tools::ProfileString(inifile,"DATABASE","ORCL_MAXCONNCNT","5").c_str());

		try 
		{
        	DBPOOL.Init(g_orclconnstr, g_orclminconncnt, g_orclmaxconncnt);
        	DBPOOL.Open();
		}catch(exception &e){
			cout << "opening DB error, "<< e.what() <<endl;
        	exit(-1);
		}  
    }
}


int transTryHttp(int fd, const string& inifile)
{
	int sock = fd;
	HttpParser parser;
	HttpRequest httpRequest;
	HttpResponse httpResp;
	try
	{
		httpRequest = parser.recvRequest(sock);
		transaction(inifile, httpRequest, httpResp);
	}
	catch(exception& ex)
	{
		cout << "exception [" <<ex.what() <<"]" <<endl;
	}
	catch(...)
	{
		cout << "unknown exception [" << strerror(errno) << "]" << endl;
	}

	string strResp = httpResp.toString();
	sendn(sock, strResp);
	return 0;
}

string getScriptName(const string& inifile, const string& url)
{
    cout << "url is:[" << url <<"]" << endl;
	string scriptname = tools::ProfileString(inifile, "script_by_url", url, "");
    scriptname = "./script/" + scriptname;
    cout << "luafile:" << scriptname << endl;
	return scriptname;
}

void executeLuaFile(const string& luafile, HttpRequest& req, HttpResponse& resp)
{
    CLua4app lua(luafile, &req, &resp);
	cout << "before lua.call(MAIN)" << endl;
    int rc = lua.call("MAIN");
	cout << "after  lua.call(MAIN)" << endl;
    if(rc != 0)
    {
        cout << "error: " << strerror(errno) << endl;
		string msg = string("lua.call error ") + strerror(errno);
        THROW_EXCEPTION("8999", msg);
    }
}

int transaction(const string& inifile, HttpRequest& httpRequest, HttpResponse& httpResponse)
{
	string scriptname = getScriptName(inifile, httpRequest.url_);

	executeLuaFile(scriptname, httpRequest, httpResponse);

    return 0;
}

int sendn(int fd, const string& str)
{
    size_t left = str.length();
    string buff;
    size_t pos = 0;
    while (left > 0)
    {
        buff = str.substr(pos);
        int nRet = write(fd, buff.c_str(), buff.length());

        if (nRet > 0)
        {
            left -= nRet;
            pos += nRet;
        }
        else if (nRet < 0)
        {
            cout << "send(), write() error: " << strerror(errno) << endl;
            return nRet; 
        }
    }

    return 0; 
}
