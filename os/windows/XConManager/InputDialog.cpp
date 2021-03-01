// InputDialog.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "XConManager.h"
#include "InputDialog.h"


// CInputDialog ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CInputDialog, CDialog)

CInputDialog::CInputDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInputDialog::IDD, pParent)
	, m_strUInputVar(_T(""))
{
	
}

CInputDialog::~CInputDialog()
{
}

void CInputDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_strUInput);
}


BEGIN_MESSAGE_MAP(CInputDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CInputDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CInputDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// CInputDialog �޽��� ó�����Դϴ�.

void CInputDialog::OnBnClickedOk()
{
	this->m_strUInput.GetWindowText(this->m_strUInputVar);
	OnOK();
}

BOOL CInputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	this->m_strUInput.SetWindowText("810233506");
	return TRUE;  // return TRUE unless you set the focus to a control
	// ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}

void CInputDialog::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnCancel();
}
