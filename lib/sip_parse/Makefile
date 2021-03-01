## by swhors@skycom.ne.kr
## SIP_PARSE Main Library
include ../Makefile.inc

TARGET=libskysip.so

CFLAGS=$(DBGLEVEL)
CFLAGS+=$(INCDIR)
CFLAGS+=$(OSDEF)
CFLAGS+=$(LIBFLAGS)

OBJECT=$(SRCDIR)/sip_parse.o\
     $(SRCDIR)/sip_method.o\
     $(SRCDIR)/debug.o\
     $(SRCDIR)/sip_via.o\
     $(SRCDIR)/sip_to_from.o\
     $(SRCDIR)/sip_callid.o\
     $(SRCDIR)/sip_contact.o\
     $(SRCDIR)/sip_cseq.o\
     $(SRCDIR)/sip_tool.o\
     $(SRCDIR)/sip_uri.o\
     $(SRCDIR)/sip_field.o\
     $(SRCDIR)/sip_info_display.o\
     $(SRCDIR)/sip_authentication.o\
     $(SRCDIR)/sip_allow.o\
     $(SRCDIR)/sip_record_route.o\
     $(SRCDIR)/sip_answer_code.o\
     $(SRCDIR)/sip_sdp_parse.o\
     $(SRCDIR)/sip_parse_prehead.o

all:$(OBJECT)
	$(CC) $(CFLAGS) -o $(LIBTARGETDIR)/$(TARGET) $(OBJECT)
	$(RM) $(OBJECT)

clean:
	$(RM) $(OBJECT)

$(SRCDIR)/sip_parse.o: $(SRCDIR)/sip_parse.c
$(SRCDIR)/sip_method.o:$(SRCDIR)/sip_method.c
$(SRCDIR)/debug.o:$(SRCDIR)/debug.c
$(SRCDIR)/sip_via.o:$(SRCDIR)/sip_via.c
$(SRCDIR)/sip_to_from.o:$(SRCDIR)/sip_to_from.c
$(SRCDIR)/sip_callid.o:$(SRCDIR)/sip_callid.c
$(SRCDIR)/sip_contact.o:$(SRCDIR)/sip_contact.c
$(SRCDIR)/sip_cseq.o:$(SRCDIR)/sip_cseq.c
$(SRCDIR)/sip_tool.o:$(SRCDIR)/sip_tool.c
$(SRCDIR)/sip_uri.o:$(SRCDIR)/sip_uri.c
$(SRCDIR)/sip_field.o:$(SRCDIR)/sip_field.c
$(SRCDIR)/sip_info_display.o:$(SRCDIR)/sip_info_display.c
$(SRCDIR)/sip_authentication.o:$(SRCDIR)/sip_authentication.c
$(SRCDIR)/sip_allow.o:$(SRCDIR)/sip_allow.c
$(SRCDIR)/sip_record_route.o:$(SRCDIR)/sip_record_route.c
$(SRCDIR)/sip_answer_code.o:$(SRCDIR)/sip_answer_code.c
$(SRCDIR)/sip_sdp_parse.o:$(SRCDIR)/sip_sdp_parse.c
$(SRCDIR)/sip_parse_prehead.o:$(SRCDIR)/sip_parse_prehead.c
