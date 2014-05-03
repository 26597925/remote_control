// FolderDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "FolderDlg.h"


// CFolderDlg �Ի���

IMPLEMENT_DYNAMIC(CFolderDlg, CDialog)

CFolderDlg::CFolderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFolderDlg::IDD, pParent)
{

}

CFolderDlg::~CFolderDlg()
{
}

void CFolderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFolderDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFolderDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFolderDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CFolderDlg ��Ϣ�������

BOOL CFolderDlg::PreTranslateMessage(MSG* pMsg)
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

void CFolderDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bOk = true;
	CString strFolder = _T("");
	GetDlgItemText(IDC_EDIT_FOLDER, strFolder);
	if (_T("") == strFolder)
	{
		::MessageBox(this->m_hWnd, _T("��������Ч��Ŀ¼����!"), _T("����"), MB_OK | MB_ICONWARNING);
		return ;
	}
	else
	{
		m_strFolderName = strFolder;
	}
	OnOK();
}

void CFolderDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bOk = false;
	OnCancel();
}

