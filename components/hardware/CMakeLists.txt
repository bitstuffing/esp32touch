idf_component_register(SRCS
                            "backlight.c"
                            "display.c"
                      INCLUDE_DIRS
                      REQUIRES driver freertos
                )
