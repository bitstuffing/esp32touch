#pragma once

#include <tf.h>

#define COLOR_BLACK 0x0000
#define COLOR_WHITE 0xFFFF
#define COLOR_GRAY 0x4A69
#define COLOR_ORANGE 0xDB60
#define COLOR_RED 0xF800
//some new colors
#define ALICEBLUE 0xF7DF
#define ANTIQUEWHITE 0xFF5A
#define AQUA 0x07FF
#define AQUAMARINE 0x7FFA
#define AZURE 0xF7FF
#define BEIGE 0xF7BB
#define BISQUE 0xFF38
#define BLACK 0x0000
#define BLANCHEDALMOND 0xFF59
#define BLUE 0x001F
#define BLUEVIOLET 0x895C
#define BROWN 0xA145
#define BURLYWOOD 0xDDD0
#define CADETBLUE 0x5CF4
#define CHARTREUSE 0x7FE0
#define CHOCOLATE 0xD343
#define CORAL 0xFBEA
#define CORNFLOWERBLUE 0x64BD
#define CORNSILK 0xFFDB
#define CRIMSON 0xD8A7
#define CYAN 0x07FF
#define DARKBLUE 0x0011
#define DARKCYAN 0x0451
#define DARKGOLDENROD 0xBC21
#define DARKGRAY 0xAD55
#define DARKGREEN 0x0320
#define DARKKHAKI 0xBDAD
#define DARKMAGENTA 0x8811
#define DARKOLIVEGREEN 0x5345
#define DARKORANGE 0xFC60
#define DARKORCHID 0x9999
#define DARKRED 0x8800
#define DARKSALMON 0xECAF
#define DARKSEAGREEN 0x8DF1
#define DARKSLATEBLUE 0x49F1
#define DARKSLATEGRAY 0x2A69
#define DARKTURQUOISE 0x067A
#define DARKVIOLET 0x901A
#define DEEPPINK 0xF8B2
#define DEEPSKYBLUE 0x05FF
#define DIMGRAY 0x6B4D
#define DODGERBLUE 0x1C9F
#define FIREBRICK 0xB104
#define FLORALWHITE 0xFFDE
#define FORESTGREEN 0x2444
#define FUCHSIA 0xF81F
#define GAINSBORO 0xDEFB
#define GHOSTWHITE 0xFFDF
#define GOLD 0xFEA0
#define GOLDENROD 0xDD24
#define GRAY 0x8410
#define GREEN 0x0400
#define GREENYELLOW 0xAFE5
#define HONEYDEW 0xF7FE
#define HOTPINK 0xFB56
#define INDIANRED 0xCAEB
#define INDIGO 0x4810
#define IVORY 0xFFFE
#define KHAKI 0xF731
#define LAVENDER 0xE73F
#define LAVENDERBLUSH 0xFF9E
#define LAWNGREEN 0x7FE0
#define LEMONCHIFFON 0xFFD9
#define LIGHTBLUE 0xAEDC
#define LIGHTCORAL 0xF410
#define LIGHTCYAN 0xE7FF
#define LIGHTGOLDENRODYELLOW 0xFFDA
#define LIGHTGREEN 0x9772
#define LIGHTGREY 0xD69A
#define LIGHTPINK 0xFDB8
#define LIGHTSALMON 0xFD0F
#define LIGHTSEAGREEN 0x2595
#define LIGHTSKYBLUE 0x867F
#define LIGHTSLATEGRAY 0x7453
#define LIGHTSTEELBLUE 0xB63B
#define LIGHTYELLOW 0xFFFC
#define LIME 0x07E0
#define LIMEGREEN 0x3666
#define LINEN 0xFF9C
#define MAGENTA 0xF81F
#define MAROON 0x8000
#define MEDIUMAQUAMARINE 0x6675
#define MEDIUMBLUE 0x0019
#define MEDIUMORCHID 0xBABA
#define MEDIUMPURPLE 0x939B
#define MEDIUMSEAGREEN 0x3D8E
#define MEDIUMSLATEBLUE 0x7B5D
#define MEDIUMSPRINGGREEN 0x07D3
#define MEDIUMTURQUOISE 0x4E99
#define MEDIUMVIOLETRED 0xC0B0
#define MIDNIGHTBLUE 0x18CE
#define MINTCREAM 0xF7FF
#define MISTYROSE 0xFF3C
#define MOCCASIN 0xFF36
#define NAVAJOWHITE 0xFEF5
#define NAVY 0x0010
#define OLDLACE 0xFFBC
#define OLIVE 0x8400
#define OLIVEDRAB 0x6C64
#define ORANGE 0xFD20
#define ORANGERED 0xFA20
#define ORCHID 0xDB9A
#define PALEGOLDENROD 0xEF55
#define PALEGREEN 0x9FD3
#define PALETURQUOISE 0xAF7D
#define PALEVIOLETRED 0xDB92
#define PAPAYAWHIP 0xFF7A
#define PEACHPUFF 0xFED7
#define PERU 0xCC27
#define PINK 0xFE19
#define PLUM 0xDD1B
#define POWDERBLUE 0xB71C
#define PURPLE 0x8010
#define RED 0xF800
#define ROSYBROWN 0xBC71
#define ROYALBLUE 0x435C
#define SADDLEBROWN 0x8A22
#define SALMON 0xFC0E
#define SANDYBROWN 0xF52C
#define SEAGREEN 0x2C4A
#define SEASHELL 0xFFBD
#define SIENNA 0xA285
#define SILVER 0xC618
#define SKYBLUE 0x867D
#define SLATEBLUE 0x6AD9
#define SLATEGRAY 0x7412
#define SNOW 0xFFDF
#define SPRINGGREEN 0x07EF
#define STEELBLUE 0x4416
#define TAN 0xD5B1
#define TEAL 0x0410
#define THISTLE 0xDDFB
#define TOMATO 0xFB08
#define TURQUOISE 0x471A
#define VIOLET 0xEC1D
#define WHEAT 0xF6F6
#define WHITE 0xFFFF
#define WHITESMOKE 0xF7BE
#define YELLOW 0xFFE0
#define YELLOWGREEN 0x9E66

extern tf_t *ui_font_black;
extern tf_t *ui_font_red;
extern tf_t *ui_font_white;

/// Load/confiugure ui
void ui_init(void);
/// Free ui resources
void ui_free(void);

/// Display an error message
void ui_message_error(const char *msg);
/// Draw the pathbar that is located under the status bar
void ui_draw_pathbar(const char *left, const char *right, bool fruncate);
/// Draw image to screen buffer
void ui_draw_image(uint16_t **pixels,int xP,int yP,int width,int height);
