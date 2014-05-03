#include "StdAfx.h"
#include <iostream>
#include "ModuleSocket.h"
using namespace std;

CModuleSocket::CModuleSocket(void)
{
}

CModuleSocket::~CModuleSocket(void)
{
}

UINT32 CModuleSocket::ConnectServer( IN const char* pIpAddr, OUT SOCKET* pSocket, OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (NULL == pIpAddr || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}

	bool bSuccess = true;
	SOCKET socket = INVALID_SOCKET;
	try
	{
		WSADATA wsadata = {0};
		int iRet = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (iRet)
		{
			cout << "socket�汾��ʼ��Ϣʧ��!" << endl;
			result = WSAGetLastError();
			bSuccess = false;
			throw result;
		}
	    socket = ::socket(AF_INET, SOCK_STREAM, 0);
		if (INVALID_SOCKET == socket)
		{
			cout << "socket��ʼ��ʧ��!" << endl;
			result = WSAGetLastError();
			bSuccess = false;
			throw result;
		}
		sockaddr_in serverAddr;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.S_un.S_addr = inet_addr(pIpAddr);
		serverAddr.sin_port = htons(m_nPort);

		iRet = connect(socket, (sockaddr*)&serverAddr, sizeof(sockaddr_in));
		if (SOCKET_ERROR == iRet)
		{
			cout << "���ӷ�������ʧ��!" << endl;
			result = WSAGetLastError();
			bSuccess = false;
			throw result;
		}
		else
		{
			cout << "���ӷ������ɹ�!" << endl;
			*pSocket = socket;
		}
	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

UINT32 CModuleSocket::SendCommand(IN const SOCKET socket, 
								  IN char* szBuf,
								  IN int bytes,
								  OUT bool* pbSuccess)
{
	UINT32 result = ERROR_SUCCESS;
	if (INVALID_SOCKET == socket || 0 == bytes || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bSuccess = true;
	const char* pCurBuf = szBuf;  //��ǰ�Ѿ�������ϵĻ���������
	try
	{
		while (bytes > 0)   //û�з������
		{
			int nTotal = send(socket, (char*)pCurBuf, bytes, 0);
			if (SOCKET_ERROR == nTotal)  //����ʧ��
			{
				result = WSAGetLastError();
				bSuccess = false;
				throw result;
			}
			else if (0 == nTotal)  //�������
			{
				break;
			}
			bytes -= nTotal;
			pCurBuf += nTotal;
		}
	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

UINT32 CModuleSocket::RecvCommand( IN const SOCKET socket, OUT char* szBuf, OUT int bytes, OUT bool *pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (INVALID_SOCKET == socket || NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bSuccess = true;
	char* pCurBuf = szBuf;  //��ǰ�Ѿ�������ϵĻ���������
	try
	{
		while (bytes > 0)   //û�з������
		{
			int nTotal = recv(socket, (char*)pCurBuf, bytes, 0);
			if (SOCKET_ERROR == nTotal)  //����ʧ��
			{
				result = WSAGetLastError();
				bSuccess = false;
				throw result;
			}
			else if (0 == nTotal)  //�������
			{
				break;
			}
			bytes -= nTotal;
			pCurBuf += nTotal;
		}
	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

void CModuleSocket::Clean()
{
	::WSACleanup();
}