//////////////////////////////////////////////////////////
//
//파일   : sip_sdp.h
//설명   : 아래의 파일은 SIP의 SDP의 구조체를 정의한 파일입니다.
//날짜   : 2005/08/12 PM 12:26
//작성자 : swhors@skycom.ne.kr
//수정   : 
//       :
//////////////////////////////////////////////////////////

#pragma once

#define OWNER_NET_TYPE_NO    0
#define OWNER_NET_TYPE_IN    1
#define OWNER_NET_TYPE_OUT   2
#define OWNER_ADDR_TYPE_NO   0
#define OWNER_ADDR_TYPE_IP4  1
#define OWNER_ADDR_TYPE_IP6  2
#define OWNER_ADDR_TYPE_OTH  3

class sdp_owner_info_t
{
public:
	sdp_owner_info_t(){};
	~sdp_owner_info_t(){};
    char owner_name[48];
    int session_id;
    int session_version;
    int owner_network_type;
    int owner_address_type;
    char owner_address[64];
};
typedef sdp_owner_info_t *p_sdp_owner_info_t;

class sdp_connection_info_t
{
public:
	sdp_connection_info_t(){};
	~sdp_connection_info_t(){};
    int networktype;
    int AddressType;
    char Address[64];
};
typedef sdp_connection_info_t* p_sdp_connection_info_t;

//sdp_time_description_t
class  sdp_time_des_t
{
public:
	sdp_time_des_t(){};
	~sdp_time_des_t(){};
    int start_time;
    int stop_time;
};
typedef sdp_time_des_t * p_sdp_time_dse_t;

//sdp_media_desctiontion_t
class sdp_media_des_t
{
public:
	sdp_media_des_t(){};
	~sdp_media_des_t(){};
    char media_type[16];
    int  port;
    char media_proto[16];
    int  media_count;
    int  media_format[10];
};
typedef sdp_media_des_t *p_sdp_media_des_t;

class sdp_media_att
{
public:
	sdp_media_att(){};
	~sdp_media_att(){};
    char attr_fieldname[16];
    char attr_value[32];
};
typedef sdp_media_att *p_sdp_media_att;

class sdp_info_t
{
public:
	sdp_info_t(){};
	~sdp_info_t(){};
    int                   version;
    sdp_owner_info_t      basic;
    char                  session_name[128];
    sdp_connection_info_t con_info;
    sdp_time_des_t        time_des;
    sdp_media_des_t       media_des;
    int                   att_count;
    sdp_media_att         pMediaAtt[13];
};
typedef sdp_info_t *p_sdp_info_t;

#define SDP_MEDIAATT_PRE    "a="
#define SDP_MEDIADES_PRE    "m="
#define SDP_TIMEDES_PRE     "t="
#define SDP_CONINFO_PRE     "c="
#define SDP_OWNERINFO_PRE   "o="
#define SDP_VER_PRE         "v="
#define SDP_SESSION_PRE     "s="
