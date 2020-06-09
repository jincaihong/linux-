#pragma once
#include "BaseSocket.h"
#include "HostAddress.h"

class CTcpSocket :
	public CBaseSocket
{
public:
	CTcpSocket(void);
	~CTcpSocket(void);
	void Create();
	CHostAddress GetRemoteAddr() const;
	void SetRemoteAddr(const CHostAddress remote_addr);
	int Connect(CHostAddress addr);
	int Read(char *buf, int buf_len);
	int Write(char *buf, int buf_len);
protected:
	CHostAddress m_remote_addr;
};

