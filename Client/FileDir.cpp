// FileDir.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "FileDir.h"


// CFileDir �Ի���

IMPLEMENT_DYNAMIC(CFileDir, CDialog)

CFileDir::CFileDir(CWnd* pParent /*=NULL*/)
	: CDialog(CFileDir::IDD, pParent)
{

}

CFileDir::~CFileDir()
{
}

void CFileDir::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CFileDir, CDialog)
	ON_BN_CLICKED(IDC_BTN_BROWSER, &CFileDir::OnBnClickedBtnBrowser)
	ON_BN_CLICKED(IDOK, &CFileDir::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CFileDir::OnBnClickedCancel)
END_MESSAGE_MAP()


// CFileDir ��Ϣ�������

BOOL CFileDir::PreTranslateMessage(MSG* pMsg)
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

BOOL CFileDir::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	wchar_t szDefaultPath[MAX_PATH] = {0};
	GetPrivateProfileString(_T("Config"), _T("DefaultPath"), _T(""), szDefaultPath, _countof(szDefaultPath), _T(".\\Config.ini"));
	GetDlgItem(IDC_EDIT_FILE_PATH)->SetWindowText(szDefaultPath);
	m_strLoactionPath = szDefaultPath;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CFileDir::OnBnClickedBtnBrowser()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strDlgTitle = _T("��ѡ���ļ�Ҫ���ص�·��:");
	m_strLoactionPath = _T("");
	GetSelectDirectoryPath(strDlgTitle, m_strLoactionPath);
	GetDlgItem(IDC_EDIT_FILE_PATH)->SetWindowText(m_strLoactionPath);
}

void CFileDir::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bool bExist = true;
	CheckFolderIsExist(m_strLoactionPath, &bExist);
	if (!bExist)
	{
		AfxMessageBox(_T("ѡ��ı���Ŀ¼������!"));
		return;
	}
	else
	{
		WritePrivateProfileStringW(_T("Config"), _T("DefaultPath"), m_strLoactionPath , _T(".\\Config.ini"));  //����·��
		m_bOk = true;
	}
	OnOK();
}

void CFileDir::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bOk = false;
	OnCancel();
}

void CFileDir::GetSelectDirectoryPath( const IN CString strDlgTitle, OUT CString& strPath )
{
	if (!strPath.IsEmpty())
	{
		strPath.Empty();
	}
	TCHAR szDir[MAX_PATH];
	BROWSEINFO bi;
	ITEMIDLIST *pidl;
	//��ʼ����ڲ���
	bi.hwndOwner = this->m_hWnd;
	bi.pidlRoot = NULL;
	bi.pszDisplayName = szDir;
	bi.lpszTitle = strDlgTitle;
	bi.ulFlags = BIF_STATUSTEXT | BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
	bi.lpfn = NULL;
	bi.iImage = 0;
	bi.lParam = 0;
	//��ʼ����ڲ�������
	pidl = SHBrowseForFolder(&bi);
	if (NULL == pidl)
	{
		return ;
	}
	if (!SHGetPathFromIDList(pidl, szDir))
	{
		return ;
	}
	strPath = szDir;
	LPMALLOC lpMalloc;
	if(FAILED(SHGetMalloc(&lpMalloc)))
	{
		return;
	}
	//�ͷ��ڴ�
	else
	{
		lpMalloc->Free(pidl);
		lpMalloc->Release();
	}
}

void CFileDir::CheckFolderIsExist(IN const CString strPath, OUT bool* pbExist)
{
	WIN32_FIND_DATA wfd;
	bool bExist = true;
	if (strPath.IsEmpty())
	{
		bExist = false;
		*pbExist = bExist;
		return ;
	}
	HANDLE hFind = FindFirstFile(strPath, &wfd);
	if (INVALID_HANDLE_VALUE != hFind && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bExist = true;
	}
	else
	{
		bExist = false;
	}
	FindClose(hFind);
	*pbExist = bExist;
}
