// TcpClientSocketThrd.h: interface for the CTcpClientSocketThrd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TCPCLIENTSOCKETTHRD_H__31ED0561_3A82_4D11_8EC2_86BAFD0AC419__INCLUDED_)
#define AFX_TCPCLIENTSOCKETTHRD_H__31ED0561_3A82_4D11_8EC2_86BAFD0AC419__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CommunicationInterface.h"
#include <deque>
#include "buffer.h"
#include <algorithm>

class CTcpClientSocketThrd
	: public IClientSocket
	, public IMsgSpliter
{
protected:
	enum { BUF_SIZE = 40960 };
	enum { SELECT_TIME_VAL = 200 };

	// WN for (Work thread Notify)
	enum MWNotify {
		WN_NONE,
		WN_CONNECT,		// 连接
		WN_SEND,		// 发送
		WN_CLOSE,		// 关闭
		WN_CLOSE_THREAD,// 终止
	};
	// WM for (Main thread Notify)
	enum WMNotify {
		MN_NONE,
		MN_CONNECTED,		// 连接成功
		MN_CONNECT_FAILED,	// 连接失败
		MN_READ,			// 收到数据
		MN_CLOSE,			// 关闭套接字
	};

	typedef std::deque<unsigned> NotifyQueue;
	typedef NotifyQueue::iterator NotifyIter;
	typedef CLockGuard<CCriticalSection> LockGuard;

public:
	CTcpClientSocketThrd();
	virtual ~CTcpClientSocketThrd();

	/*implement*/ void SetSocketEventSink(IClientSocketEventSink *sink);

	/*implement*/ bool Connect(const char *addr, u_short port);

	/*implement*/ bool Send(const void *data, u_long len);

	/*implement*/ void Close();

	/*implement*/ u_long Parse(void *data, u_long len);

	bool Start();
	void PumpMessage();

	bool IsValid() const { return m_socket != INVALID_SOCKET; }

	const char *GetAddr() const;

protected:
	void _CloseSocket(bool bPassive/*=false*/);
	void _Clear();

protected:
	SOCKET		m_socket;
	u_long		m_state;
	cyc_buf		m_bufSend;
	CCriticalSection m_lockSend;
	fixed_buf	m_bufRecv;
	CCriticalSection m_lockRecv;
	fixed_buf	m_bufTmp;
	
	u_long		m_addr;
	u_short		m_port;
	
	IClientSocketEventSink *m_pSockSink;
	
//******************************************************************************
private:
	static unsigned __stdcall _ThreadWork(void *param);
	void _WT_OnConnect();
	void _WT_OnClose();
	bool _WT_OnIdle();
	bool _WT_OnWrite();
	bool _WT_OnRead();
	
	NotifyQueue m_queMain;
	CCriticalSection m_lockMNQ;

	HANDLE		m_thrdWork;
	NotifyQueue	m_queWork;
	CCriticalSection m_lockWNQ;
	HANDLE		m_evtWTSleep;
	fixed_buf	m_wt_bufSend;
	fixed_buf	m_wt_bufRecv;

	void PostNotify2WorkThread(unsigned code, bool enableDuplex=true) {
		LockGuard lg(m_lockWNQ);
		if (enableDuplex
			|| (m_queWork.end()==std::find(m_queWork.begin(), m_queWork.end(), code))) {
			m_queWork.push_back(code);
			::SetEvent(m_evtWTSleep);
		}
	}
	void PostNotify2MainThread(unsigned code, bool enableDuplex=true) {
		LockGuard lg(m_lockMNQ);
		if (enableDuplex
			|| (m_queMain.end()==std::find(m_queMain.begin(), m_queMain.end(), code))) {
			m_queMain.push_back(code);
		}
	}
	unsigned GetNotifyWorkThread() {
		if (m_queWork.empty()) { return WN_NONE; }
		LockGuard lg(m_lockWNQ);
		unsigned code = m_queWork.front();
		m_queWork.pop_front();
		return code;
	}
	unsigned GetNotifyMainThread() {
		if (m_queMain.empty()) { return MN_NONE; }
		LockGuard lg(m_lockMNQ);
		unsigned code = m_queMain.front();
		m_queMain.pop_front();
		return code;
	}
	bool ExistWorkThreadNotify() const { return m_queWork.empty(); }
	bool ExitMainThreadNotify() const { return m_queMain.empty(); }
};

#endif // !defined(AFX_TCPCLIENTSOCKETTHRD_H__31ED0561_3A82_4D11_8EC2_86BAFD0AC419__INCLUDED_)
