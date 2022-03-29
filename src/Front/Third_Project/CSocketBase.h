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
	int sock_fd;				//������fd
	int server_port;			//�������˿�
	struct sockaddr_in server_addr;					//��������ַ��
	struct sockaddr_in client_addr;					//�ͻ��˵�ַ��

	CSocketBase();
	~CSocketBase();
	int Socket_Create();
	int Addr_Reuse();
	int Socket_Bind();
	int Socket_Listen();
	void Socket_Start();

};

