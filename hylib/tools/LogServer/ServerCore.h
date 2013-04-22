#ifndef _SERVER_CORE_H_
#define _SERVER_CORE_H_
////////////////////////////////////////////////////////////////////////////////

#pragma once
// Includes go here:
#include <hash_set>
#include "../../include/buffer.h"

struct ServerConfig {
	std::string	sAddr;
	WORD		wPort;
};

class CServerCore
{
public:
	CServerCore(void);
	~CServerCore(void);

	BOOL Init(const ServerConfig &cfg);
	BOOL Start();
	void Stop();

	void Broadcast(void *pData, unsigned nLen);

	void PostCtrlMsg2WT(unsigned nID, void *pData=NULL, unsigned nDataLen=0);

private:
	unsigned _ThrdWork();
	void _CB_Listen(struct evconnlistener *l, evutil_socket_t s,
		struct sockaddr *addr, int socklen);
	void _CB_Conn_Write(struct bufferevent *be);
	void _CB_Conn_Read(struct bufferevent *be);
	void _CB_Conn_Event(struct bufferevent *be, short events);
	
	void _CB_Ctrl_Event(evutil_socket_t s, short events);

private:
	typedef std::hash_set<bufferevent *> ConnectSet;
	typedef ConnectSet::iterator ConnectIter;

	HANDLE						_hThrdWork;
	ServerConfig				_cfg;

	ConnectSet					_setConnects;
	CCriticalSection			_lock;
	hy::cyc_buf<>				_bufWT;

	struct event_base			*_evbase;
	struct evconnlistener		*_evListener;

	evutil_socket_t				_sMain;
	evutil_socket_t				_sWork;
	event						*_evCtrl;

private:
	static unsigned __stdcall WorkThread(void *param);
	static void __cdecl ListenCB(struct evconnlistener *l, evutil_socket_t s,
		struct sockaddr *addr, int socklen, void *param);
	static void Conn_WriteCB(struct bufferevent *be, void *param);
	static void Conn_ReadCB(struct bufferevent *be, void *param);
	static void Conn_EventCB(struct bufferevent *be, short events, void *param);

	static void Ctrl_EventCB(evutil_socket_t s, short events, void *param);
};

#endif // _SERVER_CORE_H_
