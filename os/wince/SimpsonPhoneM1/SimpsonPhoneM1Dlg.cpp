// SimpsonPhoneM1Dlg.cpp : ���� ����
//

#include "stdafx.h"
#include "SimpsonPhoneM1.h"
#include "SimpsonPhoneM1Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CSimpsonPhoneM1Dlg ��ȭ ����

CSimpsonPhoneM1Dlg::CSimpsonPhoneM1Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSimpsonPhoneM1Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSimpsonPhoneM1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSimpsonPhoneM1Dlg, CDialog)
#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
	ON_WM_SIZE()
#endif
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CSimpsonPhoneM1Dlg �޽��� ó����

BOOL CSimpsonPhoneM1Dlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

#if defined(_DEVICE_RESOLUTION_AWARE) && !defined(WIN32_PLATFORM_WFSP)
void CSimpsonPhoneM1Dlg::OnSize(UINT /*nType*/, int /*cx*/, int /*cy*/)
{
	if (AfxIsDRAEnabled())
	{
		DRA::RelayoutDialog(
			AfxGetResourceHandle(), 
			this->m_hWnd, 
			DRA::GetDisplayMode() != DRA::Portrait ? 
			MAKEINTRESOURCE(IDD_SIMPSONPHONEM1_DIALOG_WIDE) : 
			MAKEINTRESOURCE(IDD_SIMPSONPHONEM1_DIALOG));
	}
}
#endif

