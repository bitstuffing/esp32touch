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
#include "ft236_touch.h"
#include "esp_log.h"
#include "i2cTools.h"
#include "max1726x.h"
#include "music.c"

//IRQ, set de los GPIO ----- TOUCH

#define GPIO_INPUT_IO_0     33
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)
#define ESP_INTR_FLAG_DEFAULT 0

static const char *TAG = "MAIN";

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

touch_t touch = { .FirstX=0x00, .FirstY=0x00, .FirstWeight=0x00, .SecondX=0x00, .SecondY=0x00, .SecondWeight=0x00, .TouchDetections=0x00, .GestType=0x00, .BeforeFirstX=0x00, .BeforeFirstY=0x00, .BeforeFirstWeight=0x00, .BeforeSecondX=0x00, .BeforeSecondY=0x00, .BeforeSecondWeight=0x00, .FirstRead=true };

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg){
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
          update_touch(&touch);
		      ESP_LOGI(TAG, "Valor de 1_X %d 1_Y %d Weight %d Valor de 2_X %d 2_Y %d Weight %d",touch.FirstX,touch.FirstY,touch.FirstWeight,touch.SecondX,touch.SecondY,touch.SecondWeight);
        }
    }
}

static void fuel_gauge_init(void){
  	ESP_LOGD(TAG, "Inicio max.....\n");
  	InitalizeFuelGauge();
  	ESP_LOGD(TAG, "\n\nPrueba de lectura en i2c direccion 0x36 ###\n");
  	//Prueba i2c

  	i2c_detect();
  	uint8_t *_data = malloc(2);
  	_data[0] = 0x00;
  	_data[1] = 0x00;
  	//i2c_master_write_slave_reg(0x36,0x18,_data,2);
  	i2c_master_read_slave_reg(0x36, 0x18, _data, 2);

  	for (int i = 0; i < 2; i++){
  		ESP_LOGD(TAG, "Valor %02x \n", _data[i]);
  	}

  	ESP_LOGD(TAG, "Valor 0x%02x%02x\n", _data[1],_data[0]);

  	ESP_LOGD(TAG, "Fin dir 18 ### \n");
  	_data[0] = 0;
  	_data[1] = 0;
  	i2c_master_read_slave_reg(0x36, 0x21, _data, 2);
  	for (int i = 0; i < 2; i++){
  		ESP_LOGD(TAG, "Valor %02x \n", _data[i]);
  	}
  	ESP_LOGD(TAG, "Fin dir 21### \n");

  	ESP_LOGD(TAG, "Ahora probamos el update\n");

  	FuelGaugeReads_t BateryData;
  	BateryData.EstimateTimeToEmpty = 0x0000 ;
  	BateryData.RemainingCapacity = 0x0000 ;
  	BateryData.StateOfChargePercentage = 0x0000;

  	free(_data);

  	UpdateStatus(&BateryData);

  	ESP_LOGD(TAG, "Tenemos restante %d mAH\n", (int)BateryData.RemainingCapacity);
  	ESP_LOGD(TAG, "Tenemos restante %f minutos\n",( (float)BateryData.EstimateTimeToEmpty * 5.625)/60 ) ;
  	ESP_LOGD(TAG, "Tenemos restante %d %%\n", (int)BateryData.StateOfChargePercentage);
  	ESP_LOGD(TAG, "Capacidad estimada %d maH\n\n", (int)(BateryData.FullCapRep  )/2 ) ;

  	i2c_dump_reg(0x36,2);

}

static void touch_init(void){

	ESP_LOGD(TAG, "\n ############## TOUCH ########\n");
	initialize_touch(&touch);

	gpio_config_t io_conf;

  //interrupt of rising edge
  io_conf.intr_type = GPIO_PIN_INTR_POSEDGE;
  //bit mask of the pins, use GPIO4/5 here
  io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
  //set as input mode
  io_conf.mode = GPIO_MODE_INPUT;
  //enable pull-up mode
  io_conf.pull_up_en = 1;
  gpio_config(&io_conf);

  //change gpio intrrupt type for one pin, revisar si es solo por low edge
  gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

  //create a queue to handle gpio event from isr
  gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
  //start gpio task
  xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

  //install gpio isr service
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  //hook isr handler for specific gpio pin
  gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
  //hook isr handler for specific gpio pin

  //remove isr handler for gpio number.
  gpio_isr_handler_remove(GPIO_INPUT_IO_0);
  //hook isr handler for specific gpio pin again
  gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);


//  for (;;){
//  		vTaskDelay(100 / portTICK_RATE_MS);
//  		ESP_LOGD(TAG, "\nMAIN#####\n");
//	}

	ft236_t TouchDescription;

	Obtain_Chip_Info(&TouchDescription);
	i2c_dump_reg(0x38,2);
	ESP_LOGD(TAG, "VersionInfo %d, FirmID %d, FocaltechPanelID %d, releaseCodeID %d\n InterruptMode %d, ControlMode %d, RateActiveMode %d, RateMonitorMode %d\n",TouchDescription.VersionInfo,TouchDescription.FirmID,TouchDescription.FocaltechPanelID,TouchDescription.ReleaseCodeID,TouchDescription.InterruptMode,TouchDescription.ControlMode,TouchDescription.RateActiveMode,TouchDescription.RateMonitorMode);
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

/**
MAIN
*/
void app_main(void)
{
    //ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    //first load image
    //load_image();
    //next background because of spi conflicts - :S
    backlight_init();

    touch_init();

    fuel_gauge_init();

    bluetooth_init();
}

/** TODO: put touch event in handler
esp_err_t event_handler(void *ctx, system_event_t *event)
{
  backlight_percentage_set(0);
  return ESP_OK;
}*/
