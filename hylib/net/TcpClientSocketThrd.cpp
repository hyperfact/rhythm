// TcpClientSocketThrd.cpp: implementation of the CTcpClientSocketThrd class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TcpClientSocketThrd.h"
#include <process.h>
#include <ASSERT.H>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTcpClientSocketThrd::CTcpClientSocketThrd()
	: m_bufSend(BUF_SIZE)
	, m_bufRecv(BUF_SIZE)
	, m_wt_bufSend(BUF_SIZE)
	, m_wt_bufRecv(BUF_SIZE)
	, m_bufTmp(MAX_SEND_SIZE)
{
	m_socket = INVALID_SOCKET;
	m_state = SS_NONE;
	m_addr = 0UL;
	m_port = 0;
	m_pSockSink = NULL;

	m_thrdWork = NULL;

	m_evtWTSleep = NULL;
}

CTcpClientSocketThrd::~CTcpClientSocketThrd()
{
	if (NULL != m_thrdWork) {
		PostNotify2WorkThread(WN_CLOSE_THREAD, false);
		if (WAIT_TIMEOUT == ::WaitForSingleObject(m_thrdWork, 2000)) {
			::TerminateThread(m_thrdWork, -1);
		}
		::CloseHandle(m_thrdWork);
	}
	if (NULL != m_evtWTSleep) {
		::CloseHandle(m_evtWTSleep);
	}
}

void CTcpClientSocketThrd::SetSocketEventSink( IClientSocketEventSink *sink )
{
	assert(sink);

	m_pSockSink = sink;
}

bool CTcpClientSocketThrd::Connect( const char *addr, u_short port )
{
	assert(!IsValid());
	assert(addr);
	
	if (m_state != SS_NONE) { return true; }
	
	m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket) { return false; }
	
	// 设置发包不延迟
	int bNodelay = 1;
	::setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY,
		(char *)&bNodelay, sizeof(bNodelay));

	m_addr = ::inet_addr(addr);
	m_port = ::htons(port);

	PostNotify2WorkThread(WN_CONNECT, false);

	return true;
}

bool CTcpClientSocketThrd::Send( const void *data, u_long len )
{
	assert(IsValid());
	
	if (m_bufSend.avail() < len) {
		return false;
	}

	do {	
		LockGuard lg(m_lockSend);
		m_bufSend.write(data, len);
	} while (0);

	PostNotify2WorkThread(WN_SEND);
	return true;
}

void CTcpClientSocketThrd::Close()
{
	assert(IsValid());
	
	_CloseSocket(false);
}

u_long CTcpClientSocketThrd::Parse( void *data, u_long len )
{
	if (len < sizeof(u_long)) {
		return 0UL;
	}
	
	return *((u_long *)data);
}

bool CTcpClientSocketThrd::Start()
{
	unsigned tid = 0;
	m_thrdWork = (HANDLE)_beginthreadex(NULL, 0, &CTcpClientSocketThrd::_ThreadWork,
		this, 0, &tid);
	m_evtWTSleep = ::CreateEvent(NULL, FALSE, TRUE, NULL);
	return m_thrdWork != NULL;
}

void CTcpClientSocketThrd::_CloseSocket(bool bPassive/*=false*/)
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

void CTcpClientSocketThrd::_Clear()
{
	m_addr = 0;
	m_port = 0;
	m_bufRecv.clear();
	m_bufSend.clear();
	m_bufTmp.clear();
	
	m_wt_bufSend.clear();
	m_wt_bufRecv.clear();
}

