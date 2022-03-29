#pragma once

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <queue>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "CTask.h"


#define ETIMEDOUT 110
class ThreadPool
{
public:
	ThreadPool(int max, int min, int waitsec);		// ����߳����������߳�������ʱ�ȴ�ʱ��
	~ThreadPool();
public:
	static void* start_routine_A(void* arg);						// �����߳�
	void add_task(CTask* task);										// �������
	void start();
	void destroy();													// �����߳�(������Ҫ�Ȱ�����˯�ߵ��̻߳��Ѳ��ܽ�����������
	void create_thread();											// �����߳�
public:
	void lock_mutex();
	void unlock_mutex();

	void lock_task_mutex();
	void unlock_task_mutex();

	void broadcast();												// ���������߳�

	void signal_cond();												// ��������������Ҳ���ǻ����߳�
	bool timewait(int waitsec);										// ��ʱ�ȴ�����Ҫת�ɾ���ʱ��Ҫ�õ�timeval��timespec
private:
	bool b_stop;													// ������־λ
	int max_num;													// ������߳���
	int min_num;													// �����߳���				
	int cur_count;													// ��ǰ�߳���
	int wait_count;													// ˯���߳���
	int waitsec;													// ��ʱ�ȴ�ʱ�䣨s��

	std::queue<CTask*>task_que;			// �������

	pthread_cond_t cond;				// �̻߳��ѵ���������
	pthread_mutex_t mutex;				// ���������Ļ�������
	pthread_mutex_t mutex2;				// ������еĻ�����  ��֤��������ѵ��߳��ھ����������ʱͬ��
};


