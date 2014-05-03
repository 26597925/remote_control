#pragma once

#include "Common.h"
#include "ModuleSocket.h"

class CCmdShell
{
public:
	CCmdShell(void);
	~CCmdShell(void);
public:
	void SetCmdSocket(SOCKET sock);
	void Cmd_Init();  //��ʼ���������ܵ�
	void Cmd_Send();   //��ȡͨ�����ݲ�������Ϣ
    void Cmd_Recv(IN char* pRecvBuf);  //�������д��ͨ����
private:
	STARTUPINFO m_si;     //����������Ҫ�Ľṹ��
	HANDLE m_hReadPipe;   //��ͨ��
	HANDLE m_hWritePipe;  //дͨ��
    HANDLE m_hReadFile;   //���ļ�
	HANDLE m_hWriteFile;  //д�ļ�
	PROCESS_INFORMATION m_pi;   //���̵Ľṹ��
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;
};
