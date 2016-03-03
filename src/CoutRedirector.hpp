#ifndef _COUTREDIRECTOR_HPP_
#define _COUTREDIRECTOR_HPP_

/*
��;:��cout������ض����ļ���,���Ҽ�⵽���ڸı�ʱ�����´��µ��ļ�
����:liaoxiaoming
����:20130228
˵��:ָ���ļ����ĺ�����Զ�����.YYYYMMDD
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
