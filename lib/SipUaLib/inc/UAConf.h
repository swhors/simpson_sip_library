///////////////////////////////////////////////////
// UAConf.h
// UA�� ȯ�� ������ �д� �ҽ�
// 2005/09/01
///////////////////////////////////////////////////

#pragma once

#ifndef __UACONF_H
#define __UACONF_H

#include "skysipua_g.h"

class SKYLIB CUAConf
{
public:
    CUAConf();
    ~CUAConf();
    int InitValue();
private:
    char proxyip[24];
    int  proxyport;
    char localhostip[24];
    int  localhostport;
};

#endif // __UACONF_H


