#pragma once

#include "Common.h"
#include "ModuleSocket.h"
#include <math.h>
#include <afxwin.h>

//��Ļ������
class CScreen
{
public:
	CScreen(void);
	~CScreen(void);
public:
	void GetScreen();  //��ȡ��Ļ��ʾ��Ϣ
	void SendBmpHeaderInfo();  //����λͼͷ��Ϣ
	void SendBmpData();   //����λͼ����
	void SendScreenData();  //������Ļ����
	void CleanData();  //�������
	SOCKET m_socket;
	HANDLE m_hThread;  //�߳̾��
	bool m_bFlag;
private:
	double m_panelsize;
	int m_nHeadTotal;  //λͼͷ�Ĵ�С
	BITMAPINFO *m_pBMPINFO;
	BYTE* m_pData;  //���͵�����
	UINT m_totalSize;  //λͼ�ܴ�С
	CModuleSocket m_moduleSocket;
};
