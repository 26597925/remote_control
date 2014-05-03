#pragma once

//�Զ���socket������
class CModuleSocket
{
public:
	CModuleSocket(void);
	~CModuleSocket(void);

	UINT32 SendCommand(IN const SOCKET socket, 
		               IN char* szBuf,
					   IN int bytes,
					   OUT bool* pbSuccess);  //������Ϣ

	UINT32 RecvCommand(IN const SOCKET socket,
		               OUT char* szBuf,
					   OUT int bytes,
					   OUT bool *pbSuccess);   //������Ϣ

	void Clean();
};
