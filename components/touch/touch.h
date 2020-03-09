#ifndef _TOUCH_H_
#define _TOUCH_H_

#include <stdlib.h>
#include "stmpe610.h"
#include "xpt2046.h"

#define TFT_LIB_TOUCH_TAG      "TFT_LIB_TOUCH"

#ifdef __cplusplus
extern "C" {
#endif


//----------------------------------------------------------
//
//----------------------------------------------------------

#define TOUCH_TYPE_NONE       0
#define TOUCH_TYPE_XPT2046    1
#define TOUCH_TYPE_STMPE610   2


//----------------------------------------------------------
//
//----------------------------------------------------------

// pin assignment for touch panel see tftspi.h

//----------------------------------------------------------
//
//----------------------------------------------------------

#if CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 1
// ** Set the touch configuration for ESP-WROVER-KIT v3
// --------------------------------------------------------
#define TOUCH_TYPE      TOUCH_TYPE_NONE
#define PIN_NUM_TCS     0       // Touch screen CS pin
// --------------------------------------------------------

#elif CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 2
// ** Set the touch configuration for Adafruit TFT Feather
// ---------------------------------------------------------
#define TOUCH_TYPE      TOUCH_TYPE_STMPE610
#define PIN_NUM_TCS     32      // Touch screen CS pin (NOT used if TOUCH_TYPE=0)
// ---------------------------------------------------------

#elif CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 3
// ** Set the touch configuration for M5Stack TFT
// ---------------------------------------------------------
#define TOUCH_TYPE      TOUCH_TYPE_NONE
#define PIN_NUM_TCS     0       // Touch screen CS pin (NOT used if TOUCH_TYPE=0)
// ---------------------------------------------------------

#elif CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 4
// ** Set the touch configuration for ESP-WROVER-KIT v4.1
// --------------------------------------------------------
#define TOUCH_TYPE      TOUCH_TYPE_NONE
#define PIN_NUM_TCS     0       // Touch screen CS pin
// --------------------------------------------------------

#else // all other display types
// ** Set the touch configuration for other boards
// --------------------------------------------------------
#define TOUCH_TYPE      CONFIG_TFT_TOUCH_CONTROLLER
#define PIN_NUM_TCS     CONFIG_TFT_PIN_NUM_TCS
// --------------------------------------------------------

#endif  // CONFIG_PREDEFINED_DISPLAY_TYPE


#define USE_TOUCH       TOUCH_TYPE  // 0 if TOUCH_TYPE == TOUCH_TYPE_NONE


//----------------------------------------------------------
//
//----------------------------------------------------------

// ==== Spi device handles for touch screen =========
extern spi_lobo_device_handle_t tft_ts_spi;

//----------------------------------------------------------
//
//----------------------------------------------------------


/*
 * Initialize pins used by display driver
 * ** MUST be executed before SPI interface initialization
 */
//----------------------------------------------
void TS_PinsInit();


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
