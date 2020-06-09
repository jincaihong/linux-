#include "task.h"
#include <string.h>
#include <list>
#include<stdio.h>
using namespace std;

CShaMemory *shm1;//共享内存1
CShaMemory *shm2;//共享内存2
CSem *sem;//信号量

list<Login_t> *login;
list<Forget_t> *Wjmm;
list<VideoList_t> *msg;//视频列表
list<VideoList_t>* smsg;//视频列表
list<Pant_t> *pant;//心跳链表

char *channel_buf;
char *msg_buf;
char *type_buf;
char *area_buf;
char* sort_buf;

/*实时日志的互斥锁*/
pthread_mutex_t log_mutex;
pthread_mutex_t channel_mutex;
pthread_mutex_t type_mutex;
pthread_mutex_t msg_mutex;
pthread_mutex_t play_mutex;
pthread_mutex_t record_mutex;
pthread_mutex_t send_mutex;
pthread_mutex_t pant_mutex;
pthread_mutex_t sort_mutex;



void sigint_fun(int sig)
{
	//delete shm1;
	//delete shm2;
	//delete sem;
	delete channel_buf;
	delete msg_buf;
	delete type_buf;
	delete area_buf;
	delete login;
	delete msg;
	delete pant;
	exit(0);
}

/********************************************前置线程池解析包回调函数*******************************************************************************************************/

CAnalyTask::CAnalyTask(char *rd_buf,int fd)
{
	memcpy(buf,rd_buf,BUFSIZE);
	this->des_fd = fd;
}

