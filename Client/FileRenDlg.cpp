// FileRenDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "FileRenDlg.h"


// CFileRenDlg �Ի���

IMPLEMENT_DYNAMIC(CFileRenDlg, CDialog)

CFileRenDlg::CFileRenDlg(CWnd* pParent /*=NULL*/, CString strFileName)
	: CDialog(CFileRenDlg::IDD, pParent)
{
	m_strDefault = strFileName;
}

CFileRenDlg::~CFileRenDlg()
{
}

void CFileRenDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFileRenDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CFileRenDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFileRenDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CFileRenDlg ��Ϣ�������
BOOL CFileRenDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CFileRenDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	GetDlgItem(IDC_EDIT_FILE_RENAME)->SetWindowText(m_strDefault);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CFileRenDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bOk = true;
	CString strFolder = _T("");
	GetDlgItemText(IDC_EDIT_FILE_RENAME, strFolder);
	if (_T("") == strFolder)
	{
		::MessageBox(this->m_hWnd, _T("��������Ч������!"), _T("����"), MB_OK | MB_ICONWARNING);
		return ;
	}
	else
	{
		m_strFileName = strFolder;
	}
	OnOK();
}

void CFileRenDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bOk = false;
	OnCancel();
}

