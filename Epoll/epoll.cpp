#include <string.h>
#include <stdio.h>
#include "../Epoll/epoll.h"

CEpoll::CEpoll(int size)
{
	epfd = epoll_create(size);
}

int CEpoll::AddEvent(int sockfd)
{
	struct epoll_event epevent;
	memset(&epevent,0,sizeof(epevent));
	epevent.data.fd = sockfd;
	epevent.events = EPOLLIN;
	return	epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&epevent);// 添加监听事件到epoll中
}

int CEpoll::DelEvent(int sockfd)
{
	struct epoll_event epevent;
	memset(&epevent,0,sizeof(epevent));
	epevent.data.fd = sockfd;
	epevent.events = EPOLLIN;
	return epoll_ctl(epfd,EPOLL_CTL_DEL,sockfd,&epevent);// 从epoll移除监听事件
}

int CEpoll::Wait(struct epoll_event *ev_read,int counts)
{
	int nfds;
	nfds = epoll_wait(epfd,ev_read,counts,-1);// 阻塞等待事件发生，返回发生事件的个数
	if (nfds == 0)
	{
		printf("epoll wait timeout\n");
	}
	return nfds;
}

int CEpoll::Getepoll()
{
	return epfd;
}