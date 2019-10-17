/*
Este dispositivo tiene una máquina de estados como sigue.

El dispositivo sufre un reset cada vez que se queda sin batería.

Hay que comprobar si se ha quedado sin batería, lo que viene siendo un reset.
Si esto ocurre se debe cargar la configuración de la batería, sino.... se continua con sus lecturas.

Check fuel gauge reset  ->No Continue
                        ->Yes, load ini cnofiguration.

REMEMBER en el max el tamaño es siempre de 2 bytes, y el data[0] es el menos significativo
siendo el data[1] el más significativo.

*/

#include "i2cTools.h"
#include "esp_log.h"
#include "max1726x.h"
#include <stdio.h>

//Parte Defines

#define MAX1726X_ADDR 0x36
#define STATUS_REG 0x00
#define FSTAT_REG 0x3D
#define HibCFG_REG 0xBA
#define XTABLE0_REG 0x90
#define SOFT_WAKE_UP_REG 0x60
#define DESIGN_CAP_REG 0x18
#define Ichg_TERM_REG 0x1E
#define VEmpty_REG 0x3A
#define MODEL_CFG_REG 0xDB
#define REP_CAP_REG 0x05 //RepCap or reported remaining capacity in mAh. This register is protected from making sudden jumps during load changes
#define REP_SOC_REG 0x06 //RepSOC is the reported state-of-charge percentage output for use by the application GUI
#define TTE_REG 0x11
#define FULL_CAP_REG 0x10

    //Definiciones de la bateria
    typedef struct
{
    uint8_t DesignCap;
    uint8_t IchgTerm;
    uint8_t FullSOCThr; //Valor de detecion de carga en % del total.
    uint8_t VEmpty;
    float ChargeVoltage;
    int BatRemoves;


}BatCharacteristics_t;

//Estado de la bateria



//Registro que contiene el estado del dispositivo




//Estructura que contendrá las lecturas importantes
typedef struct{





}FuelLecture_t;

static const char *TAG = "MAX1726";

