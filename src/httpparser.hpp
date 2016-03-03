#ifndef _HTTPPARSER_HPP_
#define _HTTPPARSER_HPP_

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdexcept>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <curl/curl.h>
#include "tools.hpp"
using namespace std;

/*
METHOD URL HTTP_VERSION\r\n
HEADER_KEY1: VALUE1\r\n
HEADER_KEY2: VALUE2\r\n
HEADER_KEY3: VALUE3\r\n
\r\n
BODY
*/
class HttpRequest
{
public:
	string method_;
	string url_;
	string version_;

	map<string, string> headers_;

	string body_;

	map<string, string> parseBodyKeyValue();
	typedef enum {
		PARSE_OK         = 0,  // 解析成功
		PARSE_ERR        = 1,  // 解析失败
		PARSE_IMCOMPLETE = 2   // 报文不全
	} PARSE_STATUS;
	HttpRequest();
	PARSE_STATUS parse(const string& strReq);
	string getHeader(const string& key) const;
	bool isOK() const;
protected:
	bool   bOK_;

	void show();
};

/*
HTTP_VERSION STATUS INFO
HEADER_KEY1: VALUE1\r\n
HEADER_KEY2: VALUE2\r\n
HEADER_KEY3: VALUE3\r\n
\r\n
BODY
*/
class HttpResponse
{
public:
	HttpResponse();
	~HttpResponse();
	string toString();

	void setHeaderDate();
	void setVersion(const string& version); 
	void setStatus(int status);
	void setHeader(const string& name, const string& value);
	void setBody(const string& body);
	string getBody() const;


private:
	void setStatusInfo();

	string version_;
	int status_;
	string statusInfo_;

	map<string, string> headers_;

	string body_;
};

class HttpParser
{
public:
	HttpParser();
	~HttpParser();

	typedef enum {
		READABLE = 0,  
		ERR      = 1,
		TIMEOUT  = 2
	} READABLE_STATUS;

	HttpRequest recvRequest(int sock);
	static bool recvOnce(int sock, string& totalReceived);
	static bool parseOnce(const string& totalReceived, HttpRequest& req);
	static int isReadable(int sock, int timeout);
	static bool isHttpProtocal(int sock);

private:
	static const int RECV_TIME_OUT = 5;
};

#endif
