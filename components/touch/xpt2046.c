// -----------------------------------------------------------------------------
//
// 2019-09-21  AWe   initial version, move touch parts from tft.c into this file
//
// -----------------------------------------------------------------------------

#if USE_TOUCH == TOUCH_TYPE_XPT2046

// ============= Touch panel functions =========================================

#include "spi_master_lobo.h"
#include "tftspi.h"              // spi_transfer_start()
#include "touch.h"
#include "xpt2046.h"

//----------------------------------------------------------
//
//----------------------------------------------------------
// get 16-bit data from touch controller for specified type
// ** Touch device must already be selected **

static int IRAM_ATTR xpt2046_get_data( uint8_t type )
{
   spi_lobo_device_select( tft_ts_spi, 0 );

   tft_ts_spi->host->hw->data_buf[0] = type;
   spi_transfer_start( tft_ts_spi, 24, 24 );

   uint32_t data = tft_ts_spi->host->hw->data_buf[0];
   uint16_t _res = ( uint16_t )( data >> 8 );
   // AWe 2019-09-21 swap bytes
   uint16_t res = ( ( _res << 8 ) | (_res >> 8 ) ) >> 3;

   spi_lobo_device_deselect( tft_ts_spi );

   return res;
}

#if 1
// loboris implementation
//-------------------------------------------------------
static int xpt2046_read_data( uint8_t type, int samples )
{
   if( tft_ts_spi == NULL ) return 0;

   int n, result, val = 0;
   uint32_t i = 0;
   uint32_t vbuf[18];
   uint32_t minval, maxval, dif;

   if( samples < 3 ) samples = 1;
   if( samples > 18 ) samples = 18;

   // one dummy read
   result = xpt2046_get_data( type );

   // read data
   while( i < 10 )
   {
      minval = 5000;
      maxval = 0;
      // get values
      for( n = 0; n < samples; n++ )
      {
         result = xpt2046_get_data( type );
         if( result < 0 ) break;

         vbuf[n] = result;
         if( result < minval ) minval = result;
         if( result > maxval ) maxval = result;
      }
      if( result < 0 ) break;
      dif = maxval - minval;
      if( dif < 40 ) break;
      i++;
   }
   if( result < 0 )
   {
      return -1;
   }

   if( samples > 2 )
   {
      // remove one min value
      for( n = 0; n < samples; n++ )
      {
         if( vbuf[n] == minval )
         {
            vbuf[n] = 5000;
            break;
         }
      }
      // remove one max value
      for( n = 0; n < samples; n++ )
      {
         if( vbuf[n] == maxval )
         {
            vbuf[n] = 5000;
            break;
         }
      }
      for( n = 0; n < samples; n++ )
      {
         if( vbuf[n] < 5000 ) val += vbuf[n];
      }
      val /= ( samples - 2 );
   }
   else val = vbuf[0];

   return val;
}

//-----------------------------------------------
// the orignal loboris code  read x from register 0sD0

int xpt2046_get_touch( int *x, int* y )
{
   int res = 0, result = -1;
   if( spi_lobo_device_select( tft_ts_spi, 0 ) != ESP_OK ) return 0;

   result = xpt2046_read_data( 0xB0, 3 ); // Z; pressure; touch detect
   if( result <= 250 ) goto exit;

   // touch panel pressed
   result = xpt2046_read_data( 0x90, 10 );
   if( result < 0 )  goto exit;

   *x = result;

   result = xpt2046_read_data( 0xD0, 10 );
   if( result < 0 )  goto exit;

   *y = result;
   res = 1;

   printf( "res=%d    x=%d, y=%d ", res, *x, *y );
   printf( "\n" );

exit:
   spi_lobo_device_deselect( tft_ts_spi );

   return res;
}

#else
// stoffregen implementation
//-----------------------------------------------

static int besttwoavg( int x, int y, int z )
{
  int da, db, dc;
  int reta = 0;
  if ( x > y ) da = x - y; else da = y - x;
  if ( x > z ) db = x - z; else db = z - x;
  if ( z > y ) dc = z - y; else dc = y - z;

  if ( da <= db && da <= dc ) reta = (x + y) >> 1;
  else if ( db <= da && db <= dc ) reta = (x + z) >> 1;
  else reta = (y + z) >> 1;   //    else if ( dc <= da && dc <= db ) reta = (x + y) >> 1;

  return (reta);
}

int xpt2046_get_touch( int *x, int* y )
{
   int data[6];
   int zraw;

   if( spi_lobo_device_select( tft_ts_spi, 0 ) != ESP_OK ) return 0;

   // idea taken from https://github.com/PaulStoffregen/XPT2046_Touchscreen/blob/master/XPT2046_Touchscreen.cpp
   int z1 = xpt2046_get_data( 0xB1 ); // Z1
   int z2 = xpt2046_get_data( 0xC1 ); // Z2
   int z = z1 + XPT2046_SMP_MAX - z2;

   // dummy X measure, 1st is always noisy
   xpt2046_get_data( 0x91 );              // X
   if (z >= Z_THRESHOLD)
   {
      data[0] = xpt2046_get_data( 0x91 ); // X
      data[1] = xpt2046_get_data( 0xD1 ); // Y
      data[2] = xpt2046_get_data( 0x91 ); // X  // make 3 x-y measurements
      data[3] = xpt2046_get_data( 0xD1 ); // Y
   }
   else
      data[0] = data[1] = data[2] = data[3] = 0;   // Compiler warns these values may be used unset on early exit.

   data[4] = xpt2046_get_data( 0x91 );    // X
   data[5] = xpt2046_get_data( 0xD0 );    // Y  Last Y touch power down

   spi_lobo_device_deselect( tft_ts_spi );

   if( z < 0 )
      z = 0;

   if( z < Z_THRESHOLD )  //  if ( !touched )
   {
      zraw = 0;
      return 0;    // not touched
   }
   zraw = z;

   // Average pair with least distance between each measured x then y
   *x = besttwoavg( data[0], data[2], data[4] );
   *y = besttwoavg( data[1], data[3], data[5] );

//   printf( "    z1=%d,z2=%d  ", z1, z2 );
//   printf( "p=%d,  %d,%d  %d,%d  %d,%d", zraw,
//           data[0], data[1], data[2], data[3], data[4], data[5] );
   printf( "    x=%d, y=%d ", *x, *y );
//   printf( "\n" );

   return 1;
}
#endif
//-----------------------------------------------

void xpt2046_test_touch()
{
   while( 1 )
   {
      uint16_t res_pressed, res_x, res_y;
      if( spi_lobo_device_select( tft_ts_spi, 0 ) != ESP_OK )
        return;

      res_pressed = xpt2046_get_data( 0xB0 ); // Z; pressure; touch detect
      if( res_pressed > 128 )
      {
         res_x = xpt2046_get_data( 0xD0 );
         res_y = xpt2046_get_data( 0x90 );

         printf( "P %04x, x %04x, y %04x\n", res_pressed, res_x, res_y );
      }
      spi_lobo_device_deselect( tft_ts_spi );
   }
}
#endif

