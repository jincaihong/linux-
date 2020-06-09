#include "TcpServer.h"

CTcpServer::CTcpServer(void)
{
}

CTcpServer::~CTcpServer(void)
{
}

void CTcpServer::Create()
{
	CBaseSocket::Create(AF_INET, SOCK_STREAM, 0);
}

int CTcpServer::Listen(int backlog)
{
	return listen(m_fd, backlog);
}

CTcpSocket CTcpServer::Accept()
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	socklen_t addr_len = sizeof(addr);

	int fd = accept(m_fd, (struct sockaddr *)&addr, &addr_len);

	

	CTcpSocket tcpclient;
	tcpclient.SetSocket(fd);

	/*printf("m_fd ip %s, port %d\n", this->GetAddress().GetIp(), this->GetAddress().GetPort());
	printf("fd ip %s, port %d\n", tcpclient.GetAddress().GetIp(), tcpclient.GetAddress().GetPort());*/
	

	CHostAddress hostaddr;
	hostaddr.SetIp(inet_ntoa(addr.sin_addr));
	hostaddr.SetPort(ntohs(addr.sin_port));
	tcpclient.SetRemoteAddr(hostaddr);
	return tcpclient;

}