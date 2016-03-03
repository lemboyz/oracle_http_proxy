#include "DBPool.hpp"

DBPool::DBPool()
{
	env = NULL;
	scp = NULL;
	
	u_name_ = "";
	u_pass_ = "";
	connstr_ = "";
	
	minCon_ = 10;
	maxCon_ = 50;
}

DBPool::~DBPool()
{
	Close();
}

string OraChangPin(const string &encpwd)
{
	string oras1="0123456789@ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz/_~!#$%^&*()-+=,.<>:;'{}[]|?`";
	string encs2="Q5dT0LIMqDmg1bsnWtGOxweC_Hj8J/XN@larF3B7pz2KucYZ9PRUoASh4ivkfyV6E]~$%=,.^|?&*()!#-}[+<>:;'{`";
	int i, pos;
	int len = encpwd.length();

	string pwd;
	for(i=0; i<len; i++)
	{
		string c = encpwd.substr(i, 1);
		pos = encs2.find(c);
		if(pos==(int)string::npos)
			throw invalid_argument(string("DB password arguments error, 'uname/upass@db'"));

		pwd += oras1.substr(pos, 1);		
	}	

	return pwd;
}

void DBPool::Init(const string &addr, int minCon, int maxCon)
{
	size_t pos;
	string tmp;
	
	pos = addr.find("/");
	if(pos==string::npos)
		throw invalid_argument(string("DB connstr arguments error, 'uname/upass@db'"));
	u_name_ = addr.substr(0,pos);
	tmp = addr.substr(pos+1);
	
	if( tmp.empty()||(pos = tmp.find("@"))==string::npos )
		throw invalid_argument(string("DB connstr arguments error, 'uname/upass@db'"));

	u_pass_ = OraChangPin(tmp.substr(0,pos));

	tmp = tmp.substr(pos+1);
	
	if( tmp.empty() )
		throw invalid_argument(string("DB connstr arguments error, 'uname/upass@db'"));
	connstr_ = tmp;
	
	minCon_ = minCon;
	maxCon_ = maxCon;
}

void DBPool::Open()
{
	// 线程模式
	env = Environment::createEnvironment (Environment::THREADED_MUTEXED);
	if(env==NULL){
		throw runtime_error("DBPool::createEnvironment() err");
	}
	
	// 创建一个线程池
	scp = env->createStatelessConnectionPool(u_name_, u_pass_, connstr_
									,maxCon_, minCon_, 2, StatelessConnectionPool::HOMOGENEOUS );
}

Connection* DBPool::getConn()
{
	lock_block<multi_threaded_local> lock(this);
	
	Connection* conn = scp->getAnyTaggedConnection("");
	// string tag=conn->getTag();
	
	if (conn == NULL)
	{
		throw runtime_error("DBPool::getAnyTaggedConnection() err");
	}
	
	return conn;
}

void DBPool::ReleaseOne(Connection* conn)
{
	if (conn)
	{
		scp->releaseConnection(conn,"");
		conn = NULL;
	}
}

void DBPool::Close()
{
	if(env!=NULL && scp!=NULL)
	{
		env->terminateStatelessConnectionPool(scp);
		scp = NULL;
	}
  
	if(env!=NULL){
  		Environment::terminateEnvironment(env);
		env = NULL;
	}
}

