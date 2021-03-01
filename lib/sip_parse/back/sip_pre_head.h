#pragma once

void init_sip_pre_hd_field(IN PVOID _pPrefield);

void free_sip_preheader(IN PVOID _pHd);
int parse_sip_preheader(IN PUCHAR dump,OUT PVOID _pHd);

// Make Message
int make_state_code_msg(int code, PUCHAR buf);
int make_res_prefix(PVOID p_hd, PUCHAR buf);
int make_req_prefix_at(PVOID p_hd, PUCHAR buf, char *ip, int port);
int make_req_prefix(PVOID p_hd, PUCHAR buf);

