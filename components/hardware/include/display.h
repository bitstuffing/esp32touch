#pragma once

#include <stdint.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "gbuf.h"
#include "rect.h"

#define DISPLAY_WIDTH (320)
#define DISPLAY_HEIGHT (240)

#define PARALLEL_LINES (5)

static const gpio_num_t SPI_PIN_NUM_MISO = GPIO_NUM_19;
static const gpio_num_t SPI_PIN_NUM_MOSI = GPIO_NUM_23;
static const gpio_num_t SPI_PIN_NUM_CLK = GPIO_NUM_18;

static const gpio_num_t LCD_PIN_NUM_CS = GPIO_NUM_5;
static const gpio_num_t LCD_PIN_NUM_DC = GPIO_NUM_21;
static const int LCD_SPI_CLOCK_RATE = SPI_MASTER_FREQ_40M;

#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_MH 0x04
#define TFT_RGB_BGR 0x08

extern gbuf_t *fb;

void display_init(void);
void display_poweroff(void);
void display_clear(uint16_t color);
void display_update(void);
void display_update_rect(rect_t r);
void display_drain(void);
void display_screenshot(const char *path);
void send_continue_line(uint16_t *line, int width, int height);
void send_lines(int ypos, int height,uint16_t *linedata);
void send_line_finish();
