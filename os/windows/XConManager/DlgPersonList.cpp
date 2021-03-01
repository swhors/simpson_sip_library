// DlgPersonList.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Resource.h"
#include <iostream>
#include <string>
#include <list>
#include <algorithm>
using namespace std;
#include "InputDialog.h"
#include "XConManager.h"
#include "DlgPersonList.h"


// CDlgPersonList ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgPersonList, CDialog)

CDlgPersonList::CDlgPersonList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgPersonList::IDD, pParent)
{

}

CDlgPersonList::~CDlgPersonList()
{
}

void CDlgPersonList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listPerson);
}


BEGIN_MESSAGE_MAP(CDlgPersonList, CDialog)
	ON_BN_CLICKED(IDDELETE, &CDlgPersonList::OnBnClickedDelete)
	ON_BN_CLICKED(IDSERT, &CDlgPersonList::OnBnClickedSert)
	ON_BN_CLICKED(IDOK, &CDlgPersonList::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgPersonList �޽��� ó�����Դϴ�.

BOOL CDlgPersonList::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	list<string>::iterator mCur;
	for( mCur = this->m_Person.begin() ; mCur!=m_Person.end(); mCur++)
	{
		this->m_listPerson.AddString((*mCur).data());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CDlgPersonList::OnBnClickedDelete()
{
	char szSelectedStr[250];
	int iSelNum = this->m_listPerson.GetCurSel();
	if( iSelNum >= 0 )
	{
		m_listPerson.GetText(iSelNum, szSelectedStr);
		list<string>::iterator mCur = find(m_Person.begin(), m_Person.end(), szSelectedStr);
		if(mCur != m_Person.end() )
		{
			m_Person.erase(mCur);
		}
		this->m_listPerson.DeleteString(iSelNum);
	}

}

void CDlgPersonList::OnBnClickedSert()
{
	CInputDialog dlg;
	if( dlg.DoModal() == IDOK )
	{
		bool bFind = false;
		CString csItem;
		
		for( int i = 0 ; i < m_listPerson.GetCount() ; i++ )
		{
			this->m_listPerson.GetText(i, csItem);
			if( !csItem.Compare(dlg.m_strUInputVar) )
				bFind = true;
		}
		if( !bFind )
		{
			this->m_listPerson.AddString(dlg.m_strUInputVar);
			//dlg.m_strUInputVar.GetBuffer(256)
			char *tbuf = (char*)dlg.m_strUInputVar.GetString();
			this->m_Person.push_back(tbuf);
		}
		else
		{
			AfxMessageBox("�̹� ���� �մϴ�.");
		}
	}
}

void CDlgPersonList::OnBnClickedOk()
{
	OnOK();
}
