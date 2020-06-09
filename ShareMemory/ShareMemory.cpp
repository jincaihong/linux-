#include <string.h>
#include "../task.h"


CShaMemory::CShaMemory(key_t key,int size)
{
	int ret = shmget(key,0,0);
	if (ret > 0)
	{
		shmctl(ret,IPC_RMID,NULL);
	}
	this->shm_id = shmget(key,size,IPC_CREAT | 0766);
	this->addstr = shmat(shm_id,NULL,0);//关联共享内存
	this->shm_size = size;
	memset((char *)addstr,0,size);
}

CShaMemory::~CShaMemory()
{
	shmdt(addstr);
	shmctl(shm_id,IPC_RMID,NULL);
}

int CShaMemory::get_head()
{
	int i;
	memcpy(&i,(char *)addstr,sizeof(int));
	return i;
}

void CShaMemory::write_block(void *buf)
{
	int i;
	memcpy(&i,(char *)addstr,sizeof(int));
	memcpy((char *)addstr+sizeof(int)+BLOCKSIZE*i,(char *)buf,BLOCKSIZE);
	i++;
	memcpy((char *)addstr,&i,sizeof(int));
}


int CShaMemory::read_block(char *buf)//返回块的数量
{
	int i;
	memcpy(&i,(char *)addstr,sizeof(int));
	memcpy(buf,(char *)addstr+sizeof(int),i*BLOCKSIZE);
	memset((char *)addstr,0,SHM1_SIZE);
	return i;
}

void CShaMemory::write_size(char *buf,int buf_size)
{
	int cur_size;	
	memcpy(&cur_size,(char *)addstr,sizeof(int));
	memcpy((char *)addstr+sizeof(int)+cur_size,buf,buf_size);
	cur_size = buf_size + cur_size;
	memcpy((char *)addstr,&cur_size,sizeof(int));
}


int CShaMemory::read_size(char *buf)//返回字节长度
{
	int cur_size;	
	memcpy(&cur_size,(char *)addstr,sizeof(int));
	memcpy(buf,(char *)addstr+sizeof(int),cur_size);
	memset((char *)addstr,0,SHM2_SIZE);
	return cur_size;
}