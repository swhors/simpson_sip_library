#pragma once

#ifdef __cplusplus
extern "C"{
#endif

#ifndef __SDP_MEDIA_H
#define __SDP_MEDIA_H

#define SDP_MEDIA_G711U_N	0
#define SDP_MEDIA_G711U_S	"ITU-T G.711 PCMU"
#define SDP_MEDIA_G729_N	18
#define SDP_MEDIA_G729_S	"ITU-T G.729"
#define SDP_MEDIA_G723_N	4
#define SDP_MEDIA_G723_S	"ITU-T G.723"

typedef struct _sdp_media_t
{
	int n;
	char *s;
}sdp_media_t, *p_sdp_media_t;

#endif

#ifdef __cplusplus
}
#endif
