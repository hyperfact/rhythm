/********************************************************************
	created:	2012/04/25
	created:	25:4:2012   9:23
	filename: 	CommunicationInterface.h
	file base:	CommunicationInterface
	file ext:	h
	author:		huaiyu
	
	purpose:	
*********************************************************************/

#ifndef _COMMUNICATION_INTERFACE_H_
#define _COMMUNICATION_INTERFACE_H_
////////////////////////////////////////////////////////////////////////////////

#pragma once
// Includes go here:
#include <winsock2.h>

#ifndef interface
#	define interface struct
#endif

////////////////////////////////////////////////////////////////////////////////
interface IClientSocketEventSink;
interface IClientSocket;
interface IMsgSpliter;

////////////////////////////////////////////////////////////////////////////////
enum SocketState
{
	SS_NONE				= 0,
	SS_DISCONNECTED		= 0,		// δ����
	
	SS_CONNECTING		= 1,		// ������
	SS_CONNECTED		= 1<<1,		// ���ӽ���
	
	SS_WRITABLE			= 1<<2,		// �ɷ���
	SS_SENDING			= 1<<3,		// ������
	
	SS_READABLE			= 1<<4,		// �ɽ���
	SS_RECEIVING		= 1<<5,		// ������
	
	SS_CLOSING			= 1<<6,		// �ر���

	SS_EXCEPT			= 1<<7,		// �����쳣
};

#ifndef MAX_SEND_SIZE
#define	MAX_SEND_SIZE	4096
#endif

////////////////////////////////////////////////////////////////////////////////
interface IClientSocket
{
	virtual void SetSocketEventSink(IClientSocketEventSink *sink) = 0;
	
	virtual bool Connect(const char *addr, u_short port) = 0;

	virtual bool Send(const void *data, u_long len) = 0;

	virtual void Close() = 0;
};

interface IClientSocketEventSink
{
	virtual void OnConnect(IClientSocket *sock, bool success) = 0;
	
	virtual void OnRead(IClientSocket *sock, const void *data, u_long len) = 0;

	virtual void OnClose(IClientSocket *sock, bool passive) = 0;

	virtual void OnExcept(IClientSocket *sock) = 0;
};

interface IMsgSpliter
{
	virtual u_long Parse(void *data, u_long len) = 0;
};

bool initCommEnv();
void uninitCommEnv();

////////////////////////////////////////////////////////////////////////////////
#endif // _COMMUNICATION_INTERFACE_H_