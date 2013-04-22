#ifndef _PIPE_CLIENT_H_
#define _PIPE_CLIENT_H_
////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <string>
// Includes go here:

class CServerCore;

struct PIO_DATA {
	OVERLAPPED	overlap;
	HANDLE		hPipe;
	char		bufRead[4096];
	DWORD		dwRead;
	void		*param;
	int			resultCR;
};

class CPipeClient
{
public:
	CPipeClient(void);
	~CPipeClient(void);

	BOOL Init(LPCTSTR szPipeName, CServerCore *pSvr);
	BOOL Start();
	void Stop();

private:
	unsigned _ThrdWork();
	BOOL	_ConnectPipe();
	BOOL	_OnRead();

	BOOL	_OnAsyncRead_Overlapped();
	BOOL	_OnOverlappedReadComplete();

	BOOL	_OnAsyncRead_CompletionRoutine();
	static void __stdcall Read_OCR(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped);

private:
	HANDLE		_hThrdWork;
	HANDLE		_hPipe;
	HANDLE		_hEventStop;
	CServerCore	*_pSvr;
	std::string	_sPipeName;
	volatile BOOL _bRun;

	PIO_DATA	_pioData;

private:
	static unsigned __stdcall WorkThread(void *param);
};
#endif // _PIPE_CLIENT_H_

