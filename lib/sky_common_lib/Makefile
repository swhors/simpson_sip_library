include ../Makefile.inc

TARGET=libskycm.so

OBJECT=$(SRCDIR)/sky_file.o\
	$(SRCDIR)/sky_string.o\
	$(SRCDIR)/sky_pstring.o\
	$(SRCDIR)/sky_sleep.o\
	$(SRCDIR)/sky_debug.o\
	$(SRCDIR)/sky_thread.o\
	$(SRCDIR)/sky_lock.o\
        $(SRCDIR)/sky_sock.o\
	$(SRCDIR)/sky_hash_4.o

CFLAGS=$(DBGLEVEL)
CFLAGS+=$(INCDIR)
CFLAGS+=$(OSDEF)
CFLAGS+=$(LIBFLAGS)

all: $(OBJECT)
	$(CC) $(CFLAGS) -o $(LIBTARGETDIR)/$(TARGET) $(OBJECT)
	$(RM) $(OBJECT)

$(SRCDIR)/sky_file.o:$(SRCDIR)/sky_file.c
$(SRCDIR)/sky_string.o:$(SRCDIR)/sky_string.c
$(SRCDIR)/sky_pstring.o:$(SRCDIR)/sky_pstring.c
$(SRCDIR)/sky_sleep.o:$(SRCDIR)/sky_sleep.c
$(SRCDIR)/sky_debug.o:$(SRCDIR)/sky_debug.c
$(SRCDIR)/sky_hash_4.o:$(SRCDIR)/sky_hash_4.c
$(SRCDIR)/sky_lock.o:$(SRCDIR)/sky_lock.c
$(SRCDIR)/sky_sock.o:$(SRCDIR)/sky_sock.c
$(SRCDIR)/sky_thread.o:$(SRCDIR)/sky_thread.c

clean:
	$(RM) $(OBJECT)
