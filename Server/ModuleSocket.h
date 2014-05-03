#pragma once
#include "stdafx.h"
#include "Common.h"

class CModuleSocket
{
public:
	CModuleSocket(void);
	~CModuleSocket(void);
    
	UINT32 ConnectServer(IN const char* pIpAddr,OUT SOCKET* pSocket, OUT bool* pbSuccess);  //���ӷ�����

	UINT32 SendCommand(IN const SOCKET socket, 
		IN char* szBuf,
		IN int bytes,
		OUT bool* pbSuccess);  //������Ϣ

	UINT32 RecvCommand(IN const SOCKET socket,
		OUT char* szBuf,
		OUT int bytes,
		OUT bool *pbSuccess);   //������Ϣ

	void Clean();

public:
	UINT  m_nPort;   //���Ӷ˿�
};
