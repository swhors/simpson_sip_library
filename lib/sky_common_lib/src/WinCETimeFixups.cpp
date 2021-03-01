#ifdef WINCE

#include "WinCETimeFixups.h"

// Initialize the handle to "file descriptor" map.
char	*_tzname_fix[ 2 ] = {"DST","STD"};


//****************************************************************
struct tm * localtime( const time_t *timer )
{
	static struct tm staticTMlocaltime;

	SYSTEMTIME lt ;
	GetLocalTime(&lt) ;

	staticTMlocaltime.tm_sec = lt.wSecond;
	staticTMlocaltime.tm_min = lt.wMinute;
	staticTMlocaltime.tm_hour = lt.wHour;
	staticTMlocaltime.tm_mday = lt.wDay;
	staticTMlocaltime.tm_mon = lt.wMonth - 1;
	staticTMlocaltime.tm_year = lt.wYear - 1900;
	staticTMlocaltime.tm_wday = lt.wDayOfWeek;
	staticTMlocaltime.tm_yday = 1;			// Not implemented
	staticTMlocaltime.tm_isdst = 0;		// Not implemented 

	return &staticTMlocaltime;
}


//****************************************************************
struct tm * gmtime( const time_t *timer )
{
	static struct tm staticTMgmtime;

	SYSTEMTIME st ;
	GetSystemTime(&st) ;

	staticTMgmtime.tm_sec = st.wSecond;
	staticTMgmtime.tm_min = st.wMinute;
	staticTMgmtime.tm_hour = st.wHour;
	staticTMgmtime.tm_mday = st.wDay;
	staticTMgmtime.tm_mon = st.wMonth - 1;
	staticTMgmtime.tm_year = st.wYear - 1900;
	staticTMgmtime.tm_wday = st.wDayOfWeek;
	staticTMgmtime.tm_yday = 1;			// Not implemented
	staticTMgmtime.tm_isdst = 0;		// Not implemented 

	return &staticTMgmtime;
}


//****************************************************************
void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
	 // Note that LONGLONG is a 64-bit value
	LONGLONG ll;

	ll = Int32x32To64(t, 10000000) + 116444736000000000;
	pft->dwLowDateTime = (DWORD)ll;
	pft->dwHighDateTime = (DWORD)(ll >> 32);
}


//****************************************************************
time_t FileTimeToUnixTime( LPFILETIME pft, int *pMillisecs = NULL )
{
	// Note that LONGLONG is a 64-bit value
	LONGLONG ll;
	time_t	ttRet;

	ll = pft->dwHighDateTime;
	ll = ll << 32;
	ll += pft->dwLowDateTime;
	ll -= 116444736000000000;
	if( pMillisecs )
	{
		int		iT;
		iT = (int) ( ll / 10000 );
		printf( "in FileTimeToUnixTime( ) - RAW number of milliseconds is %d\n", iT );
		iT = iT % 1000;
		printf( "                         - final number of milliseconds is %d\n", iT );
		*pMillisecs = iT % 1000;
	}
	ll /= 10000000;
	ttRet = (time_t)ll;
	return ttRet;
}


//****************************************************************
time_t time( time_t *ptt )
{
	BOOL		fRet;
	int			iRet = -1;
	FILETIME	ft;
	SYSTEMTIME	st;
	//  get SYSTEMTIME
	GetSystemTime( &st );
	//  convert SYSTEMTIME to FILETIME
	fRet = SystemTimeToFileTime( &st, &ft );
	//  convert FILETIME to UnixTime
    if (fRet)
	iRet = (int)FileTimeToUnixTime( &ft );
//	printf( "time( ) is about to return %d\n", iRet );
	if( ptt )
		*ptt = iRet;
	return iRet;
}


//****************************************************************
void _ftime( struct _timeb *pTb )
{
	static int	iSavedTime	= 0;
	static int	iSavedTicks	= 0;
	static int	iLastTicks	= 0;

//	static int	iArr[ 10 ] ;
//	static int	iIdx		= 0;

	int			iTicks;
	int			iSecs;

//	BOOL		fRet;
//	int			iMilli;
//	FILETIME	ft;
//	SYSTEMTIME	st;

	if( iSavedTime  ==  0 )
	{
		Sleep( 0 );		//  give up the remainder of our current timeslice so we (hopefully)
						//	won't get interrupted between the next two function calls.
		iSavedTime = (int)time( NULL );
		iSavedTicks = GetTickCount( );
	}

	iTicks = GetTickCount( );
	if( iTicks  <  iLastTicks )
	{
		//  GetTickCount( ) wrapped!!
		//  Determine number of seconds that have elapsed during the last 2^32 milliseconds and add that to iSavedTime
		//  Reset iSavedTicks to zero and continue
	}

	iTicks -= iSavedTicks;
	iSecs = iTicks / 1000;
	pTb->time = iSavedTime + iSecs;
	pTb->millitm = iTicks - (iSecs * 1000);
	iLastTicks = iTicks;
//	iArr[ iIdx++ ] = pTb->millitm;
//	if( iIdx  >=  20 )
//	{
//		printf( "_ftime( ) last 20 millitm are\n%3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d\n%3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d %3.3d\n",
//				iArr[ 0 ], iArr[ 1 ], iArr[ 2 ], iArr[ 3 ], iArr[ 4 ], iArr[ 5 ], iArr[ 6 ], iArr[ 7 ], iArr[ 8 ], iArr[ 9 ],
//				iArr[ 10 ], iArr[ 11 ], iArr[ 12 ], iArr[ 13 ], iArr[ 14 ], iArr[ 15 ], iArr[ 16 ], iArr[ 17 ], iArr[ 18 ], iArr[ 19 ] );
//		iIdx = 0;
//	}

	
//	//  get SYSTEMTIME
//	GetSystemTime( &st );
//	printf( "GetSystemTime( ) returned milliseconds of %d\n", st.wMilliseconds );
//	//  convert SYSTEMTIME to FILETIME
//	fRet = SystemTimeToFileTime( &st, &ft );
//	//  convert FILETIME to UnixTime
//	pTb->time = FileTimeToUnixTime( &ft, &iMilli );
//	pTb->millitm = iMilli;
////	printf( "_ftime( ) is about to return %d - - -  millitm is %d\n", pTb->time, pTb->millitm );
}

