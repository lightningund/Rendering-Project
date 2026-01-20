#include "thumbstick.h"
#include "spi.h"
#include "defines.h"

#define PIN_CS PORT_PB05
#define PIN_MISO PORT_PA19
#define PIN_MOSI PORT_PA16

static void begin_spi_recv() {
	spiActivate(255, true, false, true);
	PORT_REGS->GROUP[1].PORT_OUTCLR = PIN_CS;
}

static void end_spi() {
	PORT_REGS->GROUP[1].PORT_OUTSET = PIN_CS;
	spiDeactivate();
}

void init_stick() {
	spiInit();
	PORT_REGS->GROUP[0].PORT_DIRCLR = PIN_MISO;
	PORT_REGS->GROUP[0].PORT_DIRSET = PIN_MOSI;
	PORT_REGS->GROUP[1].PORT_DIRSET = PIN_CS;
}

Vec sample_stick() {
	begin_spi_recv();

	// Say we want to read CH1, the x axis
	spiWriteByte(0b110);
	uint16_t x = spiReadByte();
	spiWriteByte(0b01000000);
	x = spiReadByte() & 0b1111; // First 4 bits are garbage so we trash them
	x <<= 8;
	spiWriteByte(0);
	x |= spiReadByte();

	end_spi();
	begin_spi_recv();

	// Say we want to read CH0, the y axis
	spiWriteByte(0b110);
	uint16_t y = spiReadByte();
	spiWriteByte(0);
	y = spiReadByte() & 0b1111; // First 4 bits are garbage so we trash them
	y <<= 8;
	spiWriteByte(0);
	y |= spiReadByte();

	end_spi();
	return (Vec) {.x = x, .y = y};
}

void sample_stick_two(uint16_t* x, uint16_t* y) {
	begin_spi_recv();

	// Say we want to read CH0, the x axis
	spiWriteByte(0b1110000);
	uint16_t _ = spiReadByte();
	spiWriteByte(0);
	*x = spiReadByte();
	*x <<= 8;
	spiWriteByte(0);
	*x |= spiReadByte();
	*x >>= 4;

	end_spi();
	begin_spi_recv();

	// Say we want to read CH1, the y axis
	spiWriteByte(0b1110100);
	_ = spiReadByte();
	spiWriteByte(0);
	*y = spiReadByte();
	*y <<= 8;
	spiWriteByte(0);
	*y |= spiReadByte();
	*y >>= 4;

	end_spi();
}