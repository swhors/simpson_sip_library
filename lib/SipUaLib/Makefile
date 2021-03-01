include ../Makefile.inc

TARGET=libskyua.so

SRCDIR=./src

INCDIR+=-I./inc

CPPFLAGS=$(INCDIR)
CPPFLAGS+=$(LIBFLAGS)
CPPFLAGS+=$(LIBDIR)
CPPFLAGS+=$(OS)

OBJECTS=$(SRCDIR)/SkySock.o\
       $(SRCDIR)/UAConf.o\
       $(SRCDIR)/SkySipUaManager.o\
       $(SRCDIR)/Rtp.o

all: $(OBJECTS)
	$(CC) $(CPPFLAGS) -o $(LIBTARGETDIR)/$(TARGET) $(OBJECTS)

$(SRCDIR)/SkySock.o:$(SRCDIR)/SkySock.cpp
$(SRCDIR)/UAConf.o:$(SRCDIR)/UAConf.cpp
$(SRCDIR)/SkySipUaManager.o:$(SRCDIR)/SkySipUaManager.cpp
$(SRCDIR)/Rtp.o:$(SRCDIR)/Rtp.cpp

clean:
	rm -f $(SRCDIR)/*.o
