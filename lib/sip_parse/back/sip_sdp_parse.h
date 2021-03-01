#pragma once

// main parse..
int sdp_parse(char *inbuf, int inlen, void * _phd);
// ..
int sdp_parse_mediaatt(char *pbuff, void *phd);
int sdp_parse_mediades(char *pbuff, void *phd);
int sdp_parse_timedes(char *pbuff, void *phd);
int sdp_parse_connection_info(char *pbuff, void *phd);
int sdp_parse_owner_info(char *pbuff, void *phd);
int sdp_parse_version(char *pbuff, void *phd);
int sdp_parse_session(char *pbuff, void *phd);

void modify_sdp(char *ip, int port, void *_phd);
int make_sdp_msg(void *_phd, char *msg, int buflen);

void build_sdp_ipv4_out_info(OUT PVOID _pSdp,
                             IN char *hostip,
                             IN char* u_num,
                             IN int attcount);

void build_sdp_media_attribute(
                    OUT PVOID _pMediaAtt,
                    IN char *fieldName,
                    IN char *value);

void build_sdp_media_description(
                    OUT PVOID _pMediaDesc,
                    IN int count,
                    IN int *format);

void bzero_sdp_item(OUT PVOID _pSdp);

typedef struct _sdp_parse_t{
	_sdp_parse_t();
    void * fn;
    char *prefix;
}sdp_parse_t;

typedef sdp_parse_t* p_sdp_parse_t;

typedef int (SDP_PARSE_FN)(char *, void *);