int CAnalyTask::run()
{
	char sql_temp[128] = {0};
	PackHead_t head;
	memset(&head,0,sizeof(head));
	memcpy(&head,buf,sizeof(head));
	Service_t server;
	memset(&server,0,sizeof(Service_t));
	server.client_fd = this->des_fd;
	server.func_num = head.func_num;
	cout << head.func_num << "收" << endl;
	switch(head.func_num)
	{
	case 1001://心跳包
		{
			CLogThread::pants++;
			list<Pant_t>::iterator it = pant->begin();
			for (;it != pant->end();it++)
			{
				if (server.client_fd == (*it).fd && (*it).flag == false)
				{
					(*it).flag = true;
					break;
				}
			}
			if (it == pant->end())
			{
				Pant_t data;
				data.fd = server.client_fd;
				data.flag = true;
				pthread_mutex_lock(&pant_mutex);
				pant->push_back(data);
				pthread_mutex_unlock(&pant_mutex);
			}
			log_write(1001,this->buf,server.client_fd);
			return 0;
		}
	case 1002://登陆请求包
		{
			pthread_mutex_lock(&log_mutex);
			CLogThread::logins++;
			pthread_mutex_unlock(&log_mutex);
			Login_t data;
			memset(&data,0,sizeof(login));
			memcpy(&data,buf+sizeof(PackHead_t),sizeof(Login_t));
			list<Login_t>::iterator it = login->begin();
			for (;it != login->end();it++)
			{
				cout << data.user_name << "   " << (*it).user_name << endl;
				cout << data.passwd << "   " << (*it).passwd << endl;
				if (strcmp(data.user_name,(*it).user_name) == 0 && strcmp(data.passwd,(*it).passwd) == 0)
				{
					server.user_id = (*it).user_id;
					cout << data.user_name << endl;
					cout << data.passwd << endl;
					break;
				}
			}
			if (sem->GetVal(2) == 1)
			{
				log_write(1002,this->buf,server.user_id);
			}
			break;
		}
	case 1003://注册请求包
		{
			Register_t data;
			int id=0;
			memset(&data, 0, sizeof(login));
			memcpy(&data, buf + sizeof(PackHead_t), sizeof(Register_t));
			list<Login_t>::iterator it = login->begin();
			for (; it != login->end(); it++)
			{
				id = (*it).user_id;
			}
			id++;
			server.user_id = id;
			char sql[128] = { 0 };
			sprintf(sql, "insert into Tbl_user values('%d','%s','%s','%s')",id,data.user_name,data.passwd,data.passwd2);
			(DbSingles::GetSingle())->GetData(sql, NULL, NULL);
			char sql2[128] = { 0 };
			strcpy(sql2, "select * from Tbl_user");
			(DbSingles::GetSingle())->GetData(sql2, Login_CallBack, (void*)login);
			
		}
	case 1004://忘记密码请求包
		{
			char sql1[128] = { 0 };
			strcpy(sql1, "select * from Tbl_user");
			(DbSingles::GetSingle())->GetData(sql1, wj_CallBack, (void*)Wjmm);
			Forget_t data;
			memset(&data, 0, sizeof(login));
			memcpy(&data, buf + sizeof(PackHead_t), sizeof(Forget_t));
			list<Forget_t>::iterator it = Wjmm->begin();
			for (; it != Wjmm->end(); it++)
			{
				if (strcmp(data.user_name, (*it).user_name) == 0 && strcmp(data.passwd2, (*it).passwd2) == 0)
				{
					server.user_id = 1;
					cout << "xuigai" << endl;
					char sql[128] = { 0 };
					sprintf(sql, "update Tbl_user set user_pwd = '%s' where user_name = '%s'", data.passwd, data.user_name);
					(DbSingles::GetSingle())->GetData(sql, NULL, NULL);
					char sql2[128] = { 0 };
					strcpy(sql2, "select * from Tbl_user");
					(DbSingles::GetSingle())->GetData(sql2, Login_CallBack, (void*)login);
					break;
				}
			}
		}
	case 1005:
	{
		Search_t data;
		memcpy(&data, buf + sizeof(PackHead_t), sizeof(Search_t));
		int size;
		char sql[128] = { 0 };
		char sql1[128] = { 0 };
		char sql2[128] = { 0 };
		sprintf(sql, "select * from Tbl_video_message where  video_name = '%s'", data.vedio_name);
		cout << data.vedio_name << endl;
		smsg = new list<VideoList_t>;
		(DbSingles::GetSingle())->GetData(sql, wyfl, (void*)smsg);
		sprintf(sql2, "delete from Tbl_video_zhong");
		(DbSingles::GetSingle())->GetData(sql2, NULL, NULL);
		list<VideoList_t>::iterator it = smsg->begin();
		size = smsg->size();
		for (; it != smsg->end(); it++)
		{
			cout << "vid   " << (*it).video_id << endl;
			sprintf(sql1, "insert into Tbl_video_zhong values('%d','%d','%d','%d','%s',%d,'%s')", (*it).video_id, (*it).channel_id, (*it).area_id, (*it).type_id, (*it).video_name, (*it).play_times, (*it).play_locad);
			(DbSingles::GetSingle())->GetData(sql1, NULL, NULL);
		}
		break;
	}
	case 2001://频道请求包
		{
			pthread_mutex_lock(&channel_mutex);
			CLogThread::channels++;
			pthread_mutex_unlock(&channel_mutex);
			if (sem->GetVal(2) == 1)
			{
				log_write(2001,this->buf,0);
			}
			break;
		}
	case 2002://分类请求包
		{
			pthread_mutex_lock(&type_mutex);
			CLogThread::types++;
			pthread_mutex_unlock(&type_mutex);
			if (sem->GetVal(2) == 1)
			{
				log_write(2002,this->buf,0);
			}
			break;
		}
	case 2003://地区请求包
		{
			CLogThread::areas++;
			if (sem->GetVal(2) == 1)
			{
				log_write(2003,this->buf,0);
			}
			break;
		}
	case 2004://视频列表请求包
		{
			pthread_mutex_lock(&msg_mutex);
			CLogThread::lists++;
			pthread_mutex_unlock(&msg_mutex);

			if (sem->GetVal(2) == 1)
			{
				log_write(2004,this->buf,0);
			}
			break;
		}
	case 2005:
	{
		
		break;
	}
	case 2006:
	{
		selectVideoList_t data;
		memcpy(&data, buf + sizeof(PackHead_t), sizeof(selectVideoList_t));
		int size;
		char sql[128] = { 0 };
		char sql1[128] = { 0 };
		char sql2[128] = { 0 };
		smsg = new list<VideoList_t>;
		if (data.area_id == 1 || data.time_id == 1 || data.type_id == 1)
		{
			if (data.area_id == 1)
			{
				sql[128] = { 0 };
				sprintf(sql, "select * from Tbl_video_message where channel_id = %d and sort_id = %d and type_id = %d and plist_id = %d", data.time_id, data.sort_id, data.type_id, data.channel_id);
			}
			if (data.time_id == 1)
			{
				sql[128] = { 0 };
				sprintf(sql, "select * from Tbl_video_message where area_id = %d  and sort_id = %d and type_id = %d and plist_id = %d", data.area_id, data.sort_id, data.type_id, data.channel_id);
			}
			if (data.type_id == 1)
			{
				sql[128] = { 0 };
				sprintf(sql, "select * from Tbl_video_message where area_id = %d and channel_id = %d and sort_id = %d  and plist_id = %d", data.area_id, data.time_id, data.sort_id, data.channel_id);
			}
			if (data.area_id == 1 && data.time_id == 1)
			{
				sql[128] = { 0 };
				sprintf(sql, "select * from Tbl_video_message where sort_id = %d and type_id = %d and plist_id = %d",  data.sort_id, data.type_id, data.channel_id);
			}
			if (data.time_id == 1 && data.type_id == 1)
			{
				sql[128] = { 0 };
				sprintf(sql, "select * from Tbl_video_message where area_id = %d  and sort_id = %d  and plist_id = %d", data.area_id, data.sort_id,  data.channel_id);
			}
			if (data.area_id == 1 && data.type_id == 1)
			{
				sql[128] = { 0 };
				sprintf(sql, "select * from Tbl_video_message where channel_id = %d and sort_id = %d  and plist_id = %d", data.time_id, data.sort_id,  data.channel_id);
			}
			if (data.area_id == 1 && data.time_id == 1 && data.type_id == 1)
			{
				sql[128] = { 0 };
				sprintf(sql, "select * from Tbl_video_message where  sort_id = %d  and plist_id = %d",  data.sort_id, data.channel_id);
			}
			
		}
		else
		{
			sql[128] = { 0 };
			sprintf(sql, "select * from Tbl_video_message where area_id = %d and channel_id = %d and sort_id = %d and type_id = %d and plist_id = %d", data.area_id, data.time_id, data.sort_id, data.type_id, data.channel_id);
		}
		if (data.area_id == 1 && data.time_id == 1 && data.type_id == 1 && data.sort_id)
		{
			sql[128] = { 0 };
			sprintf(sql, "select * from Tbl_video_message where  plist_id = %d",data.channel_id);
		}
		(DbSingles::GetSingle())->GetData(sql, wyfl, (void*)smsg);
		sprintf(sql2, "delete from Tbl_video_zhong");
		(DbSingles::GetSingle())->GetData(sql2, NULL,NULL);
		list<VideoList_t>::iterator it = smsg->begin();
		size = smsg->size();
		for (; it != smsg->end(); it++)
		{
			cout << "vid   " << (*it).video_id << endl;
			sprintf(sql1, "insert into Tbl_video_zhong values('%d','%d','%d','%d','%s',%d,'%s')", (*it).video_id, (*it).channel_id, (*it).area_id, (*it).type_id, (*it).video_name, (*it).play_times, (*it).play_locad);
			(DbSingles::GetSingle())->GetData(sql1,NULL ,NULL);
		}
		break;
	}
	case 3001://视频点播请求包
		{
			pthread_mutex_lock(&play_mutex);
			CLogThread::plays++;
			pthread_mutex_unlock(&play_mutex);
			VideoPlay_t data;
			memcpy(&data,buf+sizeof(PackHead_t),sizeof(VideoPlay_t));
			server.user_id = data.user_id;
			server.video_id = data.video_id;
			cout << "================" << endl;
			cout << server.user_id << server.video_id << endl;
			if (sem->GetVal(2) == 1)
			{
				log_write(3001,this->buf,data.user_id);
			}
			break;
		}
	case 3002://视频点播时长请求包
		{
			pthread_mutex_lock(&record_mutex);
			CLogThread::play_times++;
			pthread_mutex_unlock(&record_mutex);
			VideoRecord_t data;
			memcpy(&data,buf+sizeof(PackHead_t),sizeof(VideoRecord_t));
			server.video_id = data.video_id;
			server.user_id = data.user_id;
			cout << "================" << endl;
			cout << server.user_id << server.video_id << endl;
			server.video_seek = data.video_seek;
			if (sem->GetVal(2) == 1)
			{
				log_write(3002,this->buf,data.user_id);
			}
			break;
		}
	case 4001://请求播放历史
		{
			Play_t data;
			memcpy(&data, buf + sizeof(PackHead_t), sizeof(Play_t));
			server.user_id = data.user_id;
			log_write(4001,this->buf,server.user_id);
			break;
		}
	case 4002://请求播放历史
	{
		ClearHistory_t data;
		memcpy(&data, buf + sizeof(PackHead_t), sizeof(ClearHistory_t));
		server.user_id = data.user_id;
		break;
	}

	}
	//向第一块共享内存写入解析后的业务包（对共享内存的读写都加PV锁）
	sem->Sem_P(0);
	int counts = shm1->get_head();
	if (counts == SHM1_COUNT)
	{
		while(1)
		{
			sem->Sem_V(0);
			usleep(10);
			sem->Sem_P(0);
			counts = shm1->get_head();
			if (counts < SHM1_COUNT)
			break;	
		}
	}
	shm1->write_block(&server);
	sem->Sem_V(0);
	return 0;
}


