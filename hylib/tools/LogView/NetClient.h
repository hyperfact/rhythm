#ifndef _NET_CLIENT_H_
#define _NET_CLIENT_H_
////////////////////////////////////////////////////////////////////////////////
#pragma once
// Includes go here:
#include <string>

class CNetClient
{
public:
	CNetClient(void);
	~CNetClient(void);

	void SetREOp(IRichEditOp *pIREOp) {
		ASSERT(pIREOp);
		_pIREOp = pIREOp;
	}
	BOOL Init(LPCTSTR sAddr, WORD wPort);
	BOOL Start();
	void Stop();


private:
	unsigned _ThrdWork();
	void _CB_Conn_Write(struct bufferevent *be);
	void _CB_Conn_Read(struct bufferevent *be);
	void _CB_Conn_Event(struct bufferevent *be, short events);
	void _CB_OnTimer(evutil_socket_t s, short events);

	void Connect();

private:
	HANDLE		_hThrdWork;

	std::string					_sAddr;
	WORD						_wPort;
	struct event_base			*_evbase;
	evutil_socket_t				_sConn;
	struct bufferevent			*_beConn;

	IRichEditOp					*_pIREOp;

private:
	static unsigned __stdcall WorkThread(void *param);

	static void Conn_WriteCB(struct bufferevent *be, void *param);
	static void Conn_ReadCB(struct bufferevent *be, void *param);
	static void Conn_EventCB(struct bufferevent *be, short events, void *param);
	static void TimerCB(evutil_socket_t s, short events, void *param);
};

#endif // _NET_CLIENT_H_
