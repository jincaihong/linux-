#pragma once

#include "HostAddress.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

class CBaseStream
{
public:
	CBaseStream(int fd);
	~CBaseStream(void);
	int Read(char *buf, const int buf_len);
	int Read(char *buf, const int buf_len, CHostAddress &remote_addr);
	int Write(char *buf, const int buf_len);
	int Write(char *buf, const int buf_len, CHostAddress remote_addr);
private:
	int m_fd;
};
