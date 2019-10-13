#include <ui.h>
#include <ui_controls.h>
#include <display.h>
#include <gbuf.h>
#include <tf.h>
#include <graphics.h>
#include <event.h>
#include <OpenSans_Regular_11X12.h>
#include <str_utils.h>
#include <ft236_touch.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "esp_log.h"

#define MSG "qazSwsx edc rfv tgb yhnIujmDik,Aol.Apñ-E"
#define KEYBOARD_START_Y 160
#define KEYBOARD_COLUMNS 10
#define KEYBOARD_ROWS 4
#define KEYBOARD_KEY_WIDTH 32
#define KEYBOARD_KEY_HEIGHT 20

void display_keyboard(void){

	const rect_t r = (rect_t){.x = 0, .y = KEYBOARD_START_Y, .width = DISPLAY_WIDTH, .height = DISPLAY_HEIGHT-KEYBOARD_START_Y};
	fill_rectangle(fb, r, COLOR_GRAY);
  display_update_rect(r);
  int a=0;
  for (int i=1;i<=KEYBOARD_COLUMNS;i++){
    for (int j=1;j<=KEYBOARD_ROWS;j++){
      //display rectangle with label
      const rect_t rKey = (rect_t){.x = (i-1)*KEYBOARD_KEY_WIDTH, .y = KEYBOARD_START_Y + ((j-1)*KEYBOARD_KEY_HEIGHT), .width = KEYBOARD_KEY_WIDTH, .height = KEYBOARD_KEY_HEIGHT};
    	fill_rectangle(fb, rKey, COLOR_GRAY);
      char key[2] = "";
      memset(key, 0, sizeof((char)MSG[a++]));
      sprintf(key,"%c",(char)MSG[a++]);
      tf_draw_str(fb, ui_font_white, key, (point_t){.x = (i-1)*KEYBOARD_KEY_WIDTH+3, .y = KEYBOARD_START_Y + ((j-1)*KEYBOARD_KEY_HEIGHT)+3});
      display_update_rect(rKey);
    }
  }
}

char get_keyboard_touched_key(touch_t *touch){
  unsigned int firstX = (unsigned int)touch->FirstY;
  unsigned int firstY = (unsigned int)touch->FirstX;
  //char key[2] = "";
	char key = '\0';

  if(firstY>=KEYBOARD_START_Y){
    ESP_LOGD("keyboard","%d %d",firstX,firstY);
    unsigned int yCor = (int)((firstY-KEYBOARD_START_Y)/(KEYBOARD_KEY_HEIGHT));
    unsigned int xCor = KEYBOARD_COLUMNS-1-(int)(firstX/(KEYBOARD_KEY_WIDTH));

    //sprintf(key,"%c",(char)MSG[xCor*4+yCor]);
		key = (char)MSG[xCor*4+yCor];

    ESP_LOGD("keyboard","coord. is %d,%d",xCor,yCor);
    ESP_LOGI("keyboard","key is: %c",key);
    //redraw pushed key
    //const rect_t rKey = (rect_t){.x = (xCor)*KEYBOARD_KEY_WIDTH, .y = KEYBOARD_START_Y + ((yCor)*KEYBOARD_KEY_HEIGHT), .width = KEYBOARD_KEY_WIDTH, .height = KEYBOARD_KEY_HEIGHT};
    //tf_draw_str(fb, ui_font_white, key, (point_t){.x = (xCor)*KEYBOARD_KEY_WIDTH+3, .y = KEYBOARD_START_Y + ((yCor)*KEYBOARD_KEY_HEIGHT)+3});
    //fill_rectangle(fb, rKey, COLOR_GREY);
    //display_update_rect(rKey);
    ESP_LOGD("keyboard","exit %d %d",firstX,firstY);

  }
  return key;
}
