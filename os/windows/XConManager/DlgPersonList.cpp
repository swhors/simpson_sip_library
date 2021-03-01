// DlgPersonList.cpp : 구현 파일입니다.
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


// CDlgPersonList 대화 상자입니다.

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


// CDlgPersonList 메시지 처리기입니다.

BOOL CDlgPersonList::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	list<string>::iterator mCur;
	for( mCur = this->m_Person.begin() ; mCur!=m_Person.end(); mCur++)
	{
		this->m_listPerson.AddString((*mCur).data());
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
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
			AfxMessageBox("이미 존재 합니다.");
		}
	}
}

void CDlgPersonList::OnBnClickedOk()
{
	OnOK();
}
