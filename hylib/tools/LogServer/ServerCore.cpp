#include "StdAfx.h"
#include "ServerCore.h"
#include <WinSock2.h>

struct EventDataHead {
	unsigned	nLen;
	unsigned	nID;
};

enum {
	EID_UNKNOWN			= 0,
	EID_STOP_SERVER		= 1,
	EID_BROADCAST_DATA	= 2,
};

CServerCore::CServerCore(void)
	: _hThrdWork(NULL)
	, _evbase(NULL)
	, _evListener(NULL)
	, _sMain(-1)
	, _sWork(-1)
	, _evCtrl(NULL)
	, _bufWT(4096)
{
	ZeroMemory(&_cfg, sizeof(_cfg));
}


CServerCore::~CServerCore(void)
{
	Stop();
}

BOOL CServerCore::Init(const ServerConfig &cfg)
{
	_cfg = cfg;
	return TRUE;
}

BOOL CServerCore::Start()
{
	ASSERT(NULL == _hThrdWork);

	evutil_socket_t sockpair[2] = {-1, -1};
	if (0 != evutil_socketpair(AF_INET, SOCK_STREAM, 0, sockpair)) {
		::AfxMessageBox(_T("创建socketpair失败,请检查防火墙设置，是否关闭了loopback地址"));
		return FALSE;
	}

	_sMain = sockpair[0];
	_sWork = sockpair[1];
	evutil_make_socket_nonblocking(_sMain);
	evutil_make_socket_nonblocking(_sWork);

	unsigned nTID = 0;
	_hThrdWork = (HANDLE)::_beginthreadex(NULL, 0, WorkThread, this, 0, &nTID);

	return (_hThrdWork != NULL);
}

void CServerCore::Stop()
{
	if (_hThrdWork) {
		PostCtrlMsg2WT(EID_STOP_SERVER);

		::WaitForSingleObject(_hThrdWork, INFINITE);
		::CloseHandle(_hThrdWork), _hThrdWork = NULL;

		evutil_closesocket(_sMain), _sMain = -1;
		evutil_closesocket(_sWork), _sWork = -1;

		_setConnects.clear();
	}
}

void CServerCore::PostCtrlMsg2WT( unsigned nID, void *pData/*=NULL*/, unsigned nDataLen/*=0*/ )
{
	CSingleLock l(&_lock, TRUE);

	EventDataHead head = {0};
	head.nID = nID;
	head.nLen = sizeof(head) + nDataLen;
	_bufWT.write(&head, sizeof(head));
	if (pData && nDataLen!=0) {
		_bufWT.write(pData, nDataLen);
	}

	::send(_sMain, "", 1, 0);
}

void CServerCore::Broadcast( void *pData, unsigned nLen )
{
	if (NULL==pData || 0==nLen) {
		return;
	}

	PostCtrlMsg2WT(EID_BROADCAST_DATA, pData, nLen);
}

unsigned CServerCore::_ThrdWork()
{
	struct sockaddr_in sin;

	_evbase = event_base_new();
	if (NULL == _evbase) {
		fprintf(stderr, "Could not initialize libevent!\n");
		return -1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	evutil_inet_pton(AF_INET, _cfg.sAddr.c_str(), &sin.sin_addr);
	sin.sin_port = htons(_cfg.wPort);

	_evListener = evconnlistener_new_bind(_evbase, ListenCB, (void *)this,
		LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
		(struct sockaddr*)&sin,
		sizeof(sin));
	if (NULL == _evListener) {
		fprintf(stderr, "Could not create a listener!\n");
		return -1;
	}
	
	_evCtrl = event_new(_evbase, _sWork, EV_READ|EV_PERSIST, Ctrl_EventCB, this);
	event_add(_evCtrl, NULL);

	event_base_dispatch(_evbase);

	event_free(_evCtrl), _evCtrl = NULL;
	evconnlistener_free(_evListener), _evListener = NULL;
	event_base_free(_evbase), _evbase = NULL;

	printf("done\n");
	return 0;
}

void CServerCore::_CB_Listen( struct evconnlistener *l, evutil_socket_t s,
	struct sockaddr *addr, int socklen )
{
	const sockaddr_in *sai = (sockaddr_in *)addr;
	// 	cout << "Accept new connect, addr:" << inet_ntoa(sai->sin_addr) << " port:"
	// 		<< sai->sin_port << endl;

	struct bufferevent *bev;

	bev = bufferevent_socket_new(_evbase, s, BEV_OPT_CLOSE_ON_FREE);
// 	if (!bev) {
// 		fprintf(stderr, "Error constructing bufferevent!");
// 		event_base_loopbreak(base);
// 		return;
// 	}
	bufferevent_setcb(bev, Conn_ReadCB, Conn_WriteCB, Conn_EventCB, this);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);

	_setConnects.insert(bev);
}

