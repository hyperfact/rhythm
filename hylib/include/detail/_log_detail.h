/********************************************************************
	created:	2012/07/27
	created:	27:7:2012   10:13
	filename: 	_log_detail.h
	file base:	_log_detail
	file ext:	h
	author:		huaiyu
	
	purpose:	
*********************************************************************/

#ifndef __LOG_DETAIL_H_
#define __LOG_DETAIL_H_
////////////////////////////////////////////////////////////////////////////////

#pragma once
// Includes go here:
#include "../_prolog.h"
#include "../util.h"
#include <list>

_HYLIB_BEGIN
_HYLIB_DETAIL_BEGIN

////////////////////////////////////////////////////////////////////////////////
inline const char *_log_cur_time() {
	time_t tm;
	::time(&tm);
	char *pszTime = ctime(&tm);
	char *pEnd = pszTime + (::strlen(pszTime)-1);
	while (pEnd!=pszTime && ('\n'==*pEnd || '\r'==*pEnd)) {
		*pEnd-- = '\0';
	}
	return pszTime;
}

////////////////////////////////////////////////////////////////////////////////
struct log_entity {
	virtual void release() = 0;
	virtual void write(const char *str) = 0;

	bool enable() const { return _enable; }
	void enable(bool enb) { _enable = enb; }
protected:
	log_entity() : _enable(false) {}
	bool _enable;
};

struct log_entity_empty
	: public log_entity
	, public singleton<log_entity_empty> {
	/*override*/ void release() {}
	/*override*/ void write(const char *str) {}
	
	HY_DECLARE_SINGLETON_0(log_entity_empty) {}
};

struct log_entity_std : public log_entity {
	explicit log_entity_std(std::ostream &os) : _os(os) {}

	/*override*/ void release() {}
	/*override*/ void write(const char *str) {
		_os << str;
	}

protected:
	std::ostream &_os;
};

struct log_entity_debugger
	: public log_entity
	, public singleton<log_entity_debugger>
{
	/*override*/ void release() {}
	/*override*/ void write(const char *str) {
		::OutputDebugString(str);
	}

	HY_DECLARE_SINGLETON_0(log_entity_debugger) {}
};

struct log_entity_log_view
	: public log_entity
	, public singleton<log_entity_log_view>
{
	/*override*/ void release() {}
	/*override*/ void write(const char *str) {
		g_pLogView->AppendLog(str);
	}
	
	HY_DECLARE_SINGLETON_0(log_entity_log_view) { }
};

struct log_entity_file : public log_entity {
	explicit log_entity_file(const char *szFile, int nMaxLen=0)
		: _sFileTpl(szFile)
		, _nMaxLen(nMaxLen) {
		using namespace std;

		time_t ct = time(NULL);
		const tm *ptm = ::localtime(&ct);
		_yday = ptm->tm_yday;
		
		mkdir_tree(szFile, true);
		string sCurFile =_nextFileName(0==_nMaxLen);
		
		_ofs.open(sCurFile.c_str(), ios::app|ios::out);
		assert(_ofs.is_open());
	}
	bool valid() const { return _ofs.is_open(); }

	/*override*/ void release() { delete this; }

	/*override*/ void write(const char *str) {
		using namespace std;
		_ofs << str << std::flush;

		do {
			time_t ct = time(NULL);
			const tm *ptm = ::localtime(&ct);
			if (_yday != ptm->tm_yday) {
				// 日期变化,切换文件
				_yday = ptm->tm_yday;
				break;
			}
			
			if (0 == _nMaxLen) {
				return; }
			
			std::streamoff nCurLen = _ofs.tellp();
			if (nCurLen > _nMaxLen) {
				// 文件超长,切换文件
				break;
			}

			return;
		} while (0);

		_ofs.close();
		
		std::string sFile = _nextFileName(0==_nMaxLen);
		_ofs.open(sFile.c_str());
		assert(_ofs.is_open());
	}

protected:
	std::ofstream	_ofs;
	const long		_nMaxLen;
	const std::string _sFileTpl;
	int				_yday;
protected:
	~log_entity_file() {}

	std::string _nextFileName(bool bFixed=false) {
		std::string sFile;
		char szTmpFile[_MAX_PATH] = {0};
		char szTime[16] = {0};
		unsigned nNum = 0;
		
		const time_t ltime = ::time(NULL);
		const tm *ptm = ::localtime(&ltime);
		strftime(szTime, ARR_LEN(szTime), "%Y-%m-%d", ptm);
		
		if (bFixed) {
			::_snprintf(szTmpFile, ARR_LEN(szTmpFile), "%s(%s).log", _sFileTpl.c_str(), szTime);
		} else {
			do {
				::_snprintf(szTmpFile, ARR_LEN(szTmpFile), "%s(%s-%u).log", _sFileTpl.c_str(), szTime, nNum++);
			} while (0 == _access(szTmpFile, 0));
		}
		return std::string(szTmpFile);
	}
};

