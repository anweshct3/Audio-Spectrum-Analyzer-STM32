/* Martin Thomas 4/2009 */

#ifndef FATTIME_H_
#define FATTIME_H_

/* 22 July 2019 Changes to allow for ChanFAT 0.13c. Remove integer.h */

//#include "integer.h"
#include "ff.h"

DWORD get_fattime (void);

#endif
