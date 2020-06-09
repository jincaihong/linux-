#ifndef _BASETHREAD_H_
#define _BASETHREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

class CBaseThread
{
public:
	CBaseThread();
	~CBaseThread();
	void start();
	virtual int run() = 0;
	bool isRunning() const;
	pthread_t threadId() const;
protected:
	pthread_t m_tid;
	bool m_bRun;
private:
	static void * rontine(void *arg);
};

#endif