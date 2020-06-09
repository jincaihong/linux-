#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "Socket/Socket.h"
#include "public.h"
#include "task.h"

int max_con = 500;
int max_thr = 100;

CEpoll epoll(1024);

void sigpipe_fun(int sig)//socket断开,系统捕获到SIGPIPE信号处理函数
{
	int counts = CLogThread::links;
	int i = 0;
	int fd = 7;
	for (;i<counts;i++)
	{
		epoll.DelEvent(fd);
		close(fd);
		fd++;
		if (CLogThread::links > 0)
		{
			CLogThread::links--;
		}	
	}
}


int main()
{
	signal(SIGPIPE,sigpipe_fun);//关联SIGPIPE信号
	signal(SIGINT,sigint_fun);
	init_server();
	init_db();
 	//printf("默认数据库文件：Db/mydb.dat\n");
 	//printf("默认服务器端口号：5555\n");
 	//printf("输入最大连接数（建议1000）：");
 	//max_con = put_num(4);
 	//printf("输入启动线程数上限（建议100）：");
 	//max_thr = put_num(3);
	
	CTcpServer server;
 	server.Create();
 	int b_ret = server.Bind(CHostAddress("0.0.0.0",5555));
 	if (b_ret == -1)
 	{
 		perror("bind error:");
 		exit(-1);
 	}
 	server.Listen(1024);
 	int pid = fork();
 /*前置服务器*/
 	if (pid > 0)
 	{
		struct epoll_event ev_read[40];
		epoll.AddEvent(server.GetSocket());
 		CReplyThread reply;
 		reply.start();//运行监听应答包线程

		CThreadPool pre_pool(max_thr,30);
 		pre_pool.start(20);//运行线程池
 
		CLogThread log;
		log.start();//运行实时日志线程

		CPantThread pant(&epoll);//运行心跳处理线程
		pant.start();

		int i;
 		CTcpSocket tcpclient;
 		char rd_buf[BUFSIZE] = {0};//数据接收缓存
		PackHead_t head;
		PackTail_t tail;
 		while(1)
 		{
 			memset(ev_read,0,sizeof(struct epoll_event)*40);
 			int nfds = epoll.Wait(ev_read,40);//epoll监听
 			for (i = 0;i < nfds;i++)
 			{
 				int fd = ev_read[i].data.fd;
 				if (ev_read[i].data.fd == server.GetSocket())//建立新的连接
 				{
					if (CLogThread::links == 1010)
					{
						continue;
					}
 					tcpclient = server.Accept();
 					epoll.AddEvent(tcpclient.GetSocket());
					CLogThread::links++;
 				}
 				else if (ev_read[i].events & EPOLLIN)//客户端有可读事件
 				{
 					memset(rd_buf,0,BUFSIZE);
 					int rd_size = recv(fd,rd_buf,BUFSIZE,0);//从客户端socket读取数据，下面验证包的完整性
 					if (rd_size > 0)
 					{
 						memset(&head,0,sizeof(head));
 						memcpy(&head,rd_buf,sizeof(PackHead_t));
						cout << "========================" << head.func_num << endl;
 						memcpy(&tail,rd_buf+sizeof(PackHead_t)+head.pack_size,sizeof(PackTail_t));
						cout << "========================" << tail.pack_tail << endl;
 						if (tail.pack_tail == 55 )
 						{
							log_enab(head.counts);
 							CLogThread::recv_packs++;//接收的数据包计数
 							CAnalyTask *task = new CAnalyTask(rd_buf,ev_read[i].data.fd);
 							pre_pool.addTask(task);	
							cout << "1111111111111" << head.func_num << endl;
 						}
 					}
 					else//客户端退出处理
 					{
 						epoll.DelEvent(ev_read[i].data.fd);
 						close(ev_read[i].data.fd);
						if (CLogThread::links > 0)
						{
							CLogThread::links--;
						}				
 					}
 				}
 			}
 		}
 	}
 	/*后置服务器*/
 	else if (pid == 0)
 	{
 		CThreadPool back_pool(max_thr,30);
 		back_pool.start(20);//运行后置服务器的线程池
		shm1_read(&back_pool); //监听业务函数
 	}
	return 0;
}


