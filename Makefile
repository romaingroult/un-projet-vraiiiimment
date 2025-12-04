
WARNINGS := -Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align \
            -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations \
            -Wredundant-decls -Wnested-externs -Winline -Wno-long-long \
            -Wstrict-prototypes -Wno-array-bounds

CFLAGS := -Os -DF_CPU=16000000UL -mmcu=atmega328p $(WARNINGS)

PORT := COM12
PROGRAMMER := arduino
BAUD := 115200

SRCS := main.c uart.c
LIBS := libring_buffer.a
TARGET := program

all: flash

$(TARGET).elf: $(SRCS) $(LIBS)
	avr-gcc $(CFLAGS) -o $@ $(SRCS) $(LIBS)

$(TARGET).hex: $(TARGET).elf
	avr-objcopy -O ihex $< $@

flash: $(TARGET).hex
	avrdude -v -patmega328p -c$(PROGRAMMER) -P$(PORT) -b$(BAUD) -D -Uflash:w:$<:i

clean:
	rm -f *.elf *.hex
