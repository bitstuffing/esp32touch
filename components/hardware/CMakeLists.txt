idf_component_register(SRCS
                            "backlight.c"
                            "display.c"
                            "ft236_touch.c"
                            "i2cTools.c"
                            "max1726x.c"
                      INCLUDE_DIRS
                      REQUIRES driver freertos
                )
