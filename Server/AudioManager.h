#pragma once

//����������
#include "Common.h"
#include "ModuleSocket.h"
#include "../Common/Audio.h"

class CAudioManager
{
public:
	CAudioManager(void);
	~CAudioManager(void);
	void SendRecordBuffer();   //����������Ϣ
	void RecvRecordBuffer();   //����Զ�̵�������Ϣ
	bool Initialize();  //��ʼ��

public:
	SOCKET m_socket;
    bool m_bWorking;
	HANDLE m_hSendThread;
	HANDLE m_hRecvThread;
private:
	CModuleSocket m_moduleSocket;
	CAudio	*m_lpAudio;
	static DWORD WINAPI SendAudioDataProc(LPVOID lpParameter);
	static DWORD WINAPI RecvAudioDataProc(LPVOID lpParameter);
};
