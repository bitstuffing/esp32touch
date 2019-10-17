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

#include "image.h"

#include "backlight.h"
#include "display.h"
#include "i2cTools.h"
#include "touch.c"
#include "fuelgauge.c"
#include "bluetooth.c"


/**
MAIN
*/
void app_main(void){

    backlight_init();

    touch_init();

    fuel_gauge_init();

    bluetooth_init();

    display_init();

    ui_init();

    //status_bar_draw();
    //status_bar_start();

    display_image();

    //display_keyboard();
}
