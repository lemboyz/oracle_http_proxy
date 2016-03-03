#include "tools.hpp"
using namespace std;

string tools::getfield(const string& str, const int& n, const char delimiter)
{
	int cur_pos,pre_pos,i;
	pre_pos = -1;
	if ( n<=0 ) return "";

	string line = str + delimiter;
	i = 0;
	do {
		i++;
		cur_pos = line.find(delimiter, pre_pos + 1);
		if (i == n)
		{
			return line.substr(pre_pos+1, cur_pos - pre_pos - 1);
		}
		pre_pos = cur_pos;
	} while ( cur_pos >= 0 );

	return "";
}

string tools::upper(const string& str)
{
	string rtn;

	size_t len = str.length();
	for (size_t i=0; i<len; ++i)
	{
		rtn += (unsigned char)toupper(str[i]);
	}
	return rtn;
}


string tools::lower(const string& str)
{
	string rtn;

	size_t len = str.length();
	for (size_t i=0; i<len; ++i)
	{
		rtn += (unsigned char)tolower(str[i]);
	}
	return rtn;
}

//return: YYYYMMDDhhmiss (14)
string tools::now()
{
	time_t t = time(NULL);
    struct  tm tm;
    int year=0,month=0,day=0,hour=0,minute=0,second=0;
    localtime_r( &t, &tm );
    year   = (tm.tm_year > 80) ? (tm.tm_year + 1900) : (tm.tm_year + 2000);
    month  = tm.tm_mon + 1;
    day    = tm.tm_mday;
    hour   = tm.tm_hour;
    minute = tm.tm_min;
    second = tm.tm_sec;

    char szDateTime[14+1]={0};
    snprintf(szDateTime, sizeof(szDateTime), "%04d%02d%02d%02d%02d%02d",year,month,day,hour,minute,second);
    return string(szDateTime);
}

int getUsec()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_usec;
}

// return: hh:mi:ss.microsecond
string tools::currentTime()
{
	int hour,minute,second,microsecond = 0;

	time_t t = time(NULL);
    struct tm tm;
    localtime_r(&t, &tm);
    hour   = tm.tm_hour;
    minute = tm.tm_min;
    second = tm.tm_sec;
    microsecond = getUsec(); // 百万分之一的秒 
	
	char currTime[16] = {0};
	snprintf(currTime, sizeof(currTime), "%02d:%02d:%02d.%06d", hour,minute,second,microsecond);
	return currTime;
}

string Rtrim(const string& instring,char c)
{
	string str(instring);
	string C = string(1,c);
	string r=str.erase(str.find_last_not_of(C)+1);
	return r;
}

string Ltrim(const string& instring,char c)
{
	string str(instring);
	string C = string(1,c);
	string r=str.erase(0,str.find_first_not_of(C));
	return r;
}

string tools::trim(const string& instring,char c)
{
	return Rtrim(Ltrim(instring,c),c);
}

string tools::ProfileString(string filename,string section, string key,string default_str)
{
	ifstream fin(filename.c_str());
	string line, left, right, rtn;
	size_t pos = string::npos;
	int flag = 0; 
	string tmp = "["+section+"]";
	try{
		while(getline(fin, line))
		{
			if(line.length()>0 && line[line.length()-1]=='\n')
			{
				line.erase(line.length()-1,1);
			}

			pos = line.find("#",0);
			if(pos != string::npos) { 
				line.erase(pos);
			}

			if(trim(line) == tmp) 
			{
				flag=1;
				continue;
			}

			if((flag==1) && trim(line)!=tmp && 
				line.find("[",0)!=string::npos && 
				line.find("]",0)!=string::npos) 
			{
				flag=0;
				break;
			}

			if(flag == 1)
			{
				if( (pos = line.find("=",0)) != string::npos ) 
				{
					left = line.substr(0,pos);
					right= line.substr(pos+1);
					if(trim(key) == trim(left))
					{
						rtn = right;
						break;
					}
				}
			}
		}
	}catch(...){
		rtn = "";
	}
	cout << key << "=" << rtn << endl;
	if(rtn.length()>0) return trim(rtn);
	return default_str;
}

int tools::zip(const char *czIn, int nInLen, char *czOut)
{
	int i, j, n;
	unsigned char c, c1;

	n = nInLen / 2 * 2;

	for(i = 0, j = 0; i < n; j++) {
		c  = czIn[i++];
		c1 = czIn[i++];
		c -= (c <= '9' && c >= '0') ?  '0' :
			((c <= 'Z' && c >= 'A') ? ('A' - 10): ('a' - 10));
		c1 -= (c1 <= '9' && c1 >= '0') ?  '0' :
			((c1 <= 'Z' && c1 >= 'A') ? ('A' - 10) : ('a' - 10));
		czOut[j] = (c << 4) | c1;
	}
	return n;
}

int tools::unzip(const char *czIn, int nInLen, char *czOut)
{
	int i, j, n;
	unsigned char c;

	//n = nInLen / 2 * 2;
	n = nInLen ;

	for(i = 0, j = 0; i < n; i++) {
		c = (unsigned char)((czIn[i] >> 4) & 0x0f);
		czOut[j++] = (char)((c > 9) ? ('A' + c - 10) : ('0' + c));
		c = (unsigned char)(czIn[i] & 0x0f);
		czOut[j++] = (char)((c > 9) ? ('A' + c - 10) : ('0' + c));
	}
	return j;
}

string tools::zip(const string& in)
{
	char * out = new char[in.length()+1];
	bzero(out,sizeof(out));
	zip(in.c_str(), in.length(), out);
	string rtn(out,in.length()/2);
	delete [] out;
	return rtn;
}

string tools::unzip(const string& in)
{
	char * out = new char[in.length()*2];
	bzero(out,in.length()*2);
	unzip(in.c_str(),in.length(),out);
	string rtn(out,in.length()*2);
	delete [] out;
	return rtn;
}

string XOR(string s1, string s2)
{
	string out = string(s1.length(),'\0');
	for(int i=0; i<(int)s1.length(); i++) {
		out[i] = ((unsigned char)s1[i]) ^ ((unsigned char)s2[i]);
	}
	return out;
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

string tools::urlUnescape(const string& value)
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

map<string,string> tools::parseBodyKeyValue(const string& body_)
{
    string key, value;
    map<string,string> keyValues;
	std::istringstream input;
	input.str(body_);
	string keyvalue;
	while(std::getline(input, keyvalue, '&'))
	{
        splitKeyValue(keyvalue, key, value);
        value = tools::urlUnescape(value);

		if(!key.empty())   keyValues[key] = value;
		
	}
    return keyValues;
}

void tools::mkDir(const string& dirname)
{
    DIR * d = opendir(dirname.c_str());
    if(d == NULL)
    {   
        mkdir(dirname.c_str(),0700);
    }   
    else 
    {   
        closedir(d);
    }   
}

void tools::daemon_init()
{
    pid_t pid;

    while((pid = fork()) < 0);
    if(pid != 0) exit(0); // Parent exit
    while((pid = fork()) < 0);
    if(pid != 0) exit(0); // Parent exit

    for(int i = 0; i < 64; i++)
        signal(i, SIG_IGN); // Ignare the siganl
    setpgrp();
}

