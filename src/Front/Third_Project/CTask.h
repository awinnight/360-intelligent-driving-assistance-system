#pragma once
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "ShareM.h"
using namespace std;

class CTask
{
public:

	CTask();
	~CTask();
	virtual void run() = 0;
};

class CTaskChild :public CTask
{
public:
	CTaskChild(char* buf);
	~CTaskChild();
	void run();

private:
	char* buff;
	ShareM *smw;
};