void CServerCore::_CB_Conn_Write( struct bufferevent *be )
{
	//TRACE(_T("连接%d可写\n"), bufferevent_getfd(be));
}

void CServerCore::_CB_Conn_Read( struct bufferevent *be )
{
	TRACE(_T("连接%d可读\n"), bufferevent_getfd(be));
}

void CServerCore::_CB_Conn_Event( struct bufferevent *be, short events )
{
	TRACE(_T("连接%d事件%d\n"), bufferevent_getfd(be), events);
	if ((events & BEV_EVENT_ERROR)
		|| (events & BEV_EVENT_EOF)) {
		_setConnects.erase(be);
	}
}

void CServerCore::_CB_Ctrl_Event( evutil_socket_t s, short events )
{
	while (1) {
		EventDataHead head = {0};
		hy::fixed_buf<> bufCmd(4096);
		{
			CSingleLock l(&_lock, TRUE);
			
			char ch = 0;
			int ret = ::recv(s, &ch, 1, 0);
			if (ret <= 0) {
				break;
			}

			if (_bufWT.size() < sizeof(EventDataHead)) {
				ASSERT(FALSE);
				break;
			}

			_bufWT.read(&head, sizeof(head));
			if (head.nLen < sizeof(head)) {
				ASSERT(FALSE);
				abort();
			}
			size_t nDataLen = head.nLen - sizeof(head);
			void *pBufWr = bufCmd.wlock(nDataLen);
			_bufWT.read(pBufWr, nDataLen);
			bufCmd.wunlock(pBufWr, nDataLen);
		}

		size_t nDataLen = bufCmd.size();
		const void *pData = bufCmd.rlock(nDataLen);
		switch (head.nID) {
		case EID_STOP_SERVER:
			event_base_loopbreak(_evbase);
			break;
		case EID_BROADCAST_DATA:
			{
				for (ConnectIter it = _setConnects.begin();
					it != _setConnects.end();
					++it) {
						struct bufferevent *bev = *it;
						bufferevent_write(bev, pData, nDataLen);
				}

				::OutputDebugString((LPCTSTR)pData);
			}
			break;
		default:
			break;
		}
		bufCmd.runlock(pData, nDataLen);
	}
}

unsigned __stdcall CServerCore::WorkThread( void *param )
{
	CServerCore *pThis = (CServerCore *)param;
	return pThis->_ThrdWork();
}

void __cdecl CServerCore::ListenCB( struct evconnlistener *l, evutil_socket_t s,
	struct sockaddr *addr, int socklen, void *param )
{
	CServerCore *pThis = (CServerCore *)param;
	pThis->_CB_Listen(l, s, addr, socklen);
}

void CServerCore::Conn_WriteCB( struct bufferevent *be, void *param )
{
	CServerCore *pThis = (CServerCore *)param;
	pThis->_CB_Conn_Write(be);
}

void CServerCore::Conn_ReadCB( struct bufferevent *be, void *param )
{
	CServerCore *pThis = (CServerCore *)param;
	pThis->_CB_Conn_Read(be);

}

void CServerCore::Conn_EventCB( struct bufferevent *be, short events, void *param )
{
	CServerCore *pThis = (CServerCore *)param;
	pThis->_CB_Conn_Event(be, events);
}

void CServerCore::Ctrl_EventCB( evutil_socket_t s, short events, void *param )
{
	CServerCore *pThis = (CServerCore *)param;
	pThis->_CB_Ctrl_Event(s, events);
}
