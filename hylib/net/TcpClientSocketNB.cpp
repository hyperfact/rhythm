// ClientSocketNonblocking.cpp: implementation of the CTcpClientSocketNB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TcpClientSocketNB.h"
#include <assert.h>

bool initCommEnv()
{
#ifdef _WIN32
	WSAData wsaData = {0};
	int nErr = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
	if ( nErr != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return false;
	}
	
	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 2 ) {
		return false;
	}

	return true;
#endif // _WIN32
}

void uninitCommEnv()
{
#ifdef _WIN32
	::WSACleanup();
#endif // _WIN32
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTcpClientSocketNB::CTcpClientSocketNB()
	: m_bufSend(BUF_SIZE)
	, m_bufRecv(BUF_SIZE)
	, m_bufTmp(MAX_SEND_SIZE)
{
	m_socket = INVALID_SOCKET;
	m_state = SS_NONE;
	
	m_addr = 0UL;
	m_port = 0;

	m_pSockSink = NULL;
}

CTcpClientSocketNB::~CTcpClientSocketNB()
{
	if ( IsValid()) {
		_CloseSocket(true);
	}
}

void CTcpClientSocketNB::SetSocketEventSink( IClientSocketEventSink *pSink )
{
	assert(pSink);

	m_pSockSink = pSink;
}

bool CTcpClientSocketNB::Connect( const char *addr, u_short port )
{
	assert(!IsValid());
	assert(addr);

	if (m_state != SS_NONE) { return true; }

	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket) { return false; }

	// 设置非阻塞
	u_long param = 0;
	::ioctlsocket(m_socket, FIONBIO, &param);

	// 设置发包不延迟
	int bNodelay = 1;
	::setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY,
		(char *)&bNodelay, sizeof(bNodelay));

	sockaddr_in sai = {0};
	sai.sin_family = AF_INET;
	sai.sin_port = ::htons(port);
	sai.sin_addr.s_addr = ::inet_addr(addr);
	if (SOCKET_ERROR == ::connect(m_socket, (sockaddr *)&sai, sizeof(sai))) {
		return false;
	}
	
	m_addr = sai.sin_addr.s_addr;
	m_port = sai.sin_port;
	m_state = SS_CONNECTING;

	return true;
}

bool CTcpClientSocketNB::Send( const void *data, u_long len )
{
	assert(IsValid());

	if (m_bufSend.avail() < len) {
		return false;
	}

	m_bufSend.write(data, len);
	return true;
}

void CTcpClientSocketNB::Close()
{
	assert(IsValid());

	_CloseSocket(false);
}

u_long CTcpClientSocketNB::Parse( void *data, u_long len )
{
	if (len < sizeof(u_long)) {
		return 0UL;
	}

	return *((u_long *)data);
}

void CTcpClientSocketNB::Polling()
{
	if (!IsValid()) {
		return;
	}

	fd_set fdsRead, fdsWrite, fdsExcept;
	FD_ZERO(&fdsRead);
	FD_ZERO(&fdsWrite);
	FD_ZERO(&fdsExcept);

	FD_SET(m_socket, &fdsRead);
	FD_SET(m_socket, &fdsWrite);
	FD_SET(m_socket, &fdsExcept);

	timeval tv = {0};
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	if (0 >= ::select(1, &fdsRead, &fdsWrite, &fdsExcept, &tv)) {
		return;
	}

	if (FD_ISSET(m_socket, &fdsRead)) {
		if (BT_SET(m_state, SS_CONNECTED))
		{
			u_long nRd = 0;
			if (SOCKET_ERROR != ::ioctlsocket(m_socket, FIONREAD, &nRd))
			{
				if (0 == nRd)
				{	// peer关闭套接字
					_OnPeerClose();
					return;
				}
				else
				{	// 有数据到达
					_OnRead(nRd);
				}
			}
		}
	}
	if (FD_ISSET(m_socket, &fdsWrite)) {
		if (BT_IS_SET(m_state, SS_CONNECTING))
		{	// 连接建立
			_OnConnected(true);
		}
		else if (BT_SET(m_state, SS_CONNECTED))
		{	// 可发送
			_OnWrite();
		}
	}
	if (FD_ISSET(m_socket, &fdsExcept)) {
		if (BT_IS_SET(m_state, SS_CONNECTING))
		{	// 连接失败
			_OnConnected(false);
		}
	}
}

void CTcpClientSocketNB::_OnConnected(bool bSuccess)
{
	m_state = bSuccess ? SS_CONNECTED : SS_DISCONNECTED;
	if (m_pSockSink) { m_pSockSink->OnConnect(this, bSuccess); }
}

void CTcpClientSocketNB::_OnRead(u_long nRead)
{
	assert(IsValid());

	size_t nBufLen = nRead;
	char *pBuf = (char *)m_bufRecv.wlock(nBufLen);
	int nRecv = ::recv(m_socket, pBuf, nBufLen, 0);
	m_bufRecv.wunlock(pBuf, nRecv);
	if (nRecv <= 0) {
		return; }

	size_t nMsgLen = 0;
	while ((nMsgLen=Parse(m_bufRecv, sizeof(u_long))) <= m_bufRecv.size())
	{	// A new segment
		m_bufTmp.clear();
		char *pBuf = (char *)m_bufTmp.wlock(nMsgLen);
		m_bufRecv.read(pBuf, nMsgLen);
		if (m_pSockSink) {
			m_pSockSink->OnRead(this, pBuf, nMsgLen); }
		m_bufTmp.wunlock(pBuf, nMsgLen);
		m_bufTmp.clear();
	}
}

void CTcpClientSocketNB::_OnWrite()
{
	assert(IsValid());

	if (0 != m_bufSend.size()) {
		size_t nLockSize = MAX_SEND_SIZE;
		const char *pBuf = (const char *)m_bufSend.rlock(nLockSize);
		int nSend = ::send(m_socket, pBuf, nLockSize, 0);
		if (nSend >= 0) { nLockSize = nSend; }
		m_bufSend.runlock(pBuf, nLockSize);
	}
}

void CTcpClientSocketNB::_OnPeerClose()
{
	assert(IsValid());

	_CloseSocket(true);
}

int CTcpClientSocketNB::GetError() const
{
#ifdef _WIN32
	return WSAGetLastError();
#endif // _WIN32
}

void CTcpClientSocketNB::_Clear()
{
	m_addr = 0;
	m_port = 0;
	m_bufRecv.clear();
	m_bufSend.clear();
	m_bufTmp.clear();
}

void CTcpClientSocketNB::_CloseSocket(bool bPassive/*=false*/)
{
	assert(IsValid());

	::closesocket(m_socket);
	m_socket = INVALID_SOCKET;

	m_state = SS_DISCONNECTED;

	_Clear();

	if (m_pSockSink) {
		m_pSockSink->OnClose(this, bPassive);
	}
}
