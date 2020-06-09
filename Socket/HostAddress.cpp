#include "HostAddress.h"

CHostAddress::CHostAddress(void)
{
	strcpy(m_ip,"0.0.0.0");
	m_port = 0;
}

CHostAddress::~CHostAddress(void)
{

}

CHostAddress::CHostAddress(const char *ip, unsigned short port)
{
	strncpy(m_ip, ip, 16);
	m_port = port;
}

void CHostAddress::SetIp(const char *ip)
{
	strncpy(m_ip, ip, 16);
}

const char* CHostAddress::GetIp() const
{
	return m_ip;
}

void CHostAddress::SetPort(unsigned short port)
{
	m_port = port;
}

unsigned short CHostAddress::GetPort() const
{
	return m_port;
}