/********************************************后置线程池业务处理回调函数*******************************************************************************************************/

CHandleTask::CHandleTask(char *buf)
{
	memset(&server,0,sizeof(Service_t));
	memcpy(&server,buf,sizeof(Service_t));
}

int CHandleTask::run()
{

	int wr_len = 0;
	char s_buf[LISTSIZE] = {0};
	memset(s_buf,0,sizeof(s_buf));
	PackHead_t head;
	memset(&head,0,sizeof(head));
	switch(server.func_num)
	{
	case 1002:
		{
			LoginRet_t logret;
			memset(&logret,0,sizeof(logret));
			logret.user_id = server.user_id;
			cout << logret.user_id << endl;
			logret.login_ret = (logret.user_id > 0 ? 0:1);
			head.counts = 1;
			head.func_num=1002;
			head.des_fd = server.client_fd;
			head.pack_size = sizeof(LoginRet_t);
			memcpy(s_buf,&head,sizeof(head));
			memcpy(s_buf+sizeof(PackHead_t),&logret,sizeof(LoginRet_t));
			break;
		}
	case 1003:
	{
		RegisterRet_t sigin;
		memset(&sigin, 0, sizeof(sigin));
		sigin.user_id = server.user_id;
		sigin.Register_ret = (sigin.user_id > 0 ? 0 : 1);
		head.counts = 1;
		head.func_num = 1003;
		head.des_fd = server.client_fd;
		head.pack_size = sizeof(RegisterRet_t);
		memcpy(s_buf, &head, sizeof(head));
		memcpy(s_buf + sizeof(RegisterRet_t), &sigin, sizeof(RegisterRet_t));
		memcpy(s_buf, &head, sizeof(head));
		memcpy(s_buf + sizeof(PackHead_t), &sigin, sizeof(RegisterRet_t));
		break;
	}
	case 1004:
	{
		ForgetRet_t sigin;
		memset(&sigin, 0, sizeof(sigin));
		sigin.user_id = server.user_id;
		sigin.Register_ret = (sigin.user_id > 0 ? 0 : 1);
		head.counts = 1;
		head.func_num = 1004;
		head.des_fd = server.client_fd;
		head.pack_size = sizeof(ForgetRet_t);
		memcpy(s_buf, &head, sizeof(head));
		memcpy(s_buf + sizeof(ForgetRet_t), &sigin, sizeof(ForgetRet_t));
		memcpy(s_buf, &head, sizeof(head));
		memcpy(s_buf + sizeof(PackHead_t), &sigin, sizeof(ForgetRet_t));
		break;
	}
	case 1005:
	{
		head.counts = 0;
		int size;
		char sql[128] = { 0 };
		smsg = new list<VideoList_t>;
		sprintf(sql, "select * from  Tbl_video_zhong");
		(DbSingles::GetSingle())->GetData(sql, wyfl, (void*)smsg);
		list<VideoList_t>::iterator it = smsg->begin();
		size = smsg->size();
		sort_buf = new char[sizeof(VideoList_t) * size + sizeof(PackHead_t)];
		memset(sort_buf, 0, sizeof(VideoList_t) * size + sizeof(PackHead_t));
		for (; it != smsg->end(); it++)
		{
			cout << "vid   " << (*it).video_id << endl;
			memcpy(sort_buf + sizeof(head) + sizeof(VideoList_t) * head.counts, &(*it), sizeof(VideoList_t));
			head.counts++;
		}
		head.pack_size = head.counts * sizeof(VideoList_t);
		head.func_num = 2004;
		memcpy(sort_buf, &head, sizeof(head));

		memcpy(s_buf, sort_buf, LISTSIZE);
		memcpy(&head, sort_buf, sizeof(PackHead_t));
		head.des_fd = server.client_fd;
		memcpy(s_buf, &head, sizeof(PackHead_t));
		break;
	}
	case 2001:
		{	
			memcpy(s_buf,channel_buf,LISTSIZE);
			memcpy(&head,channel_buf,sizeof(PackHead_t));
			head.des_fd = server.client_fd;
			memcpy(s_buf,&head,sizeof(PackHead_t));
			break;
		}
	case 2002:
		{	
			memcpy(s_buf,type_buf,LISTSIZE);
			memcpy(&head,type_buf,sizeof(PackHead_t));
			head.des_fd = server.client_fd;
			memcpy(s_buf,&head,sizeof(PackHead_t));
			break;
		}
	case 2003:
		{	
			memcpy(s_buf,area_buf,LISTSIZE);
			memcpy(&head,area_buf,sizeof(PackHead_t));
			head.des_fd = server.client_fd;
			memcpy(s_buf,&head,sizeof(PackHead_t));
			break;
		}
	case 2004:
		{	
			memcpy(s_buf,msg_buf,LISTSIZE);
			memcpy(&head,msg_buf,sizeof(PackHead_t));
			head.des_fd = server.client_fd;
			memcpy(s_buf,&head,sizeof(PackHead_t));
			break;
		}
	case 2005:
	{
		head.counts = 0;
		head.func_num = 2005;
		head.des_fd = server.client_fd;
		head.pack_size = 0;
		memcpy(s_buf, &head, sizeof(head));
		break;
	}
	case 2006:
	{
		head.counts = 0;
		int size;
		char sql[128] = { 0 };
		smsg = new list<VideoList_t>;
		sprintf(sql, "select * from  Tbl_video_zhong");
		(DbSingles::GetSingle())->GetData(sql, wyfl, (void*)smsg);
		list<VideoList_t>::iterator it = smsg->begin();
		size = smsg->size();
		sort_buf=new char[sizeof(VideoList_t) * size + sizeof(PackHead_t)];
		memset(sort_buf, 0, sizeof(VideoList_t) * size + sizeof(PackHead_t));
		for (; it != smsg->end(); it++)
		{
			cout << "vid   "<<(*it).video_id << endl;
			memcpy(sort_buf + sizeof(head) + sizeof(VideoList_t) * head.counts, &(*it), sizeof(VideoList_t));
			head.counts++;
		}
		head.pack_size = head.counts * sizeof(VideoList_t);
		head.func_num = 2004;
		memcpy(sort_buf, &head, sizeof(head));

		memcpy(s_buf, sort_buf, LISTSIZE);
		memcpy(&head, sort_buf, sizeof(PackHead_t));
		head.des_fd = server.client_fd;
		memcpy(s_buf, &head, sizeof(PackHead_t));
		break;
	}
	case 3001:
		{
			if (server.video_id >= 1 && server.video_id <= 29)
			{
				char sql[128] = {0};
				sprintf(sql,"update Tbl_video_message set play_counts = play_counts+1 where video_id = %d",server.video_id);
				(DbSingles::GetSingle())->GetData(sql,NULL,NULL);
				list<VideoList_t>::iterator it = msg->begin();
				for (;it != msg->end();it++)
				{
					if ((*it).video_id == server.video_id)
					{
						(*it).play_times++;
						break;
					}
				}
				UpLoad_t ack;
				ack.results = 0;
				head.counts =1;
				head.pack_size = sizeof(UpLoad_t);
				memcpy(s_buf,&head,sizeof(head));
				memcpy(s_buf+sizeof(PackHead_t),&ack,sizeof(UpLoad_t));
				break;
			}
			else
			{
				UpLoad_t ack;
				ack.results = 1;
				head.counts =1;
				head.pack_size = sizeof(UpLoad_t);
				memcpy(s_buf,&head,sizeof(head));
				memcpy(s_buf+sizeof(PackHead_t),&ack,sizeof(UpLoad_t));
				break;
			}		
		}
	case 3002:
		{
			if (server.video_id >= 1&& server.video_id <= 29 && server.user_id > 0)
			{
				char sql[256] = {0};
				char tm[6] = {0};
				sprintf(sql,"select play_times from Tbl_user_play where user_id = %d and video_id = %d",server.user_id,server.video_id);
				(DbSingles::GetSingle())->GetData(sql,List_CallBack,(void *)tm);
				if (atoi(tm) == 0)
				{
					char id[6] = {0};
					sprintf(sql,"select count(play_id) as counts from Tbl_user_play");
					(DbSingles::GetSingle())->GetData(sql,List_CallBack,(void *)id);
					int play_id = atoi(id) + 1;
					sprintf(sql,"insert into Tbl_user_play values(%d,%d,%d,%d,'')",play_id,server.user_id,server.video_id,server.video_seek);
					(DbSingles::GetSingle())->GetData(sql,NULL,NULL);
				}
				else
				{
					sprintf(sql,"update  Tbl_user_play set play_times = %d where user_id = %d and video_id = %d",server.video_seek,server.user_id,server.video_id);
					(DbSingles::GetSingle())->GetData(sql,NULL,NULL);
				}
				UpLoad_t ack;
				ack.results = 0;
				head.counts =1;
				head.pack_size = sizeof(UpLoad_t);
				memcpy(s_buf,&head,sizeof(head));
				memcpy(s_buf+sizeof(PackHead_t),&ack,sizeof(UpLoad_t));
				break;
			}
			else
			{
				UpLoad_t ack;
				ack.results = 1;
				head.counts =1;
				head.pack_size = sizeof(UpLoad_t);
				memcpy(s_buf,&head,sizeof(head));
				memcpy(s_buf+sizeof(PackHead_t),&ack,sizeof(UpLoad_t));
				break;
			}	
		}
	case 4001:
		{
			list<PlayHistory_t> his;
			char sql[128] = {0};
			sprintf(sql,"select video_id,play_times from Tbl_user_play where user_id = %d",server.user_id);
			(DbSingles::GetSingle())->GetData(sql,His_CallBack,(void *)&his);
			list<PlayHistory_t>::iterator it = his.begin();
			for (;it != his.end();it++)
			{
				memcpy(s_buf+sizeof(head)+sizeof(PlayHistory_t)*head.counts,&(*it),sizeof(PlayHistory_t));
				head.counts++;
			}
			head.pack_size = head.counts*sizeof(PlayHistory_t);
			head.func_num = 4001;
			head.des_fd = server.client_fd;
			memcpy(s_buf,&head,sizeof(head));	
			break;
		}
	case 4002:
	{
		char sql[128] = { 0 };
		sprintf(sql, "delete from Tbl_user_play where user_id = %d", server.user_id);
		(DbSingles::GetSingle())->GetData(sql, NULL, NULL);
		break;
	}
	}
	wr_len = head.pack_size + sizeof(PackHead_t);
	sem->Sem_P(1);
	int size = shm2->get_head();
	if ((SHM2_COUNT - size) < wr_len)
	{
		while(1)
		{
			sem->Sem_V(1);
			usleep(10);
			sem->Sem_P(1);
			size = shm2->get_head();
			if ((SHM2_COUNT - size) > wr_len)
				break;	
		}
	}
	shm2->write_size(s_buf,wr_len);
	sem->Sem_V(1);
	return 0;
}


