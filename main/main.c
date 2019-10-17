#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include "esp_log.h"

#include "ui.h"
#include "ui_controls.h"
#include "status_bar.h"

#include "backlight.h"
#include "display.h"
#include "i2cTools.h"
#include "touch.c"
#include "fuelgauge.c"
#include "bluetooth.c"

#include "decode_image.h"

static uint16_t **pixels;

/**
MAIN
*/
void app_main(void){
    esp_err_t ret;

    backlight_init();

    touch_init();

    fuel_gauge_init();

    bluetooth_init();

    display_init();

    ui_init();

    //Reference the binary-included jpeg file
    extern uint8_t imageJpg[] asm("_binary_background_jpg_start");

    ret=decode_image(&pixels,imageJpg,320,240);
    ESP_ERROR_CHECK(ret);

    //ui_draw_image(pixels,0,0,320,240);

    extern uint8_t imageBmpFolder[] asm("_binary_folder_bmp_start");
    //ret=decode_image(&pixels,imageJpgFolder,320,240);
    //ESP_ERROR_CHECK(ret);

    //ui_draw_image(pixels,0,0,96,96);

    //rect_t window_rect = (rect_t){.x = 0, .y = 32, .width = DISPLAY_WIDTH, .height = DISPLAY_HEIGHT - 32};

    gbuf_t img = {.width = (uint16_t)24,
        .height = (uint16_t)24,
        .bytes_per_pixel = 2,
        .data = (uint8_t *)&imageBmpFolder,
        .big_endian = false};

    blit(fb, (rect_t){.x = 0, .y = 0, .width = img.width, .height = img.height - 1}, &img,
	     (rect_t){.x = 0, .y = 1, .width = img.width, .height = img.height - 1});

    display_update_rect((rect_t){.x = 0, .y = 0, .width = img.width, .height = img.height - 1});

    //status_bar_draw();
    //status_bar_start();

    //display_keyboard();
}
