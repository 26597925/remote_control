// ProgressDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "ProgressDlg.h"
#include "SystemManager.h"

// CProgressDlg �Ի���
typedef struct
{
	TCHAR *title;
	int nWidth; 
}COLUMN_PRO;   //����List�б���Ϣ

COLUMN_PRO g_Column[] = 
{
	{_T("��������"),	    102	},
	{_T("����ID"),			 82	},
	{_T("�߳�����"),	     82	},
	{_T("�������ȼ�"),		 82	},
	{_T("����·��"),		210	}
};

static int g_Pro_Len = 0;  //�г���
static int g_Pro_Count = sizeof(g_Column) / sizeof(COLUMN_PRO) ;   //list�ؼ��б���Ŀ

IMPLEMENT_DYNAMIC(CProgressDlg, CDialog)

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CProgressDlg::IDD, pParent)
{
	m_socket = sock;
	m_pWndMsg = pParent;
	m_bRefresh = false;
}

CProgressDlg::~CProgressDlg()
{
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_PROGRESS, m_proListCtrl);
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_PROGRESS, &CProgressDlg::OnNMRClickListProgress)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PROGRESS, &CProgressDlg::OnLvnItemchangedListProgress)
	ON_COMMAND(ID_PROC_TASKKILL, &CProgressDlg::OnProcTaskkill)
	ON_COMMAND(ID_PROC_REFRESH, &CProgressDlg::OnProcRefresh)
END_MESSAGE_MAP()


// CProgressDlg ��Ϣ�������
BOOL CProgressDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CProgressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CProgressDlg::DlgInit()
{
	for (int Index = 0 ; Index < g_Pro_Count ; Index++)  //������
	{
		m_proListCtrl.InsertColumn(Index, g_Column[Index].title);
		m_proListCtrl.SetColumnWidth(Index, g_Column[Index].nWidth);
		g_Pro_Len += g_Column[Index].nWidth;
	}
	m_proListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_MenuBmp[0].LoadBitmap(IDB_BMP_TASKKILL);
	m_MenuBmp[1].LoadBitmap(IDB_BMP_REFRESH);

	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_PROGRESS;   //���̹���鿴
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	if (!bSuccess)
	{
		::MessageBox(this->m_hWnd, _T("�鿴������Ϣʧ��!"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
	}
}

void CProgressDlg::SetProcessInfo( IN PROGRESS_C tagProInfo )
{
	CString strProName = _T("");          //������
	CString strPid     = _T("");          //���̵�PID
	CString strThreadCount = _T("");      //�����µ������߳���
	CString strLevel   = _T("");          //���̵����ȼ�
	CString strProPath = _T("");          //���̵�·��

	//��ʾ��������
	strProName.Format(_T("%s"), tagProInfo.szProName);
	strPid.Format(_T("%d"), tagProInfo.nPid);
	strThreadCount.Format(_T("%d"), tagProInfo.nThreadCount);
	strLevel = GetProcessLevel(tagProInfo.nLevel);
	strProPath.Format(_T("%s"), tagProInfo.szProPath);

	if (m_bRefresh)
	{
		m_proListCtrl.DeleteAllItems();   //�����ˢ�£���ɾ��������
		m_bRefresh = false;
	}
	int nIndex = m_proListCtrl.GetItemCount();
	m_proListCtrl.InsertItem(nIndex, strProName);
	m_proListCtrl.SetItemText(nIndex, 1, strPid);
	m_proListCtrl.SetItemText(nIndex, 2, strThreadCount);
	m_proListCtrl.SetItemText(nIndex, 3, strLevel);
	m_proListCtrl.SetItemText(nIndex, 4, strProPath);
	m_proListCtrl.SetItemData(nIndex, (DWORD_PTR)tagProInfo.nPid);

	//����״̬��
	CString strCount = _T("");
	strCount.Format(_T("����:%d"), nIndex);
	CString strKillInfo = _T("");
	if (1 == tagProInfo.nTag)
	{
		strKillInfo = _T("���̽����ɹ�!");
	}
	else if (2 == tagProInfo.nTag)
	{
		strKillInfo = _T("���̽���ʧ��!");
	}
	UpDataStatusBar(strKillInfo, strCount);
}

CString CProgressDlg::GetProcessLevel(IN int proLevel)
{
	//�������ȼ�
	CString strLevel = _T("");
	switch (proLevel)
	{
	case UNKNOWN_LEVEL:
		{
			strLevel = _T("δ֪");
		}
		break;
	case NORMAL:
		{
			strLevel = _T("��ͨ");
		}
		break;
	case IDLE:
		{
			strLevel = _T("��");
		}
		break;
	case REALTIME:
		{
			strLevel = _T("ʵʱ");
		}
		break;
	case HIGH:
		{
			strLevel = _T("��");
		}
		break;
	case ABOVENORMAL:
		{
			strLevel = _T("���ڱ�׼");
		}
		break;
	case BELOWNORMAL:
		{
			strLevel = _T("���ڱ�׼");
		}
		break;
	}
	return strLevel;
}

//����״̬��
void CProgressDlg::UpDataStatusBar(IN CString strLeft, IN CString strRight)
{
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)strLeft.GetBuffer(0));
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)strRight.GetBuffer(1));	
}

