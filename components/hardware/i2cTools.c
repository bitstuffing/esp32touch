#include <stdio.h>

#include "driver/i2c.h"

// Error library
#include "esp_err.h"

#include "i2cTools.h"

#define SAMPLE_PERIOD_MS 200

#define I2C_SCL_IO 27          //19               /*!< gpio number for I2C master clock */
#define I2C_SDA_IO 26          //18               /*!< gpio number for I2C master data  */
#define I2C_FREQ_HZ 100000     /*!< I2C master clock frequency */
#define I2C_PORT_NUM I2C_NUM_0 /*!< I2C port number for master dev */
#define I2C_TX_BUF_DISABLE 0   /*!< I2C master do not need buffer */
#define I2C_RX_BUF_DISABLE 0   /*!< I2C master do not need buffer */

// I2C common protocol defines
#define WRITE_BIT I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT I2C_MASTER_READ   /*!< I2C master read */
#define ACK_CHECK_EN 0x1           /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS 0x0          /*!< I2C master will not check ack from slave */
#define ACK_VAL 0x0                /*!< I2C ack value */
#define NACK_VAL 0x1               /*!< I2C nack value */

/**
 * @brief i2c master initialization
 */

  void i2c_master_init()
 {
    //printf("\n Iniciando I2c\n");
     i2c_config_t conf;
     conf.mode = I2C_MODE_MASTER;
     conf.sda_io_num = I2C_SDA_IO;
     conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
     conf.scl_io_num = I2C_SCL_IO;
     conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
     conf.master.clk_speed = I2C_FREQ_HZ;
     i2c_param_config(I2C_PORT_NUM, &conf);
     //printf("Instalando driver ..\n");
     i2c_driver_install(I2C_PORT_NUM, conf.mode, 0, 0, 0);
 }



int i2c_detect()
{

    i2c_master_init();
  
    uint8_t address;
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\r\n");
    for (int i = 0; i < 128; i += 16)
    {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j++)
        {
            fflush(stdout);
            address = i + j;
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, (address << 1) | WRITE_BIT, ACK_CHECK_EN);
            i2c_master_stop(cmd);
            esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 50 / portTICK_RATE_MS);
            i2c_cmd_link_delete(cmd);
            if (ret == ESP_OK)
            {
                printf("%02x ", address);
            }
            else if (ret == ESP_ERR_TIMEOUT)
            {
                printf("UU ");
            }
            else
            {
                printf("-- ");
            }
        }
        printf("\r\n");
    }

    i2c_driver_delete(I2C_NUM_0);
    return 0;
}

     //Test Version 2

     /**
 * @brief test code to read i2c slave device with registered interface
 * _______________________________________________________________________________________________________
 * | start | slave_addr + rd_bit +ack | register + ack | read n-1 bytes + ack | read 1 byte + nack | stop |
 * --------|--------------------------|----------------|----------------------|--------------------|------|
 *
 */
esp_err_t i2c_master_read_slave_reg( uint8_t chip_addr, uint8_t data_addr, uint8_t *data, size_t len)
{

    i2c_master_init();
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, chip_addr << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, data_addr, ACK_CHECK_EN);
    i2c_master_start(cmd);
    
    i2c_master_write_byte(cmd, chip_addr << 1 | READ_BIT, ACK_CHECK_EN);

    if (len > 1)
    {
        i2c_master_read(cmd, data, len - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data + len - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    i2c_driver_delete(I2C_PORT_NUM);
    return ret;
         
     }



esp_err_t i2c_master_write_slave_reg(uint8_t chip_addr, uint8_t data_addr,uint8_t *data,size_t len){

    i2c_master_init();
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, chip_addr << 1 | WRITE_BIT, ACK_CHECK_EN);

    i2c_master_write_byte(cmd, data_addr, ACK_CHECK_EN);
    
    for (int i = 0; i < len; i++)
    {
        i2c_master_write_byte(cmd, data[i], ACK_CHECK_EN);
    }
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    i2c_driver_delete(I2C_PORT_NUM);
    return ret;
}



/* Memoria dump de un chip


*/

esp_err_t i2c_dump_reg(uint8_t chip_addr, int size){

    i2c_master_init();
    uint8_t data_addr;
    uint8_t data[4];
    int32_t block[16];
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f"
           "    0123456789abcdef\r\n");
    for (int i = 0; i < 128; i += 16)
    {
        printf("%02x: ", i);
        for (int j = 0; j < 16; j += size)
        {
            fflush(stdout);
            data_addr = i + j;
            i2c_cmd_handle_t cmd = i2c_cmd_link_create();
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, chip_addr << 1 | WRITE_BIT, ACK_CHECK_EN);
            i2c_master_write_byte(cmd, data_addr, ACK_CHECK_EN);
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, chip_addr << 1 | READ_BIT, ACK_CHECK_EN);
            if (size > 1)
            {
                i2c_master_read(cmd, data, size - 1, ACK_VAL);
            }
            i2c_master_read_byte(cmd, data + size - 1, NACK_VAL);
            i2c_master_stop(cmd);
            esp_err_t ret = i2c_master_cmd_begin(I2C_PORT_NUM, cmd, 50 / portTICK_RATE_MS);
            i2c_cmd_link_delete(cmd);
            if (ret == ESP_OK)
            {
                for (int k = 0; k < size; k++)
                {
                    printf("%02x ", data[k]);
                    block[j + k] = data[k];
                }
            }
            else
            {
                for (int k = 0; k < size; k++)
                {
                    printf("XX ");
                    block[j + k] = -1;
                }
            }
        }
        printf("   ");
        for (int k = 0; k < 16; k++)
        {
            if (block[k] < 0)
            {
                printf("X");
            }
            if ((block[k] & 0xff) == 0x00 || (block[k] & 0xff) == 0xff)
            {
                printf(".");
            }
            else if ((block[k] & 0xff) < 32 || (block[k] & 0xff) >= 127)
            {
                printf("?");
            }
            else
            {
                printf("%c", block[k] & 0xff);
            }
        }
        printf("\r\n");
    }
    i2c_driver_delete(I2C_PORT_NUM);
    return 0;
}