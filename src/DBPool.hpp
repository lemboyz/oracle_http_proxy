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
	该类提供连接ORACLE的一个OCCI同性质的数据库连接池
*/

class DBPool : public multi_threaded_local
{
public:
	DBPool();
	~DBPool();
	
	// 读入配置信息
	void Init(const string &addr, int minCon, int maxCon);
	// 创建数据库连接环境 及 连接池
	void Open();
	
	// 从连接池中获取一条可用的连接
	Connection* getConn();
	
	// 释放一条连接回连接池
	void ReleaseOne(Connection* conn);
	
	// 销毁数据库连接环境 及 连接池
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
