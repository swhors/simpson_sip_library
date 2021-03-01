#pragma once
#include "afxwin.h"

#define WM_USER_LIST_INVITE	WM_USER+999

// CDlgGroupList 대화 상자입니다.

class CDlgGroupList : public CDialog
{
	DECLARE_DYNAMIC(CDlgGroupList)

public:
	CDlgGroupList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgGroupList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_GROUPLIST };

	BOOL	m_bMainRun;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddgroup();
	afx_msg void OnBnClickedDelgroup();
	afx_msg void OnBnClickedEdgroup();
	CListBox m_CtlGroupList;
	afx_msg void OnBnClickedOk();
	void LoadGroup(void);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCall();
	CButton m_btnCall;
};