/********************************************前置服务器监听应答包（共享内存2）线程*******************************************************************************************************/

int CReplyThread::run()
{
	char buf[SHM2_COUNT] = {0};
	int size = 0;
	CSendThread *send;
	while(1)
	{
		memset(buf,0,sizeof(buf));
		sem->Sem_P(1);
		size = shm2->get_head();
		if (size == 0)
		{
			while(1)
			{
				sem->Sem_V(1);
				usleep(3);
				sem->Sem_P(1);
				size = shm2->get_head();
				if (size > 0)
				break;			
			}
		}
		shm2->read_size(buf);
		sem->Sem_V(1);
		send = new CSendThread(size,buf);
		send->start();
	}
	return 0;
}


CSendThread::CSendThread(int size,char *buf)
{
	this->self = this;
	this->size = size;
	memcpy(this->buf,buf,SHM2_COUNT);
}

int CSendThread::run()
{
	PackHead_t head;
	int i = 0;
	int wr_size,wr_len;
	char s_buf[LISTSIZE] = {0}; 
	while(1)
	{
		int j;
		memset(&head,0,sizeof(head));
		memcpy(&head,buf+i,sizeof(PackHead_t));
	    memset(s_buf,0,LISTSIZE);
		memcpy(s_buf,buf+i,head.pack_size+sizeof(PackHead_t));
		wr_len = head.pack_size + sizeof(PackHead_t);
		cout <<"fd          "<< head.des_fd << endl;
		if (head.func_num != 2005)
		{
			wr_size = send(head.des_fd, s_buf, LISTSIZE, 0);
		}
		cout << head.func_num << "   " << head.counts<< endl;
		switch (head.func_num)
		{
			case 1002:
			{
				LoginRet_t data;
				memset(&data, 0, sizeof(data));
				memcpy(&data, buf + sizeof(head), sizeof(data));
				if (data.login_ret == 0)
				{
					printf("login success,user_id = %d\n", data.user_id);
				}
				else
					printf("login faided,user_id = %d\n", data.user_id);

				break;
			}
			case 1003:
			{
				RegisterRet_t sigin;
				memset(&sigin, 0, sizeof(sigin));
				memcpy(&sigin, buf + sizeof(head), sizeof(sigin));
				if (sigin.Register_ret== 0)
				{
					printf("sigin success,user_id = %d\n", sigin.user_id);
				}
				else
				{
					printf("sigin faided,user_id = %d\n", sigin.user_id);
				}
			}
			case 1004:
			{
				RegisterRet_t sigin;
				memset(&sigin, 0, sizeof(sigin));
				memcpy(&sigin, buf + sizeof(head), sizeof(sigin));
				if (sigin.Register_ret == 0)
				{
					printf("xgmm success,user_id = %d\n", sigin.user_id);
				}
				else
				{
					printf("xgmm faided,user_id = %d\n", sigin.user_id);
				}
			}
			case 2001:
			{

				VideoChannel_t data;
				for (j = 0; j < head.counts; j++)
				{
					memset(&data, 0, sizeof(data));
					memcpy(&data, buf + sizeof(head) + sizeof(data) * j, sizeof(data));
					printf("%s\n", data.channel_name);
				}
				break;
			}

			case 2002:
			{
				VideoType_t data;
				for (j = 0; j < head.counts; j++)
				{
					memset(&data, 0, sizeof(data));
					memcpy(&data, buf + sizeof(head) + sizeof(data) * j, sizeof(data));
					printf("%s\n", data.type_name);
				}
				break;
			}
			case 2003:
			{
				VideoArea_t data;
				for (j = 0; j < head.counts; j++)
				{
					memset(&data, 0, sizeof(data));
					memcpy(&data, buf + sizeof(head) + sizeof(data) * j, sizeof(data));
					printf("%s\n", data.area_name);
				}
				break;
			}
			case 2004:
			{
				VideoList_t data;
				for (j = 0; j < head.counts; j++)
				{
					memset(&data, 0, sizeof(data));
					memcpy(&data, buf + sizeof(head) + sizeof(data) * j, sizeof(data));
					printf("%s %d\n", data.play_locad, data.play_times);

				}
				break;
			}
			case 2005:
			{
				break;
			}
			case 2006:
			{
				VideoList_t data;
				for (j = 0; j < head.counts; j++)
				{
					memset(&data, 0, sizeof(data));
					memcpy(&data, buf + sizeof(head) + sizeof(data) * j, sizeof(data));
					printf("%s %d\n", data.play_locad, data.play_times);

				}
				break;
			}
		}
		if (head.func_num == 2005)
		{
			FILE* fq;
			int len;
			static int x = 1;
			char ch[128] = {0};
				char  buffer[4096];
				sprintf(ch, "/root/photo/%d.png", x);
				if ((fq = fopen(ch, "rb")) == NULL)
				{
					printf("File open.\n");
				}
				bzero(buffer, sizeof(buffer));
				while (!feof(fq))
				{
					len = fread(buffer, 1, sizeof(buffer), fq);
					if (len != write(head.des_fd, buffer, len))
					{
						printf("write.\n");
						break;
					}

				}
				cout << x << endl;
				if (x == 29)
				{
					x = 0;
					
				}
				x++;
				
		}
		if (wr_size > 0)
		{
			CLogThread::send_packs++;	
		}
		i = i + wr_len;
		if (i == size)
		{
			break;
		}
	}
	delete self;
	return 0;
}

