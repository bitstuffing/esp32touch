idf_component_register(SRCS "main.c"
                            "bluetooth.c"
                            "status_bar.c"
                            "decode_image.c"
                            "image.c"
                            "http_server.c"
                    INCLUDE_DIRS "include"
                    EMBED_FILES "background.jpg" "folder.bmp" "folder.jpg" "favicon.ico" "upload_script.html")
