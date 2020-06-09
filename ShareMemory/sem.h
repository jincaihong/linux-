#ifndef _SEM_H_
#define _SEM_H_

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <unistd.h>

union semun
{
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO	*/		
};


class CSem
{
public:
	CSem(key_t key,int semnum);
	~CSem();
	int SetVal(int semnum,int val);
	int GetVal(int semnum);
	int SetAll(int semnum,unsigned short *arr);
	int GetAll(int semnum,unsigned short *arr);
	int Sem_P(unsigned short semnum);
	int Sem_V(unsigned short semnum);
	int GetSemid();
private:
	int semid;
};

#endif