void CTcpClientSocketThrd::PumpMessage()
{
	switch (GetNotifyMainThread())
	{
	case MN_CONNECTED:
		m_state = SS_CONNECTED;
		if (m_pSockSink) { m_pSockSink->OnConnect(this, true); }
		PostNotify2WorkThread(WN_NONE);	// 激活工作线程
		break;
	case MN_CONNECT_FAILED:
		m_state = SS_DISCONNECTED;
		if (m_pSockSink) { m_pSockSink->OnConnect(this, false); }
		break;
	case MN_READ:
		{
			assert(IsValid());

			LockGuard lg(m_lockRecv);

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
		break;
	default:
		break;
	}
}

//******************************************************************************
unsigned __stdcall CTcpClientSocketThrd::_ThreadWork( void *param )
{
	CTcpClientSocketThrd *pThis = (CTcpClientSocketThrd *)param;
	unsigned notify = WN_NONE;
	while ((notify=pThis->GetNotifyWorkThread()) != WN_CLOSE_THREAD)
	{
		switch (notify)
		{
		case WN_CONNECT:
			pThis->_WT_OnConnect();
			break;
		case WN_CLOSE:
			pThis->_WT_OnClose();
			break;
		case WN_SEND:
			pThis->_WT_OnWrite();
			break;
		case WN_NONE:
		default:
			if (!pThis->_WT_OnIdle()) {
				::WaitForSingleObject(pThis->m_evtWTSleep, INFINITE);
			}
			break;
		}
	}

	_endthreadex(0);
	return 0;
}

void CTcpClientSocketThrd::_WT_OnConnect()
{
	sockaddr_in sai = {0};
	sai.sin_family = AF_INET;
	sai.sin_port = m_port;
	sai.sin_addr.s_addr = m_addr;
	if (SOCKET_ERROR != ::connect(m_socket, (sockaddr *)&sai, sizeof(sai))) {
		PostNotify2MainThread(MN_CONNECTED);
	} else {
		PostNotify2MainThread(MN_CONNECT_FAILED);
	}
}

void CTcpClientSocketThrd::_WT_OnClose()
{
	
}

bool CTcpClientSocketThrd::_WT_OnIdle()
{
	if (BT_IS_CLEAR(m_state, SS_CONNECTED)) {
		return false;
	}

	fd_set fdsRead;
	FD_ZERO(&fdsRead);
	
	FD_SET(m_socket, &fdsRead);
	
	timeval tv = {0};
	tv.tv_sec = 0;
	tv.tv_usec = SELECT_TIME_VAL * 1000;
	if (0 < ::select(1, &fdsRead, NULL, NULL, &tv)) {
		if (FD_ISSET(m_socket, &fdsRead)) {
			_WT_OnRead();
		}
	}

	return true;
}

bool CTcpClientSocketThrd::_WT_OnWrite()
{
	if (BT_IS_CLEAR(m_state, SS_CONNECTED)) {
		return false;
	}
	if (m_bufSend.empty()) {
		return false;
	}

	while (!m_bufSend.empty() || !m_wt_bufSend.empty())
	{
		if (!m_bufSend.empty()) {
			LockGuard lg(m_lockSend);

			size_t sz = m_bufSend.size();
			char *pBuf = (char *)m_wt_bufSend.wlock(sz);
			sz = m_bufSend.read(pBuf, sz);
			m_wt_bufSend.wunlock(pBuf, sz);
		}

		size_t len = m_wt_bufSend.size();
		const char *pSendBuf = (const char *)m_wt_bufSend.rlock(len);
		int sret = ::send(m_socket, pSendBuf, len, 0);
		if (SOCKET_ERROR == sret) {
			m_wt_bufSend.runlock(pSendBuf, len);
			PostNotify2MainThread(MN_CLOSE, false);
			return false;
		} else {
			m_wt_bufSend.runlock(pSendBuf, sret);
		}
	}

	return true;
}

bool CTcpClientSocketThrd::_WT_OnRead()
{
	if (BT_IS_CLEAR(m_state, SS_CONNECTED)) {
		return false;
	}

	size_t nBufLen = m_wt_bufRecv.avail();
	char *pBuf = (char *)m_wt_bufRecv.wlock(nBufLen);
	int ret = ::recv(m_socket, pBuf, nBufLen, 0);
	if (ret > 0) {
		m_wt_bufRecv.wunlock(pBuf, ret);

		do {
			LockGuard lg(m_lockRecv);

			size_t sz = m_wt_bufRecv.size();
			const char *pWTRecv = (const char *)m_wt_bufRecv.rlock(sz);
			sz = m_bufRecv.write(pWTRecv, sz);
			m_wt_bufRecv.runlock(pWTRecv, sz);
		} while (0);

		PostNotify2MainThread(MN_READ);
	} else {
		m_wt_bufRecv.wunlock(pBuf, nBufLen);
		PostNotify2MainThread(MN_CLOSE);
	}
	return true;
}

const char * CTcpClientSocketThrd::GetAddr() const
{
	static char data[256] = {0};
	sockaddr_in sai = {0};
	int lsai = sizeof(sai);
	getsockname(m_socket, (sockaddr *)&sai, &lsai);
	sprintf(data, "%s:%d", inet_ntoa(sai.sin_addr), sai.sin_port);
	return data;
}
