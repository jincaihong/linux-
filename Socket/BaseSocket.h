#pragma once

#include "BaseStream.h"

class CBaseSocket
{
public:
	CBaseSocket(void);
	~CBaseSocket(void);
	int GetSocket() const;
	void SetSocket(int fd);
	CHostAddress GetAddress();
	int Bind(CHostAddress addr);
	int Close();
protected:
	void Create(int sin_farmly, int flag, int protocl);
	int m_fd;
	CBaseStream *m_stream;
};
