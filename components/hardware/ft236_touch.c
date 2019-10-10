#include "ft236_touch.h"
#include "i2cTools.h"
#include <stdio.h>
#include "esp_log.h"

#define GPIO_TOUCH_IRQ 33

#define TOUCH_ADDR 0x38

#define STATUS_REG 0x00
#define GEST_REG 0x01 //Registro que define el gesto
#define TD_STATUS_REG 0x02 //Registro que indica el numero de puntos tocados.
//Punto 1
#define P1_XH_REG 0x03
#define P1_XL_REG 0x04
#define P1_YH_REG 0x05
#define P1_YL_REG 0x06
#define P1_WEIGHT_REG 0x07
#define P1_MISC_REG 0x08
//Punto 2
#define P2_XH_REG 0x09
#define P2_XL_REG 0x0A
#define P2_YH_REG 0x0B
#define P2_YL_REG 0x0C
#define P2_WEIGHT_REG 0x0D
#define P2_MISC_REG 0x0E

#define G_MODE_REG 0xA4 //Registro que indica el modo de interrupcion 0x00 Polling, 0x01 interrupt trigger
#define FIRMID_REG 0xA6 //ID del firmware del integrado tactil.
#define FOCALTECH_ID_REG 0xA8 //Id del panel tactil
#define PERIOD_ACTIVE_REG 0x88 //Rate of active mode
#define PERIOD_MONITOR_REG 0x89 //Rate of monitor mode
#define RELEASE_CODE_REG 0xAF //Release code ID
#define CTRL_REG 0x86 //Modo activo del dispositivo, 0x00 siempre activo 0x01 modo monitor cuando no hay touch.
#define LIB_VER_REG 0xA1 //Byte H de la version de la libreria


//#######Opciones del chip de configuracion ########## ///////

#define CTRL_MODE 0x01 //0x00 always active mode 0x01 switch to monitor mode when no touch
#define G_MODE 0x01 //0x00 polling mode (continuos interrput) 0x01 trigger mode (one interrupt on every touch)

static const char *TAG = "FT236_TOUCH";

void set_InterruptMode(_Bool mode){
    uint8_t AuxCMD=mode;
    i2c_master_write_slave_reg(TOUCH_ADDR,G_MODE_REG,&AuxCMD,1);
}


void Obtain_Chip_Info(ft236_t *data){

    uint16_t AuxCMD_=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,LIB_VER_REG,&AuxCMD_,2);
    data->VersionInfo=AuxCMD_;

    uint8_t AuxCMD=0x00;
    AuxCMD=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,FIRMID_REG,&AuxCMD,1);
    data->FirmID=AuxCMD;

    AuxCMD=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,FOCALTECH_ID_REG,&AuxCMD,1);
    data->FocaltechPanelID=AuxCMD;

    AuxCMD=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,RELEASE_CODE_REG,&AuxCMD,1);
    data->ReleaseCodeID=AuxCMD;

    AuxCMD=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,G_MODE_REG,&AuxCMD,1);
    data->InterruptMode=AuxCMD;

    AuxCMD=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,CTRL_REG,&AuxCMD,1);
    data->ControlMode=AuxCMD;

    AuxCMD=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,PERIOD_ACTIVE_REG,&AuxCMD,1);
    data->RateActiveMode=AuxCMD;

    AuxCMD=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,PERIOD_MONITOR_REG,&AuxCMD,1);
    data->RateMonitorMode=AuxCMD;

}


int initialize_touch(touch_t *data){

    data->FirstX=0x00;
    data->FirstY=0x00;
    data->FirstWeight=0x00;
    data->SecondX=0x00;
    data->SecondY=0x00;
    data->SecondWeight=0x00;
    data->TouchDetections=0x00;
    data->GestType=0x00;
    data->BeforeFirstX=0x00;
    data->BeforeFirstY=0x00;
    data->BeforeFirstWeight=0x00;
    data->BeforeSecondX=0x00;
    data->BeforeSecondY=0x00;
    data->BeforeSecondWeight=0x00;
    data->FirstRead=true;

    //inicializada la estructura.

    uint8_t AuxCMD=0x00;

    //Si esta en 100b FACTORY Mode hay que ponerlo en 000b WORKING Mode
    i2c_master_read_slave_reg(TOUCH_ADDR,STATUS_REG,&AuxCMD,1);
    ESP_LOGD(TAG, "Valor %02x \n", AuxCMD);

    AuxCMD=0x00; //reiniciamos Aux.
    if (AuxCMD== 0x00) return 0;
    else i2c_master_write_slave_reg(TOUCH_ADDR,STATUS_REG,&AuxCMD,1);
    return 0;
}

