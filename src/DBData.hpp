#ifndef _DBData_h_
#define _DBData_h_

#include <map>
#include "occi.h"
#include "DBPool.hpp"

//using namespace sigslot;
using namespace oracle::occi;
using namespace std;

#define DBPOOL DBData::dbpool_

// 传入一个数据库连接, 用完之后自动释放
class DBData
{
public:
	DBData();
	~DBData();
	
	void set(int num){num_=num;}

protected:
	void Close();

public:
	static DBPool dbpool_;  // 数据库连接池

protected:
	Statement* stmt_;
	Connection* conn_;
	int num_;
	
};

#endif
