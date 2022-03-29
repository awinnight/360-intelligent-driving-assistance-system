#pragma once
#include <iostream>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <map>
#include <string>
#include "ShareM.h"
#include "UserModel.h"
#include "VideoModel.h"
#include "PictureModel.h"
using namespace std;
//map<int, PICTURE_PAGE> SmallMap;

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
	int lock;
	int w_size;
	int x;
	UserModel* usermodel;
	VideoModel* vimodel;
	PictureModel* picmodel;
	char* buff;
	ShareM* smw;
};

