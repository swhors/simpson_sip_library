#pragma once
#include "afxwin.h"


// CDlgPersonList ��ȭ �����Դϴ�.

class CDlgPersonList : public CDialog
{
	DECLARE_DYNAMIC(CDlgPersonList)

public:
	CDlgPersonList(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgPersonList();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_PERSONLIST };

public:
	list<string> m_Person;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedSert();
	afx_msg void OnBnClickedOk();
	CListBox m_listPerson;
};
