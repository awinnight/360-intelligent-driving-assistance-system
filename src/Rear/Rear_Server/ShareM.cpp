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

//����ͷ��ַ
bool ShareM::ShmInit(int key, int blksize, int blocks)
{
	int shmid = 0;

	//1. �鿴�Ƿ��Ѿ����ڹ����ڴ棬�������ɾ���ɵ�
	shmid = shmget((key_t)key, 0, 0);
	if (shmid != -1)
	{
		shmctl(shmid, IPC_RMID, NULL); 	//	ɾ���Ѿ����ڵĹ����ڴ�
	}

	//2. ���������ڴ�
	shmid = shmget((key_t)key, sizeof(shmhead_t) + blksize * blocks, 0666 | IPC_CREAT | IPC_EXCL);
	if (shmid == -1)
	{
		perror("shmget");
		exit(-1);
	}
	printf("Create shmid=%d size=%ld \n", shmid, sizeof(shmhead_t) + blksize * blocks);

	//3.���ӹ����ڴ�
	m_shmhead = shmat(shmid, (void*)0, 0);					//���ӹ����ڴ�
	if (m_shmhead == (void*)-1)
	{
		perror("shmat");
		exit(-1);
	}
	memset(m_shmhead, 0, sizeof(shmhead_t) + blksize * blocks);		//��ʼ��

	//4. ��ʼ�������ڴ���Ϣ
	shmhead_t* pHead = (shmhead_t*)(m_shmhead);
	pHead->shmid = shmid;				//�����ڴ�shmid
	pHead->blksize = blksize;			//������Ϣд��
	pHead->blocks = blocks;				//д��ÿ���С
	pHead->rd_index = 0;					//һ��ʼλ�ö��ǵ�һ��
	pHead->wr_index = 0;					//
	sem_init(&pHead->sem_mutex, 1, 1);	// ��һ��1��ʾ���Կ���̹����ڶ���1��ʾ��ʼֵ
	sem_init(&pHead->sem_empty, 1, 0);	// ��һ��1��ʾ���Կ���̹����ڶ���0��ʾ��ʼֵ
	sem_init(&pHead->sem_full, 1, blocks);// ��һ��1��ʾ���Կ���̹����ڶ���blocks��ʾ��ʼֵ

	//5. �����ƹ����ڴ����Ϣ
	m_payload = (char*)(pHead + 1);	//ʵ�ʸ�����ʼλ��
	m_open = true;

	return true;
}

void ShareM::destroy()
{
	shmhead_t* pHead = (shmhead_t*)m_shmhead;
	int shmid = pHead->shmid;

	//ɾ���ź���
	sem_destroy(&pHead->sem_full);
	sem_destroy(&pHead->sem_empty);
	sem_destroy(&pHead->sem_mutex);
	shmdt(m_shmhead); //�����ڴ�����

	//���ٹ����ڴ�
	if (shmctl(shmid, IPC_RMID, 0) == -1)		//ɾ�������ڴ�
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

	//1. �鿴�Ƿ��Ѿ����ڹ����ڴ棬�������ɾ���ɵ�
	shmid = shmget((key_t)key, 0, 0);
	if (shmid != -1)
	{
		printf("Delete shmid=%d \n", shmid);
		shmctl(shmid, IPC_RMID, NULL); 	//	ɾ���Ѿ����ڵĹ����ڴ�
	}
}

//����ͷ��ַ
bool ShareM::ShmOpen(int key, int blksize, int blocks)
{
	int shmid;

	this->ShmClose();

	//1. �鿴�Ƿ��Ѿ����ڹ����ڴ棬�������ɾ���ɵ�
	shmid = shmget((key_t)key, 0, 0);
	if (shmid == -1)
	{
		return this->ShmInit(key, blksize, blocks);
	}
	mid = shmid;
	//2.���ӹ����ڴ�
	m_shmhead = shmat(shmid, (void*)0, 0);					//���ӹ����ڴ�
	if (m_shmhead == (void*)-1)
	{
		perror("shmat");
	}

	//3. �����ƹ����ڴ����Ϣ
	m_payload = (char*)((shmhead_t*)m_shmhead + 1);	//ʵ�ʸ�����ʼλ��
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
		shmdt(m_shmhead); //�����ڴ�����
		m_shmhead = NULL;
		m_payload = NULL;
		m_open = false;
	}
}

void ShareM::ShmWrite(const void* buf)
{
	shmhead_t* pHead = (shmhead_t*)m_shmhead;

	sem_wait(&pHead->sem_full);				    //�Ƿ�����Դд��	����д��Դ-1
	sem_wait(&pHead->sem_mutex);				//�Ƿ���������д��		
	//memset(m_payload + (pHead->wr_index) * (pHead->blksize), 0, pHead->blksize);
	memcpy(m_payload + (pHead->wr_index) * (pHead->blksize), buf, pHead->blksize);
	pHead->wr_index = (pHead->wr_index + 1) % (pHead->blocks);	//дλ��ƫ��		
	sem_post(&pHead->sem_mutex);				//�������	
	sem_post(&pHead->sem_empty);				//���ö���Դ+1
}

void ShareM::ShmRead(void* buf)
{
	shmhead_t* pHead = (shmhead_t*)m_shmhead;

	sem_wait(&pHead->sem_empty);				//���д��Դ�Ƿ����		
	sem_wait(&pHead->sem_mutex);
	//memset(buf, 0, sizeof(buf));
	memcpy(buf, m_payload + (pHead->rd_index) * (pHead->blksize), pHead->blksize);
	//memset(m_payload + (pHead->rd_index) * (pHead->blksize), 0, pHead->blksize);
	//��λ��ƫ��
	pHead->rd_index = (pHead->rd_index + 1) % (pHead->blocks);
	sem_post(&pHead->sem_mutex);				//�������
	sem_post(&pHead->sem_full);					//���ӿ�д��Դ	
}