#ifdef _WIN32
template <bool _AutoDelete=true>
struct log_entity_named_pipe : public log_entity {
	explicit log_entity_named_pipe(const char *pipeName) {
		ASSERT(pipeName);
		std::ostringstream oss;
		oss << _T("\\\\.\\pipe\\") << pipeName;
		_hPipe = ::CreateNamedPipe(oss.str().c_str(), PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE|PIPE_READMODE_BYTE|PIPE_NOWAIT,
			1, 4096, 4, 0, NULL);
	}
	~log_entity_named_pipe() {
		if (NULL != _hPipe) {
			::DisconnectNamedPipe(_hPipe);
			::CloseHandle(_hPipe);
			_hPipe = NULL;
		}
	}

	bool valid() const { return NULL != _hPipe; }
	
	/*override*/ void release() {
		if (_AutoDelete) { delete this; }
	}
	
	/*override*/ void write( const char *str ) {
		ASSERT(str);
		if (!valid()) { return; }

		DWORD dwErr = 0;
		if (!ConnectNewClient(dwErr)) {
			if (ERROR_NO_DATA != dwErr) {
				// 连接建立失败
				return;
			}

			// 对端已关闭: 断掉前一个连接,建立新连接
			::DisconnectNamedPipe(_hPipe);
			if (!ConnectNewClient(dwErr)) {
				// 此时处于监听状态, ASSERT(ERROR_PIPE_LISTENING==dwErr)
				return;
			}
		}

		DWORD dwWritten = 0;
		::WriteFile(_hPipe, str, _tcslen(str)+1, &dwWritten, NULL);
	}
	
protected:
	BOOL ConnectNewClient(DWORD &dwErr) {
		dwErr = ERROR_SUCCESS;
		if (!::ConnectNamedPipe(_hPipe, NULL)) {
			dwErr = ::GetLastError();
			if (ERROR_PIPE_CONNECTED == dwErr) {
				return TRUE;
			}
			return FALSE;
		}
		return TRUE;
	}

protected:
	HANDLE	_hPipe;
};

template <bool _AutoDelete=true>
struct log_entity_named_pipe_async : public log_entity {
	struct PerIOData {
		OVERLAPPED	overlap;
		//char		buf[4096];
	};
	typedef std::list<PerIOData *> PerIOVsl;
	typedef typename PerIOVsl::iterator PerIOIter;

	explicit log_entity_named_pipe_async(const char *pipeName)
		: _hPipe(NULL), _hEvent(NULL), _pPIODataCon(NULL)
	{
		ASSERT(pipeName);
		std::ostringstream oss;
		oss << _T("\\\\.\\pipe\\") << pipeName;
		_hPipe = ::CreateNamedPipe(oss.str().c_str(), PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED,
			PIPE_TYPE_BYTE|PIPE_READMODE_BYTE, 1, 4096, 4, 0, NULL);
		if (NULL != _hPipe) {
			_hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
 		}
		Listen(FALSE);
	}

	~log_entity_named_pipe_async() {
		stop();

		if (NULL != _hEvent) {
			::CloseHandle(_hEvent), _hEvent = NULL;
		}

		if (NULL != _hPipe) {
			::DisconnectNamedPipe(_hPipe);
			::CloseHandle(_hPipe), _hPipe = NULL;
		}

		assert(_vslPendingPerIOData.empty());
		for (PerIOIter it = _vslFreePerIOData.begin();
			it != _vslFreePerIOData.end();
			++it) {
			SafeDelete(*it);
		}
	}
	
	bool valid() const {
		return NULL!=_hPipe && NULL!=_hEvent;
	}

	void stop() {
		CancelIo(_hPipe);
		_vslFreePerIOData.insert(_vslFreePerIOData.end(),
			_vslPendingPerIOData.begin(), _vslPendingPerIOData.end());
	}

	bool isconnected() const {
		return NULL == _pPIODataCon;
	}
	
	/*override*/ void release() {
		if (_AutoDelete) { delete this; }
	}
	
