//////////////////////////////////////////////////////
// ������ SetTimer�Լ��� ���� �ҽ��� ��� �����Դϴ�.
// ��¥ : 2005/10/07 swhors@skycom.ne.kr
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
