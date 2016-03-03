#ifndef _COUTREDIRECTOR_HPP_
#define _COUTREDIRECTOR_HPP_

/*
用途:将cout的输出重定向到文件中,并且检测到日期改变时回重新打开新的文件
作者:liaoxiaoming
日期:20130228
说明:指定文件名的后面会自动增加.YYYYMMDD
*/

#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
using namespace std;

class CoutRedirector
{
public:
    CoutRedirector(const string& filename);
    ~CoutRedirector();
    void redirectByDay();
	void redirectByHour();

private:
    string getFilenameByDay() const;
	string getFilenameByHour() const;

    streambuf* coutBuf_;
    ofstream of_;
    string filename_;
    int day_;
	int hour_;

public:
    static int currentDay();
    static int currentHour();
    static void now(int& year, int& month, int& day, int& hour);
};

#endif
