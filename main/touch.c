#include "ft236_touch.h"

//IRQ, set de los GPIO ----- TOUCH

#define GPIO_INPUT_IO_0     33
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)
#define ESP_INTR_FLAG_DEFAULT 0

static const char *TAG = "MAIN";


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
		      //ESP_LOGI(TAG, "Valor de 1_X %d 1_Y %d Weight %d Valor de 2_X %d 2_Y %d Weight %d",touch.FirstX,touch.FirstY,touch.FirstWeight,touch.SecondX,touch.SecondY,touch.SecondWeight);
          char key = get_keyboard_touched_key(&touch);
          ESP_LOGI(TAG,"returned is: '%c'",key);
        }
    }
}

static void touch_init(void){

	ESP_LOGI(TAG, "\n ############## TOUCH ########\n");
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

	ft236_t TouchDescription;

	Obtain_Chip_Info(&TouchDescription);
	i2c_dump_reg(0x38,2);
	ESP_LOGI(TAG, "VersionInfo %d, FirmID %d, FocaltechPanelID %d, releaseCodeID %d\n InterruptMode %d, ControlMode %d, RateActiveMode %d, RateMonitorMode %d\n",TouchDescription.VersionInfo,TouchDescription.FirmID,TouchDescription.FocaltechPanelID,TouchDescription.ReleaseCodeID,TouchDescription.InterruptMode,TouchDescription.ControlMode,TouchDescription.RateActiveMode,TouchDescription.RateMonitorMode);
}
