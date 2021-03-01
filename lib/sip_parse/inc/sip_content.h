#pragma once

typedef enum _SIPCONTENTTYPE
{
	e_sipcontent_none = 0, 
	e_sipcontent_xml,
	e_sipcontent_sdp
}SIPCONTENTTYPE;

class sip_content
{
public:
	sip_content(void);
	~sip_content(void);

	void free();

	SIPCONTENTTYPE eContentType;
	sdp_info_t	sdp;
	string		xml;
};

typedef sip_content SIPCONTENT;


