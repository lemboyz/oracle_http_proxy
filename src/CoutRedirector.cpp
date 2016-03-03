#include "CoutRedirector.hpp"

//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
CoutRedirector::CoutRedirector(const string& filename)
:filename_(filename)
{
    day_ = 0;
    coutBuf_ = cout.rdbuf();
}

CoutRedirector::~CoutRedirector()
{
    cout.rdbuf(coutBuf_);
    of_.flush();
    of_.close();
}

void CoutRedirector::redirectByDay()
{
//pthread_mutex_lock(&lock);

    int day = currentDay();
    if (day != day_)
    {
        day_ = day;
        of_.close();
        of_.open(getFilenameByDay().c_str(), ios_base::app);
        streambuf* fileBuf = of_.rdbuf();
        cout.rdbuf(fileBuf);
    }

//pthread_mutex_unlock(&lock);
}

void CoutRedirector::redirectByHour()
{
	int hour = currentHour();
	if (hour != hour_)
	{
		hour_ = hour;
		of_.close();
		of_.open(getFilenameByHour().c_str(), ios_base::app);
		streambuf* fileBuf = of_.rdbuf();
		cout.rdbuf(fileBuf);
	}
}

int CoutRedirector::currentDay()
{
	time_t t = time(NULL);
	struct tm tm;
    localtime_r( &t, &tm);
    return tm.tm_mday;
}

int CoutRedirector::currentHour()
{
	time_t t = time(NULL);
	struct tm tm;
	localtime_r(&t, &tm);
	return tm.tm_hour;
}

void CoutRedirector::now(int& year, int& month, int& day, int& hour)
{
	time_t t = time(NULL);
    struct  tm tm;
    localtime_r( &t, &tm);
    year = (tm.tm_year > 80) ? (tm.tm_year + 1900) : (tm.tm_year + 2000);
    month = tm.tm_mon + 1;
    day = tm.tm_mday;
	hour = tm.tm_hour;
}

string CoutRedirector::getFilenameByDay() const
{
    int year, month, day, hour;
    now(year, month, day, hour);
    char sDate[8+1]={0};
    snprintf(sDate,sizeof(sDate),"%04d%02d%02d",year,month,day);
    string filename = filename_ + "." + sDate;
    return filename;
}

string CoutRedirector::getFilenameByHour() const
{
	int year, month, day, hour;
	now(year, month, day, hour);

	char filename[64] = {0};
	snprintf(filename, sizeof(filename), "%s.%4d%02d%02d.%02d",filename_.c_str(),year,month,day,hour);
	return filename;
}
