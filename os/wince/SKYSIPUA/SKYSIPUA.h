// SKYSIPUA.h : SKYSIPUA DLL�� �⺻ ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resourceppc.h"

// CSKYSIPUAApp
// �� Ŭ������ ������ ������ SKYSIPUA.cpp�� �����Ͻʽÿ�.
//

class CSKYSIPUAApp : public CWinApp
{
public:
	CSKYSIPUAApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

