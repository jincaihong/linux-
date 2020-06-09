#include "../ThreadPool/BaseThread.h"

CBaseThread::CBaseThread()
:m_bRun(false),m_tid(0)
{

}

CBaseThread::~CBaseThread()
{

}

void CBaseThread::start()
{
	if (m_bRun == false)
	{
		if( pthread_create(&m_tid,NULL,rontine,(void *)this) != 0)
		{
			perror("create thread error : ");
		}

	}
}

bool CBaseThread::isRunning() const
{
	return m_bRun;
}

pthread_t CBaseThread::threadId() const
{
	return m_tid;
}

void * CBaseThread::rontine(void *arg)
{
	//////////////////////////////////////////////////////////////////////////
	//posix线程执行有两种状态joinable状态和unjoinable状态，
	//在joinable状态，当线程函数自己返回或pthread_exit时
	//不会释放线程所占用堆栈和线程描述符（总计8K多）。
	//只有当你调用了pthread_join之后这些资源才会被释放。
	//若是unjoinable状态的线程，这些资源在线程函数退出时或pthread_exit时自动会被释放。
	//unjoinable属性可以在pthread_create时指定，或在线程创建后在线程中pthread_detach自己, 
	//如：pthread_detach(pthread_self())，将状态改为unjoinable状态，确保资源的释放。
	//或者将线程置为 joinable,然后适时调用pthread_join.
	//其实简单的说就是在线程函数头加上 pthread_detach(pthread_self())的话，
	//线程状态改变为unjoinable，在pthread_exit线程就会在退出时自动释放资源。

	pthread_detach(pthread_self());//设计线程退出时自动释放资源，unjoinable
	CBaseThread *thr = (CBaseThread *)arg;
	thr->m_bRun = true;
	int ret = thr->run();
	thr->m_bRun = false;
	pthread_exit((void *)ret);
}

