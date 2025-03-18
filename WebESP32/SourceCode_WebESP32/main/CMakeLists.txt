# Add all files .c to compile
idf_component_register(SRCS "main.c" "HTTP_lib/HTTP_lib.c" "NVSFlash_lib/NVSFlash_lib.c" "UART_lib/UART_lib.c" "Queue_lib/Queue_lib.c" "Manage_lib/Manage_lib.c"
                    INCLUDE_DIRS ".")
