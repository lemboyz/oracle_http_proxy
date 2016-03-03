#include "httpparser.hpp"

static const string CRLF = "\r\n";

///////////////////////////////////////////////////////////////////////
// static functions:

static string trimLeft(const string& source)
{
    string str(source);
    string s = str.erase(0,str.find_first_not_of(" "));
    return s;
}

static string toLower(const string& str)
{
    int i;
    string s;

    for (i=0; i<(int)str.length(); ++i)
    {
        s += (unsigned char)tolower(str[i]);
    }
    return s;
}

static string Int2Str(int n)
{
	stringstream newstr;
	newstr << n;
	return newstr.str();
}

static string clearDuplicatedSpace(const string& s)
{
    string str(s);
    size_t pos = string::npos;
    while ((pos = str.find("  ")) != string::npos)
    {
        str.replace(pos, 2, " ");
    }
    return str;
}

static vector<string> splitByCRLF(const string& str)
{
    string s = str+CRLF;
    vector<string> vec;
    size_t pos = string::npos;
    size_t start = 0;
    while ((pos = s.find(CRLF, start)) != string::npos)
    {
        string part = s.substr(start, pos - start);
        vec.push_back(part);
        start = pos + 2;
    }
    return vec;
}

static void splitKeyValue(const string& keyvalue, string& k, string& v)
{
    k = "";
    v = "";
    size_t pos = keyvalue.find("=");
    if (pos != string::npos)
    {
        k = keyvalue.substr(0,pos);
        v = keyvalue.substr(pos+1);
    }
}

// firstLine [POST /url HTTP/1.1\r\n]
static void parseFirstLine(const string& firstLine, string& method, string& url, string& version)
{
	size_t pos;
	size_t start = 0;

	pos = firstLine.find(" ", start);
	method = firstLine.substr(start, pos - start);
	start = pos + 1;

	pos = firstLine.find(" ", start);
	url = firstLine.substr(start, pos - start);
	//start = pos + 1;

	version = firstLine.substr(pos+1);
}

static size_t parseHeaders(vector<string>& lines, map<string,string>& headers)
{
	size_t idxEndOfHeaders = lines.size() - 1;
	for (size_t i=1; i<lines.size(); ++i)
	{
		if (lines[i] == "")
		{
			idxEndOfHeaders = i;
			break;
		}
		string line = lines[i];
		size_t pos = line.find(":");
		if (pos != string::npos)
		{
			string k = toLower(line.substr(0, pos));
			string v = trimLeft(line.substr(pos+1));
			headers[k] = v;
		}
	}
	return idxEndOfHeaders;
}

// return: YYYY-MM-DD hh:mi:ss
static string currentDateTime()
{
	int year,month,day,hour,minute,second=0;

    struct timeval tv;
    gettimeofday(&tv,NULL);
    struct  tm* tm;
    tm = localtime( &(tv.tv_sec));
    year = (tm->tm_year > 80) ? (tm->tm_year + 1900) : (tm->tm_year + 2000);
    month = tm->tm_mon + 1;
    day = tm->tm_mday;

    hour   = tm->tm_hour;
    minute = tm->tm_min;
    second = tm->tm_sec;

    char currDateTime[20] = {0};
    snprintf(currDateTime,sizeof(currDateTime),"%4d-%02d-%02d %02d:%02d:%02d",
        year,month,day,hour,minute,second);
    return currDateTime;
}

/*
// ' ' -> '%20'
static string urlEscape(const string& value)
{
	char *p = curl_escape(value.c_str(), value.size());
	string str = string(p, strlen(p));
	curl_free(p);
	return str;
}
*/

// '%20' -> ' '
static string urlUnescape(const string& value)
{
	string outstr;
    CURL *curl = curl_easy_init();
    if(NULL != curl)
    {
        int outlen = 0;
        char* p = curl_easy_unescape(curl, value.c_str(), value.length(), &outlen);
        outstr  = string(p, outlen);
        curl_easy_cleanup(curl);
    }
	return outstr;
}


/////////////////////////////////////////////////////////////////////
// body: k1=v1&k2=v2&k3=v3...
map<string,string> HttpRequest::parseBodyKeyValue()
{
    string body = this->body_+"&";
    string key, value;
    size_t pos = string::npos;
    size_t start = 0;
	map<string,string> keyValues;
    while( (pos=body.find("&", start)) != string::npos)
    {
        string keyValue = body.substr(start, pos-start);
        splitKeyValue(keyValue, key, value);
		value = urlUnescape(value);
        cout << key << "=" << value << endl;
		keyValues[key] = value;
        start = pos+1;
    }
	return keyValues;
}

HttpRequest::HttpRequest() : bOK_(false)
{
}

bool HttpRequest::isOK() const
{
	return bOK_;
}

