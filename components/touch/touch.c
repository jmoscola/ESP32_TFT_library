// -----------------------------------------------------------------------------
//
// 2019-09-21  AWe   initial version, move touch parts from tft.c into this file
//
// -----------------------------------------------------------------------------

#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"       // vTaskDelay()
#include "driver/gpio.h"
#include "tftspi.h"
#include "spi_master_lobo.h"     // spi_lobo_device_handle_t
#include "touch.h"
#include "xpt2046.h"
#include "stmpe610.h"

//----------------------------------------------------------
//
//----------------------------------------------------------

// Spi device handles for touch screen
spi_lobo_device_handle_t tft_ts_spi = NULL;

int ts_orientation = PORTRAIT;

//----------------------------------------------------------
//
//----------------------------------------------------------

void TS_PinsInit()
{
    // Route pins to GPIO control
#if TOUCH_TYPE
    gpio_pad_select_gpio(PIN_NUM_TCS);
    gpio_set_direction(PIN_NUM_TCS, GPIO_MODE_OUTPUT);
#endif
}

//----------------------------------------------------------
//
//----------------------------------------------------------

void TS_setRotation( uint8_t rot )
{
    ts_orientation = rot % 4;
}

//----------------------------------------------------------
//
//----------------------------------------------------------

int TS_get_touch( int *x, int *y, uint8_t raw )
{
    *x = 0;     // display coordinates
    *y = 0;
    
    if( tft_ts_spi == NULL ) return 0;
#if TOUCH_TYPE == TOUCH_TYPE_NONE
    return 0;
#else
    int result = -1;
    int ts_x = 0, ts_y = 0;    // touch sensor coordinates
    
#if TOUCH_TYPE == TOUCH_TYPE_XPT2046
    uint32_t tft_ts_calx = TS_CALX_XPT2046;
    uint32_t tft_ts_caly = TS_CALY_XPT2046;
    result = xpt2046_get_touch( &ts_x, &ts_y );
    if( result == 0 )
    {
        return 0;
    }
#elif TOUCH_TYPE == TOUCH_TYPE_STMPE610
    uint32_t tft_ts_calx = TS_CALX_STMPE610;
    uint32_t tft_ts_caly = TS_CALY_STMPE610;
    uint16_t Xx, Yy, Z = 0;
    result = stmpe610_get_touch( &Xx, &Yy, &Z );
    if( result == 0 )
        return 0;
    ts_x = Xx;
    ts_y = Yy;
#else
    return 0;
#endif
    
    // touch sensor pressed
    if( raw )
    {
        *x = ts_x;
        *y = ts_y;
        
        return 1;
    }
    
    // Calibrate the result
    int xleft   = ( tft_ts_calx >> 16 ) & 0x3FFF;
    int xright  = tft_ts_calx & 0x3FFF;
    int ytop    = ( tft_ts_caly >> 16 ) & 0x3FFF;
    int ybottom = tft_ts_caly & 0x3FFF;
    
    if( ( ( xright - xleft ) <= 0 ) || ( ( ybottom - ytop ) <= 0 ) )
    {
        return 0;
    }
    
#if TOUCH_TYPE == TOUCH_TYPE_XPT2046
    // In normal orientation of the display is portrait. The origin is
    // the upper left corner while the connector are at bottom edge.
    // The touch sensor normal orientation is landscape. The origin is
    // also the upper left corner while the connetor is at the left edge.
    // This means that the touch sensor is rotated 90 degrees clockwise
    // to the display.
    
    // in normal orientation of the display the width is smaller than height
    // when display is rotated to landscape width is greater than height
    
    // swap for portrait, because of the different normal
    int ts_width = tft_height;       // 240
    int ts_height = tft_width;       // 320
    
    if( tft_width > tft_height )
    {
        ts_width = tft_width;
        ts_height = tft_height;
    }
    
    ts_x = ( ( ts_x - xleft ) * ts_width ) / ( xright - xleft );
    ts_y = ( ( ts_y - ytop ) * ts_height ) / ( ybottom - ytop );
    
    //   printf( "ts_w=%d, ts_h=%d, ts_x=%d, ts_y=%d\n", ts_width, ts_height, ts_x, ts_y );
    
    if( ts_x < 0 ) ts_x = 0;
    if( ts_x > ts_width - 1 ) ts_x = ts_width - 1;
    if( ts_y < 0 ) ts_y = 0;
    if( ts_y > ts_height - 1 ) ts_y = ts_height - 1;
    
    switch( ts_orientation )
    {
            // 2019-09-25  AWe modified
            // same as STMPE610, but ts_height and ts_width swapped
            
        case PORTRAIT:                // 0  o.k.
            *x = ts_y;
            *y = ts_width - ts_x - 1;
            break;
        case LANDSCAPE:               // 1  o.k.
            *x = ts_width - ts_x - 1;
            *y = ts_height - ts_y - 1;
            break;
        case PORTRAIT_FLIP:           // 2  o.k.
            *x = ts_height - ts_y - 1;
            *y = ts_x;
            break;
        case LANDSCAPE_FLIP:          // 3  o.k.
            *x = ts_x;
            *y = ts_y;
            break;
    }
    
#elif TOUCH_TYPE == TOUCH_TYPE_STMPE610
    // 2019-09-25 AWe not changed, not tested
    
    int ts_width = tft_width;
    int ts_height = tft_height;
    
    if( tft_width > tft_height )
    {
        ts_width = tft_height;
        ts_height = tft_width;
    }
    ts_x = ( ( ts_x - xleft ) * ts_width ) / ( xright - xleft );
    ts_y = ( ( ts_y - ytop ) * ts_height ) / ( ybottom - ytop );
    
    if( ts_x < 0 ) ts_x = 0;
    if( ts_x > ts_width - 1 ) ts_x = ts_width - 1;
    if( ts_y < 0 ) ts_y = 0;
    if( ts_y > ts_height - 1 ) ts_y = ts_height - 1;
    
    switch( ts_orientation )
    {
        case PORTRAIT:                   // 0
            *x = ts_x;
            *y = ts_y;
            break;
        case LANDSCAPE:                  // 1
            *x = ts_x;
            *y = ts_width - ts_x - 1;
            break;
        case PORTRAIT_FLIP:              // 2
            *x = ts_width - ts_x - 1;
            *y = ts_height - ts_y - 1;
            break;
        case LANDSCAPE_FLIP:             // 3
            *x = ts_height - ts_y - 1;
            *y = ts_x;
            break;
    }
#endif
    
    return 1;
#endif
}

//----------------------------------------------------------
//
//----------------------------------------------------------

int TS_test_touch( void )
{
    int tx, ty;
    
    printf( "Run touch sensor test ...\n" );
    
    while( 1 )
    {
        if( TS_get_touch( &tx, &ty, 0 ) )
        {
            // Touched
            printf( "Touched x=%d y=%d\n", tx, ty );
        }
        
        vTaskDelay( 10 / portTICK_RATE_MS );
    }
    return 0;
}

//----------------------------------------------------------
//
//----------------------------------------------------------
