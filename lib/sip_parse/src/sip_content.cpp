#include <stdio.h>

#include <iostream>
#include <string>

using namespace std;
#include "host_addr.h"
#include "skytool.h"
#include "sip_tool.h"

#include "SipFieldBase.h"

#include "sip_sdp.h"

#include "sip_content.h"

sip_content::sip_content(void)
{
	this->eContentType = e_sipcontent_none;
}

sip_content::~sip_content(void)
{
	free();
}

void sip_content::free()
{
	this->eContentType = e_sipcontent_none;
	this->sdp.free();
	if( this->xml.size() > 0 )
		this->xml.erase(xml.begin(), xml.end());
}
