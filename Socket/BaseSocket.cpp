#include "BaseSocket.h"

CBaseSocket::CBaseSocket(void)
:m_fd(0),
m_stream(NULL)
{
}

CBaseSocket::~CBaseSocket(void)
{
}

void CBaseSocket::Create(int sin_farmly, int flag, int protocl)
{
	m_fd = socket(sin_farmly, flag, protocl);
	int on=1;  
	if((setsockopt(m_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))<0)  
	{  
		perror("setsockopt failed");  
		exit(-1);  
	} 
	if (m_stream != NULL)
	{
		delete m_stream;
		m_stream = NULL;
	}

	m_stream = new CBaseStream(m_fd);
}

void CBaseSocket::SetSocket(int fd)
{
	m_fd = fd;

	if (m_stream != NULL)
	{
		delete m_stream;
		m_stream = NULL;
	}

	m_stream = new CBaseStream(m_fd);
}
int CBaseSocket::GetSocket() const
{
	return m_fd;
}
CHostAddress CBaseSocket::GetAddress()
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	socklen_t addr_len = sizeof(addr);

	getsockname(m_fd, (struct sockaddr *)&addr, &addr_len);
	
	CHostAddress hostaddr;
	hostaddr.SetIp(inet_ntoa(addr.sin_addr));
	hostaddr.SetPort(ntohs(addr.sin_port));
	return hostaddr;

}

int CBaseSocket::Bind(CHostAddress hostaddr)
{
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(hostaddr.GetIp());
	addr.sin_port = htons(hostaddr.GetPort());

	return bind(m_fd, (struct sockaddr *)&addr, sizeof(addr));

}

int CBaseSocket::Close()
{
	return close(m_fd);
}