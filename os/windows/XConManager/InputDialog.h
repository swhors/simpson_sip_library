#pragma once
#include "afxwin.h"


// CInputDialog ��ȭ �����Դϴ�.

class CInputDialog : public CDialog
{
	DECLARE_DYNAMIC(CInputDialog)

public:
	CInputDialog(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CInputDialog();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_strUInput;
	afx_msg void OnBnClickedOk();
	// ����ڰ� �Է��� ���� ���� ����
	CString m_strUInputVar;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
};