void CProgressDlg::OnNMRClickListProgress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nItem = pNMItemActivate->iItem;  //ѡ���������ֵ
	if (-1 == nItem)
	{
		return ;
	}
	CMenu processMenu;   //����Ŀ¼
	processMenu.LoadMenu(IDR_MENU_PROCESS);
	CMenu *pSubMenu = processMenu.GetSubMenu(0);
	pSubMenu->SetMenuItemBitmaps(ID_PROC_TASKKILL, MF_BYCOMMAND, &m_MenuBmp[0], &m_MenuBmp[0]);  //ͼ��
	pSubMenu->SetMenuItemBitmaps(ID_PROC_REFRESH, MF_BYCOMMAND, &m_MenuBmp[1], &m_MenuBmp[1]); 

	CPoint point;
	GetCursorPos(&point);  //��ȡ���λ��
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}

void CProgressDlg::OnLvnItemchangedListProgress(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strShowMsg = _T("��������: ");
	int nItem = pNMLV->iItem;
	if (-1 == nItem)
	{
		return;
	}
	CString strProName = m_proListCtrl.GetItemText(nItem, 0);  //������
	CString strProPid  = m_proListCtrl.GetItemText(nItem, 1);  //���̵�PID
	strShowMsg = strShowMsg + strProName + _T("  ID:") + strProPid;
	::SendMessage( ((CSystemManager*)this->m_pWndMsg)->m_sysStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)strShowMsg.GetBuffer(0));
	*pResult = 0;
}

//�ر�ָ���Ľ���
void CProgressDlg::OnProcTaskkill()
{
	// TODO: �ڴ���������������
	POSITION pos;
	pos = m_proListCtrl.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		return ;
	}
	int nItem = m_proListCtrl.GetNextSelectedItem(pos);
	PROGRESS_C tagProInfo;
	memset(&tagProInfo, 0, sizeof(PROGRESS_C));
	tagProInfo.nPid = m_proListCtrl.GetItemData(nItem);  //��ȡPIDֵ
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_PROC_TASKKILL;  //��������
	memcpy((char*)tagMsgInfo.context, (char*)&tagProInfo, sizeof(PROGRESS_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	m_bRefresh = true;
}

//ˢ�½����б�
void CProgressDlg::OnProcRefresh()
{
	// TODO: �ڴ���������������
	m_bRefresh = true;
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_PROGRESS;  //����ˢ������
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

void CProgressDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	DestroyWindow();
	delete this;
}

void CProgressDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	((CSystemManager*)this->m_pParentWnd)->m_progressDlg = NULL; 
	CDialog::PostNcDestroy();
}
