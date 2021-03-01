#pragma once

void ssleep(int sec, int usec);
#ifdef LINUX
#define Sleep(a) usleep(a)
#endif
