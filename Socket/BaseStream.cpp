#include "BaseStream.h"


CBaseStream::CBaseStream(int fd)
:m_fd(fd)
{
}

CBaseStream::~CBaseStream(void)
{
}

int CBaseStream::Read(char *buf, const int buf_len)
{
	return recv(m_fd, buf, buf_len, 0);
}

int CBaseStream::Read(char *buf, const int buf_len, CHostAddress &remote_addr)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	socklen_t addr_len = sizeof(addr);

	int nread = recvfrom(m_fd, buf, buf_len, 0, (struct sockaddr *)&addr, &addr_len);
	remote_addr.SetIp(inet_ntoa(addr.sin_addr));
	remote_addr.SetPort(ntohs(addr.sin_port));

	return nread;

}

int CBaseStream::Write(char *buf, const int buf_len)
{
	return send(m_fd, buf, buf_len, 0);
}

int CBaseStream::Write(char *buf, const int buf_len, CHostAddress remote_addr)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(remote_addr.GetIp());
	addr.sin_port = htons(remote_addr.GetPort());

	return sendto(m_fd, buf, buf_len, 0, (struct sockaddr *)&addr, sizeof(addr));

}