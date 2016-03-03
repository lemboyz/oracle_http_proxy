#ifndef _LISTENER_HPP_
#define _LISTENER_HPP_

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <iostream>
using namespace std;

class listener
{
public:
	listener();
	~listener();

	int open(const string& ip, int port);
	int acceptConnection();

public:
	int listenFd_;
};

#endif
