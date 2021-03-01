#include <stdio.h>
#ifdef WIN32
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#endif

#include <iostream>
#include <string>

using namespace std;

#include "skytool.h"
#include "debug.h"
#include "sip_tool.h"
#include "sip_info.h"
#include "sip_field.h"
#include "SIpFieldBase.h"

#include "sip_method.h"
#include "sip_uri.h"
#include "sip_authentication.h"
#include "sip_record_route.h"
#include "sip_cseq.h"
#include "sip_contact.h"
#include "sip_via.h"
#include "sip_callid.h"
#include "sip_to_from.h"
#include "sip_info_display.h"

void display_cseq(void * p_hd)
{
    p_cseq_hd_t phd = (p_cseq_hd_t)p_hd;
    printf("CSeq: %d %s\r\n", 
        phd->cseq, get_sip_method_name(phd->type));
}

void display_authentication(void * p_hd)
{
    p_sip_authentication_t phd = (p_sip_authentication_t)p_hd;
    if( phd->type == WWW_AUTHENTICATE)
        printf("WWW-Authenticate: ");
    else if( phd->type == AUTHORIZATION )
        printf("Authorization: ");
    else
        return;
    if( phd->username.len )
        printf("username=\"%s\" ", phd->username.str);
    if( phd->realm.len)
        printf("realm=\"%s\" ", phd->realm.str);
    if( phd->domain.len)
        printf("domain=\"%s\" ", phd->domain.str);
    if( phd->qop.len )
        printf("qop=\"%s\" ", phd->qop.str);
    if( phd->nonce.len )
        printf("nonce=\"%s\" ", phd->nonce.str);
    if( phd->cnonce.len )
        printf("cnonce=\"%s\" ", phd->cnonce.str);
    if( phd->opaque.len )
        printf("opaque=\"%s\" ", phd->opaque.str);
    if( phd->sip_uri.len )
        printf("uri=\"%s\" ", phd->sip_uri.str);
    if( phd->response.len )
        printf("response=\"%s\" ", phd->response.str);
    if( phd->stale ==1 )
        printf("stale=TRUE ");
    else if( phd->stale == 2)
        printf("stale=FALSE ");
    if( phd->algorithm == 1)
        printf("algorithm=MD5 ");
    printf("\r\n");

}

void display_to_from(void * p_hd)
{
    p_tofrom_tag_t phd = (p_tofrom_tag_t)p_hd; 
    if( phd->type == to_tag)
        printf("To: ");
    else
        printf("From: ");
    if(phd->display.len)
        printf("%s<", phd->display.str);
    printf("sip:");
    if(phd->uri.user_number.len)
        printf("%s@", phd->uri.user_number.str);
    if(phd->uri.haddr.type == ip)
    {
        printf("%s",phd->uri.haddr.eadr.ip.sadr);
        if( phd->uri.haddr.eadr.ip.port )
            printf(":%d", phd->uri.haddr.eadr.ip.port);
    } else {
#if 0
        if(phd->uri.haddr.eadr.name.sadr.len)
            printf("%s", phd->uri.haddr.eadr.name.sadr.str);
#endif
    }
    if(phd->uri.user.len)
        printf(";user=%s", phd->uri.user.str);
    if( phd->display.len) printf(">\r\n");
    else printf("\r\n");
}

void display_max_forward(int val)
{
    printf("Max-Forwards: %d\r\n", val);
}

void display_content_length(int data)
{
    printf("Content-Length: %d\r\n", data);
}

void display_content_type(PSTRING type)
{
    if(type->len)
        printf("Content-Type: %s\r\n", type->str);
}
void display_subject(PSTRING subject)
{
    if(subject->len)
        printf("Subject: %s\r\n", subject->str);
}

void display_call_id(void * p_hd)
{
    p_call_id_t call_id = (p_call_id_t)p_hd;
    if( call_id->callid.len )
    {
        printf("Call-ID: ");
        printf("%s", call_id->callid.str);
    } else
        return;
    if( strlen(call_id->hostaddr))
        printf("@%s\r\n", call_id->hostaddr);
    else
        printf("\r\n");
}

void display_expire(int data)
{
    printf("Expires: %d\r\n", data);
}

void display_record_route(void * p_hd)
{
    p_record_route_t proute;
    p_record_route_uri_t puri;
    proute = (p_record_route_t)p_hd;

#ifdef LINUX
    if( !proute ||((int)proute==0xffffffff)||((int)proute==0xdddddddd))
#elif defined WIN32
    if( !proute )
#endif
        return;
    puri = proute->route_uri;
    if( puri )
    {
        printf("Route: ");
        while(puri)
        {
            display_uri(puri);
            puri = (p_record_route_uri_t)puri->p_next;
            if( puri )
                printf(",");
        }
        printf("\r\n");
    }
    return ;
}

void display_viacom(void * p_hd)
{
    p_via_component_t viacom = (p_via_component_t)p_hd; 
    printf("Via: SIP/%d.%d/",
        viacom->ver.major,viacom->ver.minor);
    if( viacom->type == sip_tcp)
        printf("TCP ");
    else
        printf("UDP ");
    if( viacom->haddr.type == ip)
        printf("%s:%d", viacom->haddr.eadr.ip.sadr, viacom->haddr.eadr.ip.port);
#if 0
    else
        printf("%s", viacom->haddr.eadr.name.sadr.str);
#endif
    if( viacom->branch.len)
        printf(";branch=%s\r\n", viacom->branch.str);
    else
        printf("\r\n");
}

void display_uri(void * p_hd)
{
    p_uri_addr_t phd = (p_uri_addr_t)p_hd;
    printf("<sip:");
    if(phd->user_number.len)
        printf("%s@", phd->user_number.str);
    if(phd->haddr.type == ip)
    {
        if( phd->haddr.eadr.ip.port > 0)
            printf("%s:%d", phd->haddr.eadr.ip.sadr, 
                        phd->haddr.eadr.ip.port);
        else
            printf("%s", phd->haddr.eadr.ip.sadr); 

    }
    else if( phd->haddr.type == h_name)
    {
#if 0
        if( phd->haddr.eadr.name.sadr.len)
        {
            if( phd->haddr.eadr.name.port > 0)
                printf("%s:%d", phd->haddr.eadr.name.sadr.str,
                           phd->haddr.eadr.name.port);
            else
                printf("%s", phd->haddr.eadr.name.sadr.str);
        }
#endif
    }
    if( phd->user.len)
    {
        printf(";user=%s", phd->user.str);
    }
    printf(">");
}

void display_contact(void * p_hd)
{
    p_sip_contact_hd_t contact;
    contact = (p_sip_contact_hd_t)p_hd;
    printf("Contact: ");
    display_uri(&contact->uri);
    if( contact->action.len)
        printf(";action=%s",contact->action.str);
    if( contact->expires > 0)
        printf(";expires=%d", contact->expires);
    printf("\r\n");
}

void display_date(p_date_t hd)
{
    char date[128];
    printf("Date: ");
    //printf("%s %d\n", __FILE__, __LINE__);
    strftime(date, 128,"%a, %d %b %Y %H:%M:%S", &hd->ltm);
    printf("%s %s\r\n", date, hd->tm_zone);
}

//
////////////////////////////////////////////////////////////////
//
