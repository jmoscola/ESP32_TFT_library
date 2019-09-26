#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------
//
//----------------------------------------------------------

extern uint32_t tft_tp_calx;        // touch screen X calibration constant
extern uint32_t tft_tp_caly;        // touch screen Y calibration constant

// pin assignment for touch panel see tftspi.h

//----------------------------------------------------------
//
//----------------------------------------------------------

#define TOUCH_TYPE_NONE       0
#define TOUCH_TYPE_XPT2046    1
#define TOUCH_TYPE_STMPE610   2

#if 0
                           // xleft         xright                                 xleft  xright
#define TS_CALX_XPT2046    ( (  114 << 16 ) + 1816 )  // 0x720718   0x0072 0x0718    114  1816
                           // ytop         ybottom                                  ytop  ybottom
#define TS_CALY_XPT2046    ( ( 1865 << 16 ) +  154 )  // 0x749009A  0x0749 0x009A   1865   154
#else
                           // xleft         xright
#define TS_CALX_XPT2046    ( (  200 << 16 ) + 4000 )
                           // ytop         ybottom
#define TS_CALY_XPT2046    ( (  800 << 16 ) + 3800 )
#endif

#define TS_CALX_STMPE610   ( (  326 << 16 ) + 3796 )  // 0x1460ED4  0x0146 0x0ED4    326  3796
#define TS_CALY_STMPE610   ( (  180 << 16 ) + 3664 )  // 0xB40E50   0x00B4 0x0E50    180  3664

// --------------------------------------------------------------

#define USE_TOUCH CONFIG_TFT_TOUCH_CONTROLLER

//----------------------------------------------------------
//
//----------------------------------------------------------

// ==== Spi device handles for touch screen =========
extern spi_lobo_device_handle_t tft_ts_spi;

//----------------------------------------------------------
//
//----------------------------------------------------------

/*
 * Get the touch panel coordinates.
 * The coordinates are adjusted to screen tp_orientation if raw=0
 *
 * Params:
 *       x: pointer to X coordinate
 *       y: pointer to Y coordinate
 *      raw: if 0 returns calibrated screen coordinates; if 1 returns raw touch controller coordinates
 *
 * Returns:
 *       0 if touch panel is not touched; x=y=0
 *       1 if touch panel is touched; x&y are the valid coordinates
 */
//----------------------------------------------
int TS_get_touch( int *x, int *y, uint8_t raw );

void TS_setRotation( uint8_t rot );
int TS_test_touch( void );

//----------------------------------------------------------
//
//----------------------------------------------------------
#ifdef __cplusplus
}
#endif

#endif   // _TOUCH_H_