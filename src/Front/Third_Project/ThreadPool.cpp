#include "ThreadPool.h"
#include <iostream>

using namespace std;

ThreadPool::ThreadPool(int max, int min, int waitsec)
{
	this->b_stop = false;
	this->max_num = max;
	this->min_num = min;
	this->waitsec = waitsec;

	this->wait_count = 0;
	this->cur_count = 0;

	pthread_cond_init(&this->cond, NULL);
	pthread_mutex_init(&this->mutex, NULL);
}

ThreadPool::~ThreadPool()
{
	destroy();
}

void* ThreadPool::start_routine_A(void* arg)
{
	pthread_detach(pthread_self());
	ThreadPool* pool = (ThreadPool*)arg;
	while (1)
	{
		pool->lock_mutex();
		if (pool->task_que.empty())
		{
			if (pool->b_stop)
			{
				cout << "thread is destory,pid = " << pthread_self() << endl;
				pool->cur_count--;
				pool->unlock_mutex();
				pthread_exit(NULL);
			}
			pool->wait_count++;															// 阻塞前等待线程++
			bool b_signal = pool->timewait(pool->waitsec);
			pool->wait_count--;															// 结束阻塞等待线程--
			if (!b_signal && pool->cur_count > pool->min_num)							// 超时等待 + 当前线程数大于核心线程数时允许销毁线程（也就是保留核心线程，只销毁非核心线程）
			{
				pool->cur_count--;
				pool->unlock_mutex();
				pthread_exit(NULL);
			}
		}
		pool->unlock_mutex();
		pool->lock_task_mutex();
		if (!pool->task_que.empty())
		{
			CTask* t = pool->task_que.front();
			pool->task_que.pop();
			pool->unlock_task_mutex();
			t->run();
			delete t;
		}
		else
		{
			pool->unlock_task_mutex();
		}
	}
}

void ThreadPool::add_task(CTask* task)
{
	cout << "-------------Add task-------------" << endl;
	if (b_stop)
	{
		return;
	}
	lock_task_mutex();
	task_que.push(task);
	unlock_task_mutex();

	lock_mutex();
	if (wait_count)
	{
		signal_cond();											// 唤醒一个线程
	}
	else if (cur_count < max_num)
	{
		create_thread();
	}
	unlock_mutex();
}

void ThreadPool::start()
{
	lock_mutex();
	b_stop = false;
	unlock_mutex();
}

void ThreadPool::destroy()
{
	b_stop = true;
	while (cur_count > 0)
	{
		lock_mutex();
		broadcast();
		unlock_mutex();
		sleep(1);
	}
}

void ThreadPool::create_thread()
{
	pthread_t pid;
	int res = pthread_create(&pid, NULL, start_routine_A, (void*)this);
	if (res < 0)
	{
		perror("ThreadPool::create_thread");
	}
	else
	{
		cur_count++;
	}

}

void ThreadPool::lock_mutex()
{
	pthread_mutex_lock(&this->mutex);
}

void ThreadPool::unlock_mutex()
{
	pthread_mutex_unlock(&this->mutex);
}

void ThreadPool::lock_task_mutex()
{
	pthread_mutex_lock(&this->mutex2);
}

void ThreadPool::unlock_task_mutex()
{
	pthread_mutex_unlock(&this->mutex2);
}

void ThreadPool::broadcast()
{
	cout << "All thread is wake" << endl;
	pthread_cond_broadcast(&cond);
}

void ThreadPool::signal_cond()
{
	pthread_cond_signal(&cond);
}

bool ThreadPool::timewait(int waitsec)
{
	struct timeval now;
	struct timespec outtime;

	gettimeofday(&now, NULL);
	outtime.tv_sec = now.tv_sec + waitsec;

	int ret = pthread_cond_timedwait(&this->cond, &this->mutex, &outtime);
	if (ret == ETIMEDOUT)
	{
		cout << "no core thread is time out." << endl;
		return false;
	}
	else if (ret != 0)
	{
		perror("no core thread error:");
	}
	cout << "no core Thread is wake up" << endl;
	return true;
}

