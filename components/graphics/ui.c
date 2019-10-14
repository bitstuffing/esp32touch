#include <ui.h>
#include "esp_log.h"
#include <display.h>
#include <gbuf.h>
#include <tf.h>
#include <graphics.h>
#include <event.h>
#include <OpenSans_Regular_11X12.h>
#include <str_utils.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>


tf_t *ui_font_black;
tf_t *ui_font_white;
tf_t *ui_font_red;

void ui_init(void)
{
	ui_font_black = tf_new(&font_OpenSans_Regular_11X12, COLOR_BLACK, 0, TF_WORDWRAP);
	ui_font_white = tf_new(&font_OpenSans_Regular_11X12, COLOR_WHITE, 0, TF_WORDWRAP);
	ui_font_red = tf_new(&font_OpenSans_Regular_11X12, COLOR_RED, 0, TF_WORDWRAP);
}

void ui_free(void)
{
	tf_free(ui_font_black);
	tf_free(ui_font_white);
	tf_free(ui_font_red);
}

void ui_message_error(const char *msg)
{
	const int ypos = 112;
	const rect_t r = (rect_t){.x = 0, .y = ypos, .width = DISPLAY_WIDTH, .height = 16};
	fill_rectangle(fb, r, COLOR_RED);
	tf_draw_str(fb, ui_font_white, msg, (point_t){.x = 3, .y = ypos + 3});
	fprintf(stderr, "error: %s\n", msg);
	display_update_rect(r);

	event_t event;
	for (;;) {
		wait_event(&event);
		if (event.type == EVENT_TYPE_QUIT || (event.type == EVENT_TYPE_KEYPAD && event.keypad.pressed))
			break;
	}
}

void ui_draw_pathbar(const char *left, const char *right, bool fruncate)
{
	assert(left != NULL);

	fill_rectangle(fb, (rect_t){.x = 0, .y = 16, .width = DISPLAY_WIDTH, .height = 15}, 0xFFFF);

	// Draw left side
	const int max_left_len = 35;
	char left_buf[max_left_len + 1];
	const size_t left_len = strlen(left);
	// Eventually truncate left size
	if (left_len > max_left_len) {
		if (fruncate) {
			fruncate_str(left_buf, left, max_left_len);
		} else {
			truncate_str(left_buf, left, max_left_len);
		}
		left = left_buf;
	}
	tf_draw_str(fb, ui_font_black, left, (point_t){.x = 3, .y = 18});

	// Draw right side if present
	if (right != NULL) {
		const tf_metrics_t m = tf_get_str_metrics(ui_font_black, right);
		tf_draw_str(fb, ui_font_black, right, (point_t){.x = DISPLAY_WIDTH - m.width - 3, .y = 18});
	}

	display_update_rect((rect_t){.x = 0, .y = 16, .width = DISPLAY_WIDTH, .height = 15});
}

static inline uint16_t get_bgnd_pixel(uint16_t **pixels,int x, int y)
{
    return pixels[y][x];
}

static int8_t xofs[320], yofs[240];
static int8_t xcomp[320], ycomp[240];

void ui_draw_image(uint16_t **pixels,int xP,int yP,int width,int height){

		uint16_t *lines[2];
    //Allocate memory for the pixel buffers
    for (int i=0; i<2; i++) {
        lines[i]=heap_caps_malloc(320*PARALLEL_LINES*sizeof(uint16_t), MALLOC_CAP_DMA);
        assert(lines[i]!=NULL);
    }
    int frame=0;
    //Indexes of the line currently being sent to the LCD and the line we're calculating.
    int sending_line=-1;
    int calc_line=0;

    if(1) { //TODO
        frame++;
        for (int y=0; y<240; y+=PARALLEL_LINES) {
            //Calculate each line.

            uint16_t *dest = lines[calc_line]; //target line

            for (int yy=y; yy<y+PARALLEL_LINES; yy++) {
                for (int x=0; x<320; x++) {
                    *dest++=get_bgnd_pixel(pixels,x+yofs[yy]+xcomp[x], yy+xofs[x]+ycomp[yy]);
                }
            }

            //Finish up the sending process of the previous line, if any
            if (sending_line!=-1) send_line_finish();
            //Swap sending_line and calc_line
            sending_line=calc_line;
            calc_line=(calc_line==1)?0:1;
            //Send the line we currently calculated.
            send_lines(y, lines[sending_line]);
            //The line set is queued up for sending now; the actual sending happens in the
            //background. We can go on to calculate the next line set as long as we do not
            //touch line[sending_line]; the SPI sending process is still reading from that.
        }
    }


}
