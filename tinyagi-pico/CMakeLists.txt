cmake_minimum_required(VERSION 3.13)

project(tinyagi_pico)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


add_executable(tinyagi_pico
    vga/vga.c
    audio/pwm_synth.c
    agi_sound_player/agi_sound.c
    archive.h
    main.c
)

pico_generate_pio_header(tinyagi_pico ${CMAKE_CURRENT_LIST_DIR}/video_first.pio)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(tinyagi_pico tinyagi pico_stdlib hardware_pio hardware_clocks hardware_dma hardware_sync hardware_irq hardware_pwm pico_multicore)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(tinyagi_pico)

pico_enable_stdio_usb(tinyagi_pico 0)
pico_enable_stdio_uart(tinyagi_pico 1)