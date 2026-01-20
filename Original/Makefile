all: main.elf

# device specific naming and paths
DEVICE = same51j20a
CPU = Cortex-M4
BOARD = microchip_same51_curiosity_nano
PACK = E51-pack

# standard compiling setup
CC = arm-none-eabi-gcc
DEVICE_UPPER = $(shell echo $(DEVICE) | tr '[:lower:]' '[:upper:]')

INCLUDE_PATHS = -I../../$(PACK)/include -I../../Core/include
CFLAGS = -x c -mthumb -mcpu=$(CPU) -D__$(DEVICE_UPPER)__ -O0 -ffunction-sections -Wall -c -std=gnu99
LDFLAGS = -Wl,--start-group -lm -Wl,--end-group -Wl,--gc-sections -mthumb -mcpu=$(CPU) -T../../$(PACK)/gcc/gcc/$(DEVICE)_flash.ld

SYS_OBJS = ../../$(PACK)/gcc/system_$(DEVICE).o ../../$(PACK)/gcc/gcc/startup_$(DEVICE).o ../dcc_stdio.o

HDRS = $(wildcard *.h)
SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

debug: CFLAGS += -g
debug: all
fastprinter: CFLAGS += -O2
fastprinter: all
release: CFLAGS += -DNDEBUG -O2
release: all

.PRECIOUS: %.o

# build an executable file
%.elf: $(OBJS)
	$(CC) -o $*.elf $(LDFLAGS) $(OBJS) $(SYS_OBJS) -lm
	arm-none-eabi-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature  $*.elf $*.hex
	arm-none-eabi-objdump -h -S $*.elf > $*.lss
	arm-none-eabi-size $*.elf

# build object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) $*.c -o $*.o -lm

install: main-install

# put an executable on the device
%-install: %.elf
	openocd -f board/$(BOARD).cfg -c "program $*.elf verify reset exit"

clean:
	rm -f *.o *.elf *.hex *.lss
