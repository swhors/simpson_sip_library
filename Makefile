##############################
# skyproxyd make file
# date : 02/17/2005
# 
# swhors@kornet.net
#
# __________________________
# Copyright (c) 2005. SKYCOM
# All Rights Reserved.      
# __________________________

include Makefile.inc

all:
	##############################
	#                            #
	# Start to make skyproxyd.   #
	#                            #
	# __________________________ #
	# Copyright (c) 2005. SKYCOM #
	# All Rights Reserved.       #
	# __________________________ #
	#                            #
	##############################
	# Make libsiphash.so         #
	##############################
	cd hash_library; make
	##############################
	# Make libskycm.so           #
	##############################
	cd sky_common_lib; make
	##############################
	# Make libskysip.so          #
	##############################
	cd sip_parse; make
	##############################
	# Make interface library     #
	##############################
	cd interface; make
	##############################
	# Make libskyconf.so         #
	##############################
	cd sky_conf_lib; make
	##############################
	# make example program.      #
	##############################
	#cd libtest; make
	##############################
	# make libskyregister.so     #
	##############################
	cd skyregister; make
	##############################
	# make libskysipproxy        #
	##############################
	cd skyproxylib; make
	##############################
	# make cgi_bin               #
	##############################
	cd cgi_bin; make
	##############################
	# make skyproxymd            #
	##############################
	cd skyproxyd_manager; make
	##############################
	#                            #
	# End to make skyproxyd.     #
	#                            #
	# __________________________ #
	# Copyright (c) 2005. SKYCOM #
	# All Rights Reserved.       #
	# __________________________ #
	#                            #
	##############################
clean:
	##############################
	# make start..               #
	##############################
	cd hash_library; make clean
	cd sky_common_lib; make clean
	cd sip_parse; make clean
	cd interface; make clean
	cd sky_conf_lib; make clean
	cd libtest; make clean
	cd skyregister; make clean
	cd skyproxylib; make clean
	cd skyproxyd; make clean
	cd cgi_bin; make clean
	cd skyproxyd_manager; make clean
	#$(RM) lib/*.so bin/skyproxyd bin/core*
	##############################
	# make end...                #
	##############################