	/*override*/ void write( const char *str ) {
		ASSERT(str);
		if (!valid()) { return; }

		Write(str, _tcslen(str)+1);
	}
	
protected:
	BOOL CheckConnect() {
		if (NULL == _pPIODataCon) {
			// 连接已经建立
			return TRUE;
		}

		DWORD dwTrans = 0;
		if (!GetOverlappedResult(_hPipe, (LPOVERLAPPED)_pPIODataCon, &dwTrans, FALSE)) {
			DWORD dwErr = ::GetLastError();
			if (ERROR_IO_INCOMPLETE == dwErr) {
				// 正在监听
				return FALSE;
			}
			// 重新连接
			_pPIODataCon = NULL;
			return Listen(TRUE);
		}

		// 成功建立连接
		_pPIODataCon = NULL;
		return TRUE;
	}

	BOOL Write(const void *pData, int nLen) {
		if (NULL==pData || 0==nLen) { return TRUE; }
		
		Check(FALSE);

		if (!CheckConnect()) { return FALSE; }
		assert(isconnected());

		PerIOData *pPIOData = NewPerIOData();
		if (NULL == pPIOData) { return FALSE; }
		
		if (::WriteFile(_hPipe, pData, nLen, NULL, (LPOVERLAPPED)pPIOData)) {
			return TRUE;
		}
		
		DWORD dwErr = ::GetLastError();
		if (ERROR_IO_PENDING != dwErr) {
			Listen(TRUE);
			return FALSE;
		}

		PendPerIoData(pPIOData);
		return TRUE;
	}

	BOOL Check(BOOL bWait = FALSE) {
		for (PerIOIter it = _vslPendingPerIOData.begin();
			it != _vslPendingPerIOData.end();
			/* */) {
			PerIOData *pData = *it;
			DWORD dwTrans = 0;
			if (::GetOverlappedResult(_hPipe, (LPOVERLAPPED)pData, &dwTrans, bWait)) {
				it = FreePerIoData(it);
				continue;
			}

			DWORD dwErr = ::GetLastError();
			if (ERROR_IO_INCOMPLETE != dwErr) {
				it = FreePerIoData(it);
				Listen(TRUE);
				continue;
			}

			++it;
		}
		return TRUE;
	}

	BOOL Listen(BOOL bReconnect) {
		if (NULL != _pPIODataCon) {
			return FALSE;
		}

		_pPIODataCon = NewPerIOData();
		if (NULL == _pPIODataCon) { return FALSE; }
		
		if (bReconnect) {
			::DisconnectNamedPipe(_hPipe);
		}
		DWORD dwErr = ERROR_SUCCESS;
		if (!::ConnectNamedPipe(_hPipe, (LPOVERLAPPED)_pPIODataCon)) {
			dwErr = ::GetLastError();
			if (ERROR_PIPE_CONNECTED == dwErr) {
				_pPIODataCon = NULL;
				return TRUE;
			}
			return FALSE;
		}
		_pPIODataCon = NULL;
		return TRUE;
	}

	PerIOData *NewPerIOData() {
		PerIOData *pData = NULL;
		if (!_vslFreePerIOData.empty()) {
			pData = _vslFreePerIOData.front();
		} else {
			pData = new PerIOData;
			if (NULL == pData) { return NULL; }
			_vslFreePerIOData.push_back(pData);
		}
		ZeroMemory(pData, sizeof(PerIOData));
		pData->overlap.hEvent = _hEvent;
		return pData;
	}
	void PendPerIoData(PerIOData *pData) {
		assert(!_vslFreePerIOData.empty());
		_vslFreePerIOData.pop_front();
		_vslPendingPerIOData.push_back(pData);
	}
	void FreePerIoData(PerIOData *pData, BOOL bImmediate) {
		if (!bImmediate) {
			assert(_vslFreePerIOData.end() == std::find(_vslFreePerIOData.begin(), _vslFreePerIOData.end(), pData));
			assert(_vslPendingPerIOData.end() != std::find(_vslPendingPerIOData.begin(), _vslPendingPerIOData.end(), pData));
			_vslFreePerIOData.push_back(pData);
			_vslPendingPerIOData.remove(pData);
		}
	}
	PerIOIter FreePerIoData(PerIOIter iter) {
		_vslFreePerIOData.push_back(*iter);
		return _vslPendingPerIOData.erase(iter);
	}

protected:
	HANDLE		_hPipe;
	HANDLE		_hEvent;
	PerIOData	*_pPIODataCon;
	PerIOVsl	_vslPendingPerIOData;
	PerIOVsl	_vslFreePerIOData;
};

#else
# error 暂不支持非Win32平台的命名管道
#endif // _WIN32

////////////////////////////////////////////////////////////////////////////////
_HYLIB_DETAIL_END
_HYLIB_END

#endif // __LOG_DETAIL_H_