//------------------------------------------------------------------------------ 
// 
// Get machine ip addresses by sockaddr for sockaddr_in/sockaddr_in6. 
// 
// AUTHOR  : Yubin Lim 
// DATE    : 2004-11-05 
// EMAIL   : purewell at purewell dot biz 
// REQUIRE : MICROSOFT PLATFORM SDK(IPHLPAPI.LIB), 
//           WINDOWS XP, 
//           Visual C/C++ Compiler 
// 
//------------------------------------------------------------------------------ 

#include <windows.h>
#include <cstdio> 
#include <cstdlib> 
#include <iostream> 

#include "skysipua_g.h"

using namespace std; 
#if defined WINXP || defined WINCE
#include <winsock2.h> 
#ifdef WINCE
#undef WIN32
#endif
#include <ws2tcpip.h> 
#include <iptypes.h> 
#ifndef WINCE
#pragma comment(lib, "ws2_32")
#endif

#endif
#include <iphlpapi.h> 

#pragma comment(lib, "iphlpapi") 

SKYLIB bool win32_getMyIP(int nFamily, char * myip) 
{
#if 0
#ifdef WINXP
  DWORD dwRet; 
  PIP_ADAPTER_ADDRESSES pAdpAddrs; 
  PIP_ADAPTER_ADDRESSES pThis; 
  PIP_ADAPTER_UNICAST_ADDRESS pThisAddrs; 
  unsigned long ulBufLen = sizeof(IP_ADAPTER_ADDRESSES); 

  pAdpAddrs = (PIP_ADAPTER_ADDRESSES)malloc( ulBufLen ); 
  if ( !pAdpAddrs ) return false; 

  dwRet = GetAdaptersAddresses(nFamily, 0, NULL, pAdpAddrs, &ulBufLen); 
  if (dwRet == ERROR_BUFFER_OVERFLOW) 
  { 
    free ( pAdpAddrs ); 
    pAdpAddrs = (PIP_ADAPTER_ADDRESSES)malloc( ulBufLen ); 

    if ( !pAdpAddrs ) return false; 
  } 

  dwRet = GetAdaptersAddresses(nFamily, 0, NULL, pAdpAddrs, &ulBufLen); 
  if ( dwRet != NO_ERROR ) 
  { 
    free ( pAdpAddrs ); 
    return false; 
  } 

  for ( pThis = pAdpAddrs; NULL != pThis; pThis = pThis->Next) 
  { 
    for ( pThisAddrs = pThis->FirstUnicastAddress; 
          NULL != pThisAddrs; 
          pThisAddrs = pThisAddrs->Next ) 
    { 
      if ( nFamily == AF_INET ) 
      { 
        struct sockaddr_in* pAddr 
          = (struct sockaddr_in*)pThisAddrs->Address.lpSockaddr;
        sprintf(myip, "%s", inet_ntoa(pAddr->sin_addr));
		if( memcmp(myip, "127.0.0.1", 9))
		{
            free( pAdpAddrs );
			return TRUE;
		}
      } 
    }//for 
  }//for 

  free ( pAdpAddrs ); 
#else
    PIP_ADAPTER_INFO Info;
    DWORD size;
    int result;
    //CString Mac;
    
    ZeroMemory(&Info, sizeof(PIP_ADAPTER_INFO));
    size = sizeof( PIP_ADAPTER_INFO );

    result = GetAdaptersInfo( Info, &size);

    if( result == ERROR_BUFFER_OVERFLOW)
	// GetAdaptersInfo가 메모리가 부족하면 재 할당하고 재호출
    {
        Info = (PIP_ADAPTER_INFO)malloc(size);
		GetAdaptersInfo( Info, &size);
    }
    sprintf(myip,"%s", Info->IpAddressList.IpAddress.String);
                                                   
    return 0;    

#endif
#else
	{
		struct hostent *myent;
		struct in_addr *myen;
		long int *add;
		char tbuf[32];
        myen = (struct in_addr *)malloc( sizeof( struct in_addr ));
		memset(myen, 0, sizeof( struct in_addr ));
		myent = gethostbyname("");
		if( myent )
		{
			while(*myent->h_addr_list != NULL)
			{
				add = (long int *)*myent->h_addr_list;
				myen->s_addr = *add;
				sprintf(tbuf,"%s",inet_ntoa(*myen));
				if(memcmp(tbuf, "127.0.0.1", 9))
				{
					sprintf(myip, "%s", tbuf);
				    break;
				}
				myent->h_addr_list++;
			}
		}
		free(myen);
	}
#endif
  return true; 
} 
