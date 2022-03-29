#include "ShareM.h"


ShareM::ShareM(int key, int blksize, int blocks)
{
	this->ShmOpen(key, blksize, blocks);
}

ShareM::ShareM()
{
	m_shmhead = NULL;
	m_payload = NULL;
	m_open = false;
}

ShareM::~ShareM()
{
	this->ShmClose();
}

//返回头地址
bool ShareM::ShmInit(int key, int blksize, int blocks)
{
	int shmid = 0;

	//1. 查看是否已经存在共享内存，如果有则删除旧的
	shmid = shmget((key_t)key, 0, 0);
	if (shmid != -1)
	{
		shmctl(shmid, IPC_RMID, NULL); 	//	删除已经存在的共享内存
	}

	//2. 创建共享内存
	shmid = shmget((key_t)key, sizeof(shmhead_t) + blksize * blocks, 0666 | IPC_CREAT | IPC_EXCL);
	if (shmid == -1)
	{
		perror("shmget");
		exit(-1);
	}
	printf("Create shmid=%d size=%ld \n", shmid, sizeof(shmhead_t) + blksize * blocks);

	//3.连接共享内存
	m_shmhead = shmat(shmid, (void*)0, 0);					//连接共享内存
	if (m_shmhead == (void*)-1)
	{
		perror("shmat");
		exit(-1);
	}
	memset(m_shmhead, 0, sizeof(shmhead_t) + blksize * blocks);		//初始化

	//4. 初始化共享内存信息
	shmhead_t* pHead = (shmhead_t*)(m_shmhead);
	pHead->shmid = shmid;				//共享内存shmid
	pHead->blksize = blksize;			//共享信息写入
	pHead->blocks = blocks;				//写入每块大小
	pHead->rd_index = 0;					//一开始位置都是第一块
	pHead->wr_index = 0;					//
	sem_init(&pHead->sem_mutex, 1, 1);	// 第一个1表示可以跨进程共享，第二个1表示初始值
	sem_init(&pHead->sem_empty, 1, 0);	// 第一个1表示可以跨进程共享，第二个0表示初始值
	sem_init(&pHead->sem_full, 1, blocks);// 第一个1表示可以跨进程共享，第二个blocks表示初始值

	//5. 填充控制共享内存的信息
	m_payload = (char*)(pHead + 1);	//实际负载起始位置
	m_open = true;

	return true;
}

void ShareM::destroy()
{
	shmhead_t* pHead = (shmhead_t*)m_shmhead;
	int shmid = pHead->shmid;

	//删除信号量
	sem_destroy(&pHead->sem_full);
	sem_destroy(&pHead->sem_empty);
	sem_destroy(&pHead->sem_mutex);
	shmdt(m_shmhead); //共享内存脱离

	//销毁共享内存
	if (shmctl(shmid, IPC_RMID, 0) == -1)		//删除共享内存
	{
		printf("Delete shmid=%d \n", shmid);
		perror("shmctl");
		exit(-1);
	}

	m_shmhead = NULL;
	m_payload = NULL;
	m_open = false;
}

void ShareM::ShmDestroy(int key)
{
	int shmid = 0;

	//1. 查看是否已经存在共享内存，如果有则删除旧的
	shmid = shmget((key_t)key, 0, 0);
	if (shmid != -1)
	{
		printf("Delete shmid=%d \n", shmid);
		shmctl(shmid, IPC_RMID, NULL); 	//	删除已经存在的共享内存
	}
}

//返回头地址
bool ShareM::ShmOpen(int key, int blksize, int blocks)
{
	int shmid;

	this->ShmClose();

	//1. 查看是否已经存在共享内存，如果有则删除旧的
	shmid = shmget((key_t)key, 0, 0);
	if (shmid == -1)
	{
		return this->ShmInit(key, blksize, blocks);
	}
	mid = shmid;
	//2.连接共享内存
	m_shmhead = shmat(shmid, (void*)0, 0);					//连接共享内存
	if (m_shmhead == (void*)-1)
	{
		perror("shmat");
	}

	//3. 填充控制共享内存的信息
	m_payload = (char*)((shmhead_t*)m_shmhead + 1);	//实际负载起始位置
	m_open = true;
	cout<<"-----"<< mid<< endl;
	return true;
}

int ShareM::getMid()
{
	return mid;
}

void ShareM::ShmClose(void)
{
	if (m_open)
	{
		shmdt(m_shmhead); //共享内存脱离
		m_shmhead = NULL;
		m_payload = NULL;
		m_open = false;
	}
}

void ShareM::ShmWrite(const void* buf)
{
	shmhead_t* pHead = (shmhead_t*)m_shmhead;

	sem_wait(&pHead->sem_full);				    //是否有资源写？	可用写资源-1
	sem_wait(&pHead->sem_mutex);				//是否有人正在写？		
	//memset(m_payload + (pHead->wr_index) * (pHead->blksize), 0, pHead->blksize);
	memcpy(m_payload + (pHead->wr_index) * (pHead->blksize), buf, pHead->blksize);
	pHead->wr_index = (pHead->wr_index + 1) % (pHead->blocks);	//写位置偏移		
	sem_post(&pHead->sem_mutex);				//解除互斥	
	sem_post(&pHead->sem_empty);				//可用读资源+1
}

void ShareM::ShmRead(void* buf)
{
	shmhead_t* pHead = (shmhead_t*)m_shmhead;

	sem_wait(&pHead->sem_empty);				//检测写资源是否可用		
	sem_wait(&pHead->sem_mutex);
	//memset(buf, 0, sizeof(buf));
	memcpy(buf, m_payload + (pHead->rd_index) * (pHead->blksize), pHead->blksize);
	//memset(m_payload + (pHead->rd_index) * (pHead->blksize), 0, pHead->blksize);
	//读位置偏移
	pHead->rd_index = (pHead->rd_index + 1) % (pHead->blocks);
	sem_post(&pHead->sem_mutex);				//解除互斥
	sem_post(&pHead->sem_full);					//增加可写资源	
}