//****************************************************************
void GetSystemTimeAsFileTime( LPFILETIME pFT )
{
//	printf( "GetSystemTimeAsFileTime( ) NOT IMPLEMENTED\n" );
	SYSTEMTIME	st;
	//  get SYSTEMTIME
	GetSystemTime( &st );
	//  convert SYSTEMTIME to FILETIME
	SystemTimeToFileTime( &st, pFT );
}
// Have to build stuff not just from WinBase but also from WinUser.h

//****************************************************************
BOOL PostThreadMessageA(DWORD idThread, UINT Msg,
                      WPARAM wParam,
                      LPARAM lParam) 
{
	int	iRet;

	if( wParam  ==  NULL )
	{
		iRet = PostThreadMessageW( idThread, Msg, wParam, lParam );
		//printf( "PostThreadMessageA( ) - nothing to translate - PostThreadMessageW( ) returned %d\n", iRet );
		return iRet;
	}
	else
	{
		//printf( "PostThreadMessageA( ) - Msg is 0x%8.8X,  wParam is 0x%8.8X,  *wParam = 0x%8.8X\n", Msg, wParam, *((DWORD *)wParam) );
		iRet = PostThreadMessageW( idThread, Msg, wParam, lParam );
		//printf( "PostThreadMessageA( ) - PostThreadMessageW( ) returned %d\n\n", iRet );
		return iRet;
	}

	return 0;
}

//***************************************************************
int GetMessageA( LPMSG lpMsg, HWND hWnd, unsigned int wMsgFilterMin, unsigned int wMsgFilterMax ) 
{
//	printf( "GetMessageA( ) NOT IMPLEMENTED\n" );
    int iRet;
    iRet = GetMessageW( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax );
    //printf( "GetMessageW( ) just returned %d\n", iRet );
    return iRet;
};

/**
  CreateEventA Redefined

  The header redefines this ison purpose and the code will not work
  for other librariesif this is not done.
  #undef CreateEventA
  #define CreateEventA CE_CreateEventA

**/

HANDLE CE_CreateEventA(
					  LPSECURITY_ATTRIBUTES lpEventAttributes, 
					  BOOL bManualReset, 
					  BOOL bInitialState, 
					  LPTSTR lpName) 
{
//	printf( "CE_CreateEventA( ) NOT IMPLEMENTED\n" );
    wchar_t wBuf[ MAX_PATH + 1 ];
   	wchar_t	*pW  = NULL;
    int     iRet = 1;
    if( lpName )
    {
        iRet = MultiByteToWideChar( CP_ACP, 0, (LPCSTR)lpName, strlen( (char*)lpName ), wBuf, MAX_PATH );
        wBuf[ iRet ] = 0;
//  printf( "  after MultiByteToWideChar( ) - it returned %d\n", iRet );
//  printf( "  wBuf is *%S*\n", wBuf );
        pW = wBuf;
    }
    if( iRet )
    {
        HANDLE hRet;
        hRet = CreateEventW( lpEventAttributes, bManualReset, bInitialState, pW );
        printf( "CE_CreateEventA( *%S* ) returned %8.8X\n", pW, hRet );
        return hRet;
    }
    else
    {
        printf( "CE_CreateEventA( *%S* ) returned NULL\n", pW );
        return NULL;
    }
    return NULL;
}

/////////////
// mktime()
int month_to_day[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
time_t mktime(struct tm *t)
{
        short  month, year;
        time_t result;

        month = t->tm_mon;
        year = t->tm_year + month / 12 + 1900;
        month %= 12;
        if (month < 0)
        {
                year -= 1;
                month += 12;
        }
        result = (year - 1970) * 365 + (year - 1969) / 4 + month_to_day[month];
        result = (year - 1970) * 365 + month_to_day[month];
        if (month <= 1)
                year -= 1;
        result += (year - 1968) / 4;
        result -= (year - 1900) / 100;
        result += (year - 1600) / 400;
        result += t->tm_mday;
        result -= 1;
        result *= 24;
        result += t->tm_hour;
        result *= 60;
        result += t->tm_min;
        result *= 60;
        result += t->tm_sec;
        return(result);
}
#endif // WINCE
