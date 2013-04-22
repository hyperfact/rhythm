// TcpClientSocketNB.h: interface for the CTcpClientSocketNB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCLIENTSOCKETNB_H__4AC7BA40_DF26_4DA0_8EEF_CCE0D80AB0C9__INCLUDED_)
#define AFX_TCPCLIENTSOCKETNB_H__4AC7BA40_DF26_4DA0_8EEF_CCE0D80AB0C9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommunicationInterface.h"
#include <assert.h>
#include "buffer.h"

////////////////////////////////////////////////////////////////////////////////
// class CTcpClientSocketNB:
class CTcpClientSocketNB
	: public IClientSocket
	, public IMsgSpliter
{
//******************************************************************************
protected:
	enum { BUF_SIZE = 40960 };

//******************************************************************************
public:
	CTcpClientSocketNB();
	virtual ~CTcpClientSocketNB();

//******************************************************************************
public:
	/*implement*/ void SetSocketEventSink(IClientSocketEventSink *pSink);
	/*implement*/ bool Connect(const char *addr, u_short port);
	
	/*implement*/ bool Send(const void *data, u_long len);
	
	/*implement*/ void Close();

	/*implement*/ u_long Parse(void *data, u_long len);

	void Polling();

	int GetError() const;

	bool IsValid() const { return m_socket != INVALID_SOCKET; }

protected:
	void _Clear();

	void _CloseSocket(bool bPassive=false);

	void _OnConnected(bool bSuccess);
	void _OnRead(u_long nRead);
	void _OnWrite();
	void _OnPeerClose();

//******************************************************************************
protected:
	SOCKET		m_socket;
	u_long		m_state;
	cyc_buf		m_bufSend;
	fixed_buf	m_bufRecv;
	fixed_buf	m_bufTmp;

	u_long		m_addr;
	u_short		m_port;

	IClientSocketEventSink *m_pSockSink;
};

#endif // !defined(AFX_TCPCLIENTSOCKETNB_H__4AC7BA40_DF26_4DA0_8EEF_CCE0D80AB0C9__INCLUDED_)
