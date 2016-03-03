#ifndef _DBPOOL_HPP_
#define _DBPOOL_HPP_

#include <string>
#include <occi.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "sigslot.h"

using namespace sigslot;
using namespace oracle::occi;
using namespace std;                	

/*
	�����ṩ����ORACLE��һ��OCCIͬ���ʵ����ݿ����ӳ�
*/

class DBPool : public multi_threaded_local
{
public:
	DBPool();
	~DBPool();
	
	// ����������Ϣ
	void Init(const string &addr, int minCon, int maxCon);
	// �������ݿ����ӻ��� �� ���ӳ�
	void Open();
	
	// �����ӳ��л�ȡһ�����õ�����
	Connection* getConn();
	
	// �ͷ�һ�����ӻ����ӳ�
	void ReleaseOne(Connection* conn);
	
	// �������ݿ����ӻ��� �� ���ӳ�
	void Close();

protected:
	Environment *env;
	StatelessConnectionPool *scp;
	
	string u_name_;
	string u_pass_;
	string connstr_;
	int minCon_;
	int maxCon_;
};

#endif
