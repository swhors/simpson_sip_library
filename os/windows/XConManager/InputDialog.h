#pragma once
#include "afxwin.h"


// CInputDialog 대화 상자입니다.

class CInputDialog : public CDialog
{
	DECLARE_DYNAMIC(CInputDialog)

public:
	CInputDialog(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CInputDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_INPUT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_strUInput;
	afx_msg void OnBnClickedOk();
	// 사용자가 입력한 값을 갖는 버퍼
	CString m_strUInputVar;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCancel();
};
