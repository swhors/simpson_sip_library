#pragma once

#define SPACE_SKIP(a) while(*a==' ')a++;
#define REMOVE_CRNL(a,b) {\
                     int i;\
					 for(i=0;i<b;i++,a++){if(*a=='\r'||*a=='\n'){ *a=0;}}\
                 }
