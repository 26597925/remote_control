// SystemManager.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "SystemManager.h"
#include "BroilerData.h"

static UINT idArray[] = {ID_RES_SYSTEM1,     //״̬��
                         ID_RES_SYSTEM2};


// CSystemManager �Ի���

IMPLEMENT_DYNAMIC(CSystemManager, CDialog)

CSystemManager::CSystemManager(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CSystemManager::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_SYS_MANAGER);
	m_socket = sock;
}

CSystemManager::~CSystemManager()
{
}

void CSystemManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SYSTEM, m_sysTab);
}


BEGIN_MESSAGE_MAP(CSystemManager, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SYSTEM, &CSystemManager::OnTcnSelchangeTabSystem)
END_MESSAGE_MAP()


// CSystemManager ��Ϣ�������
BOOL CSystemManager::PreTranslateMessage(MSG* pMsg)
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

BOOL CSystemManager::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CSystemManager::DlgInit()
{
	m_ImageTab.Create(16, 16, ILC_COLOR24|ILC_MASK, 1, 1);
	HICON hIcon[3];
	hIcon[0] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_SYSPROG));
	hIcon[1] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_SYSSERVER));
    hIcon[2] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_READPSW));
	for(int i = 0; i < 3; i++)
	{
		m_ImageTab.Add(hIcon[i]);
	}
	m_sysTab.SetImageList(&m_ImageTab);
	
	m_sysTab.InsertItem(0, _T("���̹���"), 0);
	m_sysTab.InsertItem(1, _T("�������"), 1);
	m_sysTab.InsertItem(2, _T("�ʺ���Ϣ"), 2);

	m_progressDlg = new CProgressDlg(this, m_socket);
	m_progressDlg->Create(IDD_DLG_PROGRESS, &m_sysTab);
	CRect rect;
	m_sysTab.GetClientRect(rect);  //��ȡTab�������С
	rect.DeflateRect(0, 20, 0, 0);  //��С
	m_progressDlg->MoveWindow(rect);

	m_pSysServerDlg = new CSystemServerDlg(this, m_socket);
	m_pSysServerDlg->Create(IDD_DLG_SERVER, &m_sysTab);
	m_sysTab.GetClientRect(rect);  //��ȡTab�������С
	rect.DeflateRect(0, 20, 0, 0);  //��С
	m_pSysServerDlg->MoveWindow(rect);

	m_pReadPswSlg = new CReadPswDlg(this, m_socket);
	m_pReadPswSlg->Create(IDD_DLG_READPSW, &m_sysTab);
	m_sysTab.GetClientRect(rect);  //��ȡTab�������С
	rect.DeflateRect(0, 20, 0, 0);  //��С
	m_pReadPswSlg->MoveWindow(rect);

	m_sysStatusBar.Create(this);
	m_sysStatusBar.SetIndicators(idArray, sizeof(idArray) / sizeof(UINT));
	m_sysStatusBar.SetPaneInfo(0, idArray[0], SBPS_NORMAL, 400);
	m_sysStatusBar.SetPaneInfo(1, idArray[1], SBPS_NORMAL, 200);
	m_sysStatusBar.SetPaneText(0 , _T("����"));
	m_sysStatusBar.SetPaneText(1 , _T(""));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //��ʾ��������״̬������仰һ��Ҫ����, ���򹤾�������ʾ��

	m_progressDlg->ShowWindow(SW_SHOW);
	m_pSysServerDlg->ShowWindow(SW_HIDE);
	m_pReadPswSlg->ShowWindow(SW_HIDE);
}

void CSystemManager::OnTcnSelchangeTabSystem(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nIndex = m_sysTab.GetCurFocus();   //�õ���ǰ����
	switch (nIndex)
	{
	case 0:
		{
			m_progressDlg->ShowWindow(SW_SHOW);
			m_pSysServerDlg->ShowWindow(SW_HIDE);
			m_pReadPswSlg->ShowWindow(SW_HIDE);
		}
		break;
	case 1:
		{
			m_progressDlg->ShowWindow(SW_HIDE);
			m_pSysServerDlg->ShowWindow(SW_SHOW);
			m_pReadPswSlg->ShowWindow(SW_HIDE);
		}
		break;
	case 2:
		{
			m_progressDlg->ShowWindow(SW_HIDE);
			m_pSysServerDlg->ShowWindow(SW_HIDE);
			m_pReadPswSlg->ShowWindow(SW_SHOW);
		}
		break;
	default:
		break;
	}
	*pResult = 0;
}

void CSystemManager::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pSystemDlg = NULL;
}

void CSystemManager::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
    DestroyWindow();
	delete this;
}