/********************************************后置服务器监听业务包（共享内存1）函数*******************************************************************************************************/
void shm1_read(CThreadPool *back_pool)
{
	int i;
	char buf[SHM1_SIZE-4] = {0};
	while(1)
	{
		memset(buf,0,sizeof(buf));
		sem->Sem_P(0);
		int counts = shm1->get_head();
		if (counts == 0)
		{
			while(1)
			{
				sem->Sem_V(0);
				usleep(3);
				sem->Sem_P(0);
				counts = shm1->get_head();
				if (counts > 0)
				break;			
			}
		}
		shm1->read_block(buf);
		sem->Sem_V(0);
		char buf1[BLOCKSIZE] = {0};
		for (i = 0;i < counts;i++)
		{
			memset(buf1,0,BLOCKSIZE);
			memcpy(buf1,buf+i*BLOCKSIZE,BLOCKSIZE);
			CHandleTask *task = new CHandleTask(buf1);
			back_pool->addTask(task);
		}
	}
}

/****************************************************心跳处理线程*******************************************************************************************************/
CPantThread::CPantThread(CEpoll *epoll)
{
	this->epoll = epoll;
}
int CPantThread::run()
{
	list<Pant_t>::iterator it = pant->begin();
	while(1)
	{
		pthread_mutex_lock(&pant_mutex);
		for (it = pant->begin();it != pant->end();)
		{
			if ((*it).flag == true)
			{
				(*it).flag = false;
				it++;
			}
			else if ((*it).flag == false)
			{
				if (CLogThread::links > 0)
				{
						pant->erase(it++);
						continue;
				}
				epoll->DelEvent((*it).fd);
				close((*it).fd);
				if (CLogThread::links > 0)
				{
					CLogThread::links--;
				}			
				pant->erase(it++);
			}
		}
		pthread_mutex_unlock(&pant_mutex);
		sleep(6);
	}
	return 0;
}

