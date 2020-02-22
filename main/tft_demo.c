/* TFT Programming Challenge
 
 This is the solution for a programming challenge that simply
 draws the olympic logo onto the TFT LCD.
 */

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "tftspi.h"
#include "tft.h"
#include "touch.h"
#include "esp_spiffs.h"
#include "esp_log.h"


// ==========================================================
// Define which spi bus to use TFT_VSPI_HOST or TFT_HSPI_HOST
#define SPI_BUS TFT_HSPI_HOST

// ==========================================================
#define DEFAULT_TFT_ORIENTATION LANDSCAPE


void draw_olympic_logo()
{
    // ************************************************
    // draw frame and background
    // ************************************************
    uint8_t border_width = 10;
    TFT_fillRect(0, 0, tft_width-1, tft_height-1, TFT_ORANGE);  // orange border
    TFT_fillRect(border_width, border_width, tft_width-(border_width<<1)-1, tft_height-(border_width<<1)-1, TFT_WHITE);  // white background
    
    // ************************************************
    // draw the rings
    // ************************************************
    uint16_t ringRad = 30;
    uint16_t ringTh = 6;
    uint16_t ringVDist = ringRad; // 30;
    uint16_t ringHDist = ringRad * 1.07;   //32;
    uint16_t verticalOffset = (ringRad >> 1);
    uint16_t centerRingXPos = (tft_dispWin.x2 - tft_dispWin.x1) / 2;
    uint16_t centerRingYPos = ((tft_dispWin.y2 - tft_dispWin.y1) / 2) - verticalOffset;
    
    TFT_drawArc(centerRingXPos-(ringHDist<<1), centerRingYPos, ringRad, ringTh, 100, 99, TFT_BLUE, TFT_BLUE);  // blue ring
    TFT_drawArc(centerRingXPos, centerRingYPos, ringRad, ringTh, 100, 99, TFT_BLACK, TFT_BLACK);  // black ring
    TFT_drawArc(centerRingXPos+(ringHDist<<1), centerRingYPos, ringRad, ringTh, 262, 261, TFT_RED, TFT_RED);  // red ring
    
    TFT_drawArc(centerRingXPos-ringHDist, centerRingYPos+ringVDist, ringRad, ringTh, 349, 348, TFT_YELLOW, TFT_YELLOW);  // yellow ring
    TFT_drawArc(centerRingXPos+ringHDist, centerRingYPos+ringVDist, ringRad, ringTh, 349, 348, TFT_GREEN, TFT_GREEN);  // green ring
    
    // patches to cover rings to get the proper overlap pattern
    TFT_drawArc(centerRingXPos-(ringHDist<<1), centerRingYPos, ringRad, ringTh, 90, 120, TFT_BLUE, TFT_BLUE);  // blue over yellow
    TFT_drawArc(centerRingXPos, centerRingYPos, ringRad, ringTh, 90, 120, TFT_BLACK, TFT_BLACK);  // black over green
    TFT_drawArc(centerRingXPos, centerRingYPos, ringRad, ringTh, 180, 200, TFT_BLACK, TFT_BLACK);  // black over yellow
    TFT_drawArc(centerRingXPos+(ringHDist<<1), centerRingYPos, ringRad, ringTh, 160, 240, TFT_RED, TFT_RED); // red over green
    
    // ************************************************
    // add title "TOKYO 2020"
    // ************************************************
    tft_bg = TFT_WHITE;
    tft_fg = TFT_NAVY;
    tft_font_rotate = 0;
    tft_font_transparent = 1;

    const char *file_font = "/spiffs/fonts/DIN2014NarrowDemi36.fon";
    TFT_setFont(USER_FONT, file_font);
    int fontheight = TFT_getfontheight();
        
    // bounds and center point of upper white area
    int bottom_of_upper_area = ((tft_height >> 1) - (int)(1.5*ringRad));
    int top_of_upper_area = border_width;
    int center_of_upper_area = ((bottom_of_upper_area - top_of_upper_area) >> 1) + border_width;
    int baseline_for_tokyo = center_of_upper_area - (fontheight >> 1);

    // bounds and center point of lower white area
    int top_of_lower_area = ((tft_height - 1) >> 1) + (int)(1.5 * ringRad);
    int bottom_of_lower_area = (tft_height - 1) - border_width;
    int center_of_lower_area = (tft_height - 1) - border_width - ((bottom_of_lower_area - top_of_lower_area) >> 1);
    int baseline_for_2020 = center_of_lower_area - (fontheight >> 1);
    
    // center line
//    TFT_drawFastHLine(0, (tft_height >> 1), 320, TFT_PURPLE);
    
    // debug lines for upper white area
//    TFT_drawFastHLine(0, bottom_of_upper_area, 320, TFT_GREEN);
//    TFT_drawFastHLine(0, top_of_upper_area, 320, TFT_GREEN);
//    TFT_drawFastHLine(0, center_of_upper_area, 320, TFT_RED);
//    TFT_drawFastHLine(0, baseline_for_tokyo, 320, TFT_BLACK);
    
    // debug lines for lower white area
//    TFT_drawFastHLine(0, top_of_lower_area, 320, TFT_GREEN);
//    TFT_drawFastHLine(0, bottom_of_lower_area, 320, TFT_GREEN);
//    TFT_drawFastHLine(0, center_of_lower_area, 320, TFT_RED);
//    TFT_drawFastHLine(0, baseline_for_2020, 320, TFT_BLACK);
    
    TFT_print("TOKYO", CENTER, baseline_for_tokyo);
    TFT_print("2020", CENTER, baseline_for_2020);
}


