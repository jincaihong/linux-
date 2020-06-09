#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include <list>
#include <iostream>
#include <pthread.h>
using namespace std;

//任务基类，其他任务都是继承它
class CTask
{
public:
	CTask(){}
	~CTask(){}
	virtual int run()= 0;
};

//线程池类
class CThreadPool
{
public:  
	CThreadPool(int maxCount = 100, int waittime = 30);
	int start(int threadcount = 30);			//默认创建threadcount个线程来等待任务
	int addTask(CTask *task);					//把任务添加到线程池中  
	int destroy();									//停止线程池
protected:
	static void* rontine(void * threadData);	//新线程的线程函数
private:
	void createThread();
	void lock();							//互斥量上锁
	void unlock();						//互斥量解锁
	int  wait();							//设置条件变量永久等待
	int  waittime(int sec);				//设置条件变量等待时间
	int  signal();						//发送信号
	int  broatcast();					//广播所有线程信号
	list<CTask*> m_taskList;				//任务列表
	pthread_mutex_t m_mutex;				//线程同步锁  
	pthread_cond_t m_cond;				//线程同步的条件变量
	bool m_bStop;						//线程停止标识
	int m_maxCount;						//线程的最大限制
	int m_waitCount;						//线程等待数
	int m_count;								//当前线程池中的线程数量
	int m_waittime;							//线程无任务的等待时间

}; 

#endif