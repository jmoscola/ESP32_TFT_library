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

#if 0
// xleft         xright                                 xleft  xright
#define TS_CALX_XPT2046    ( (  114 << 16 ) + 1816 )  // 0x720718   0x0072 0x0718    114  1816
// ytop         ybottom                                  ytop  ybottom
#define TS_CALY_XPT2046    ( ( 1865 << 16 ) +  154 )  // 0x749009A  0x0749 0x009A   1865   154
#else
// the parameters may vary depending on the device.
// the parameter was taken fron the middle of each edge
// at the corner we get other values

// xleft         xright
#define TS_CALX_XPT2046    ( (  140 << 16 ) + 3900 )
// ytop         ybottom
#define TS_CALY_XPT2046    ( (  754 << 16 ) + 3785 )
#endif


int xpt2046_get_touch( int *x, int *y );
void xpt2046_test_touch();

//----------------------------------------------------------
//
//----------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif   // _XPT2046_H_
