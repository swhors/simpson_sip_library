// SimpsonPhoneM1.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "SimpsonPhoneM1.h"
#include "SimpsonPhoneM1Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSimpsonPhoneM1App

BEGIN_MESSAGE_MAP(CSimpsonPhoneM1App, CWinApp)
END_MESSAGE_MAP()


// CSimpsonPhoneM1App ����
CSimpsonPhoneM1App::CSimpsonPhoneM1App()
	: CWinApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CSimpsonPhoneM1App ��ü�Դϴ�.
CSimpsonPhoneM1App theApp;

// CSimpsonPhoneM1App �ʱ�ȭ

BOOL CSimpsonPhoneM1App::InitInstance()
{
    // CAPEDIT �� SIPPREF�� ���� ��� Windows Mobile ���� ��Ʈ���� �ʱ�ȭ�ϱ� ����
    // ���� ���α׷��� �ʱ�ȭ�ϴ� ���� SHInitExtraControls�� �� �� ȣ���ؾ� �մϴ�.
    SHInitExtraControls();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// ǥ�� �ʱ�ȭ
	// �̵� ����� ������� �ʰ� ���� ���� ������ ũ�⸦ ���̷���
	// �Ʒ����� �ʿ� ���� Ư�� �ʱ�ȭ 
	// ��ƾ�� �����ؾ� �մϴ�.
	// �ش� ������ ����� ������Ʈ�� Ű�� �����Ͻʽÿ�.
	// TODO: �� ���ڿ��� ȸ�� �Ǵ� ������ �̸��� ����
	// ������ �������� �����ؾ� �մϴ�.
	SetRegistryKey(_T("���� ���� ���α׷� �����翡�� ������ ���� ���α׷�"));

	CSimpsonPhoneM1Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: ���⿡ [Ȯ��]�� Ŭ���Ͽ� ��ȭ ���ڰ� ������ �� ó����
		//  �ڵ带 ��ġ�մϴ�.
	}

	// ��ȭ ���ڰ� �������Ƿ� ���� ���α׷��� �޽��� ������ �������� �ʰ�  ���� ���α׷��� ���� �� �ֵ��� FALSE��
	// ��ȯ�մϴ�.
	return FALSE;
}
