// FileManager.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "FileManager.h"
#include "BroilerData.h"

// CFileManager �Ի���

static UINT idArray[] = {ID_RES_FILE1,     //״̬��
                         ID_RES_FILE2};


IMPLEMENT_DYNAMIC(CFileManager, CDialog)

CFileManager::CFileManager(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CFileManager::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_FILE_MANAGER);
	m_socket = sock;
}

CFileManager::~CFileManager()
{
}

void CFileManager::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_FILE, m_tabCtrl);
}


BEGIN_MESSAGE_MAP(CFileManager, CDialog)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_FILE, &CFileManager::OnTcnSelchangeTabFile)
END_MESSAGE_MAP()


// CFileManager ��Ϣ�������

BOOL CFileManager::PreTranslateMessage(MSG* pMsg)
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

BOOL CFileManager::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
    InitDlg();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CFileManager::InitDlg()
{
	GetClientRect(&m_rect);
	m_ImageTab.Create(16, 16, ILC_COLOR24|ILC_MASK, 1, 1);
	HICON hIcon[2];
	hIcon[0] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_FILEVIEW));
	hIcon[1] = (HICON)(AfxGetApp()->LoadIcon(IDI_ICON_FILETRANS));
    for(int i = 0; i < 2; i++)
	{
		m_ImageTab.Add(hIcon[i]);
	}
	m_tabCtrl.SetImageList(&m_ImageTab);
	m_tabCtrl.InsertItem(0, _T("�ļ��鿴"), 0);
	m_tabCtrl.InsertItem(1, _T("�ļ�����"), 1);

	m_pFileView = new CFileView(this, m_socket);
	m_pFileView->Create(IDD_DLG_FILEVIEW, &m_tabCtrl);
	CRect rect;
	m_tabCtrl.GetClientRect(rect);  //��ȡTab�������С
	rect.DeflateRect(0, 20, 0, 0);  //��С
	m_pFileView->MoveWindow(rect);

	m_pFileTrans = new CFileTrans(this, m_socket);
	m_pFileTrans->Create(IDD_DLG_FILETRANS, &m_tabCtrl);
	m_tabCtrl.GetClientRect(rect);  //��ȡTab�������С
	rect.DeflateRect(0, 20, 0, 0);  //��С
	m_pFileTrans->MoveWindow(rect);

	m_pFileTrans->ShowWindow(SW_SHOW);

	m_fileStatusBar.Create(this);
	m_fileStatusBar.SetIndicators(idArray, sizeof(idArray) / sizeof(UINT));
	m_fileStatusBar.SetPaneInfo(0, idArray[0], SBPS_NORMAL, 200);
	m_fileStatusBar.SetPaneInfo(1, idArray[1], SBPS_NORMAL, 200);
	m_fileStatusBar.SetPaneText(0 , _T("����"));
	m_fileStatusBar.SetPaneText(1 , _T(""));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //��ʾ��������״̬������仰һ��Ҫ����, ���򹤾�������ʾ��

	m_pFileView->ShowWindow(SW_SHOW);
	m_pFileTrans->ShowWindow(SW_HIDE);
}

void CFileManager::OnTcnSelchangeTabFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
    int nIndex = m_tabCtrl.GetCurFocus();   //�õ���ǰ����
	switch (nIndex)
	{
	case 0:
		{
			m_pFileView->ShowWindow(SW_SHOW);
			m_pFileTrans->ShowWindow(SW_HIDE);
		}
		break;
	case 1:
		{
			m_pFileView->ShowWindow(SW_HIDE);
			m_pFileTrans->ShowWindow(SW_SHOW);
		}
		break;
	default:
		break;
	}
	*pResult = 0;
}

void CFileManager::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	DestroyWindow();
	delete this;
}

void CFileManager::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pFileDlg = NULL;   //ΪʲôҪ����ΪNULL ������Delete
//	delete ((CBroilerData*)this->m_pParentWnd)->m_pFileDlg;   //ԭ�� �ڵ�����DestoryWindow()���ڲ��Ѿ����ڴ�ռ�ɾ������
}