/*******************************************实时日志显示线程*******************************************************************************************************/
int CLogThread::log_fd = 0;
int CLogThread::links = 0;
int CLogThread::recv_packs = 0;
int CLogThread::send_packs = 0;
int CLogThread::logins = 0;
int CLogThread::channels = 0;
int CLogThread::types = 0;
int CLogThread::areas = 0;
int CLogThread::lists = 0;
int CLogThread::plays = 0;
int CLogThread::play_times = 0;
int CLogThread::pants = 0;
int CLogThread::run()
{
	while (1)
	{
		sleep(1);
		printf("\\***********************************************************\\\n");
		printf("有效连接数：%d\n",links);
		printf("接收数据包：%d  发送数据包：%d  心跳包：%d\n\n",recv_packs,send_packs,pants);
		printf("用户登入数：%d\n",logins);
		printf("获取视频频道：%d  获取视频分类：%d  获取视频地区：%d\n",channels,types,areas);
		printf("获取视频列表：%d  上传播放数据：%d  上传播放时长：%d\n",lists,plays,play_times);
		printf("\\***********************************************************\\\n");
	}
}

/*******************************************日志生成函数*******************************************************************************************************/
void log_enab(int num)
{
	if (num == 1000 && sem->GetVal(2) == 1)
	{
		sem->SetVal(2,0);
	}
	else if (num != 1000 && sem->GetVal(2) == 0)
	{
		sem->SetVal(2,1);
	}
}

