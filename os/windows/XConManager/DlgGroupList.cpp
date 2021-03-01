// DlgGroupList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "XConManager.h"
#include "InputDialog.h"

#include <string>
#include <iostream>
#include <list>
#include <algorithm>

using namespace std;

#include "DlgPersonList.h"

#include "CallGroupList.h"

#include "DlgGroupList.h"

list<CallGroupList> m_GroupList;

// CDlgGroupList 대화 상자입니다.

IMPLEMENT_DYNAMIC(CDlgGroupList, CDialog)

CDlgGroupList::CDlgGroupList(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgGroupList::IDD, pParent)
{
	m_bMainRun = FALSE;
}

CDlgGroupList::~CDlgGroupList()
{
}

void CDlgGroupList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GROUP, m_CtlGroupList);
	DDX_Control(pDX, IDCALL, m_btnCall);
}


BEGIN_MESSAGE_MAP(CDlgGroupList, CDialog)
	ON_BN_CLICKED(IDADDGROUP, &CDlgGroupList::OnBnClickedAddgroup)
	ON_BN_CLICKED(IDDELGROUP, &CDlgGroupList::OnBnClickedDelgroup)
	ON_BN_CLICKED(IDEDGROUP, &CDlgGroupList::OnBnClickedEdgroup)
	ON_BN_CLICKED(IDOK, &CDlgGroupList::OnBnClickedOk)
	ON_BN_CLICKED(IDCALL, &CDlgGroupList::OnBnClickedCall)
END_MESSAGE_MAP()


// CDlgGroupList 메시지 처리기입니다.

void CDlgGroupList::OnBnClickedAddgroup()
{
	CInputDialog dlg;
	if( dlg.DoModal() == IDOK )
	{
		bool bFind = false;
		for( int i = 0 ; i < m_CtlGroupList.GetCount() ; i++ )
		{
			CString csItem;
			this->m_CtlGroupList.GetText(i, csItem);
			if( !csItem.Compare(dlg.m_strUInputVar) )
				bFind = true;
		}
		if( !bFind )
		{
			this->m_CtlGroupList.AddString(dlg.m_strUInputVar);
		}
		else
		{
			AfxMessageBox("이미 존재 합니다.");
		}
	}
}

void CDlgGroupList::OnBnClickedDelgroup()
{
	char szSelectedStr[250];
	int iSelNum = this->m_CtlGroupList.GetCurSel();
	if( iSelNum >= 0 )
	{
		m_CtlGroupList.GetText(iSelNum, szSelectedStr);
		list<CallGroupList>::iterator mCur = find(m_GroupList.begin(), m_GroupList.end(), szSelectedStr);
		while(mCur != m_GroupList.end() )
		{
			m_GroupList.erase(mCur);
			mCur = find(m_GroupList.begin(), m_GroupList.end(), szSelectedStr);
		}
		this->m_CtlGroupList.DeleteString(iSelNum);
	}
}

void CDlgGroupList::OnBnClickedEdgroup()
{
	char szSelectedStr[250];
	int iSelNum = this->m_CtlGroupList.GetCurSel();
	if( iSelNum >= 0 )
	{
		m_CtlGroupList.GetText(iSelNum, szSelectedStr);
		list<CallGroupList>::iterator mCur = find(m_GroupList.begin(), m_GroupList.end(), szSelectedStr);
		if( mCur != m_GroupList.end() )
		{
			CDlgPersonList pDlg;
			for(list<string>::iterator mPerson= (*mCur).listPerson.begin();
				mPerson != (*mCur).listPerson.end() ; mPerson++)
			{
				pDlg.m_Person.push_back((*mPerson));
			}
			if( pDlg.DoModal() == IDOK )
			{
				(*mCur).DelPerson("");
				for(list<string>::iterator mPerson= pDlg.m_Person.begin();
					mPerson != pDlg.m_Person.end() ; mPerson++)
				{
					(*mCur).AddPerson((*mPerson));
				}
			}
		}
	}
}

