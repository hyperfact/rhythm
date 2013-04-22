#include "StdAfx.h"
#include "NetClient.h"

#include <event2/buffer.h>
#include <event2\bufferevent.h>
#include "../../include/buffer.h"

CNetClient::CNetClient(void)
	: _hThrdWork(NULL)
	, _evbase(NULL)
	, _sConn(-1)
	, _wPort(0)
	, _beConn(NULL)
	, _pIREOp(NULL)
{
}

CNetClient::~CNetClient(void)
{
}

BOOL CNetClient::Init( LPCTSTR sAddr, WORD wPort )
{
	ASSERT(sAddr);

	_sAddr = sAddr;
	_wPort = wPort;
	return TRUE;
}

BOOL CNetClient::Start()
{
	unsigned nTID = 0;
	_hThrdWork = (HANDLE)_beginthreadex(NULL, 0, WorkThread, this, 0, &nTID);

	return (NULL != _hThrdWork);
}

void CNetClient::Stop()
{

}

unsigned CNetClient::_ThrdWork()
{
	_evbase = event_base_new();
	if (NULL == _evbase) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return -1;
	}

	_sConn = ::socket(AF_INET, SOCK_STREAM, 0);
	_beConn = bufferevent_socket_new(_evbase, _sConn, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(_beConn, Conn_ReadCB, Conn_WriteCB, Conn_EventCB, this);
	bufferevent_enable(_beConn, EV_WRITE);
	bufferevent_enable(_beConn, EV_READ);

	Connect();

	event_base_dispatch(_evbase);

	bufferevent_free(_beConn), _beConn = NULL;
	event_base_free(_evbase), _evbase = NULL;

	printf("done\n");
	return 0;
}

void CNetClient::_CB_Conn_Write( struct bufferevent *be )
{
	
}

void CNetClient::_CB_Conn_Read( struct bufferevent *be )
{
	size_t nLen = evbuffer_get_length(bufferevent_get_input(be));
	static hy::fixed_buf<> buf(4096);
	buf.clear();
	void *pWr = buf.wlock(nLen);
	VERIFY(nLen==bufferevent_read(be, pWr, nLen));
	buf.wunlock(pWr, nLen);

	nLen = buf.size();
	const void *pRd = buf.rlock(nLen);
	LPCTSTR pStr = (LPCTSTR)pRd;
	while (pStr < ((LPCTSTR)(pRd)+nLen)) {
		if (*pStr) {
			_pIREOp->AppendText(pStr);
		}
		pStr += ::_tcslen(pStr)+1;
	}
	buf.runlock(pRd, nLen);
}

void CNetClient::_CB_Conn_Event( struct bufferevent *be, short events )
{
	if (events & BEV_EVENT_CONNECTED) {
		TRACE(_T("Connection has established!\n"));
	} else if ((events & BEV_EVENT_ERROR)
		|| (events & BEV_EVENT_EOF)) {
		TRACE(_T("Connection has failed!\n"));

		timeval tv = {0};
		tv.tv_sec = 1;
		event_base_once(_evbase, _sConn, EV_TIMEOUT, TimerCB, this, &tv);
	}
}

void CNetClient::_CB_OnTimer( evutil_socket_t s, short events )
{
	Connect();
}

void CNetClient::Connect()
{
	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	evutil_inet_pton(AF_INET, _sAddr.c_str(), &sin.sin_addr);
	sin.sin_port = htons(_wPort);

	bufferevent_socket_connect(_beConn, (sockaddr *)&sin, sizeof(sin));
}

unsigned __stdcall CNetClient::WorkThread( void *param )
{
	CNetClient *pThis = (CNetClient *)param;
	return pThis->_ThrdWork();
}

void CNetClient::Conn_WriteCB( struct bufferevent *be, void *param )
{
	CNetClient *pThis = (CNetClient *)param;
	return pThis->_CB_Conn_Write(be);
}

void CNetClient::Conn_ReadCB( struct bufferevent *be, void *param )
{
	CNetClient *pThis = (CNetClient *)param;
	return pThis->_CB_Conn_Read(be);
}

void CNetClient::Conn_EventCB( struct bufferevent *be, short events, void *param )
{
	CNetClient *pThis = (CNetClient *)param;
	return pThis->_CB_Conn_Event(be, events);
}

void CNetClient::TimerCB( evutil_socket_t s, short events, void *param )
{
	CNetClient *pThis = (CNetClient *)param;
	return pThis->_CB_OnTimer(s, events);
}