HttpRequest::PARSE_STATUS HttpRequest::parse(const string& strReq)
{
	if (strReq.substr(0,2)!="PO")
	{
		cout << "PARSE_ERR: method must be POST" << endl;
		return PARSE_ERR;
	}

	vector<string> lines = splitByCRLF(strReq);
	if (lines.size() < 3)
	{
		cout << "PARSE_ERR: Too few line size: " << lines.size() << endl;
		cout << "[" << strReq << "]" << endl;
		return PARSE_ERR;
	}
	string firstLine = clearDuplicatedSpace(lines[0]);
	parseFirstLine(firstLine, method_, url_, version_);

	size_t idxEndOfHeaders = parseHeaders(lines, headers_);
	size_t content_length = atoi(headers_["content-length"].c_str());

	try	
	{
		body_ = lines.at(idxEndOfHeaders+1);
	}
	catch(exception& e)
	{
		cout << e.what() << endl;
		cout << "warning: body is empty." << endl;
		cout << "[" << strReq << "]" << endl;
	}

	if (body_.size() == content_length)
	{
		headers_ = parseBodyKeyValue();
		cout << "[" << strReq << "]" << endl;
		cout << "PARSE_OK" << endl;
		bOK_ = true;
		return PARSE_OK;
	}
	else if (body_.size() > content_length)
	{
		cout << "PARSE_ERR: received body size:"<<body_.size() << " > Content-Length:"<<content_length<< endl;
		cout << "[" << strReq << "]" << endl;
		return PARSE_ERR;
	}
	else if (body_.size() < content_length)
	{
		cout<<"PARSE_IMCOMPLETE: body_.size:"<<body_.size()<<" < Content-Length:"<<content_length<<endl;
		cout << "[" << strReq << "]" << endl;
		return PARSE_IMCOMPLETE;
	}
	return PARSE_ERR;
}

string HttpRequest::getHeader(const string& key) const
{
	string value;
	try {
		value = headers_.at(key);
	}catch(...){}
	return value;
}

void HttpRequest::show()
{
	cout << "method: " << method_ << endl;
	cout << "url: " << url_ << endl;
	cout << "version: " << version_ << endl;

	map<string, string>::iterator it;
	for (it=headers_.begin(); it!=headers_.end(); ++it)
	{
		cout << it->first << ": " << it->second << endl;
	}

	cout << "body: " << body_ << endl;
}
//////////////////////////////////////////////////////////////

HttpResponse::HttpResponse()
{
	// 设置默认参数
	setVersion("HTTP/1.1");
	setStatus(200);
	setHeaderDate();
	setHeader("Content-Type", "text/html");
	setHeader("Server", "C++ HttpResponse");
	setHeader("Connection", "close"); // close: not support keepalive
}
HttpResponse::~HttpResponse()
{
}

string HttpResponse::getBody() const
{
	return body_;
}

string HttpResponse::toString()
{
	string resp = version_ + " " + Int2Str(status_) + " " + statusInfo_ + CRLF; 
	map<string,string>::iterator it;
    for (it=headers_.begin(); it!=headers_.end(); ++it)
	{
		if (it->first.length()>0 && it->second.length()>0)
		{
			resp += it->first + ": " + it->second + CRLF;
		}
	}
	resp += CRLF;
	resp += body_;
	return resp;
}

void HttpResponse::setVersion(const string& version)
{
	this->version_ = version;
}

void HttpResponse::setStatus(int status)
{
	this->status_ = status;
	setStatusInfo();
}

void HttpResponse::setStatusInfo()
{
    if      (status_ == 100) statusInfo_ = "Continue";
    else if (status_ == 101) statusInfo_ = "Switching Protocols";
    else if (status_ == 200) statusInfo_ = "OK";
    else if (status_ == 201) statusInfo_ = "Created";
    else if (status_ == 202) statusInfo_ = "Accepted";
    else if (status_ == 203) statusInfo_ = "Non-Authoritative Information";
    else if (status_ == 204) statusInfo_ = "No Content";
    else if (status_ == 205) statusInfo_ = "Reset Content";
    else if (status_ == 206) statusInfo_ = "Partial Content";
    else if (status_ == 300) statusInfo_ = "Multiple Choices";
    else if (status_ == 301) statusInfo_ = "Moved Permanently";
    else if (status_ == 302) statusInfo_ = "Found";
    else if (status_ == 303) statusInfo_ = "See Other";
    else if (status_ == 304) statusInfo_ = "Not Modified";
    else if (status_ == 305) statusInfo_ = "Use Proxy";
    else if (status_ == 307) statusInfo_ = "Temporary Redirect";
    else if (status_ == 400) statusInfo_ = "Bad Request";
    else if (status_ == 401) statusInfo_ = "Unauthorized";
    else if (status_ == 402) statusInfo_ = "Payment Required";
    else if (status_ == 403) statusInfo_ = "Forbidden";
    else if (status_ == 404) statusInfo_ = "Not Found";
    else if (status_ == 405) statusInfo_ = "Method Not Allowed";
    else if (status_ == 406) statusInfo_ = "Not Acceptable";
    else if (status_ == 407) statusInfo_ = "Proxy Authentication Required";
    else if (status_ == 408) statusInfo_ = "Request Time-out";
    else if (status_ == 409) statusInfo_ = "Conflict";
    else if (status_ == 410) statusInfo_ = "Gone";
    else if (status_ == 411) statusInfo_ = "Length Required";
    else if (status_ == 412) statusInfo_ = "Precondition Failed";
    else if (status_ == 413) statusInfo_ = "Request Entity Too Large";
    else if (status_ == 414) statusInfo_ = "Request-URI Too Large";
    else if (status_ == 415) statusInfo_ = "Unsupported Media Type";
    else if (status_ == 416) statusInfo_ = "Requested range not satisfiable";
    else if (status_ == 417) statusInfo_ = "Expectation Failed";
    else if (status_ == 500) statusInfo_ = "Internal Server Error";
    else if (status_ == 501) statusInfo_ = "Not Implemented";
    else if (status_ == 502) statusInfo_ = "Bad Gateway";
    else if (status_ == 503) statusInfo_ = "Service Unavailable";
    else if (status_ == 504) statusInfo_ = "Gateway Time-out";
    else if (status_ == 505) statusInfo_ = "HTTP Version not supported";
    else                     statusInfo_ = "Undefined Status";

}

