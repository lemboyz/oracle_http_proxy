#include "listener.hpp"

listener::listener()
:listenFd_(-1)
{
}

listener::~listener()
{
	if (listenFd_ > 0)
	{
		close(listenFd_);
	}
}

int listener::open(const string& ip, int port)
{
    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd_ < 0)
    {
        cout << "socket() error: " <<  strerror(errno) << endl;
		return -1;
    }

    int optval = 1;
    if (setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) <0)
    {
        cout << "open(), setsockopt() error: " << strerror(errno) << endl;
		return -1;
    }
    struct sockaddr_in sockAddr;
    memset(&sockAddr, '\0', sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    sockAddr.sin_port = htons(port);

    if (bind(listenFd_, (struct sockaddr*)&sockAddr, sizeof(sockAddr)) < 0)
    {
		cout << "bind() error, " << strerror(errno) << endl;
		return -1;
    }

    if (listen(listenFd_, 100) < 0)
    {
        cout << "listen() error: " << strerror(errno) << endl;
		return -1;
    }

	return 0;
}

int listener::acceptConnection()
{
    int fd = accept(listenFd_, 0, 0);

    if (fd < 0)
    {
        cout << "accept() error: " << strerror(errno) << endl;
		return -1;
    }

    return fd;
}
