#ifndef I2CTOOLS_H
#define I2CTOOLS_H

#include "driver/i2c.h"

//Inicializacion de sistema I2C
void i2c_master_init();

//Lectura de registro con longitud N
esp_err_t i2c_master_read_slave_reg(uint8_t chip_addr, uint8_t data_addr, uint8_t *data, size_t len);
//Escritura de registro con longitud N
esp_err_t i2c_master_write_slave_reg(uint8_t chip_addr, uint8_t data_addr, uint8_t *data, size_t len);

esp_err_t i2c_dump_reg(uint8_t chip_addr, int size);

//Funcion de deteccion de dispositivos
int i2c_detect();



#endif /* I2CTOOLS */
