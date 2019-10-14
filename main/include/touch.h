#include "ft236_touch.h"

//IRQ, set de los GPIO ----- TOUCH

#define GPIO_INPUT_IO_0     33
#define GPIO_INPUT_PIN_SEL  (1ULL<<GPIO_INPUT_IO_0)
#define ESP_INTR_FLAG_DEFAULT 0

static const char *TAG = "TOUCH";

touch_t touch = { .FirstX=0x00, .FirstY=0x00, .FirstWeight=0x00, .SecondX=0x00, .SecondY=0x00, .SecondWeight=0x00, .TouchDetections=0x00, .GestType=0x00, .BeforeFirstX=0x00, .BeforeFirstY=0x00, .BeforeFirstWeight=0x00, .BeforeSecondX=0x00, .BeforeSecondY=0x00, .BeforeSecondWeight=0x00, .FirstRead=true };

static xQueueHandle gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg);
static void gpio_task_example(void* arg);
static void touch_init(void);
