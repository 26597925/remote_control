#pragma once

#include "ModuleSocket.h"
#include "Common.h"
#include "../Common/VideoCap.h"
#include "../Common/zlib.h"
#include "../Common/zconf.h"
#pragma comment(lib, "../Common/zlib1.lib")

class CViewVideo
{
public:
	CViewVideo(void);
	~CViewVideo(void);
	bool VideoInit();  //��ʼ������ͷ����
//	void SendVideoInfo(); //ѭ��������������
	void SendViedoHeaderInfo();   //��������ͷ����
	void SendVideoData();  //������������
	void CleanData();  //�������
public:
	SOCKET m_socket;
	CModuleSocket m_moduleSocket;
	CVideoCap m_Cap;
	HANDLE m_hThread;  //�߳̾��
//	bool m_bStop;  //�Ƿ�ر�
	SYSTEMTIME m_dwLastSend;
	BYTE* m_pDataCompress;  //�������ݿ�
};