void HttpResponse::setHeaderDate()
{
	setHeader("Date", currentDateTime());
}

void HttpResponse::setHeader(const string& key, const string& value)
{
	this->headers_[key] = value;
}

void HttpResponse::setBody(const string& body)
{
	size_t len = body.length();
	setHeader("Content-Length", Int2Str(len));
	body_ = body;
}

///////////////////////////////////////////////////////////////

HttpParser::HttpParser()
{
}
HttpParser::~HttpParser()
{
}

bool HttpParser::recvOnce(int sock, string& totalReceived)
{
	bool bContinue = false;
	char buf[BUFSIZ] = {0}; // BUFSIZ: 8192
	int flags = 0;
	int len = recv(sock, buf, BUFSIZ, flags);
	if (len > 0)
	{
		totalReceived.append(buf, len);
	}
	else if (len<0 && errno == EAGAIN)
	{
		bContinue = true;
	}
	else if (len == 0)
	{
		throw runtime_error("recv() return 0, client closed while recv.");
	}
	else if (len < 0)
	{
		string err = string("recv() error:") + strerror(errno);
		throw runtime_error(err);
	}
	return bContinue;
}

bool HttpParser::parseOnce(const string& totalReceived, HttpRequest& httpReq)
{
	bool bContinue = false;
	HttpRequest::PARSE_STATUS status = httpReq.parse(totalReceived);
	if (status == HttpRequest::PARSE_OK)
	{
		bContinue = false;
	}
	else if (status == HttpRequest::PARSE_IMCOMPLETE)
	{
		bContinue = true;
	}
	else
	{
		throw runtime_error("Fail to parse http msg.");
	}
	return bContinue;
}

HttpRequest HttpParser::recvRequest(int sock)
{
	HttpRequest httpRequest;
	string totalReceived;
	while ( 1 )
	{
		int readable_status = isReadable(sock, RECV_TIME_OUT);
		if (readable_status == READABLE)
		{
			bool bContinue = recvOnce(sock, totalReceived);
			if (bContinue)
				continue;

			bContinue = parseOnce(totalReceived, httpRequest);
			if (bContinue)
				continue;

			return httpRequest;
		}
		else if (readable_status == TIMEOUT)
		{
			throw runtime_error("isReadable(): time out.");
		}
		else
		{
			string err = string("error:") + strerror(errno);
			throw runtime_error(err);
		}
	}
}

int HttpParser::isReadable(int sock, int timeout)
{
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(sock, &readSet);

	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	int ret = select(sock+1, &readSet, NULL, NULL, &tv);
	if (ret < 0)
	{
		cout << "error: " << strerror(errno) << endl;
		return ERR;
	}
	else if (ret == 0)
	{
		cout << "select() time out" << endl;
		return TIMEOUT;
	}
	else
	{
		cout << "select() return: " << ret << endl;
	}

	if (FD_ISSET(sock, &readSet))
	{
		return READABLE;
	}
	return ERR;
}

bool HttpParser::isHttpProtocal(int sock)
{
	int readable_status = isReadable(sock, RECV_TIME_OUT);
	if (readable_status == READABLE)
	{
		char firstTwoBytes[3] = {0};
		int flags = MSG_PEEK;
		int len = recv(sock, firstTwoBytes, 2, flags);
		if (len == 2)
		{
			string s(firstTwoBytes,2);
			if (s=="PO" || s=="GE")
			{
				cout << "First 2 bytes:" << s << ", I guess it's http." << endl;
				return true;
			}
		}
	}

	return false;
}
