############################################################
############################################################
CC=gcc
RM=rm -f

OS=LINUX

INCDIR=-I ./inc -I ../sip_parse/inc -I ../skyproxyd/inc -I ../skyregister/inc -I ../sky_common_lib/inc -I ../hash_library/inc -I ../sky_conf_lib/inc -I ../interface/inc -I ../skyproxylib/inc

OSDEF=-D $(OS)

DBG_DEF0=
DBG_DEF1=-D DBG_LEVEL1
DBG_DEF2=-D DBG_LEVEL1 -D DBG_LEVEL2
DBG_DEF3=-D DBG_LEVEL1 -D DBG_LEVEL2 -D DBG_LEVEL3
DBG_DEF4=-D DBG_LEVEL1 -D DBG_LEVEL2 -D DBG_LEVEL3 -D DBG_LEVEL4

DBGLEVEL=-D DBG_LEVEL3 -g
#DBGLEVEL=-D DBG_LEVEL3 

LIBDIR=-L /usr/local/lib/skysip
BINDEFLIB=-lskysip -lpthread -lsiphash -lskyregister -lskycm -lskyconf -lskyinter -lskysipproxy

SRCDIR=./src

LIBTARGETDIR=/usr/local/lib/skysip
#BINTARGETDIR=../bin
BINTARGETDIR=/opt/skyproxy/bin
MANTARGETDIR=/opt/skyoroxy/man

LIBFLAGS=-fPIC -shared -DMAIN_TERMINAL_MODE

