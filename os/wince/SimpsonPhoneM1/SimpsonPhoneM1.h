// SimpsonPhoneM1.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#ifdef POCKETPC2003_UI_MODEL
#include "resourceppc.h"
#endif 

// CSimpsonPhoneM1App:
// �� Ŭ������ ������ ���ؼ��� SimpsonPhoneM1.cpp�� �����Ͻʽÿ�.
//

class CSimpsonPhoneM1App : public CWinApp
{
public:
	CSimpsonPhoneM1App();
	
// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CSimpsonPhoneM1App theApp;
