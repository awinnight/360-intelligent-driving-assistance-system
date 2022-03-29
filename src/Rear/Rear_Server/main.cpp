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
#include "ShareM.h"
#include "ThreadPool.h"
#define LOGIN_IN 1
#define LOGIN_OUT 2
map<string, map<int, PICTURE_PAGE>> BigMap;
map<int, VideoTable*> Vlmap;
map<int, PictureTable*> Plmap;

char buff[2048];

int main()
{
    ThreadPool* tp = new ThreadPool(100, 5, 60);
	ShareM shm(1234, sizeof(COM_PAGE), 10);

	while (1)
	{
        cout<<"start!" << endl;
		memset(buff, 0, sizeof(buff));
		shm.ShmRead(buff);
        cout << strlen(buff) << "----" <<shm.getMid()<< endl;

        CTaskChild* tc = new CTaskChild(buff);
        CTask* task = tc;
        tp->add_task(task);

		
	}
	return 0;
}