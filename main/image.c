
#include "image.h"

void display_image(){

  esp_err_t ret;

  //Reference the binary-included jpeg file
  extern uint8_t imageJpg[] asm("_binary_background_jpg_start");

  ret=decode_image(&pixels,imageJpg,320,240);
  ESP_ERROR_CHECK(ret);

  ui_draw_image(pixels,0,0,320,240);

  extern uint8_t imageBmpFolder[] asm("_binary_folder_bmp_start");
/*

  TODO, fix
  ret=decode_image(&pixels,imageBmpFolder,320,240);
  ESP_ERROR_CHECK(ret);

  ui_draw_image(pixels,0,0,96,96);
*/
  //rect_t window_rect = (rect_t){.x = 0, .y = 32, .width = DISPLAY_WIDTH, .height = DISPLAY_HEIGHT - 32};

  gbuf_t img = {.width = (uint16_t)24,
      .height = (uint16_t)24,
      .bytes_per_pixel = 2,
      .data = (uint8_t *)&imageBmpFolder,
      .big_endian = false};

  blit(fb, (rect_t){.x = 0, .y = 0, .width = img.width, .height = img.height - 1}, &img,
     (rect_t){.x = 0, .y = 1, .width = img.width, .height = img.height - 1});

  display_update_rect((rect_t){.x = 0, .y = 0, .width = img.width, .height = img.height - 1});


}
