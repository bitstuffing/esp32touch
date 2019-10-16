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

    ui_draw_image(pixels,0,0,320,240);

    extern uint8_t imageJpgFolder[] asm("_binary_folder_jpg_start");
    ret=decode_image(&pixels,imageJpgFolder,320,240);
    ESP_ERROR_CHECK(ret);

    //ui_draw_image(pixels,0,0,96,96);

    status_bar_draw();
    status_bar_start();

    //display_keyboard();
}
