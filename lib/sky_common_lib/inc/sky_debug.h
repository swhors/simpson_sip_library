#pragma once


void file_write_lock_init();
void file_write_lock_release();

void sip_write_log( char * in_ip, char * out_ip, int state, int dbg_level, int req_level);
void write_log(char * sbuff, int dbg_level, int req_level);
void write_log_withtime(char * sbuff, int dbg_level, int req_level);
void write_log_withname(char * name, char * buff, int dbg_level, int req_level);
