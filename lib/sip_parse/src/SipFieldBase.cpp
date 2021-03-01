// SipFieldBase.cpp: implementation of the CSipFieldBase class.
//
//////////////////////////////////////////////////////////////////////
#include "skytool.h"
#include "SipFieldBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSipFieldBase::CSipFieldBase()
{

}

CSipFieldBase::~CSipFieldBase()
{

}

int CSipFieldBase::operator==(const e_sip_field_t &eType)
{
	if( this->eFieldType == eType)
		return 1;
	return 0;
}

int CSipFieldBase::operator<(const e_sip_field_t &eType)
{
	if( this->eFieldType < eType )
		return 1;
	return 0;
}

int CSipFieldBase::operator>(const e_sip_field_t &eType)
{
	if( this->eFieldType > eType )
		return 1;
	return 0;
}

int CSipFieldBase::operator<=(const e_sip_field_t &eType)
{
	if( this->eFieldType <= eType )
		return 1;
	return 0;
}

int CSipFieldBase::operator>=(const e_sip_field_t &eType)
{
	if( this->eFieldType >= eType )
		return 1;
	return 0;
}

unsigned char * CSipFieldBase::parse(unsigned char * inStr)
{
	return NULL;
}

int CSipFieldBase::print(unsigned char * outBuf)
{
	return 0;
}

CSipFieldBase & CSipFieldBase::operator=(const CSipFieldBase &cSeq)
{
	return *this;
}
