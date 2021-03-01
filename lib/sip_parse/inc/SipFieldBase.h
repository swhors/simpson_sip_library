// SipFieldBase.h: interface for the CSipFieldBase class.
//
//////////////////////////////////////////////////////////////////////

#pragma once



typedef enum _e_sip_field_t
{
    e_field_unknown=0,
    e_field_accept,
    e_field_accept_encoding,
    e_field_accept_language,
    e_field_alert_info,
    e_field_allow,
    e_field_allow_events,
    e_field_authentication_info,
    e_field_authorization,
    e_field_call_id,
    e_field_call_info,
    e_field_contact,
    e_field_content_disposition,
    e_field_content_encoding,
    e_field_content_language,
    e_field_content_length,
    e_field_content_type,
    e_field_cseq,
    e_field_date,
    e_field_error_info,
    e_field_event,
    e_field_expires,
    e_field_from,
    e_field_in_reply_to,
    e_field_max_forwards,
    e_field_mime_version,
    e_field_min_expires,
    e_field_organization,
    e_field_p_access_network_info,
    e_field_p_asserted_identity,
    e_field_p_associated_uri,
    e_field_p_called_party_id,
    e_field_p_charging_function_address,
    e_field_p_charging_vector,
    e_field_p_dcs_trace_party_id,
    e_field_p_dcs_osps,
    e_field_p_dcs_billing_info,
    e_field_p_dcs_laes,
    e_field_p_media_authorization,
    e_field_p_preferred_identity,
    e_field_p_visited_network_id,
    e_field_path,
    e_field_privacy,
    e_field_priority,
    e_field_proxy_authenticate,
    e_field_proxy_authorization,
    e_field_proxy_require,
    e_field_rack,
    e_field_reason,
    e_field_record_route,
    e_field_refer_to,
    e_field_referred_by,
    e_field_replaces,
    e_field_reply_to,
    e_field_require,
    e_field_retry_after,
    e_field_route,
    e_field_rseq,
    e_field_server,
    e_field_service_route,
    e_field_session_expire,
    e_field_sip_etag,
    e_field_sip_if_match,
    e_field_subject,
    e_field_subscription_state,
    e_field_supported,
    e_field_timestamp,
    e_field_to,
    e_field_unsupported,
    e_field_user_agent,
    e_field_via,
    e_field_warning,
    e_field_www_authenticate
} e_sip_field_t;

class CSipFieldBase  
{
public:
	//constructor/destructor
	CSipFieldBase();
	virtual ~CSipFieldBase();

	int operator == (const e_sip_field_t &eType);
	int operator <(const e_sip_field_t &eType);
	int operator >(const e_sip_field_t &eType);
	int operator <= (const e_sip_field_t &eType );
	int operator >= (const e_sip_field_t &eType );

	virtual int print(OUT PBYTE outBuf=NULL);
	virtual unsigned char* parse(IN PBYTE inStr);
	virtual void free(){};

	//virtual int operator==(const CMHOST &inHost){return 0;};
	virtual CSipFieldBase &operator=(const CSipFieldBase &cSeq);
public:
	//variable
	e_sip_field_t eFieldType;
};

typedef e_sip_field_t ESIPFIELD;
typedef e_sip_field_t * PESIPFIELD;

