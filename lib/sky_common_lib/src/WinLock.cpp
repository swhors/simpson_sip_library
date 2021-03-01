//
// �Ʒ��� �ҽ��� ���ø����̼� ������ �������� ���� 
// �����ϱ� ���� �Լ� �Դϴ�.
//
// 
#ifdef WIN32
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include "WinLock.h"

#define DEFAULT_SPIN_COUNT	20

void sky_lock(long * _flag)
{
	int nspins;
	for(;;)
	{
		for( nspins = DEFAULT_SPIN_COUNT;nspins > 0; nspins --)
		{
			if( *_flag  == 0)
			{
				if(!InterlockedExchange(_flag, 1))
					return;
			}
		    __asm pause;
		}
		Sleep(7);
	}
}

void sky_unlock(long *_flag)
{
	*_flag = 0;
}

#endif // WIN32