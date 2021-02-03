export PATH="/opt/gcc-arm/bin:$PATH"
make clean
make
arm-none-eabi-objcopy -Obinary rgb3216.elf rgb3216.bin
st-flash write rgb3216.bin 0x08000000
