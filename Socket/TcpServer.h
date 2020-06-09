#pragma once

#include "BaseSocket.h"
#include "TcpSocket.h"

class CTcpServer : public CBaseSocket
{
public:
	CTcpServer(void);
	~CTcpServer(void);
	void Create();
	int Listen(int backlog);
	CTcpSocket Accept();
};
