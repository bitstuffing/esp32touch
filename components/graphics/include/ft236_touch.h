#ifndef FT236_TOUCH_H
#define FT236_TOUCH_H



/*El dispositivo es capaz de detectar gestos en vez de pulsaciones, tipo hacer zoom in/out.

Cuando es solo una pulsacion obtenemos un 0x00 No gesture
 
Gesture ID[7:0]
Gesture ID
0x10 Move Up 
0x14 Move Right 
0x18 Move Down 
0x1C Move Left 
0x48 Zoom In 
0x49 Zoom Out 
0x00 No Gesture

El dispositivo hace una IRQ por Low Edge en el pin 33 del esp32.

TODO: Revisar
0xA4 G_MODE
default value 0x01
[7:0]
0x00: Interrupt Polling mode 0x01: Interrupt Trigger mode

*/


typedef struct {

    unsigned int FirstX;
    unsigned int FirstY;
    unsigned char FirstWeight;
    unsigned int SecondX;
    unsigned int SecondY;
    unsigned char SecondWeight;
    unsigned char TouchDetections;
    unsigned char GestType;

    //Valores anteriores al actual

    unsigned int BeforeFirstX;
    unsigned int BeforeFirstY;
    unsigned char BeforeFirstWeight;
    unsigned int BeforeSecondX;
    unsigned int BeforeSecondY;
    unsigned char BeforeSecondWeight;

    //Valor que reconoce la primera lectura
    _Bool FirstRead;

}touch_t;

typedef struct {
    unsigned int VersionInfo;
    unsigned char FirmID;
    unsigned char FocaltechPanelID;
    unsigned char ReleaseCodeID;
    unsigned char InterruptMode;
    unsigned char ControlMode;
    unsigned char RateActiveMode;
    unsigned char RateMonitorMode;




}ft236_t;

int initialize_touch(touch_t *data);
void update_touch(touch_t *data);
void Obtain_Chip_Info(ft236_t *data);
void set_InterruptMode(_Bool mode);
void MoveNowTouchToBefore(touch_t *data,int Touchs);
#endif







