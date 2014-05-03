// AttriDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "AttriDlg.h"


// CAttriDlg �Ի���

IMPLEMENT_DYNAMIC(CAttriDlg, CDialog)

CAttriDlg::CAttriDlg(CWnd* pParent /*=NULL*/ , ATTRIBUTE_C* tagArrtibute)
	: CDialog(CAttriDlg::IDD, pParent)
{
	m_tagArrtibute = *tagArrtibute;
}

CAttriDlg::~CAttriDlg()
{
}

void CAttriDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_ATTR, m_picCtrl);
	DDX_Control(pDX, IDC_CHECK_READONLY, m_ReadOnlyCheck);
	DDX_Control(pDX, IDC_CHECK_HIDE, m_HideCheck);
	DDX_Control(pDX, IDC_CHECK_ARCHIVE, m_ArchiveCheck);
	DDX_Control(pDX, IDC_CHECK_SYSTEM, m_SystemCheck);
}


BEGIN_MESSAGE_MAP(CAttriDlg, CDialog)
END_MESSAGE_MAP()


// CAttriDlg ��Ϣ�������
BOOL CAttriDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CAttriDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

//�Ի����ʼ��
void CAttriDlg::DlgInit()
{
	CString strFilePath = _T("");   //����·��
	strFilePath.Format(_T("%s"), m_tagArrtibute.szFilePath);
	GetDlgItem(IDC_STATIC_LOACTION)->SetWindowText(strFilePath);   //λ��
	CString strFileName = _T("");  //�ļ���
	strFileName.Format(_T("%s"), m_tagArrtibute.szFileName);
	GetDlgItem(IDC_ATTR_FILE_NAME)->SetWindowText(strFileName);   //����
	CString strTitle = _T(" ����");
	strTitle = strFileName + strTitle;
	this->SetWindowText(strTitle);  //���öԻ���ı���

	switch (m_tagArrtibute.nType)
	{
	case FILE_ATTR:    //�ļ�
		{
		
			GetDlgItem(IDC_STATIC_TYPE)->SetWindowText(_T("�ļ�"));
			SHFILEINFO shInfo; //mfc�ļ���Ϣ�ṹ��
			memset((char*)&shInfo, 0, sizeof(shInfo));

			//�����ļ���չ��������ͼ����ʾ�����紫��������a.exe�����ҵ��ͻ���exe�ļ�ͼ����ʾ�ľ��.
			if (SHGetFileInfo(strFileName, 0, &shInfo, sizeof(shInfo), SHGFI_ICON |SHGFI_USEFILEATTRIBUTES))
			{
				m_picCtrl.SetIcon(shInfo.hIcon);
				SetIcon(shInfo.hIcon, FALSE);   //���öԻ����ͼ��
			}
			else   //�ļ�ͼ������ȡʧ��! �������Դ������ļ�ͼ��
			{
				m_picCtrl.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_FILE));
			}
		}
		break;
	case FOLDER_ATTR:  //�ļ���
		{
			GetDlgItem(IDC_STATIC_TYPE)->SetWindowText(_T("�ļ���"));    //����
			m_picCtrl.SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_FOLDER));
		}
		break;
	default:
		break;
	}

	//����ʱ��
	wchar_t szDateTime[MAX_PATH] = {0};
    wsprintfW(szDateTime,                                          
		      _T("%02d��%02d��%02d��, %02d:%02d:%02d"),
		       m_tagArrtibute.sysCreateTime.wYear,
			    m_tagArrtibute.sysCreateTime.wMonth,         
			    m_tagArrtibute.sysCreateTime.wDay, 
			    m_tagArrtibute.sysCreateTime.wHour, 
			    m_tagArrtibute.sysCreateTime.wMinute, 
			    m_tagArrtibute.sysCreateTime.wSecond);
	GetDlgItem(IDC_STATIC_CREATE_TIME)->SetWindowText(szDateTime);   //����ʱ��
	memset(szDateTime, 0, sizeof(szDateTime));
	wsprintfW(szDateTime,
		_T("%02d��%02d��%02d��, %02d:%02d:%02d"),
		m_tagArrtibute.sysVisitTime.wYear,
		m_tagArrtibute.sysVisitTime.wMonth,         
		m_tagArrtibute.sysVisitTime.wDay, 
		m_tagArrtibute.sysVisitTime.wHour, 
		m_tagArrtibute.sysVisitTime.wMinute, 
		m_tagArrtibute.sysVisitTime.wSecond);
	GetDlgItem(IDC_STATIC_VISIT_TIME)->SetWindowText(szDateTime);   //����ʱ��
	memset(szDateTime, 0, sizeof(szDateTime));
	wsprintfW(szDateTime,
		_T("%02d��%02d��%02d��, %02d:%02d:%02d"),
		m_tagArrtibute.sysModifyTime.wYear,
		m_tagArrtibute.sysModifyTime.wMonth,         
		m_tagArrtibute.sysModifyTime.wDay, 
		m_tagArrtibute.sysModifyTime.wHour, 
		m_tagArrtibute.sysModifyTime.wMinute, 
		m_tagArrtibute.sysModifyTime.wSecond);
	GetDlgItem(IDC_STATIC_MODI_TIME)->SetWindowText(szDateTime);   //����޸�ʱ��

	//�����ļ�(��)����
	m_ReadOnlyCheck.SetCheck(m_tagArrtibute.bReadOnly);
	m_HideCheck.SetCheck(m_tagArrtibute.bHide);
	m_ArchiveCheck.SetCheck(m_tagArrtibute.bArchive);
	m_SystemCheck.SetCheck(m_tagArrtibute.bSystem);
}
