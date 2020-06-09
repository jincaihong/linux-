#ifndef _EPOLL_H
#define  _EPOLL_H

#include <sys/epoll.h>

class CEpoll
{
public:
	CEpoll(int size);
	int AddEvent(int sockfd);
	int DelEvent(int sockfd);
	int Wait(struct epoll_event *ev_read,int counts);
	int Getepoll();
private:
	int epfd;
};

#endif