#pragma once
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "CData.h"
using namespace std;

#define SUCCESS 1
#define FAILED 0
#define SHM_READ 3
#define SHM_WRITE 2

typedef struct shmhead_st
{
    int shmid;					// 共享内存ID

    unsigned int blksize;		// 块大小
    unsigned int blocks;		// 总块数
    unsigned int rd_index;		// 读索引
    unsigned int wr_index;		// 写索引

    //必须放在共享内存内部才行
    sem_t sem_mutex;			// 用来互斥用的信号量
    sem_t sem_full;				// 用来控制共享内存是否满的信号量
    sem_t sem_empty;			// 用来控制共享内存是否空的信号量
}shmhead_t;

class ShareM
{
    /*
public:
    void* shm ;                   //分配的共享内存的原始首地址
    int running ;                    //程序是否继续运行的标志  
    char buffer[BUFSIZ + 1];      
    SUS* shared;                        //指向shm   
    int shmid;                      //共享内存标识符 //创建共享内存   

    ShareM();
    ~ShareM();
    int SHM_Create(key_t shm_key);
    int SHM_Mat();
    void SHM_InterInfo();
    void SHM_Start(key_t shm_key);*/
public:
    ShareM(int key, int blksize, int blocks);
    ShareM();
    virtual ~ShareM();
    //创建和销毁
    bool ShmInit(int key, int blksize, int blocks);
    void destroy(void);
    static void ShmDestroy(int key); //静态删除共享内存方法

    // 打开和关闭
    bool ShmOpen(int key, int blksize, int blocks);
    void ShmClose(void);

    //读取和存储
    void ShmWrite(const void* buf);
    void ShmRead(void* buf);
protected:
    //进程控制信息块
    int mid;
    bool m_open;
    void* m_shmhead;		// 共享内存头部指针
    char* m_payload;			// 有效负载的起始地址
    


};



