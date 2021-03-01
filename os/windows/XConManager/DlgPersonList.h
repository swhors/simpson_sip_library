#pragma once
#include "afxwin.h"


// CDlgPersonList 대화 상자입니다.

class CDlgPersonList : public CDialog
{
	DECLARE_DYNAMIC(CDlgPersonList)

public:
	CDlgPersonList(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDlgPersonList();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_PERSONLIST };

public:
	list<string> m_Person;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedSert();
	afx_msg void OnBnClickedOk();
	CListBox m_listPerson;
};
