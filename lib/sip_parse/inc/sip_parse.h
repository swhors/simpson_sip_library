/* SIP 파싱의 메인 함수의 헤더 파일
 * First    : 2004/12/1 swhors@naver.com 
 */
#pragma once


#ifndef __SIP_PARSE_H
#define __SIP_PARSE_H

sip_hd_t * new_hd();
int free_hd(void*p);
//sip_hd_t * pasre_sip_msg(PUCHAR dump_mem, int len);
sip_hd_t * parse_sip_msg(PUCHAR dump_mem, int len,PVOID p_hd);

int parseerror(char * sbuff, int error);
int systemerror(char *sbuff, int error);
void set_error_hook(PVOID p_parse_error, PVOID p_system_error);

#endif // __SIP_PARSE_H