void update_touch(touch_t *data){
    uint8_t AuxCMD=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,GEST_REG,&AuxCMD,1);
    ESP_LOGD(TAG, "Valor gesture %02x \n", AuxCMD);

    //If especial de retorno si esta en reset, sin tocar
    if (AuxCMD ==0xFF){
        ESP_LOGD(TAG, "No se ha pulsado la pantall aun \n");
        return;
    }

    data->GestType=AuxCMD;

    AuxCMD=0x00;
    i2c_master_read_slave_reg(TOUCH_ADDR,TD_STATUS_REG,&AuxCMD,1);
    ESP_LOGD(TAG, "Hemos tocado %d veces la pantalla\n",(unsigned char)AuxCMD);
    data->TouchDetections=AuxCMD;

    uint8_t H_Pos=0x00;
    uint8_t L_Pos=0x00;



    //Añadimos condicion para la primera lectura
    if (data->FirstRead == false) MoveNowTouchToBefore(data,(unsigned char)AuxCMD);

    switch ((unsigned char)AuxCMD){
        case 0: ESP_LOGD(TAG, "No se ha detectado touch aun..\n");
        break;
        case 1:
        i2c_master_read_slave_reg(TOUCH_ADDR,P1_XH_REG,&H_Pos,1);
        i2c_master_read_slave_reg(TOUCH_ADDR,P1_XL_REG,&L_Pos,1);
        data->FirstX= (unsigned int) ( ( (H_Pos & 0x07)<<8 ) | L_Pos );

        H_Pos=0x00;
        L_Pos=0x00;
        i2c_master_read_slave_reg(TOUCH_ADDR,P1_YH_REG,&H_Pos,1);
        i2c_master_read_slave_reg(TOUCH_ADDR,P1_YL_REG,&L_Pos,1);
        data->FirstY= (unsigned int) ( ( (H_Pos & 0x07)<<8 ) | L_Pos );

        AuxCMD=0x00;
        i2c_master_read_slave_reg(TOUCH_ADDR,P1_WEIGHT_REG,&AuxCMD,1);
        data->FirstWeight=(unsigned char) AuxCMD;

        break;

        case 2:
        //Dos puntos, por lo que leemos ambos
        //Primer punto
        i2c_master_read_slave_reg(TOUCH_ADDR,P1_XH_REG,&H_Pos,1);
        i2c_master_read_slave_reg(TOUCH_ADDR,P1_XL_REG,&L_Pos,1);
        data->FirstX= (unsigned int) ( ( (H_Pos & 0x07)<<8 ) | L_Pos );

        H_Pos=0x00;
        L_Pos=0x00;
        i2c_master_read_slave_reg(TOUCH_ADDR,P1_YH_REG,&H_Pos,1);
        i2c_master_read_slave_reg(TOUCH_ADDR,P1_YL_REG,&L_Pos,1);
        data->FirstY= (unsigned int) ( ( (H_Pos & 0x07)<<8 ) | L_Pos );

        //Segundo punto
        i2c_master_read_slave_reg(TOUCH_ADDR,P2_XH_REG,&H_Pos,1);
        i2c_master_read_slave_reg(TOUCH_ADDR,P2_XL_REG,&L_Pos,1);
        data->SecondX= (unsigned int) ( ( (H_Pos & 0x07)<<8 ) | L_Pos );

        H_Pos=0x00;
        L_Pos=0x00;
        i2c_master_read_slave_reg(TOUCH_ADDR,P2_YH_REG,&H_Pos,1);
        i2c_master_read_slave_reg(TOUCH_ADDR,P2_YL_REG,&L_Pos,1);
        data->SecondY= (unsigned int) ( ( (H_Pos & 0x07)<<8 ) | L_Pos );


        AuxCMD=0x00;
        i2c_master_read_slave_reg(TOUCH_ADDR,P2_WEIGHT_REG,&AuxCMD,1);
        data->SecondWeight=(unsigned char) AuxCMD;

        break;

        default: break;
    }




     ESP_LOGD(TAG, "#### Actualizado###\n");
     data->FirstRead=false; //Primera lectura con lo que lo ponemos a cero.

}

void MoveNowTouchToBefore(touch_t *data,int Touchs){


    switch (Touchs){
        case 0: break;
        case 1:
        data->BeforeFirstX=data->FirstX;
        data->FirstX=0x00;

        data->BeforeFirstY=data->FirstY;
        data->FirstY=0x00;

        data->BeforeFirstWeight=data->FirstWeight;
        data->FirstWeight=0x00;


        break;
        case 2:
        data->BeforeFirstX=data->FirstX;
        data->FirstX=0x00;

        data->BeforeFirstY=data->FirstY;
        data->FirstY=0x00;

        data->BeforeFirstWeight=data->FirstWeight;
        data->FirstWeight=0x00;

        //segundo touch
        data->BeforeSecondX=data->SecondX;
        data->SecondX=0x00;

        data->BeforeSecondY=data->SecondY;
        data->SecondY=0x00;

        data->BeforeSecondWeight=data->SecondWeight;
        data->SecondWeight=0x00;


        break;
        default: break;
    }





}
