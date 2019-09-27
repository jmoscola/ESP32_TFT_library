#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------
//
//----------------------------------------------------------

// pin assignment for touch panel see tftspi.h

//----------------------------------------------------------
//
//----------------------------------------------------------

#define TOUCH_TYPE_NONE       0
#define TOUCH_TYPE_XPT2046    1
#define TOUCH_TYPE_STMPE610   2

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
 * The coordinates are adjusted to screen ts_orientation if raw=0
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