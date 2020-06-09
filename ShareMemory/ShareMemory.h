#ifndef _SHAREMEMORY_H_
#define _SHAREMEMORY_H_

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

class CShaMemory
{
public:
	CShaMemory(key_t key,int size);
	~CShaMemory();
	int get_head();
	void write_block(void *buf);
	int read_block(char *buf);
	void write_size(char *buf,int buf_size);
	int read_size(char *buf);
	void *addstr;
private:
	int shm_id;
	int shm_size;
};

#endif