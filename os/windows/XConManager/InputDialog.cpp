// InputDialog.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "XConManager.h"
#include "InputDialog.h"


// CInputDialog 대화 상자입니다.

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


// CInputDialog 메시지 처리기입니다.

void CInputDialog::OnBnClickedOk()
{
	this->m_strUInput.GetWindowText(this->m_strUInputVar);
	OnOK();
}

BOOL CInputDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	this->m_strUInput.SetWindowText("810233506");
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CInputDialog::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}
