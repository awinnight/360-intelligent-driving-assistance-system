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
	ThreadPool(int max, int min, int waitsec);		// 最大线程数、核心线程数、超时等待时间
	~ThreadPool();
public:
	static void* start_routine_A(void* arg);						// 工作线程
	void add_task(CTask* task);										// 添加任务
	void start();
	void destroy();													// 销毁线程(这里需要先把所有睡眠的线程唤醒才能将其所有销毁
	void create_thread();											// 创建线程
public:
	void lock_mutex();
	void unlock_mutex();

	void lock_task_mutex();
	void unlock_task_mutex();

	void broadcast();												// 唤醒所有线程

	void signal_cond();												// 唤醒条件变量，也就是唤醒线程
	bool timewait(int waitsec);										// 超时等待（需要转成绝对时间要用到timeval与timespec
private:
	bool b_stop;													// 结束标志位
	int max_num;													// 最大工作线程数
	int min_num;													// 核心线程数				
	int cur_count;													// 当前线程数
	int wait_count;													// 睡眠线程数
	int waitsec;													// 超时等待时间（s）

	std::queue<CTask*>task_que;			// 任务队列

	pthread_cond_t cond;				// 线程唤醒的条件变量
	pthread_mutex_t mutex;				// 条件变量的互斥所锁
	pthread_mutex_t mutex2;				// 任务队列的互斥锁  保证多个被唤醒的线程在竞争任务队列时同步
};


