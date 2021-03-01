/* 
 * 이 파일은 SIP에서의 METHOD를 정의한 것입니다.
 * First  : 2004/11/29 swhors@naver.com
 * Second : 2004/12/01 swhors@naver.com
 *         - SIP 메소드를 정의한 구조체를 정의
 *         - SIP 메소드 타입을 구별시 사용됨.
 */
#pragma once

#ifndef _SIP_METHOD_H
#define _SIP_METHOD_H

/*SIP에서 인식이 가능한 메쏘드의 전체 수*/
#define SIP_METHOD_COUNT   19

typedef enum sip_method_type{
    msg_invalid=0,
    msg_ack,
    msg_bye,
    msg_cancel,
    msg_do,
    msg_info,
    msg_invite,
    msg_message,
    msg_notify,
    msg_options,
    msg_prack,
    msg_qauth,
    msg_refer,
    msg_register, // 13
    msg_sprack,
    msg_subscribe,
    msg_update,
    msg_publish   // 17
}sip_method_type_t; 

typedef sip_method_type_t ESIPMSGTYPE;

PBYTE parse_msg_type(PBYTE strMsg,ESIPMSGTYPE &type);

PBYTE get_msg_type_string(ESIPMSGTYPE type);
PBYTE msgtypetostr(ESIPMSGTYPE type);
//int get_sip_method_name_len(int num);
//char *get_sip_method_name(int num);
//int parse_method_type(PUCHAR dump);
#endif // _SIP_METHOD_H
