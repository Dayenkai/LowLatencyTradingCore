#ifndef     _UTILS_H
#define     _UTILS_H

#include    "CppStandard.h"

void        pinThreadToCore(pthread_t &thread, uint32_t   &coreId);

#endif