//Update de los valores que lee.
void UpdateStatus(FuelGaugeReads_t *FuelGaugeData)
{


    uint8_t AuxCMD_ = 0x000;
    //leemos Status
    i2c_master_read_slave_reg(MAX1726X_ADDR,STATUS_REG,&AuxCMD_,2);
    AuxCMD_=0x0000;
    i2c_master_write_slave_reg(MAX1726X_ADDR,STATUS_REG,&AuxCMD_,2);
    //Reiniciamos status.


    i2c_master_read_slave_reg(MAX1726X_ADDR,FULL_CAP_REG,&AuxCMD_,2);
    FuelGaugeData->FullCapRep=AuxCMD_;

    i2c_master_read_slave_reg(MAX1726X_ADDR,REP_CAP_REG, &AuxCMD_, 2);
    FuelGaugeData->RemainingCapacity=AuxCMD_;
    AuxCMD_=0x0000;
    i2c_master_read_slave_reg(MAX1726X_ADDR, REP_SOC_REG, &AuxCMD_, 2);

    FuelGaugeData->StateOfChargePercentage =(AuxCMD_ &0xFF00)>>8;
    AuxCMD_ = 0x0000;
    i2c_master_read_slave_reg(MAX1726X_ADDR, TTE_REG, &AuxCMD_, 2);
    FuelGaugeData->EstimateTimeToEmpty=AuxCMD_;

}

    int
    InitalizeFuelGauge()
{
    //###################Asignamos los valores de la strcutura de caracteristicas
    // de la bateria, luego esto podria ser dinamico ###########

    BatCharacteristics_t Batery;
    Batery.DesignCap = 0x0FA0; //2000 (0x07D0) mah que en decimal son 4,000(0x0FA0). Comprobar si tiene que ser doble como pensamos
    Batery.IchgTerm = 0x0280; //valor por defecto app.
    Batery.FullSOCThr = 0x5F05; //95% del total como deteccion de carga completa
    Batery.VEmpty = 0xA561;//(3.3V / 3.88V) Valor de vacio / Valor de reset del vacio para volver a detectarlo.
    Batery.ChargeVoltage= 4.40; //Revisar en el cargador **

    //####### FIN ########

    //Primero miramos si tenemos el POR a uno, este bit indica
    //un reset de la bateria, tanto software como hardware. Si se detecta debe ser bajado
    //por el software para poder revisar el siguiente reset.

    uint8_t StatusPor = 0x0000;
    i2c_master_read_slave_reg(MAX1726X_ADDR, STATUS_REG, &StatusPor, 2);

    ESP_LOGD(TAG, "Valor 0x%04x\n", (StatusPor & 0x0002));

    if ((StatusPor & 0x0002) == 0) //Significa que no estamos en reset
    {                              //No tenemos nada que hacer
        ESP_LOGD(TAG, "\n #### Ya esta configurado ### \n");
        return 1;
} else{ //Si tenemos un 1, en el bit D1 entonces significa que se ha reseteado y hay que configurarla
//Para poder comprobar que tenemos el dato listo debemos ver el D0 de la ADDR 3D. Este bit
//Lo coloca el Fuel cuando termina de actualizar los registros todos, tarda como 710ms si se saca la bateria.

//##Revisar con bit si llegamos a este estado con la placa jugando.... que lagearia.

uint8_t FSTAT_DNR = 0x0000;
do{

    i2c_master_read_slave_reg(MAX1726X_ADDR, FSTAT_REG, &FSTAT_DNR, 2);
    ESP_LOGD(TAG, "Espera bucle while\n");
}while (FSTAT_DNR & 0x0001); //Si tenemos el bit D0 a 0, significa que la Fuel ya tiene todo listo.
ESP_LOGD(TAG, "Valor 0x%04x", FSTAT_DNR);
ESP_LOGD(TAG, "Salimos........");

/* Una vez en este punto debemos hacer la configuracion del dispositivo


*/

uint8_t HibCFG = 0x0000;

i2c_master_read_slave_reg(MAX1726X_ADDR, HibCFG_REG, &HibCFG, 2); //Guardamos el valor original

//Para salir del modo "hibernate" se deben mandar 3 comandos.
uint8_t AuxCMD=0x0090;
AuxCMD=0x0000;
i2c_master_write_slave_reg(MAX1726X_ADDR, HibCFG_REG, &AuxCMD, 2);
i2c_master_write_slave_reg(MAX1726X_ADDR, SOFT_WAKE_UP_REG, &AuxCMD, 2);

//Ahora estamos fuera del modo sleep e iniciamos configuracion.
//2.1 OPTION 1 EZ Config(No INI file is needed) :

i2c_master_write_slave_reg(MAX1726X_ADDR,DESIGN_CAP_REG,&Batery.DesignCap,2);
i2c_master_write_slave_reg(MAX1726X_ADDR, Ichg_TERM_REG, &Batery.IchgTerm, 2);
i2c_master_write_slave_reg(MAX1726X_ADDR, VEmpty_REG, &Batery.VEmpty, 2);


//Metemos un MODEL_CFG en funcion del punto de carga maximo.
if (Batery.ChargeVoltage > 4.275){
    AuxCMD = 0x8400;
    i2c_master_write_slave_reg(MAX1726X_ADDR, MODEL_CFG_REG, &AuxCMD, 2);
}else{
    AuxCMD= 0x8000;
    i2c_master_write_slave_reg(MAX1726X_ADDR, MODEL_CFG_REG, &AuxCMD, 2);
}


//Poll ModelCFG.Refresh(highest bit),
//proceed to Step 3 when ModelCFG.Refresh=0.
uint8_t ModelCFG_Refresh=0x000;
do {
    i2c_master_read_slave_reg(MAX1726X_ADDR, MODEL_CFG_REG, &ModelCFG_Refresh, 2);
    ESP_LOGD(TAG, "\n Bucle actualizacion modelo\n");
} while (ModelCFG_Refresh & 0x8000);

i2c_master_write_slave_reg(MAX1726X_ADDR,HibCFG_REG, &HibCFG, 2);//Restituimos el HibCFG.

//Finalizamos configuracion
uint8_t Status=0x0000;
i2c_master_read_slave_reg(MAX1726X_ADDR, STATUS_REG, &Status, 2);
Status= Status & 0xFFFD;
i2c_master_write_slave_reg(MAX1726X_ADDR, STATUS_REG, &Status, 2);



ESP_LOGD(TAG, "\n## FIN CONFIGURACION ##\n");
return 0;
}

}
