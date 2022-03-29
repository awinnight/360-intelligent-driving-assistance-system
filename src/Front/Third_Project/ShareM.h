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
    int shmid;					// �����ڴ�ID

    unsigned int blksize;		// ���С
    unsigned int blocks;		// �ܿ���
    unsigned int rd_index;		// ������
    unsigned int wr_index;		// д����

    //������ڹ����ڴ��ڲ�����
    sem_t sem_mutex;			// ���������õ��ź���
    sem_t sem_full;				// �������ƹ����ڴ��Ƿ������ź���
    sem_t sem_empty;			// �������ƹ����ڴ��Ƿ�յ��ź���
}shmhead_t;

class ShareM
{
    /*
public:
    void* shm ;                   //����Ĺ����ڴ��ԭʼ�׵�ַ
    int running ;                    //�����Ƿ�������еı�־  
    char buffer[BUFSIZ + 1];      
    SUS* shared;                        //ָ��shm   
    int shmid;                      //�����ڴ��ʶ�� //���������ڴ�   

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
    //����������
    bool ShmInit(int key, int blksize, int blocks);
    void destroy(void);
    static void ShmDestroy(int key); //��̬ɾ�������ڴ淽��

    // �򿪺͹ر�
    bool ShmOpen(int key, int blksize, int blocks);
    void ShmClose(void);

    //��ȡ�ʹ洢
    void ShmWrite(const void* buf);
    void ShmRead(void* buf);
protected:
    //���̿�����Ϣ��
    int mid;
    bool m_open;
    void* m_shmhead;		// �����ڴ�ͷ��ָ��
    char* m_payload;			// ��Ч���ص���ʼ��ַ
    


};



