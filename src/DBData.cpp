#include "DBData.hpp"

DBPool DBData::dbpool_;

DBData::DBData()
{
	stmt_=NULL;
	conn_=NULL;
	num_=0;
}
DBData::~DBData()
{
	Close();
}

void DBData::Close()
{
	try
	{
		if (conn_!=NULL && stmt_!=NULL)
		{
			conn_->terminateStatement(stmt_);
			stmt_ = NULL;
		}
		if (conn_!=NULL)
		{
			DBPOOL.ReleaseOne(conn_);
			conn_ = NULL;
		}
	}
	catch(exception &e)
	{
		throw runtime_error("DBData::terminateStatement() err");
	}
}

