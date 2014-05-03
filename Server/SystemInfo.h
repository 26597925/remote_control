#pragma once

//���ܲ�����
#include "ModuleSocket.h"
#include "Common.h"

class CSystemInfo
{
public:
	CSystemInfo(void);
	~CSystemInfo(void);

	int GetSysVersion(); //��ȡ����ϵͳ�汾��Ϣ
	void GetHostName(OUT char* hostName, OUT char* szLocalIP, OUT bool* pbSuccess);   //��ȡ����ϵͳ������
	void GetInternetIP(OUT char* pWANIP, OUT bool* pbSuccess); //��ȡ����IP��ַ
	UINT32 SendSysInfo(IN SOCKET sock, OUT bool* pbSuccess);
	DWORD GetProcessorCoreCount();  //��ȡCPU�ĺ�����
	DWORD GetCPULevel();  //��ȡCPU����Ƶ��Ϣ
	BOOL IsWebCam();  //�Ƿ�������ͷ
	DWORD GetTotalDiskInfo();
private:
	CModuleSocket m_moduleSocket;
};
