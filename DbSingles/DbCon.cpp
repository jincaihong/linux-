#include <stdlib.h>
#include <string.h>
#include "../DbSingles/DbCon.h"
#include <list>
#include<iostream>
using namespace std;


DbSingles * DbSingles::pS = NULL;
DbSingles *DbSingles::GetSingle()
{
	if (pS == NULL)
	{
		pS = new DbSingles;
		
	}
	return pS;
}

DbSingles::DbSingles()
{
	int errcode;
	errcode = sqlite3_open("/root/projects/yktext/Db/mydb.dat",&db);
	if (errcode != SQLITE_OK)
	{
		printf("打开数据库失败:%s\n",sqlite3_errmsg(db));
		exit(1);// 非正常退出
	}
}


DbSingles::~DbSingles()
{
	sqlite3_close(db);
}


int DbSingles::GetData(char *sql,sqlite3_callback pFun,void *pData)
{
	char *errmsg;
	return  sqlite3_exec(db,sql,pFun,pData,&errmsg);

}



void ReleaseDb()
{
	delete DbSingles::pS;
}



int List_CallBack(void *pData,int cols,char **colvalu,char **colname)
{
	char name[20] = {0};
	switch(cols)
	{
	case 1://获取登录用户ID
		{
			char *data = (char *)pData;
			strcpy(data,colvalu[0]);
			break;
		}
	case 2://视频频道
		{
			list<VideoChannel_t> *channel = (list<VideoChannel_t> *)pData;
			VideoChannel_t chan_data;
			chan_data.channel_id = atoi(colvalu[0]);
			strcpy(chan_data.channel_name,colvalu[1]);
			channel->push_back(chan_data);
			break;
		}
	case 3://视频分类
		{
			list<VideoType_t> *type = (list<VideoType_t> *)pData;
			VideoType_t type_data;
			type_data.type_id = atoi(colvalu[0]);
			type_data.channel_id = atoi(colvalu[1]);
			strcpy(type_data.type_name,colvalu[2]);
			type->push_back(type_data);
			break;
		}
	case 4://视频地区
		{
			list<VideoArea_t> *area = (list<VideoArea_t> *)pData;
			VideoArea_t area_data;
			area_data.area_id = atoi(colvalu[0]);
			area_data.channel_id = atoi(colvalu[1]);
			strcpy(area_data.area_name,colvalu[2]);
			area->push_back(area_data);
			break;
		}
	//case 6://视频列表
	//	{
	//		list<VideoList_t> *video_list = (list<VideoList_t> *)pData;
	//		VideoList_t data;
	//		data.video_id = atoi(colvalu[0]);
	//		data.channel_id = atoi(colvalu[1]);
	//		data.area_id = atoi(colvalu[2]);
	//		data.type_id = atoi(colvalu[3]);
	//		strcpy(data.video_name,colvalu[4]);
	//		data.play_times = atoi(colvalu[5]);
	//		strcpy(data.play_locad, colvalu[6]);
	//		cout <<"999"<< endl;
	//		video_list->push_back(data);
	//		break;
	//	}
	//case 7://视频筛选
	//	{
	//		list<Videosort_t>* video_list = (list<Videosort_t>*)pData;
	//		Videosort_t data;
	//		data.sort = atoi(colvalu[0]);
	//		strcpy(data.sortname, colvalu[2]);
	//		video_list->push_back(data);
	//		break;
	//	}
	}
	return 0;
}


int Login_CallBack(void *pData,int cols,char **colvalu,char **colname)
{
	list<Login_t> *login = (list<Login_t> *)pData;
	Login_t data;
	data.user_id = atoi(colvalu[0]);
	strcpy(data.user_name,colvalu[1]);
	strcpy(data.passwd,colvalu[2]);
	login->push_back(data);
	return 0;
}
int wj_CallBack(void* pData, int cols, char** colvalu, char** colname)
{
	list<Forget_t>* wj_login = (list<Forget_t>*)pData;
	Forget_t data;
	strcpy(data.user_name, colvalu[1]);
	strcpy(data.passwd, colvalu[2]);
	strcpy(data.passwd2, colvalu[3]);
	wj_login->push_back(data);
	return 0;
}
int wyfl(void* pData, int cols, char** colvalu, char** colname)
{
	list<VideoList_t>* video_list = (list<VideoList_t>*)pData;
	VideoList_t data;
	data.video_id = atoi(colvalu[0]);
	data.channel_id = atoi(colvalu[1]);
	data.area_id = atoi(colvalu[2]);
	data.type_id = atoi(colvalu[3]);
	strcpy(data.video_name, colvalu[4]);
	data.play_times = atoi(colvalu[5]);
	strcpy(data.play_locad, colvalu[6]);
	video_list->push_back(data);
	return 0;
}

int sort_sql(void* pData, int cols, char** colvalu, char** colname)
{
	list<VideoList_t>* video_list = (list<VideoList_t>*)pData;
	VideoList_t data;
	data.video_id = atoi(colvalu[0]);
	data.channel_id = atoi(colvalu[1]);
	data.area_id = atoi(colvalu[2]);
	data.type_id = atoi(colvalu[3]);
	strcpy(data.video_name, colvalu[4]);
	data.play_times = atoi(colvalu[5]);
	strcpy(data.play_locad, colvalu[6]);
	video_list->push_back(data);
	return 0;
}
int His_CallBack(void *pData,int cols,char **colvalu,char **colname)
{
	list<PlayHistory_t> *his = (list<PlayHistory_t> *)pData;
	PlayHistory_t data;
	char name[20] = {0};
	char sql[256] = {0};
	data.video_id = atoi(colvalu[0]);
	data.video_seek = atoi(colvalu[1]);
	sprintf(sql,"select video_name from Tbl_video_message where video_id = %s",colvalu[0]);
	(DbSingles::GetSingle())->GetData(sql,List_CallBack,(void *)name);
	strcpy(data.video_name,name);
	sprintf(sql,"select play_counts from Tbl_video_message where video_id = %s",colvalu[0]);
	(DbSingles::GetSingle())->GetData(sql,List_CallBack,(void *)name);
	data.play_times = atoi(name);
	his->push_back(data);
	return 0;
}