include ../Makefile.inc

TARGET=libsiphash.so

CFLAGS=$(INCDIR)
CFLAGS+=$(LIBFLAGS)
CFLAGS+=$(DBGLEVEL)


OBJECT=$(SRCDIR)/md5.o\
	$(SRCDIR)/md4.o

all:$(OBJECT)
	$(CC) $(CFLAGS) -o $(LIBTARGETDIR)/$(TARGET) $(OBJECT)
	$(RM) $(OBJECT)

clean:
	$(RM) $(OBJECT) *.log

$(SRCDIR)/md5.o:$(SRCDIR)/md5.c
$(SRCDIR)/md4.o:$(SRCDIR)/md4.c