void app_main()
{
    //test_sd_card();
    // ========  PREPARE DISPLAY INITIALIZATION  =========
    
    esp_err_t ret;
    
    // === SET GLOBAL VARIABLES ==========================
    
    // ===================================================
    // ==== Set maximum spi clock for display read    ====
    //      operations, function 'find_rd_speed()'    ====
    //      can be used after display initialization  ====
    tft_max_rdclock = 8000000;
    // ===================================================
    
    // ====================================================================
    // === Pins MUST be initialized before SPI interface initialization ===
    // ====================================================================
    TFT_PinsInit();
    
    // ====  CONFIGURE SPI DEVICES(s)  ====================================================================================
    
    spi_lobo_device_handle_t spi;
    
    spi_lobo_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,                // set SPI MISO pin
        .mosi_io_num=PIN_NUM_MOSI,                // set SPI MOSI pin
        .sclk_io_num=PIN_NUM_CLK,                // set SPI CLK pin
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .max_transfer_sz = 6*1024,
    };
    spi_lobo_device_interface_config_t devcfg={
        .clock_speed_hz=8000000,                // Initial clock out at 8 MHz
        .mode=0,                                // SPI mode 0
        .spics_io_num=-1,                       // we will use external CS pin
        .spics_ext_io_num=PIN_NUM_CS,           // external CS pin
        .flags=LB_SPI_DEVICE_HALFDUPLEX,        // ALWAYS SET  to HALF DUPLEX MODE!! for display spi
    };
    
        
    vTaskDelay(500 / portTICK_RATE_MS);
    
    printf("\r\n==============================\r\n");
    printf("TFT display DEMO, LoBo 11/2017\r\n");
    printf("==============================\r\n");
    printf("Pins used: miso=%d, mosi=%d, sck=%d, cs=%d\r\n", PIN_NUM_MISO, PIN_NUM_MOSI, PIN_NUM_CLK, PIN_NUM_CS);
    printf("==============================\r\n\r\n");
    
    // ==================================================================
    // ==== Initialize the SPI bus and attach the LCD to the SPI bus ====
    
    ret=spi_lobo_bus_add_device(SPI_BUS, &buscfg, &devcfg, &spi);
    assert(ret==ESP_OK);
    printf("SPI: display device added to spi bus (%d)\r\n", SPI_BUS);
    tft_disp_spi = spi;
    
    // ==== Test select/deselect ====
    ret = spi_lobo_device_select(spi, 1);
    assert(ret==ESP_OK);
    ret = spi_lobo_device_deselect(spi);
    assert(ret==ESP_OK);
    
    printf("SPI: attached display device, speed=%u\r\n", spi_lobo_get_speed(spi));
    printf("SPI: bus uses native pins: %s\r\n", spi_lobo_uses_native_pins(spi) ? "true" : "false");
    
    // ================================
    // ==== Initialize the Display ====
    printf("SPI: display init...\r\n");
    TFT_display_init();
    printf("OK\r\n");
    
    // ---- Detect maximum read speed ----
    tft_max_rdclock = find_rd_speed();
    printf("SPI: Max rd speed = %u\r\n", tft_max_rdclock);
    
    // ==== Set SPI clock used for display operations ====
    spi_lobo_set_speed(spi, DEFAULT_SPI_CLOCK);
    printf("SPI: Changed speed to %u\r\n", spi_lobo_get_speed(spi));
    
    printf("\r\n---------------------\r\n");
    printf("Olympic Logo Started \r\n");
    printf("---------------------\r\n");
    
    tft_font_rotate = 0;
    tft_text_wrap = 0;
    tft_font_transparent = 0;
    tft_font_forceFixed = 0;
    tft_gray_scale = 0;
    TFT_setGammaCurve(DEFAULT_GAMMA_CURVE);
    TFT_setRotation( DEFAULT_TFT_ORIENTATION );
    TFT_setFont(DEFAULT_FONT, NULL);
    TFT_resetclipwin();
    
    
    tft_fg = TFT_CYAN;
    
    // ==== Initialize the file system ====
    TFT_print("Initializing SPIFFS...", CENTER, CENTER);
    
    const esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "storage", // NULL
        .max_files = 5,
        .format_if_mount_failed = false
    };
    
    
    printf("\r\n\n");
    esp_vfs_spiffs_register(&conf);
    if (!esp_spiffs_mounted(conf.partition_label)) {
        tft_fg = TFT_RED;
        TFT_print("SPIFFS not mounted !", CENTER, LASTY+TFT_getfontheight()+2);
    }
    else {
        tft_fg = TFT_GREEN;
        TFT_print("SPIFFS Mounted.", CENTER, LASTY+TFT_getfontheight()+2);
    }
    vTaskDelay( pdMS_TO_TICKS(2000) );
    
    //=========
    // Run demo
    //=========
    draw_olympic_logo();
}
