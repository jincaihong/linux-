#include "TcpSocket.h"

CTcpSocket::CTcpSocket(void)
{
}

CTcpSocket::~CTcpSocket(void)
{
}

void CTcpSocket::Create()
{
	CBaseSocket::Create(AF_INET, SOCK_STREAM, 0);
}

CHostAddress CTcpSocket::GetRemoteAddr() const
{
	return m_remote_addr;
}

void CTcpSocket::SetRemoteAddr(const CHostAddress remote_addr)
{
	m_remote_addr.SetIp(remote_addr.GetIp());
	m_remote_addr.SetPort(remote_addr.GetPort());
}

int CTcpSocket::Connect(CHostAddress hostaddr)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(hostaddr.GetIp());
	addr.sin_port = htons(hostaddr.GetPort());

	int ret = connect(m_fd, (struct sockaddr *)&addr, sizeof(addr));

	if(ret == 0)
		SetRemoteAddr(hostaddr);

	return ret;
}

int CTcpSocket::Read(char *buf, int buf_len)
{
	return m_stream->Read(buf, buf_len);
}

int CTcpSocket::Write(char *buf, int buf_len)
{
	return m_stream->Write(buf, buf_len);
}