void CDlgGroupList::OnBnClickedOk()
{
	FILE *fp = NULL;
	if( !fopen_s(&fp, "GroupList.ini", "w+t"))
	{
		list<CallGroupList>::iterator mCur;
		for( mCur = m_GroupList.begin();
			mCur != m_GroupList.end(); mCur++)
		{
			for(list<string>::iterator mPerson=(*mCur).listPerson.begin();
				mPerson !=(*mCur).listPerson.end(); mPerson++)
			{
				fprintf_s(fp, "%s:%s\n", (*mCur).strGroupName.c_str(), (*mPerson).c_str());
			}
		}
		fclose(fp);
	}
	OnOK();
}

void CDlgGroupList::LoadGroup(void)
{
	FILE *fp = NULL;
	if( !fopen_s(&fp, "GroupList.ini", "r+t") )
	{
		char lineBuf[512];
		char groupName[128];
		char personID[128];
		m_GroupList.erase(m_GroupList.begin(), m_GroupList.end());
		
		while(fgets(lineBuf, 512, fp) )
		{
			char * szFind = strstr(lineBuf, ":");
			if( szFind)
			{
				int decCount = 0;
				char *szTerm = strstr(szFind, "\r\n");
				if( szTerm )
				{
					decCount = 2;
				}
				else
				{
					szTerm = strstr(szFind, "\r");
					if( szTerm )
						decCount = 1;
					else
					{
						szTerm = strstr(szFind, "\n");
						if( szTerm )
						{
							decCount =1;
						}
						else
							szTerm = strstr(szFind, "");
					}
				}
				memset(groupName, 0, 128);
				memset(personID, 0, 128);
				memcpy(groupName, lineBuf, (szFind-lineBuf));
				++szFind;
				memcpy(personID, szFind, (szTerm -szFind));
				//sprintf_s(personID, 128,"%s", 
				list<CallGroupList>::iterator mCur;
				mCur = find(m_GroupList.begin(), m_GroupList.end(), groupName);
				if( mCur != m_GroupList.end() )
				{
					(*mCur).AddPerson(personID);
				}
				else
				{
					CallGroupList *pGroup = new CallGroupList();
					if( pGroup )
					{
						pGroup->strGroupName = groupName;
						pGroup->AddPerson(personID);
						this->m_CtlGroupList.AddString(groupName);
						m_GroupList.push_back(*pGroup);
					}
				}
			}
		}
		fclose(fp);
	}
}

BOOL CDlgGroupList::OnInitDialog()
{
	CDialog::OnInitDialog();
	this->m_btnCall.EnableWindow(this->m_bMainRun);
	LoadGroup();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgGroupList::OnBnClickedCall()
{
#if 0 //muk test
	AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_LIST_INVITE, (WPARAM)"msml", (LPARAM)"MSINVITE");
	CDialog::OnOK();
	return;
#endif

	char szSelectedStr[250];
	int iSelNum = this->m_CtlGroupList.GetCurSel();
	if( iSelNum >= 0 )
	{
		m_CtlGroupList.GetText(iSelNum, szSelectedStr);
		list<CallGroupList>::iterator mCur = find(m_GroupList.begin(), m_GroupList.end(), szSelectedStr);
		if(mCur != m_GroupList.end() )
		{
			for(list<string>::iterator mPerson=(*mCur).listPerson.begin();
				mPerson!=(*mCur).listPerson.end(); mPerson++)
			{
				char * szCalled = new char[128];
				if( szCalled )
				{
					sprintf_s(szCalled, 128, "%s", (*mPerson).c_str());
					AfxGetApp()->m_pMainWnd->SendMessage(WM_USER_LIST_INVITE, (WPARAM)szCalled, NULL);
				}
			}
		}
	}
	CDialog::OnOK();
}
