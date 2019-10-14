#include "max1726x.h"

static void fuel_gauge_init(void){
  	ESP_LOGI(TAG, "Inicio max.....\n");
  	InitalizeFuelGauge();
  	ESP_LOGI(TAG, "\n\nPrueba de lectura en i2c direccion 0x36 ###\n");
  	//Prueba i2c

  	i2c_detect();
  	uint8_t *_data = malloc(2);
  	_data[0] = 0x00;
  	_data[1] = 0x00;
  	//i2c_master_write_slave_reg(0x36,0x18,_data,2);
  	i2c_master_read_slave_reg(0x36, 0x18, _data, 2);

  	for (int i = 0; i < 2; i++){
  		ESP_LOGI(TAG, "Valor %02x \n", _data[i]);
  	}

  	ESP_LOGI(TAG, "Valor 0x%02x%02x\n", _data[1],_data[0]);

  	ESP_LOGI(TAG, "Fin dir 18 ### \n");
  	_data[0] = 0;
  	_data[1] = 0;
  	i2c_master_read_slave_reg(0x36, 0x21, _data, 2);
  	for (int i = 0; i < 2; i++){
  		ESP_LOGI(TAG, "Valor %02x \n", _data[i]);
  	}
  	ESP_LOGI(TAG, "Fin dir 21### \n");

  	ESP_LOGI(TAG, "Ahora probamos el update\n");

  	FuelGaugeReads_t BateryData;
  	BateryData.EstimateTimeToEmpty = 0x0000 ;
  	BateryData.RemainingCapacity = 0x0000 ;
  	BateryData.StateOfChargePercentage = 0x0000;

  	free(_data);

  	UpdateStatus(&BateryData);

  	ESP_LOGI(TAG, "Tenemos restante %d mAH\n", (int)BateryData.RemainingCapacity);
  	ESP_LOGI(TAG, "Tenemos restante %f minutos\n",( (float)BateryData.EstimateTimeToEmpty * 5.625)/60 ) ;
  	ESP_LOGI(TAG, "Tenemos restante %d %%\n", (int)BateryData.StateOfChargePercentage);
  	ESP_LOGI(TAG, "Capacidad estimada %d maH\n\n", (int)(BateryData.FullCapRep  )/2 ) ;

  	i2c_dump_reg(0x36,2);

}
