#ifndef _TASK_H_
#define _TASK_H_

#include <termios.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "public.h"
#include "ShareMemory/sem.h"
#include "ShareMemory/ShareMemory.h"
#include "DbSingles/DbCon.h"
#include "ThreadPool/BaseThread.h"
#include "ThreadPool/ThreadPool.h"
#include "Epoll/epoll.h"

typedef struct Pant//心跳结构体
{
	int fd; //客户端fd
	bool flag;//心跳状态，true-在线，flase-离线
}Pant_t;
typedef struct sort_port
{

	int channel_id;//频道ID
	int sort_id;//排序ID
	int type_id;//分类ID
	int area_id;//地区ID
	int time_id;//年份id
}sort_port_t;
typedef struct Service//解析完后的业务包
{
	int func_num;//请求功能号
	int client_fd;//客服端fd
	int user_id;  //用户ID
	int video_id; //视频ID
	int video_seek; //播放时长
	//sort_port_t sort_tish;;
}Service_t; 



#define  BUFSIZE 200
#define  LISTSIZE sizeof(VideoList_t)*29+sizeof(PackHead_t)
#define  BLOCKSIZE sizeof(Service_t)
#define  SHM1_SIZE BLOCKSIZE*2000+sizeof(int)
#define  SHM2_SIZE BLOCKSIZE*4000+sizeof(int)
#define  SHM1_COUNT 2000
#define  SHM2_COUNT SHM2_SIZE-sizeof(int)
#define  SHMKEY1 (key_t)0001
#define  SHMKEY2 (key_t)0002
#define  SEMKEY (key_t)0003


class CAnalyTask :public CTask//解析任务
{
public:
	CAnalyTask(char *rd_buf,int fd);
	int run();
private:
	char buf[BUFSIZE];
	int des_fd;
};


class CHandleTask :public CTask//业务处理
{
public:
	CHandleTask(char *buf);
	int run();
private:
	Service_t server;
};

class CReplyThread :public CBaseThread//监听应答包线程
{
public:
	int run();
};

class CSendThread :public CBaseThread//发送应答包线程
{
public:
	CSendThread(int size,char *buf);
	int run();
private:
	CSendThread *self;
	int size;
	char buf[SHM2_COUNT];
};

class CLogThread :public CBaseThread//实时日志线程
{
public:
	int run();
	static int log_fd;
	static int links;
	static int recv_packs;
	static int send_packs;
	static int logins;
	static int channels;
	static int types;
	static int areas;
	static int lists;
	static int plays;
	static int play_times;
	static int pants;
};

class CPantThread :public CBaseThread//心跳线程
{
public:
	CPantThread(CEpoll *epoll);
	int run();
private:
	CEpoll *epoll;
};

void log_enab(int num);

void log_write(int num,char *buf,int user_id);//日志生成函数

void char_hex(char *buf,char *GetLog);

void init_server();//服务器启动初始化函数

void init_db();

void shm1_read(CThreadPool *back_pool);//监听业务包的函数

int put_num(int counts);//终端字符输入控制函数

void sigint_fun(int sig);

#endif