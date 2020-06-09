#pragma once

#include "BaseSocket.h"

class CUdpSocket : public CBaseSocket
{
public:
	CUdpSocket(void);
	~CUdpSocket(void);
	void Create();
	int Read(char *buf, const int buf_len, CHostAddress &remote_addr);
	int Write(char *buf, const int buf_len, CHostAddress remote_addr);
};
