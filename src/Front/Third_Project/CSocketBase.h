#pragma once
#include <iostream>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>      
#include <sys/socket.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sqlite3.h>
#include "CData.h"

using namespace std;

#define SUCCESS 1
#define FAILED 0
#define LISTEN_NUM     10

class CSocketBase
{
private:

public:
	int sock_fd;				//服务器fd
	int server_port;			//服务器端口
	struct sockaddr_in server_addr;					//服务器地址族
	struct sockaddr_in client_addr;					//客户端地址族

	CSocketBase();
	~CSocketBase();
	int Socket_Create();
	int Addr_Reuse();
	int Socket_Bind();
	int Socket_Listen();
	void Socket_Start();

};

