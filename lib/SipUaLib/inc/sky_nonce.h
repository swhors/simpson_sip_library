#pragma once

#if defined WIN32 || defined LINUX || defined MACOS
char * build_nonce(void);
long check_nonce(char *nonce);
void init_nonce();
void close_nonce();
#elif defined WINCE
char * build_nonce(void){return NULL;};
long check_nonce(char *nonce){return 0;};
void init_nonce(){};
void close_nonce(){};
#endif

