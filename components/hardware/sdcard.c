#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "soc/sdmmc_periph.h"
#include "soc/sdio_slave_periph.h"
#include "esp_log.h"
#include "esp_attr.h"
#include "esp_slave.h"
#include "sdkconfig.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"

#include "esp_vfs_fat.h"

static const char *TAG = "SDCARDDRIVER";

#include "sdcard.h"


#define SDCARD_IO_MISO GPIO_NUM_19
#define SDCARD_IO_MOSI GPIO_NUM_23
#define SDCARD_IO_CLK GPIO_NUM_18
#define SDCARD_IO_CS GPIO_NUM_22
#define SDCARD_IO_INT GPIO_NUM_5

static sdmmc_card_t *sdcard = NULL;

esp_err_t sdcard_init(const char *mount_path){

    ESP_LOGI(TAG, "Initializing SD card");

    esp_err_t ret = NULL;

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.flags = SDMMC_HOST_FLAG_SPI;
    sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
    slot_config.gpio_miso = SDCARD_IO_MISO;
    slot_config.gpio_mosi = SDCARD_IO_MOSI;
    slot_config.gpio_sck  = SDCARD_IO_CLK;
    slot_config.gpio_cs   = SDCARD_IO_CS;
    //slot_config.gpio_int = SDCARD_IO_INT;

//    ret = gpio_install_isr_service(0);
//    ESP_ERROR_CHECK(ret);
    ret = sdspi_host_init();
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "Using SPI peripheral");

    ret = sdspi_host_init_slot(HSPI_HOST, &slot_config);
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "Probe using SPI...\n");

    sdmmc_card_t *card = (sdmmc_card_t *)malloc(sizeof(sdmmc_card_t));
    if (card == NULL) {
        return ESP_ERR_NO_MEM;
    }

    for (;;) {
        if (sdmmc_card_init(&host, card) == ESP_OK) {
            break;
        }
        ESP_LOGW(TAG, "slave init failed, retry...");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, "slave init DONE!");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };

    ret = esp_vfs_fat_sdmmc_mount(mount_path, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return;
    }

    return ret;
}

esp_err_t sdcard_deinit()
{
    if (!sdcard) {
        return ESP_FAIL;
    }

    return esp_vfs_fat_sdmmc_unmount();
    ESP_LOGI(TAG, "Card unmounted");
}

bool sdcard_present(void)
{
    return sdcard != NULL;
}
