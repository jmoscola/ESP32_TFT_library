#ifndef _XPT2046_H_
#define _XPT2046_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------
//
//----------------------------------------------------------

#define Z_THRESHOLD        400
#define Z_THRESHOLD_INT    75
#define XPT2046_SMP_MAX   4095

int xpt2046_get_touch( int *x, int *y );
void xpt2046_test_touch();

//----------------------------------------------------------
//
//----------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif   // _XPT2046_H_
