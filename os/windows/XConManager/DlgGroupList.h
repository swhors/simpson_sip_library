#pragma once
#include "afxwin.h"

#define WM_USER_LIST_INVITE	WM_USER+999

// CDlgGroupList ��ȭ �����Դϴ�.

class CDlgGroupList : public CDialog
{
	DECLARE_DYNAMIC(CDlgGroupList)

public:
	CDlgGroupList(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgGroupList();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_GROUPLIST };

	BOOL	m_bMainRun;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
