// ReadPswDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ReadPswDlg.h"
#include "SystemManager.h"

// CReadPswDlg �Ի���

IMPLEMENT_DYNAMIC(CReadPswDlg, CDialog)

CReadPswDlg::CReadPswDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CReadPswDlg::IDD, pParent)
{
	m_pWndMsg = pParent;
	m_socket = sock;
	m_strContent = _T("");
}

CReadPswDlg::~CReadPswDlg()
{
}

void CReadPswDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_READ_PSW, m_editCtrl);
}


BEGIN_MESSAGE_MAP(CReadPswDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_GETPSW, &CReadPswDlg::OnBnClickedBtnGetpsw)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CReadPswDlg ��Ϣ�������
BOOL CReadPswDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CReadPswDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CReadPswDlg::DlgInit()
{
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_GETPWD;   //�ʺ�����鿴
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	CString strLeft = _T("����");
	CString strRight = _T("");
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)strLeft.GetBuffer(0));
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)strRight.GetBuffer(1));	
}

void CReadPswDlg::OnBnClickedBtnGetpsw()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CReadPswDlg::SetReadPswInfo(IN READPSWDATA_C tagRPDATA)
{
	CString strUserName = _T("");
	strUserName.Format(_T("UserName: %s\r\n"), tagRPDATA.szUserName);
	m_strContent += strUserName;
	CString strUserPwd = _T("");
	strUserPwd.Format(_T("password: %s\r\n"), tagRPDATA.szUserPwd);
		m_strContent += strUserPwd;
	CString strDomain = _T("");
	strDomain.Format(_T("LogonDomain: %s\r\n"), tagRPDATA.szDomain);
	m_strContent += strDomain;
	CString strError = _T("");
	strError.Format(_T("%s\r\n"), tagRPDATA.szErrorMsg);
	m_strContent += strError;
	m_strContent += _T("\r\n");
    
	m_editCtrl.SetWindowText(m_strContent);
}

HBRUSH CReadPswDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (IDC_STATIC_READPSW == pWnd->GetDlgCtrlID() && CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetTextColor(RGB(255,0,0));   //������ɫ
		pDC->SetBkMode(TRANSPARENT);
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


void CReadPswDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	DestroyWindow();
	delete this;
}

void CReadPswDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	((CSystemManager*)this->m_pParentWnd)->m_pReadPswSlg = NULL; 
	CDialog::PostNcDestroy();
}
