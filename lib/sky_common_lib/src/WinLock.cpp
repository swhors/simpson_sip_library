//
// 아래의 소스는 어플리케이션 레벨의 윈도우즈 록을 
// 수행하기 위한 함수 입니다.
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