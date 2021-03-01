//////////////////////////////////////////////////////
// 다음은 SetTimer함수의 래핑 소스의 헤더 파일입니다.
// 날짜 : 2005/10/07 swhors@skycom.ne.kr
//////////////////////////////////////////////////////

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __SKY_TIMER_H
#define __SKY_TIMER_H

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
void SkySetTimer(IN void *handle,IN int id,IN int interval,IN void * pFunc);

#endif // __SKY_TIMER_H

#ifdef __cplusplus
}
#endif

//////////////////////////////////////////////////////
// End of File 
//////////////////////////////////////////////////////
