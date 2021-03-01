#pragma once

// make_trying_msg
// input :
//        PVOID p_call - call_information
// output:
//        PUCHAR msg - printed message
// return:
//        If success, return 1.
int make_trying_msg(IN PVOID _pSipHd,
                    OUT PUCHAR msg,IN char *hostip);

int make_ringing_msg(IN PVOID _pSipHd,
                     OUT PUCHAR msg,IN char *hostip);

int make_sip_authentication(PVOID p_hd, PUCHAR buff);

int make_req_reg_auth(IN PVOID _pSipHd,
                      OUT PUCHAR msg,IN char *hostip);

// make_ok_msg
// input : 
//        PVOID p_call - call_information
// output:
//        PUCHAR msg - printed message
// return:
//        if success, return 1.

int make_ua_ok_msg(IN PVOID _pSipHd,
				   OUT PUCHAR msg,
				   IN char *hostip);
int make_ok_msg(IN PVOID _pSipHd,
                OUT PUCHAR msg,
                IN char *hostip);
int make_ok_msg_with_sdp(IN PVOID _pSipHd,
                         OUT PUCHAR msg,
                         IN char *hostip,
                         IN PVOID _pSdp);
int make_ok_msg_ua_with_sdp(IN PVOID _pSipHd,
                         OUT PUCHAR msg,
                         IN char *hostip,
                         IN PVOID _pSdp);

int make_fail_ua_msg(IN PVOID _pSipHd,
                  OUT PUCHAR msg,
                  IN int error_code,
                  IN int opt,
                  IN char *hostip);

int make_pre_answer_msg(IN PVOID _pSipHd,
                        OUT PUCHAR msg,
                        IN int code,
                        IN int opt,
                        IN char *hostip,
                        IN PVOID _pSdp,
						IN int via_opt );

int make_fail_msg(IN PVOID _pSipHd,
                  OUT PUCHAR msg,
                  IN int error_code,
                  IN int opt,
                  IN char *hostip);

int make_msg_body_by_pcall(IN PVOID _pSipHd,
                           OUT PUCHAR msg,
                           IN int www_auth,
                           IN int contact,
                           IN char *to_ip,
                           IN int to_port,
                           IN char *hostip,
                           IN int rtp_proxy,
                           IN PVOID _psdp,
						   IN int viaopt,
						   IN int b_ua);

int make_forward_msg_to(IN PVOID _pSipHd,
                        OUT PUCHAR msg,
                        IN int buflen,
                        IN int contact,
                        IN char *to_ip,
                        IN int port,
                        IN char *hostip,
                        IN int rtp_proxy,
                        IN PVOID _psdp);

int make_forward_msg(IN PVOID _pSipHd,
                     OUT PUCHAR msg,
                     IN int buflen,
                     IN int contact,
                     IN char *hostip,
                     IN int rtp_proxy,
                     IN PVOID _psdp);

int make_send_msg(IN PVOID _pSipHd,
                  OUT PUCHAR msg,
                  IN PUCHAR sfield,
                  IN char *hostip);

int make_ua_msg(IN PVOID _pSipHd,
                OUT PUCHAR msg,
				IN int www_auth,
                IN int buflen,
                IN int contact,
                IN char *to_ip,
                IN int port,
                IN char *hostip,
                IN PVOID _psdp);
