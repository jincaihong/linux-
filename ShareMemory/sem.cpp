#include "../ShareMemory/sem.h"


CSem::CSem(key_t key,int semnum)
{
	int ret = semget(key,0,0);
	if (ret > 0)
	{
		semctl(ret,0,IPC_RMID);//销毁成功返回值为0
	}
	this->semid = semget(key,semnum,IPC_CREAT | 0766);	
}

CSem::~CSem()
{
	semctl(this->semid,0,IPC_RMID);
}

int CSem::SetVal(int semnum,int val)
{
	union semun array;
	array.val = val;
	int ret = semctl(semid,semnum,SETVAL,array);
	if (ret < 0)
	{
		perror("sem setval error!");
	}
	return ret;
}

int CSem::GetVal(int semnum)
{
	int semval = semctl(semid,semnum,GETVAL);
	if (semval < 0)
	{
		perror("sem getval error!");
	}
	return semval;
}

int CSem::SetAll(int semnum,unsigned short *arr)
{
	union semun array;
	array.array = arr;
	int ret = semctl(semid,semnum,SETALL,array);
	if (ret < 0)
	{
		perror("sem setall error!");
	}
	return ret;
}

int CSem::GetAll(int semnum,unsigned short *arr)
{
	union semun array;
	array.array = arr;
	int ret = semctl(semid,semnum,GETALL,array);//成功返回0
	if (ret < 0)
	{
		perror("sem getall error!");
	}
	return ret;
}

int CSem::Sem_P(unsigned short semnum)
{
	struct sembuf buf = {semnum,-1,0};
	int ret = semop(semid,&buf,1);
	if (ret == -1)
	{
		perror("semop p error!");
	}
	return ret;
}

int CSem::Sem_V(unsigned short semnum)
{
	struct sembuf buf = {semnum,1,0};
	int ret = semop(semid,&buf,1);
	if (ret == -1)
	{
		perror("semop v error!");
	}
	return ret;
}

int CSem::GetSemid()
{
	return semid;
}
