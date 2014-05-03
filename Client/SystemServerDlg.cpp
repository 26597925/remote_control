// SystemServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "SystemServerDlg.h"
#include "SystemManager.h"

// CSystemServerDlg �Ի���

IMPLEMENT_DYNAMIC(CSystemServerDlg, CDialog)

CSystemServerDlg::CSystemServerDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CSystemServerDlg::IDD, pParent)
{
	m_socket = sock;
}

CSystemServerDlg::~CSystemServerDlg()
{
}

void CSystemServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSystemServerDlg, CDialog)
END_MESSAGE_MAP()


// CSystemServerDlg ��Ϣ�������

BOOL CSystemServerDlg::PreTranslateMessage(MSG* pMsg)
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

void CSystemServerDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	DestroyWindow();
	delete this;
}

void CSystemServerDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	((CSystemManager*)this->m_pParentWnd)->m_pSysServerDlg = NULL; 
	CDialog::PostNcDestroy();
}
