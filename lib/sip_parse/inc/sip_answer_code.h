#pragma once

#ifndef _SKY_SIP_ANSWER_CODE_H
#define _SKY_SIP_ANSWER_CODE_H

// provisional 1xx     : 5
#define PRO_TRYING_NUM	100
#define PRO_TRYING_STR	"Trying"
#define PRO_RINGING_NUM	180
#define PRO_RINGING_STR	"Ringing"
#define PRO_CALL_IS_BEING_FORWARDED_NUM 181
#define PRO_CALL_IS_BEING_FORWARDED_STR "Call Is Being Forwarded"
#define PRO_QUEUED_NUM	182
#define PRO_QUEUED_STR	"Queued"
#define PRO_SESSION_PROGRESS_NUM 183
#define PRO_SESSION_PROGRESS_STR "Session Progress"
// successful 2xx      : 1
#define SUC_OK_NUM	200
#define SUC_OK_STR	"OK"
// redirection 3xx     : 5
#define RED_MULTIPLE_CHOICES_NUM 300
#define RED_MULTIPLE_CHOICES_STR "Multiple Choices"
#define	RED_MOVED_PERMANENTLY_NUM 301
#define	RED_MOVED_PERMANENTLY_STR "Moved Permanently"
#define RED_MOVED_TEMPORARY_NUM   302
#define RED_MOVED_TEMPORARY_STR   "Moved Temporarily"
#define RED_USE_PROXY_NUM         305
#define RED_USE_PROXY_STR         "Use Proxy"
#define RED_ALTERNATIVE_SERVICE_NUM 380
#define RED_ALTERNATIVE_SERVICE_STR "Alternative Service"
// request failure 4xx : 28
#define FAIL_BAD_REQUEST_NUM     400
#define FAIL_BAD_REQUEST_STR     "Bad Request"
#define FAIL_UNAUTHORIZED_NUM    401
#define FAIL_UNAUTHORIZED_STR    "Unauthorized"
#define FAIL_PAYMENT_REQUIRED_NUM 402
#define FAIL_PAYMENT_REQUIRED_STR "Payment Required"
#define FAIL_FORBIDDEN_NUM        403
#define FAIL_FORBIDDEN_STR        "Forbidden"
#define FAIL_NOT_FOUND_NUM        404
#define FAIL_NOT_FOUND_STR        "Not Found"
#define FAIL_METHOD_NOT_ALLOWED_NUM 405
#define FAIL_METHOD_NOT_ALLOWED_STR "Method Not Allowed"
#define FAIL_NOT_ACCEPTABLE_NUM     406
#define FAIL_NOT_ACCEPTABLE_STR     "Not Acceptable"
#define FAIL_PROXY_AUTHENTICATION_REQUIRED_NUM 407
#define FAIL_PROXY_AUTHENTICATION_REQUIRED_STR "Proxy Authentication Required"
#define FAIL_REQUEST_TIMEOUT_NUM    408
#define FAIL_REQUEST_TIMEOUT_STR    "Request Timeout"
#define FAIL_GONE_NUM               410
#define FAIL_GONE_STR               "Gone"
#define FAIL_REQUEST_ENTITY_TOO_LARGE_NUM 413
#define FAIL_REQUEST_ENTITY_TOO_LARGE_STR "Request Entity Too Large"
#define FAIL_REQUEST_URI_TOO_LONG_NUM     414
#define FAIL_REQUEST_URI_TOO_LONG_STR     "Request-URI Too Long"
#define FAIL_UNSUPPORTED_MEDIA_TYPE_NUM   415
#define FAIL_UNSUPPORTED_MEDIA_TYPE_STR   "Unsupported Media Type"
#define FAIL_UNSUPPORTED_URI_SCHEME_NUM   416
#define FAIL_UNSUPPORTED_URI_SCHEME_STR   "Unsupported URI Scheme"
#define FAIL_BAD_EXTENSION_NUM 420
#define FAIL_BAD_EXTENSION_STR "Bad Extension"
#define FAIL_EXTENSION_REQUIRED_NUM 421
#define FAIL_EXTENSION_REQUIRED_STR "Extension Required"
#define FAIL_INTERVAL_TOO_BRIEF_NUM 423
#define FAIL_INTERVAL_TOO_BRIEF_STR "Interval Too Brief"
#define FAIL_TEMPORARILY_UNAVAILABLE_NUM 480
#define FAIL_TEMPORARILY_UNAVAILABLE_STR "Temporarily Unavailabe"
#define FAIL_CALL_TRANSACTION_DOES_NOT_EXIST_NUM 481
#define FAIL_CALL_TRANSACTION_DOES_NOT_EXIST_STR "Call/Transaction Does Not Exist"
#define FAIL_LOOP_DETECTED_NUM 482
#define FAIL_LOOP_DETECTED_STR "Loop Detected"
#define FAIL_TOO_MANY_HOPS_NUM 483
#define FAIL_TOO_MANY_HOPS_STR "Too Many Hops"
#define FAIL_ADDRESS_INCOMPLETE_NUM 484
#define FAIL_ADDRESS_INCOMPLETE_STR "Address Incomplete"
#define FAIL_AMBIGUOUS_NUM 485
#define FAIL_AMBIGUOUS_STR "Ambiguous"
#define FAIL_BUSY_HERE_NUM 486
#define FAIL_BUSY_HERE_STR "Busy Here"
#define FAIL_REQUEST_TERMINATED_NUM 487
#define FAIL_REQUEST_TERMINATED_STR "Request Terminated"
#define FAIL_NOT_ACCEPTABLE_HERE_NUM 488
#define FAIL_NOT_ACCEPTABLE_HERE_STR "Not Acceptable Here"
#define FAIL_REQUEST_PENDING_NUM 491
#define FAIL_REQUEST_PENDING_STR "Request Pending"
#define FAIL_UNDECIPHERABLE_NUM  493
#define FAIL_UNDECIPHERABLE_STR  "Undecipherable"
// server failure 5xx  : 7
#define FAIL_SERVER_INTERNAL_ERROR_NUM 500
#define FAIL_SERVER_INTERNAL_ERROR_STR "Server Internal Error"
#define FAIL_NOT_IMPLEMENTED_NUM       501
#define FAIL_NOT_IMPLEMENTED_STR       "Not Implemented"
#define FAIL_BAD_GATEWAY_NUM           502
#define FAIL_BAD_GATEWAY_STR           "Bad Gateway"
#define FAIL_SERVICE_UNAVAILABLE_NUM   503
#define FAIL_SERVICE_UNAVAILABLE_STR   "Service Unavailable"
#define FAIL_SERVER_TIME_OUT_NUM       504
#define FAIL_SERVER_TIME_OUT_STR       "Server Time-out"
#define FAIL_VERSION_NOT_SUPPORTED_NUM 505
#define FAIL_VERSION_NOT_SUPPORTED_STR "Version Not Supported"
#define FAIL_MESSAGE_TOO_LARGE_NUM     513
#define FAIL_MESSAGE_TOO_LARGE_STR     "Message Too Large"
// global failurs 6xx  : 4
#define GFAIL_BUSY_EVERYWHERE_NUM       600
#define GFAIL_BUSY_EVERYWHERE_STR       "Busy Everywhere"
#define GFAIL_DECLINE_NUM               601
#define GFAIL_DECLINE_STR               "Decline"
#define GFAIL_DOES_NOT_EXIST_ANYWHERE_NUM 604
#define GFAIL_DOES_NOT_EXIST_ANYWHERE_STR "Does Not Exist Anywhere"
#define GFAIL_NOT_ACCEPTABLE_NUM        606
#define GFAIL_NOT_ACCEPTABLE_STR        "Not Acceptable"

typedef struct _sip_answer_code
{
	int code;
	char * str;
}sip_answer_code_t, *p_sip_answer_code_t;

#define ANSWER_CODE_NUM	50

int get_sip_answer_code_str(int code, char *buf);

#endif // _SKY_SIP_ANSWER_CODE_H