void log_write(int num,char *buf,int user_id)
{
	char log[2000] = {0};
	char str[100] = {0};
	time_t timep;
	struct tm *t;
	time(&timep);
	t = localtime(&timep);
	sprintf(str,"\r\n时间：%d-%d-%d %d:%d:%d\r\n",(1900+t->tm_year),(1+t->tm_mon),t->tm_mday,t->tm_hour,t->tm_min,t->tm_sec);
	strcat(log,str);
	switch (num)
	{
	case 1001:
		strcat(log,"功能：心跳处理\r\n");
		break;
	case 1002:
		strcat(log,"功能：登录\r\n");
		break;
	case 2001:
		strcat(log,"功能：视频频道获取\r\n");
		break;
	case 2002:
		strcat(log,"功能：视频分类获取\r\n");
		break;
	case 2003:
		strcat(log,"功能：视频地区获取\r\n");
		break;
	case 2004:
		strcat(log,"功能：视频列表获取\r\n");
		break;
	case 3001:
		strcat(log,"功能：上传播放数据\r\n");
		break;
	case 3002:
		strcat(log,"功能：上传播放时长\r\n");
		break;
	case 4001:
		strcat(log,"功能：播放历史获取\r\n");
		break;
	}
	strcat(log,"类型：接收\r\n");
	if (num == 1001)
	{
		sprintf(str,"客户端FD：%d\r\n",user_id);
	}
	else
		sprintf(str,"用户ID：%d\r\n",user_id);
	strcat(log,str);
	strcat(log,"数据包：\r\n");
	char_hex(buf,log);
	flock(CLogThread::log_fd,LOCK_EX);
	write(CLogThread::log_fd,log,strlen(log));
	fsync(CLogThread::log_fd);//刷新fd内容到磁盘
	flock(CLogThread::log_fd,LOCK_UN);
}


void char_hex(char *buf,char *GetLog)
{
	int i,j;
	int len = 64;
	char acBuf[64] = {0};
	unsigned char tmp[512];
	memset(tmp,0,512);
	memcpy(tmp,buf,len);  
	memset(acBuf, 0x00 ,64);
	for(i=0;i<len/16+1;i++)
	{
		if((i*16) >= len)
		{
			break;
		}
		for(j=0;j<16;j++)
		{
			if((i*16+j) >= len)
			{
				break;
			}
			sprintf(acBuf,"%02x",tmp[i*16+j]);
			strcat(GetLog, acBuf);
			memset(acBuf, 0x00 ,64);

			if(((j+1)%4 != 0 ) && ((i*16+j)<(len-1)))
			{
				strcat(GetLog, " ");
			}
			if((j+1)%4 == 0 )
			{
				strcat(GetLog, " ");
			}
		}
		if(len/16 == i)
		{
			for(j=0;j<((16-(len%16)-1))*3;j++)
			{
				strcat(GetLog, " ");
			}
		}
		strcat(GetLog, "\r\n");
	}
}

/*******************************************服务器初始化函数*******************************************************************************************************/
void init_server()
{
	char str[50] = {0};
	time_t timep;
	struct tm *t;
	time(&timep);
	t = localtime(&timep);
	sprintf(str,"Log/%d-%d-%d.log",(1900+t->tm_year),(1+t->tm_mon),t->tm_mday);
	CLogThread::log_fd = open(str,O_WRONLY | O_CREAT | O_APPEND);
	shm1 = new CShaMemory(SHMKEY1,SHM1_SIZE);
	shm2 = new CShaMemory(SHMKEY2,SHM2_SIZE);
	sem = new CSem(SEMKEY,3);
	sem->SetVal(0,1);
	sem->SetVal(1,1);
	sem->SetVal(2,0);
}

