file(GLOB FILES *.c *.h)
add_library(si4703 ${FILES})
target_link_libraries(si4703
	 pico_stdlib
	 pico_time
	 pico_printf
	 hardware_i2c
	 hardware_gpio
)

target_include_directories(si4703 PUBLIC ./)
