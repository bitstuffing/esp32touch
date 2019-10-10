#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "decode_image.h"
#include <math.h>
#include "backlight.h"
#include "display.h"

void load_image();

static uint16_t **pixels;

//Grab a rgb16 pixel from the esp32_tiles image
static inline uint16_t get_bgnd_pixel(int x, int y)
{
    //Image has an 8x8 pixel margin, so we can also resolve e.g. [-3, 243]
    x+=8;
    y+=8;
    return pixels[y][x];
}

//This variable is used to detect the next frame.
static int prev_frame=-1;

//Instead of calculating the offsets for each pixel we grab, we pre-calculate the valueswhenever a frame changes, then re-use
//these as we go through all the pixels in the frame. This is much, much faster.
static int8_t xofs[320], yofs[240];
static int8_t xcomp[320], ycomp[240];

/**
Display the image to spi
*/
void display_image(spi_device_handle_t spi){
    uint16_t *lines[2];
    //Allocate memory for the pixel buffers
    for (int i=0; i<2; i++) {
        lines[i]=heap_caps_malloc(320*PARALLEL_LINES*sizeof(uint16_t), MALLOC_CAP_DMA);
        assert(lines[i]!=NULL);
    }
    int frame=0;
    //Indexes of the line currently being sent to the LCD and the line we're calculating.
    int sending_line=-1;
    int calc_line=0;

    if(1) { //TODO
        frame++;
        for (int y=0; y<240; y+=PARALLEL_LINES) {
            //Calculate each line.

            uint16_t *dest = lines[calc_line]; //target line

            for (int yy=y; yy<y+PARALLEL_LINES; yy++) {
                for (int x=0; x<320; x++) {
                    *dest++=get_bgnd_pixel(x+yofs[yy]+xcomp[x], yy+xofs[x]+ycomp[yy]);
                }
            }

            //Finish up the sending process of the previous line, if any
            if (sending_line!=-1) send_line_finish(spi);
            //Swap sending_line and calc_line
            sending_line=calc_line;
            calc_line=(calc_line==1)?0:1;
            //Send the line we currently calculated.
            send_lines(spi, y, lines[sending_line]);
            //The line set is queued up for sending now; the actual sending happens in the
            //background. We can go on to calculate the next line set as long as we do not
            //touch line[sending_line]; the SPI sending process is still reading from that.
        }
    }
}

/**
TODO: put touch event in handler
*/
esp_err_t event_handler(void *ctx, system_event_t *event)
{
  backlight_percentage_set(0);
  return ESP_OK;
}

/**
MAIN
*/
void app_main(void)
{

    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    //first load image
    load_image();
    //next background because of spi conflicts - :S
    backlight_init();

}

void load_image(){
  esp_err_t ret;
  spi_device_handle_t spi;
  spi_bus_config_t buscfg={
      .miso_io_num=PIN_NUM_MISO,
      .mosi_io_num=PIN_NUM_MOSI,
      .sclk_io_num=PIN_NUM_CLK,
      .quadwp_io_num=-1,
      .quadhd_io_num=-1,
      .max_transfer_sz=PARALLEL_LINES*320*2+8
  };
  spi_device_interface_config_t devcfg={
    #ifdef CONFIG_LCD_OVERCLOCK
            .clock_speed_hz=26*1000*1000,           //Clock out at 26 MHz
    #else
            .clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
    #endif
            .mode=0,                                //SPI mode 0
            .spics_io_num=PIN_NUM_CS,               //CS pin
            .queue_size=7,                          //We want to be able to queue 7 transactions at a time
            .pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
  };
  //Initialize the SPI bus
  ret=spi_bus_initialize(HSPI_HOST, &buscfg, 1);
  ESP_ERROR_CHECK(ret);
  //Attach the LCD to the SPI bus
  ret=spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
  ESP_ERROR_CHECK(ret);
  //Initialize the LCD
  lcd_init(spi);

  //decode image with tjpgd library
  ret=decode_image(&pixels);
  ESP_ERROR_CHECK(ret);

  //send image to spi
  display_image(spi);
}
