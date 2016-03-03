#ifndef _DBData_h_
#define _DBData_h_

#include <map>
#include "occi.h"
#include "DBPool.hpp"

//using namespace sigslot;
using namespace oracle::occi;
using namespace std;

#define DBPOOL DBData::dbpool_

// ����һ�����ݿ�����, ����֮���Զ��ͷ�
class DBData
{
public:
	DBData();
	~DBData();
	
	void set(int num){num_=num;}

protected:
	void Close();

public:
	static DBPool dbpool_;  // ���ݿ����ӳ�

protected:
	Statement* stmt_;
	Connection* conn_;
	int num_;
	
};

#endif
