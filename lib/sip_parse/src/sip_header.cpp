#include <iostream>
#include <string>
#include <vector>
using namespace std;
#include "host_addr.h"
#include "skysip.h"
#include "skytool.h"
#include "sip_tool.h"
#include "sip_header.h"

sip_hd_t::sip_hd_t()
{
}
sip_hd_t::~sip_hd_t()
{
	this->preheadline.free();
	this->sipfield.free();
	this->sipContent.free();
}

int	sip_hd_t::print(PBYTE outStr)
{
	unsigned char ttbuf[1024];
	memset(ttbuf, 0, 1024);

	this->preheadline.print(ttbuf);
	int len = strlen((char*)&ttbuf);
	strcat((char*)outStr, (char*)&ttbuf);
	this->sipfield.print((PBYTE)(outStr+len));
	if( this->sipContent.eContentType == e_sipcontent_sdp)
	{
		if( this->sipContent.sdp.print((PBYTE)&ttbuf) )
		{
			this->sipfield.content_length = strlen((char*)&ttbuf);
		}
	}
	else if( this->sipContent.eContentType == e_sipcontent_xml)
	{
		this->sipfield.content_length = this->sipContent.xml.size();
		sprintf_s((char*)&ttbuf, 1024, "%s", sipContent.xml.c_str());
	}
	strcatfm((char*)outStr, "Content-Length: %d\r\n", this->sipfield.content_length);
	strcat((char*)outStr, "\r\n");
	if( this->sipfield.content_length > 0 )
	{
		strcat((char*)outStr, (char*)&ttbuf);
	}
	return strlen((char*)outStr);
}

PBYTE sip_hd_t::parse(PBYTE inStr)
{
	PBYTE cur = this->preheadline.parse(inStr);
	if( cur!= NULL && cur != inStr)
	{
		cur = this->sipfield.pasre(cur);
		if( !this->sipfield.content_type.compare("application/sdp") )
		{
			this->sipContent.eContentType = e_sipcontent_sdp;
			this->sipContent.sdp.parse(cur);
		}
		else
		{
		}
	}
	return cur;
}