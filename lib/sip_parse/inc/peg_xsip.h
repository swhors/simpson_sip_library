#pragma once

#include "peg_def.h"
namespace peg_templ
{
// Essential ext
typedef Or<Char<'a'>,Char<'A'> > CHAR_A;
typedef Or<Char<'b'>,Char<'B'> > CHAR_B;
typedef Or<Char<'c'>,Char<'C'> > CHAR_C;
typedef Or<Char<'d'>,Char<'D'> > CHAR_D;
typedef Or<Char<'e'>,Char<'E'> > CHAR_E;
typedef Or<Char<'f'>,Char<'F'> > CHAR_F;
typedef Or<Char<'g'>,Char<'G'> > CHAR_G;
typedef Or<Char<'h'>,Char<'H'> > CHAR_H;
typedef Or<Char<'i'>,Char<'I'> > CHAR_I;
typedef Or<Char<'j'>,Char<'J'> > CHAR_J;
typedef Or<Char<'k'>,Char<'K'> > CHAR_K;
typedef Or<Char<'l'>,Char<'L'> > CHAR_L;
typedef Or<Char<'m'>,Char<'M'> > CHAR_M;
typedef Or<Char<'n'>,Char<'N'> > CHAR_N;
typedef Or<Char<'o'>,Char<'O'> > CHAR_O;
typedef Or<Char<'p'>,Char<'P'> > CHAR_P;
typedef Or<Char<'q'>,Char<'Q'> > CHAR_Q;
typedef Or<Char<'r'>,Char<'R'> > CHAR_R;
typedef Or<Char<'s'>,Char<'S'> > CHAR_S;
typedef Or<Char<'t'>,Char<'T'> > CHAR_T;
typedef Or<Char<'u'>,Char<'U'> > CHAR_U;
typedef Or<Char<'v'>,Char<'V'> > CHAR_V;
typedef Or<Char<'w'>,Char<'W'> > CHAR_W;
typedef Or<Char<'x'>,Char<'X'> > CHAR_X;
typedef Or<Char<'y'>,Char<'Y'> > CHAR_Y;
typedef Or<Char<'z'>,Char<'Z'> > CHAR_Z;
typedef Or<Char<' '>,Char<':'> > CHAR_STOP;
typedef Char<'/'> CHAR_SL;
typedef Char<'0'> CHAR_0;
typedef Char<'1'> CHAR_1;
typedef Char<'2'> CHAR_2;
typedef Char<'3'> CHAR_3;
typedef Char<'4'> CHAR_4;
typedef Char<'5'> CHAR_5;
typedef Char<'6'> CHAR_6;
typedef Char<'7'> CHAR_7;
typedef Char<'8'> CHAR_8;
typedef Char<'9'> CHAR_9;


// HTTP Rule

typedef Or<Char<'0'>,Char<'1'> > BIT;
typedef In<0x00, 0xff> OCTET;
typedef In<0x01,0x7f > CHAR;
typedef In<'A','Z'> UPALPHA;
typedef In<'a','z'> LOALPHA;
typedef Or<UPALPHA,LOALPHA> ALPHA;
typedef In<'0','9'> DIGIT;
typedef In<0x00,0x1f > CTL;

typedef Char<' '> BLANK;
typedef And<Char<'\r'>,Char<'\n'>> NEWLINE;

typedef Char<'/r'> CR;
typedef Char<'/n'> LF;
typedef Char<' '> SP;
typedef Char<'\t'> HT;

typedef Char<'\"'> DQUOTE;

typedef And<CR,LF> CRLF;

typedef Or<CHAR_A,CHAR_B,CHAR_C,CHAR_D,CHAR_E,CHAR_F,DIGIT> HEX;

typedef Or<Char<'+'>,Char<'*'> > SpeChar;
typedef In<'0','9'> AllNum;
typedef Or<In<'0','9'>,In<'A','E'>,In<'a','e'> > HEXDIG;
typedef Or<In<'0','9'>,In<'a','e'> > LHEX;
typedef Or<SP, HT > WSP;
typedef Or<CRLF, And<PlusRepeat<WSP>,CRLF > >lws1;
typedef Or<And<lws1,PlusRepeat<WSP> >,PlusRepeat<WSP> > LWS;
typedef ForCheck<0,1,LWS > SWS;
typedef Or<And<
               Char<':'>,
			   SWS >,
           And<
		       PlusRepeat<WSP>,
			   Char<':'>,SWS >
		   > HCOLON;
// define rfc_1035
typedef ALPHA letter;
typedef AllNum digit;
typedef Or<letter,digit > let_dig;
typedef Or<let_dig,Char<'-'> > let_dig_hyp;
typedef PlusRepeat<let_dig_hyp > ldh_str;
typedef Or<And<ldh_str,let_dig>,let_dig > label_1;
typedef Or<And<letter,label_1 >,letter > label;
typedef Or<
           And<
               PlusRepeat<
                          And<label,Char<'.'> >
                         >,
               label
              >,
           label
          > subdomain;
typedef Or<subdomain, Char<' '> > domain;


typedef Or<In<0xc0,0xdf>,In<0xe0,0xef>,In<0xf0,0xf7>,
           In<0xf8,0xfb>,In<0xfc,0xfd> > UTF8_NONASCII;
typedef In<0x80,0xbf > UTF8_CONT;
typedef Or<In<0x21,0x7e>, UTF8_NONASCII > TEXT_UTF8char;
typedef Or<TEXT_UTF8char, And<PlusRepeat<LWS>,TEXT_UTF8char > > part1;
typedef And<PlusRepeat<TEXT_UTF8char>, PlusRepeat<part1> > part2;
typedef Or<part2, PlusRepeat<TEXT_UTF8char > > TEXT_UTF8_TRIM;
typedef And<SWS, DQUOTE > LDQUOT;
typedef And<DQUOTE, SWS > RDQUOT;
typedef And<SWS, Char<'*'>, SWS > STAR;
typedef And<SWS, Char<'/'>, SWS > SLASH;
typedef And<SWS, Char<'='>, SWS > EQUAL;
typedef And<SWS, Char<'('>, SWS > LPAREN;
typedef And<SWS, Char<')'>, SWS > RPAREN;
typedef And<Char<'>'>, SWS > RAQUOT;
typedef And<SWS, Char<'<'> > LAQUOT;
typedef And<SWS, Char<','> ,SWS > COMMA;
typedef And<SWS, Char<';'> ,SWS > SEMI;

typedef Or<ALPHA, DIGIT > AllChar;

typedef And<PlusRepeat<AllNum >, Char<'.'>,
            PlusRepeat<AllNum >, Char<'.'>,
            PlusRepeat<AllNum >, Char<'.'>,
            PlusRepeat<AllNum > > hostNum;
typedef And<PlusRepeat< And< PlusRepeat<AllChar >, Char<'.'> > >,
            PlusRepeat<AllChar > > hostAlpha;

typedef And<And<Or<SpeChar,AllChar>,Not<Char<'<'> >,Not<Char<' '> > >,
            PlusRepeat<AllChar>,
            Or<Char<'@'>,Char<';'>,Char<'<'> >
        > telNum_1;

typedef Or<SP, Char<';'>,Char<','>,Char<'\n'>,Char<'\r'> > sipNewItem;
typedef And<Or<hostNum,hostAlpha >,Or<Char<':'>,sipNewItem,Char<'>'> > > hostaddr;
typedef Or<hostNum,hostAlpha > hostaddr1;
typedef And<Char<':'>, PlusRepeat<AllNum >, sipNewItem> port;
typedef And<Char<':'>, PlusRepeat<AllNum > > port1;

typedef Or<And<Char<'\r'>,Char<'\n'> >,Char<'\n'> > cnrl;

// basic define
typedef Or<AllChar,Char<'-'>,Char<'.'>,Char<'!'>,Char<'%'>,
           Char<'*'>,Char<'_'>,Char<'+'>,Char<'\''>,Char<'~'> >token_ch;
typedef PlusRepeat<token_ch > token;

typedef Or<Char<'('>,Char<'>'>,Char<'<'>,Char<'>'>,Char<'@'>,
           Char<','>,Char<';'>,Char<':'>,Char<'\\'>,DQUOTE,
           Char<'/'>,Char<'['>,Char<']'>,Char<'?'>,Char<'='>,
           Char<'{'>,Char<'}'>,SP,HT > seperators;
typedef Or<AllChar,Char<'-'>,Char<'.'>,Char<'!'>,Char<'%'>,
           Char<'*'>,Char<'_'>,Char<'+'>,Char<'`'>,Char<'\''>,
           Char<'~'>,Char<'('>,Char<')'>,Char<'<'>,Char<'>'>,
           Char<':'>,Char<'\\'>,DQUOTE,Char<'/'>,Char<'['>,
           Char<']'>,Char<'?'>,Char<'{'>,Char<'}'> > word;

typedef Or<Char<' '>,Char<'\t'>,Char<'\''>,
           Char<'\"'>,Char<','>,Char<'^'>,
		   Char<'#'>,Char<'<'>,Char<'>'> > special;

typedef Or<Char<';'>,Char<'/'>,Char<'?'>,Char<':'>,
           Char<'@'>,Char<'&'>,Char<'='>,Char<'+'>,
           Char<'$'>,Char<','> > reserved;
typedef Or<Char<'-'>,Char<'_'>,Char<'.'>,Char<'!'>,
           Char<'~'>,Char<'*'>,Char<'\''>,Char<'('>,
           Char<')'> > mark;
typedef Or<AllChar, mark > unreserved;

typedef And<Char<'%'>, HEXDIG, HEXDIG > escaped;
typedef Or<ALPHA,DIGIT > alphanum;

typedef And<Char<'\\'>,Or<In<0x00,0x09>,In<0x0B,0x0C>,In<0x0E,0x7F> > > quoted_pair;
typedef Or<LWS,Char<0x32>,In<0x23,0x5b>,In<0x5d,0x7e>,UTF8_NONASCII > qdtext;

//typedef Or<LWS,Char<0x21>,In<0x23,0x5b>,In<0x5d,0x7e>, UTF8_NONASCII;

typedef Or<qdtext,quoted_pair > quote_str;
typedef And<SWS,DQUOTE,PlusRepeat<quote_str >,DQUOTE > quoted_string;

typedef Or<In<0x21,0x27>,In<0x2a,0x5b>,In<0x5d,0x7e>,UTF8_NONASCII,LWS > ctext;

typedef Or<And<LPAREN,RPAREN>,
           And<LPAREN,PlusRepeat<Or<ctext,quoted_pair > >,RPAREN >
           > comment;
////////////////////////////////////////////////////
// rfc_2806 -- start
////////////////////////////////////////////////////
typedef Or<Char<'*'>,Char<'#'>,CHAR_A,CHAR_B,CHAR_C,CHAR_D > dtmf_digit;
typedef Char<'w'> wait_for_dial_tone;
typedef Char<'p'> one_second_pause;
typedef Or<wait_for_dial_tone,one_second_pause> pause_ch;
typedef pause_ch pause_character;
typedef Or<Char<'-'>,Char<'.'>,Char<'('>,Char<')'> >visual_separator;
typedef Or<DIGIT,visual_separator > phonedigit;
typedef PlusRepeat<phonedigit > base_phone_number;
typedef And<Char<';'>,Char<'p'>,Char<'o'>,Char<'s'>,
            Char<'t'>,Char<'d'>,Char<'='> > postd;
typedef And<postd, PlusRepeat<Or<phonedigit,dtmf_digit > > > post_dial;
typedef And<Char<';'>,Char<'i'>,Char<'s'>,Char<'u'>,
            Char<'b'>,Char<'='> > isub;
typedef And<isub, PlusRepeat<phonedigit> > isdn_address;
typedef isdn_address isdn_subaddress;
typedef And<Char<'+'>,PlusRepeat<phonedigit> > global_phone_num;
typedef PlusRepeat<Or<phonedigit,dtmf_digit,pause_ch> >local_phone_num;
typedef global_phone_num global_phone_number;
typedef local_phone_num local_phone_number;
typedef Or<global_phone_num, local_phone_num > telephone_subscriber;
typedef Or<Char<0x21>,In<0x23,0x27>,In<0x2a,0x2b>,
           In<0x2d,0x2e>,In<0x30,0x39>,In<0x41,0x5a>,
           In<0x5e,0x7a>,Char<0x7c>,Char<0x7e> > token_char; // rfc-2396
typedef And<Char<';'>,PlusRepeat<token_char> > fe1;
typedef PlusRepeat<token_char > fe2;
typedef And<Char<'?'>,PlusRepeat<token_char> > fe3;
typedef quoted_string fe4;
typedef Or<And<fe2,fe3 >,fe2 > fe5;
typedef And<Char<'='>,Or<fe5,quoted_string > > fe6;
typedef Or<And<fe1,fe6>,fe1 > future_extension;
typedef domain  provider_hostname;
typedef And<Char<'t'>,Char<'s'>,Char<'p'> > provider_tag;
typedef And<Char<';'>,provider_tag,Char<'='>,provider_hostname > service_provider;
typedef PlusRepeat<Or<phonedigit,dtmf_digit,pause_character > >lacal_neteotk_prefix;
typedef And<Char<'t'>,Char<'e'>,Char<'l'> > telephone_scheme;
typedef And<telephone_scheme,Char<';'>,telephone_subscriber> telephone_uri;
////////////////////////////////////////////////////
// rfc_2806 -- end
////////////////////////////////////////////////////

////////////////////////////////////////////////////
// media template -- start
////////////////////////////////////////////////////
typedef token ietf_token;
typedef token iana_token;
typedef And<And<Char<'x'>,Char<'-'> >, token > x_token;
typedef And<CHAR_T,CHAR_E,CHAR_X,CHAR_T > dtype1;
typedef And<CHAR_I,CHAR_M,CHAR_A,CHAR_G,CHAR_E > dtype2;
typedef And<CHAR_A,CHAR_U,CHAR_D,CHAR_I,CHAR_O > dtype3;
typedef And<CHAR_V,CHAR_I,CHAR_D,CHAR_E,CHAR_O > dtype4;
typedef And<CHAR_A,CHAR_P,CHAR_P,CHAR_L,CHAR_I,
            CHAR_C,CHAR_A,CHAR_T,CHAR_I,CHAR_N > dtype5;
typedef Or<ietf_token,x_token > extension_token;
typedef Or<extension_token, iana_token > m_subtype;

typedef Or<token, quoted_string > m_value;
typedef token m_attribute;

typedef And<m_attribute,EQUAL,m_value > m_parameter;

typedef And<CHAR_M,CHAR_E,CHAR_S,CHAR_S,CHAR_A,
            CHAR_G,CHAR_E > t_message;
typedef And<CHAR_M,CHAR_U,CHAR_L,CHAR_T,CHAR_I,
            CHAR_P,CHAR_A,CHAR_R,CHAR_T > t_multipart;

typedef Or<t_message,t_multipart,extension_token > composite_type;
typedef Or<dtype1,dtype2,dtype3,dtype4,dtype5,extension_token > discrete_type;
typedef Or<discrete_type, composite_type > m_type;

typedef And<m_type,SLASH,m_subtype> m_type1;
typedef Or<And<m_type1,PlusRepeat<And<SEMI,m_parameter> > >,m_type1 > media_type;

typedef Or<unreserved,escaped,Char<':'>,Char<'@'>,
           Char<'&'>,Char<'='>,Char<'+'>,Char<'$'>,
           Char<','> > pchar;
typedef ForCheck<0,100,pchar > param;
typedef Or<And<param,
               PlusRepeat<And<Char<';'>,param > >
              >,
           param > segment;
typedef Or<And<segment,
               PlusRepeat<And<Char<'/'>,segment> >
              >,
           segment > path_segments;

typedef PlusRepeat<Or<unreserved,escaped,Char<'$'>,Char<','>,
                      Char<';'>,Char<':'>,Char<'@'>,Char<'&'>,
                      Char<'='>,Char<'+'> > > reg_name;
typedef Or<reserved,unreserved,escaped > uric;
typedef Or<unreserved,escaped,Char<';'>,Char<'?'>,
           Char<':'>,Char<'@'>,Char<'&'>,Char<'='>,
           Char<'+'>,Char<'$'>,Char<','> > uric_no_slash;
typedef ForCheck<0,200,uric > query;
typedef Or<And<uric_no_slash,PlusRepeat<uric> >, uric_no_slash > opaque_part;
typedef Or<AllChar,Char<'+'>,Char<'-'>,Char<'.'> > scheme_ch;
typedef Or< And<ALPHA,
                PlusRepeat<scheme_ch>
               >,
            ALPHA > scheme;
typedef ForCheck<1,3,DIGIT> ip4num;
typedef And<ip4num,Char<'.'>,
            ip4num,Char<'.'>,
            ip4num,Char<'.'>,ip4num > IPv4address;
typedef ForCheck<1, 12, DIGIT > port_l;
typedef ForCheck<1, 4, HEXDIG > hex4;
typedef Or<And<
               hex4,
               PlusRepeat<And<Char<':'>,hex4> >
              >,
              hex4
          > hexseq;

typedef Or<alphanum,Char<'-'> > toplabel1;
typedef And<ALPHA,PlusRepeat<toplabel1> > toplabel2;
typedef Or<toplabel2,ALPHA > toplabel;

typedef And<alphanum,
            PlusRepeat<toplabel1 > > domainlabel2;
typedef Or<domainlabel2,alphanum > domainlabel;

typedef Or<
           And<
               toplabel,
               Char<'.'>
              >,
           toplabel
        > hostname_1;

typedef And<PlusRepeat< And<toplabel,Char<'.'> > >, toplabel > hostname_2;
typedef Or< hostname_2, toplabel > hostname;

typedef Or<hostname, IPv4address > host;

typedef Or<And<host,Char<':'>,port>,host > hostport;
typedef Or<Char<'&'>,Char<'='>,Char<'+'>,Char<'$'>,
           Char<','>,Char<';'>,Char<'?'>,Char<'/'> > user_unreserved;
typedef And<unreserved,escaped,user_unreserved > user_l;
typedef PlusRepeat<user_l > user;
typedef Or<unreserved,escaped,Char<'&'>,Char<'='>,
           Char<'+'>,Char<'$'>,Char<','> > password_ch;
typedef ForCheck<0,100,password_ch > password;
typedef Or<user,telephone_subscriber> userinfo_prefix;
typedef Or<And<Char<';'>,password,Char<'@'> >,Char<'@'> > userinfo_tail;
typedef And<userinfo_prefix,userinfo_tail > userinfo;
typedef Or<And<userinfo,Char<'@'>,hostport>,hostport > srvr1;
typedef ForCheck<0,10,srvr1> srvr;
typedef Or<srvr,reg_name > authority;
typedef And<Char<'/'>,path_segments > abs_path;
typedef And<Char<'/'>,Char<'/'>, authority> net_path1;
typedef Or<And<net_path1,abs_path>, net_path1 > net_path;
typedef And<Char<'?'>,query > hier_part1;
typedef Or<net_path,abs_path> hier_part2;
typedef Or<And<hier_part2,hier_part1>,hier_part2> hier_part;

typedef Or<token,host,quoted_string > gen_value;
typedef Or<And<token,EQUAL,gen_value>,token > generic_param;


typedef Or<In<0x21,0x22>,In<0x24,0x27>,Char<0x2c>,Char<0x2f>,Char<0x3a>,
        In<0x3c,0x40>,In<0x45,0x4f>,In<0x51,0x56>,In<0x58,0x60>,
        In<0x65,0x6f>,In<0x71,0x76>,In<0x78,0x7e> > private_prefix1;
typedef Or<And<private_prefix1,PlusRepeat<Or<In<0x21,0x3a>,In<0x3c,0x7e> > > >,
           private_prefix1 > private_prefix;

typedef And<scheme,Char<':'>,Or<hier_part,opaque_part> > absoluteURI;

// rfc 2833
//
typedef Or< Char<'.'>,Char<'?'>,Char<':'>,Char<'/'>,
			Char<'='>,Char<'+'>,Char<'-'>,Char<'*'>,
			Char<'&'>,Char<'%'>,Char<'!'>,Char<'~'>,
			Char<'@'> > SigChar;
typedef Char<';'>  fieldSeperator;
typedef Or<ALPHA, DIGIT, SigChar> VisibleChar;

// Basic Rule
typedef Or<ALPHA,DIGIT> alphnum;

typedef Or< Char<';'>, Char<'/'>, Char<'?'>, Char<':'>,
            Char<'@'>, Char<'&'>, Char<'='>, Char<'+'>,
            Char<'$'>, Char<','> > reserved;

typedef Or< Char<'-'>, Char<'_'>, Char<'.'>, Char<'!'>,
            Char<'~'>, Char<'*'>, Char<'\''>, Char<'('>,
            Char<')'>  > mark;

typedef Or<alphanum, mark> unreserved;
}