void init_db()
{
	int size;
	PackHead_t head;
	memset(&head,0,sizeof(head));
	char sql[256] = {0};
	list<VideoChannel_t> channel;
	strcpy(sql,"select * from Tbl_video_channel");
	(DbSingles::GetSingle())->GetData(sql,List_CallBack,(void *)&channel);
	size = channel.size();
	channel_buf = new char[sizeof(VideoChannel_t)*size+sizeof(PackHead_t)];
	memset(channel_buf,0,sizeof(VideoChannel_t)*size+sizeof(PackHead_t));
	list<VideoChannel_t>::iterator it1 = channel.begin();
	for (;it1 != channel.end();it1++)
	{
		memcpy(channel_buf+sizeof(head)+sizeof(VideoChannel_t)*head.counts,&(*it1),sizeof(VideoChannel_t));
		head.counts++;
	}
	head.pack_size = head.counts*sizeof(VideoChannel_t);
	head.func_num = 2001;
	memcpy(channel_buf,&head,sizeof(head));
	
	memset(&head,0,sizeof(head));
	list<VideoType_t> type;
	strcpy(sql,"select * from Tbl_video_type");
	(DbSingles::GetSingle())->GetData(sql,List_CallBack,(void *)&type);
	size = type.size();
	type_buf = new char[sizeof(VideoType_t)*size+sizeof(PackHead_t)];
	memset(type_buf,0,sizeof(VideoType_t)*size+sizeof(PackHead_t));
	list<VideoType_t>::iterator it2 = type.begin();
	for (;it2 != type.end();it2++)
	{
		memcpy(type_buf+sizeof(head)+sizeof(VideoType_t)*head.counts,&(*it2),sizeof(VideoType_t));
		head.counts++;
	}
	head.pack_size = head.counts*sizeof(VideoType_t);
	head.func_num = 2002;
	memcpy(type_buf,&head,sizeof(head));

	memset(&head,0,sizeof(head));
	list<VideoArea_t> area;
	strcpy(sql,"select * from Tbl_video_area");
	(DbSingles::GetSingle())->GetData(sql,List_CallBack,(void *)&area);
	size = area.size();
	area_buf = new char[sizeof(VideoArea_t)*size+sizeof(PackHead_t)];
	memset(area_buf,0,sizeof(VideoArea_t)*size+sizeof(PackHead_t));
	list<VideoArea_t>::iterator it3 = area.begin();
	for (;it3 != area.end();it3++)
	{
		memcpy(area_buf+sizeof(head)+sizeof(VideoArea_t)*head.counts,&(*it3),sizeof(VideoArea_t));
		head.counts++;
	}
	head.pack_size = head.counts*sizeof(VideoArea_t);
	head.func_num = 2003;
	memcpy(area_buf,&head,sizeof(head));	

	memset(&head,0,sizeof(head));
	msg = new list<VideoList_t>;
	strcpy(sql,"select * from Tbl_video_message");
	(DbSingles::GetSingle())->GetData(sql, wyfl,(void *)msg);
	size = msg->size();
	msg_buf = new char[sizeof(VideoList_t)*size+sizeof(PackHead_t)];
	memset(msg_buf,0,sizeof(VideoList_t)*size+sizeof(PackHead_t));
	list<VideoList_t>::iterator it4 = msg->begin();
	for (;it4 != msg->end();it4++)
	{
		memcpy(msg_buf+sizeof(head)+sizeof(VideoList_t)*head.counts,&(*it4),sizeof(VideoList_t));
		head.counts++;
	}
	head.pack_size = head.counts*sizeof(VideoList_t);
	head.func_num = 2004;
	memcpy(msg_buf,&head,sizeof(head));	


	login = new list<Login_t>;
	strcpy(sql,"select * from Tbl_user");
	(DbSingles::GetSingle())->GetData(sql,Login_CallBack,(void *)login);

	Wjmm = new list<Forget_t>;
	strcpy(sql, "select * from Tbl_user");
	(DbSingles::GetSingle())->GetData(sql, wj_CallBack, (void*)Wjmm);

	pant = new list<Pant_t>;

}

int put_num(int counts)
{
	struct termios old_ter;
	struct termios new_ter;
	tcgetattr(0,&old_ter);
	new_ter=old_ter;
	new_ter.c_lflag &= ~(ICANON|ECHO);
	new_ter.c_cc[VMIN] = 1;
	new_ter.c_cc[VTIME] = 0;
	tcsetattr(0,TCSANOW,&new_ter);

	char num[10] = {0};
	char t;
	int i = 0;
	int valu = 0;
	t = getchar();
	while(1)
	{
		if ( i< counts && t>='0' && t<='9')
		{
			if (i != 0 || t != '0')
			{
				num[i] = t;
				i++;
				printf("%c",t);
			}
		
		}
		else if ( t== 10 && i > 0)
		{
			break;
		}
		else if (i > 0 && t == 127)
		{
			printf("\b \b");
			i--;
		}
		t = getchar();
	}
	num[i] = '\0';
	valu = atoi(num);
	tcsetattr(0,TCSANOW,&old_ter);
	printf("\n");
	return valu;
}