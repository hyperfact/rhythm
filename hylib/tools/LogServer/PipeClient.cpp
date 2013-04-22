#include "StdAfx.h"
#include "PipeClient.h"

#include "../../include/buffer.h"
#include "ServerCore.h"

enum OpResult {
	OR_NONE,
	OR_FAIL
};

CPipeClient::CPipeClient(void)
	: _hThrdWork(NULL)
	, _hPipe(NULL)
	, _pSvr(NULL)
	, _bRun(FALSE)
	, _hEventStop(NULL)
{
	ZeroMemory(&_pioData, sizeof(_pioData));
}


CPipeClient::~CPipeClient(void)
{
	Stop();
}

BOOL CPipeClient::Init(LPCTSTR szPipeName, CServerCore *pSvr)
{
	ASSERT(szPipeName && szPipeName[0]);
	ASSERT(pSvr);

	_sPipeName = szPipeName;
	_pSvr = pSvr;
	return TRUE;
}

BOOL CPipeClient::Start()
{
	_hEventStop = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	if (NULL == _hEventStop) {
		return FALSE;
	}

	unsigned nTID = 0;
	_bRun = TRUE;
	_hThrdWork = (HANDLE)::_beginthreadex(NULL, 0, WorkThread, this, 0, &nTID);
	
	return NULL != _hThrdWork;
}

void CPipeClient::Stop()
{
	if (_hThrdWork) {
		_bRun = FALSE;
		::SetEvent(_hEventStop);
		WaitForSingleObject(_hThrdWork, INFINITE);
		CloseHandle(_hThrdWork), _hThrdWork = NULL;
		CloseHandle(_hEventStop), _hEventStop = NULL;
	}
}

unsigned CPipeClient::_ThrdWork()
{
 	DWORD dwErr = 0;
 	BOOL fSuccess = FALSE;

	_pioData.overlap.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if (NULL == _pioData.overlap.hEvent) {
		return -1;
	}

	while (_bRun) {
		if (!_ConnectPipe()) {
			break;
		}
		
		::OutputDebugString("Open pipe successfully!\n");
		
		if (!_OnAsyncRead_Overlapped()) {
			break;
		}
	}

	if (_hPipe) {
		CloseHandle(_hPipe), _hPipe = NULL;
	}
	if (_pioData.overlap.hEvent) {
		CloseHandle(_pioData.overlap.hEvent), _pioData.overlap.hEvent = NULL;
	}
	return 0;
}

BOOL CPipeClient::_ConnectPipe()
{
	DWORD dwErr = 0;
	while (_bRun) {
		_hPipe = CreateFile(
			_sPipeName.c_str(),   // pipe name 
			GENERIC_READ,  // read and write access 
			0,              // no sharing 
			NULL,           // no security attributes
			OPEN_EXISTING,  // opens existing pipe 
			FILE_FLAG_OVERLAPPED, // default attributes 
			NULL);          // no template file 

		// Break if the pipe handle is valid. 
		if (INVALID_HANDLE_VALUE != _hPipe) {
			return TRUE;
		}

		dwErr = GetLastError();
		if (ERROR_OPERATION_ABORTED  == dwErr) {
			// Exit thread
			return FALSE;
		}

		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
		if (dwErr != ERROR_PIPE_BUSY) {
			OutputDebugString("Could not open pipe, pipe busy!\n");
		}

		// All pipe instances are busy, so wait for 1 seconds. 
		if (!WaitNamedPipe(_sPipeName.c_str(), 1000)) {
			OutputDebugString("Could not open pipe\n");
			
			Sleep(1000);
		}
	}
	return FALSE;
}

BOOL CPipeClient::_OnAsyncRead_Overlapped()
{
	DWORD dwErr = 0;
	BOOL fSuccess = FALSE;
	while (_bRun) {
		// Read from the pipe. 
		_pioData.hPipe = _hPipe;
		ZeroMemory(_pioData.bufRead, sizeof(_pioData.bufRead));
		fSuccess = ReadFile(
			_hPipe,		// pipe handle 
			_pioData.bufRead,	// buffer to receive reply 
			sizeof(_pioData),	// size of buffer 
			NULL,	// number of bytes read 
			(LPOVERLAPPED)&_pioData); // overlapped
		if (!fSuccess && ERROR_IO_PENDING!=::GetLastError()) {
			return TRUE; // Reconnect
		}
		
		HANDLE arHandles[2] = { _pioData.overlap.hEvent, _hEventStop };
		DWORD dwRet = WaitForMultipleObjects(2, arHandles, FALSE, INFINITE);
		if (1 == dwRet-WAIT_OBJECT_0) {
			return FALSE;
		} else if (0 != dwRet) {
			return TRUE; // Reconnect
		}

		DWORD dwTrans = 0;
		fSuccess = GetOverlappedResult(_hPipe, (LPOVERLAPPED)&_pioData, &dwTrans, FALSE);
		if (!fSuccess && ERROR_IO_INCOMPLETE!=::GetLastError()) {
			return TRUE; // Reconnect
		}
		_pSvr->Broadcast(_pioData.bufRead, dwTrans);
	}
	return FALSE;
}

