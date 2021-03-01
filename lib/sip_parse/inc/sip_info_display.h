/*
 * sip_info_display.c
 * 
 * update : 2005/3/2 by swhors@skycom.ne.kr
 *          일반적인 구조체 형식으로 선언된 함수를 void * 타입으로 변환
 *          record_route에 관한 함수 추가.
 */
#pragma once

#ifndef _SIP_INFO_DISPLAY_H
#define _SIP_INFO_DISPLAY_H

void display_uri(void * p_hd);
void display_cseq(void * p_hd);
void display_to_from(void * hd);
void display_max_forward(int val);
void display_content_length(int data);
void display_content_type(PSTRING type);
void display_subject(PSTRING subject);
void display_call_id(void * p_hd);
void display_expire(int data);
void display_viacom(void * p_hd);
void display_contact(void * p_hd);
#endif // _SIP_INFO_DISPLAY_H
