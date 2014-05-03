#include "StdAfx.h"
#include "ModuleSocket.h"
#include "Common.h"

CModuleSocket::CModuleSocket(void)
{
}

CModuleSocket::~CModuleSocket(void)
{
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
	if (INVALID_SOCKET == socket || NULL == szBuf || NULL == pbSuccess)
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
			int nTotal = recv(socket, (char*)pCurBuf, sizeof(bytes), 0);
			if (SOCKET_ERROR == nTotal)  //����ʧ��  10054 Զ������ǿ�ȹر���һ�����е�����
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