BOOL CPipeClient::_OnOverlappedReadComplete()
{
	return TRUE;
}

BOOL CPipeClient::_OnAsyncRead_CompletionRoutine()
{
	DWORD dwErr = 0;
	BOOL fSuccess = FALSE;
	while (_bRun) {
		// Read from the pipe. 
		_pioData.hPipe = _hPipe;
		_pioData.param = this;
		_pioData.resultCR = 0;
		ZeroMemory(_pioData.bufRead, sizeof(_pioData.bufRead));
		fSuccess = ReadFileEx(
			_hPipe,		// pipe handle 
			_pioData.bufRead,	// buffer to receive reply 
			sizeof(_pioData),	// size of buffer  
			(LPOVERLAPPED)&_pioData,
			Read_OCR); // overlapped
		if (!fSuccess && ERROR_IO_PENDING != GetLastError()) {
			return TRUE; // Reconnect
		}

		HANDLE arHandles[2] = { _pioData.overlap.hEvent, _hEventStop };
		DWORD dwRet = WaitForMultipleObjectsEx(2, arHandles, FALSE, INFINITE, TRUE);
		if (1 == dwRet-WAIT_OBJECT_0) {
			break;
		} else if (WAIT_IO_COMPLETION == dwRet) {
			if (-1 == _pioData.resultCR) {
				return TRUE; // Reconnect
			} else {
				continue;
			}
		} else if (0 != dwRet) {
			return TRUE; // Reconnect
		}
	}
	return FALSE;
}

void __stdcall CPipeClient::Read_OCR( DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped )
{
	PIO_DATA *pIO = (PIO_DATA *)lpOverlapped;
	if (dwErrorCode != ERROR_SUCCESS) {
		pIO->resultCR = -1;
		return;
	}
	//GetOverlappedResult(_hPipe, lpOverlapped, &dwTrans, FALSE);
	CPipeClient *pThis = (CPipeClient *)pIO->param;
	pThis->_pSvr->Broadcast(pIO->bufRead, dwNumberOfBytesTransfered);
	pIO->resultCR = 0;
}

BOOL CPipeClient::_OnRead()
{
	DWORD dwErr = 0;
	BOOL fSuccess = FALSE;

	hy::fixed_buf<> buf(4096);
	while (_bRun) {
		unsigned nDataLen = buf.avail();
		void *pBuf = buf.wlock(nDataLen);
		// Read from the pipe. 
		DWORD cbRead = 0;
		fSuccess = ReadFile(
			_hPipe,		// pipe handle 
			pBuf,		// buffer to receive reply 
			nDataLen,	// size of buffer 
			&cbRead,	// number of bytes read 
			NULL);		// not overlapped 
		buf.wunlock(pBuf, cbRead);

		if (!fSuccess) {
			dwErr = ::GetLastError();
			if (ERROR_MORE_DATA == dwErr) {
				// go ahead
			} else if (ERROR_BROKEN_PIPE == dwErr || ERROR_PIPE_NOT_CONNECTED == dwErr) {
				//cout << _T("Server broken!") << endl;
				buf.clear();
				return TRUE; // Reconnect
			} else if (ERROR_OPERATION_ABORTED == dwErr) {
				buf.clear();
				return FALSE;
			} else {
				continue;
			}
		}

		if (cbRead != 0) {
			size_t nRdLen = buf.size();
			const void *pRd = buf.rlock(nRdLen);
			_pSvr->Broadcast(pBuf, nRdLen);
			buf.runlock(pRd, nRdLen);
		}

		// Reply from the pipe is written to STDOUT. 
		// 			cout.write(chBuf, cbRead);
		// 			cout << endl;
	}
	return FALSE;
}

unsigned __stdcall CPipeClient::WorkThread( void *param )
{
	CPipeClient *pThis = (CPipeClient *)param;
	return pThis->_ThrdWork();
}
