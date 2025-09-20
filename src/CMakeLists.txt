add_executable(${ProjectName}
    main.c button.c led.c ws2812.c game0.c games.c
)

pico_generate_pio_header(${ProjectName}
    ${CMAKE_CURRENT_LIST_DIR}/ws2812.pio
    OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/../include/generated
)

target_include_directories(${ProjectName} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ../include ../include/generated
)

target_link_libraries(${ProjectName} 
    pico_stdlib 
    FreeRTOS-Kernel-Heap4 
    hardware_pio
)

pico_add_extra_outputs(${ProjectName})

# enable USB serial (as stdio)
pico_enable_stdio_usb(${ProjectName} 1)
pico_enable_stdio_uart(${ProjectName} 0)
