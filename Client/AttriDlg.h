#pragma once

//�ļ����ļ��е����ԶԻ���
// CAttriDlg �Ի���

#include "Common.h"
#include "afxwin.h"

class CAttriDlg : public CDialog
{
	DECLARE_DYNAMIC(CAttriDlg)

public:
	CAttriDlg(CWnd* pParent = NULL, ATTRIBUTE_C* tagArrtibute = NULL);   // ��׼���캯��
	virtual ~CAttriDlg();

// �Ի�������
	enum { IDD = IDD_DLG_ATTRIBUTE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
private:
	void DlgInit();
	ATTRIBUTE_C m_tagArrtibute;
public:
	CStatic m_picCtrl;
	CButton m_ReadOnlyCheck;
	CButton m_HideCheck;
	CButton m_ArchiveCheck;
	CButton m_SystemCheck;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
