echo Setting Fuses................

avrdude -v -patmega128 -cusb -Pusb -e -Ulock:w:0x3f:m -Uefuse:w:0xff:m -Uhfuse:w:0b11000110:m -Ulfuse:w:0b10111111:m

echo Writing Bootloader...........

avrdude -v -patmega128 -cusb -Pusb -Uflash:w:optiboot_flash_atmega128_UART0_38400_8000000L_B5_BIGBOOT.hex -Ulock:w:0x0f:m

pause