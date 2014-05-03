// ScreenDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ScreenDlg.h"
#include "BroilerData.h"

// CScreenDlg �Ի���

IMPLEMENT_DYNAMIC(CScreenDlg, CDialog)

CScreenDlg::CScreenDlg(CWnd* pParent /*=NULL*/,  SOCKET sock)
	: CDialog(CScreenDlg::IDD, pParent)
{
	m_socket = sock;
}

CScreenDlg::~CScreenDlg()
{
}

void CScreenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CScreenDlg, CDialog)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CScreenDlg ��Ϣ�������
BOOL CScreenDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if ( WM_KEYDOWN == pMsg->message)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (VK_RETURN == nVirtKey || VK_ESCAPE == nVirtKey)  //������µ��ǻس�����ESC������ض���Ϣ
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CScreenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CScreenDlg::DlgInit()
{
	char strMsg[] = "open";  //�����߳�
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_SCREEN;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

void CScreenDlg::GetScreenDataInfo(BMPDATA_C tagBmpData) //��ʾͼ��
{
	switch(tagBmpData.Id)
	{
	case 0:   //λͼͷ��Ϣ
		{
			m_pBMPINFO = (BITMAPINFO*)LocalAlloc(LPTR,tagBmpData.HeadSize);
			memcpy(m_pBMPINFO,&tagBmpData.bmpinfo,tagBmpData.HeadSize);
			m_pData = new BYTE[tagBmpData.Size];
			memset(m_pData,0,tagBmpData.Size);
		}
		break;
	case 1:  //λͼ����
		{
			//��������
			for(int i=tagBmpData.Begin,j=0;j < SCREEN_BUF_SIZE; i++, j++)
			{
				m_pData[i] = tagBmpData.Data[j];
			}
		}
		break;
	case 2:  //���һ�θ�������
 		{
			for(int i=tagBmpData.Begin,j=0;i<tagBmpData.Size;i++,j++)
			{
				m_pData[i] = tagBmpData.Data[j];
			}
		}
		break;
	default:
		{
			MessageBox(_T("δ֪��ͼ������ID"),_T("��ʾ"),MB_OK);
			delete[] m_pData;
			LocalFree(m_pBMPINFO);
			return;
		}
	}
	//�жϴ������Ժ��Ƿ������ʾͼ��
	if(tagBmpData.bShow)
	{
		SCROLLINFO hStructure,vStructure;
		memset(&hStructure,0,sizeof(SCROLLINFO));
		memset(&vStructure,0,sizeof(SCROLLINFO));
		hStructure.cbSize = sizeof(SCROLLINFO);
		vStructure.cbSize = sizeof(SCROLLINFO);
		//��ȡ��������λ�ã�����λ�û���ͼ��
		GetScrollInfo(SB_HORZ, &hStructure);
		GetScrollInfo(SB_VERT, &vStructure);

		CRect rc1;
		GetClientRect(&rc1);
		//��ʾͼ��
		::StretchDIBits(GetDC()->m_hDC,    
			0,
			31,
			m_pBMPINFO->bmiHeader.biWidth,
			m_pBMPINFO->bmiHeader.biHeight,
			hStructure.nPos,
			-vStructure.nPos,
			m_pBMPINFO->bmiHeader.biWidth,
			m_pBMPINFO->bmiHeader.biHeight,
			m_pData, //λͼ����
			m_pBMPINFO, //BITMAPINFO λͼ��Ϣͷ
			DIB_RGB_COLORS,
			SRCCOPY
			);

		hStructure.fMask		= SIF_ALL;
		hStructure.nMin			= 0;
		//hStructure.nMax			= bitmap.bmWidth;
		hStructure.nMax			= m_pBMPINFO->bmiHeader.biWidth;
		hStructure.nPage		= rc1.right;
		SetScrollInfo(SB_HORZ, &hStructure);  //���ù�����

		vStructure.fMask		= SIF_ALL;
		vStructure.nMin			= 0;
		//vStructure.nMax			= bitmap.bmHeight + 31;
		vStructure.nMax			= m_pBMPINFO->bmiHeader.biHeight + 31;
		vStructure.nPage		= rc1.bottom;
		SetScrollInfo(SB_VERT, &vStructure);

		delete []m_pData;
		LocalFree(m_pBMPINFO);
		m_pData = NULL;
		m_pBMPINFO = NULL;
	}
	return;
}

void CScreenDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pScreenDlg = NULL;
}

void CScreenDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	char strMsg[] = "quit";   //�˳��߳�
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_SCREEN;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //�����˳���Ϣ
	Sleep(30);
	DestroyWindow();
	delete this;
}

void CScreenDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nSBCode != SB_ENDSCROLL)
	{
		SCROLLINFO hStructure;
		GetScrollInfo(SB_VERT, &hStructure);
		hStructure.nPos = nPos;
		SetScrollInfo(SB_VERT, &hStructure);
	}
	CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void CScreenDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if (nSBCode != SB_ENDSCROLL)
	{
		SCROLLINFO hStructure;
		GetScrollInfo(SB_HORZ, &hStructure);
		hStructure.nPos = nPos;
		SetScrollInfo(SB_HORZ, &hStructure);
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

