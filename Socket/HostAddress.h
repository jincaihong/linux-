#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

class CHostAddress
{
public:
	CHostAddress(void);
	~CHostAddress(void);
	CHostAddress(const char *ip, unsigned short port);	
	void  SetIp(const char *ip);
	const char* GetIp() const;
	void  SetPort(unsigned short port);
	unsigned short GetPort() const;
private:
	char m_ip[16];
	unsigned short m_port;
};
