idf_component_register(SRCS
                            "backlight.c"
                            "display.c"
                            "ft236_touch.c"
                            "i2cTools.c"
                            "max1726x.c"
                            "bt_app_core.c"
                            "gbuf.c"
                      INCLUDE_DIRS "include"
                      REQUIRES driver freertos
                )
