#ifndef _PB_EXCEPTION_HEADER
#define _PB_EXCEPTION_HEADER
#include <stdexcept>
#include <string>
using namespace std;

class PBException : public runtime_error
{
public:
	PBException(const string& errcode, const string& msg, const string& filename, int linenumber) 
			: runtime_error(msg), m_errcode(errcode), m_memo(msg), m_filename(filename), m_linenumber(linenumber)
	{
	}
	~PBException() throw() {}

	int linenumber()  { return m_linenumber; }
	string filename() { return m_filename; }
	string errcode()  { return m_errcode; }
	string errmsg()   { return m_memo; }
private:
	string           m_errcode;
	string           m_memo;
	string           m_filename;
	int              m_linenumber;
};

#endif
