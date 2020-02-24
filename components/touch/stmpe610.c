// -----------------------------------------------------------------------------
//
// 2019-09-21  AWe   initial version, move touch parts into this file
//
// -----------------------------------------------------------------------------

#if TOUCH_TYPE == TOUCH_TYPE_STMPE610

// ============= Touch panel functions =========================================

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"       // vTaskDelay()
#include "spi_master_lobo.h"
#include "tftspi.h"              // spi_transfer_start()
#include "touch.h"
#include "stmpe610.h"

//----------------------------------------------------------
//
//----------------------------------------------------------
// Send 1 byte display command, display must be selected

static void IRAM_ATTR stmpe610_write_reg( uint8_t reg, uint8_t val )
{
    
    spi_lobo_device_select( tft_ts_spi, 0 );
    
    tft_ts_spi->host->hw->data_buf[0] = ( val << 8 ) | reg;
    spi_transfer_start( tft_ts_spi, 16, 0 );
    
    spi_lobo_device_deselect( tft_ts_spi );
}

//----------------------------------------------------------
//
//----------------------------------------------------------

static uint8_t IRAM_ATTR stmpe610_read_byte( uint8_t reg )
{
    spi_lobo_device_select( tft_ts_spi, 0 );
    
    tft_ts_spi->host->hw->data_buf[0] = ( reg << 8 ) | ( reg | 0x80 );
    spi_transfer_start( tft_ts_spi, 16, 16 );
    uint8_t res = tft_ts_spi->host->hw->data_buf[0] >> 8;
    
    spi_lobo_device_deselect( tft_ts_spi );
    return res;
}

//----------------------------------------------------------
//
//----------------------------------------------------------

static uint16_t IRAM_ATTR stmpe610_read_word( uint8_t reg )
{
    spi_lobo_device_select( tft_ts_spi, 0 );
    
    tft_ts_spi->host->hw->data_buf[0] = ( ( ( ( reg + 1 ) << 8 ) | ( ( reg + 1 ) | 0x80 ) ) << 16 ) | ( reg << 8 ) | ( reg | 0x80 );
    spi_transfer_start( tft_ts_spi, 32, 32 );
    uint16_t res = ( uint16_t )( tft_ts_spi->host->hw->data_buf[0] & 0xFF00 );
    res |= ( uint16_t )( tft_ts_spi->host->hw->data_buf[0] >> 24 );
    
    spi_lobo_device_deselect( tft_ts_spi );
    return res;
}

//----------------------------------------------------------
//
//----------------------------------------------------------

uint32_t stmpe610_getID()
{
    uint16_t tid = stmpe610_read_word( 0 );
    uint8_t tver = stmpe610_read_byte( 2 );
    return ( tid << 8 ) | tver;
}

//----------------------------------------------------------
//
//----------------------------------------------------------

void stmpe610_Init()
{
    stmpe610_write_reg( STMPE610_REG_SYS_CTRL1, 0x02 );      // Software chip reset
    vTaskDelay( 10 / portTICK_RATE_MS );
    
    stmpe610_write_reg( STMPE610_REG_SYS_CTRL2, 0x04 );      // Temperature sensor clock off, GPIO clock off, touch clock on, ADC clock on
    
    stmpe610_write_reg( STMPE610_REG_INT_EN, 0x00 );         // Don't Interrupt on INT pin
    
    stmpe610_write_reg( STMPE610_REG_ADC_CTRL1, 0x48 );      // ADC conversion time = 80 clock ticks, 12-bit ADC, internal voltage refernce
    vTaskDelay( 2 / portTICK_RATE_MS );
    stmpe610_write_reg( STMPE610_REG_ADC_CTRL2, 0x01 );      // ADC speed 3.25MHz
    stmpe610_write_reg( STMPE610_REG_GPIO_AF, 0x00 );        // GPIO alternate function - OFF
    stmpe610_write_reg( STMPE610_REG_TSC_CFG, 0xE3 );        // Averaging 8, touch detect delay 1ms, panel driver settling time 1ms
    stmpe610_write_reg( STMPE610_REG_FIFO_TH, 0x01 );        // FIFO threshold = 1
    stmpe610_write_reg( STMPE610_REG_FIFO_STA, 0x01 );       // FIFO reset enable
    stmpe610_write_reg( STMPE610_REG_FIFO_STA, 0x00 );       // FIFO reset disable
    stmpe610_write_reg( STMPE610_REG_TSC_FRACT_XYZ, 0x07 );  // Z axis data format
    stmpe610_write_reg( STMPE610_REG_TSC_I_DRIVE, 0x01 );    // max 50mA touchscreen line current
    stmpe610_write_reg( STMPE610_REG_TSC_CTRL, 0x30 );       // X&Y&Z, 16 reading window
    stmpe610_write_reg( STMPE610_REG_TSC_CTRL, 0x31 );       // X&Y&Z, 16 reading window, TSC enable
    stmpe610_write_reg( STMPE610_REG_INT_STA, 0xFF );        // Clear all interrupts
    stmpe610_write_reg( STMPE610_REG_INT_CTRL, 0x00 );       // Level interrupt, disable interrupts
}

//----------------------------------------------------------
//
//----------------------------------------------------------

int stmpe610_get_touch( uint16_t *x, uint16_t *y, uint16_t *z )
{
    if( !( stmpe610_read_byte( STMPE610_REG_TSC_CTRL ) & 0x80 ) ) return 0;
    
    // Get touch data
    uint8_t fifo_size = stmpe610_read_byte( STMPE610_REG_FIFO_SIZE );
    while( fifo_size < 2 )
    {
        if( !( stmpe610_read_byte( STMPE610_REG_TSC_CTRL ) & 0x80 ) ) return 0;
        fifo_size = stmpe610_read_byte( STMPE610_REG_FIFO_SIZE );
    }
    while( fifo_size > 120 )
    {
        if( !( stmpe610_read_byte( STMPE610_REG_TSC_CTRL ) & 0x80 ) ) return 0;
        *x = stmpe610_read_word( STMPE610_REG_TSC_DATA_X );
        *y = stmpe610_read_word( STMPE610_REG_TSC_DATA_Y );
        *z = stmpe610_read_byte( STMPE610_REG_TSC_DATA_Z );
        fifo_size = stmpe610_read_byte( STMPE610_REG_FIFO_SIZE );
    }
    for( uint8_t i = 0; i < ( fifo_size - 1 ); i++ )
    {
        *x = stmpe610_read_word( STMPE610_REG_TSC_DATA_X );
        *y = stmpe610_read_word( STMPE610_REG_TSC_DATA_Y );
        *z = stmpe610_read_byte( STMPE610_REG_TSC_DATA_Z );
    }
    
    *x = 4096 - *x;
    /*
     // Clear the rest of the fifo
     {
     stmpe610_write_reg(STMPE610_REG_FIFO_STA, 0x01);    // FIFO reset enable
     stmpe610_write_reg(STMPE610_REG_FIFO_STA, 0x00);    // FIFO reset disable
     }
     */
    return 1;
}

//----------------------------------------------------------
//
//----------------------------------------------------------
#endif
