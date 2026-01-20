#include "display.h"
#include "spi.h"
#include <sam.h>

#define PIN_CS PORT_PB04
#define PIN_RST PORT_PB06
#define PIN_EN PORT_PB07
#define PIN_RW PORT_PB08
#define PIN_DC PORT_PB09

uint16_t pixels[DISP_BUF_SIZE * DISP_BUF_SIZE];

static void display_send(uint8_t val) {
	PORT_REGS->GROUP[1].PORT_OUTCLR = PIN_DC;
	spiWriteByte(val);
	PORT_REGS->GROUP[1].PORT_OUTSET = PIN_DC;
}

static void begin_spi() {
	spiActivate(1, 0, 0, 1);
	PORT_REGS->GROUP[1].PORT_OUTCLR = PIN_CS;
}

static void end_spi() {
	PORT_REGS->GROUP[1].PORT_OUTSET = PIN_CS;
	spiDeactivate();
}

// Initialize the display, must be called first
void display_init() {
    PORT_REGS->GROUP[1].PORT_DIRSET = PIN_CS | PIN_EN | PIN_RW | PIN_DC | PIN_RST;
    PORT_REGS->GROUP[1].PORT_OUTSET = PIN_CS | PIN_EN | PIN_DC;
    PORT_REGS->GROUP[1].PORT_OUTCLR = PIN_RW | PIN_RST;
	// RST is special and different and so has to be set back again
    PORT_REGS->GROUP[1].PORT_OUTSET = PIN_RST;

	spiInit();
	begin_spi();
	// Enable display
	display_send(0xAF);
	// Set a bunch of settings
	display_send(0xA0);
	spiWriteByte(0x32);
	// Set the starting row
	display_send(0xA1);
	spiWriteByte(0x20);
	// Enhance display performance??
	// Why is this a setting lol
	display_send(0xB2);
	spiWriteByte(0xA4);
	spiWriteByte(0);
	spiWriteByte(0);
	end_spi();
}

// Clear the screen (does not affect the framebuffer)
void display_clear() {
	display_clear_section((ScreenRect) {
		.ax = 0, .ay = 0, .bx = DISP_BUF_SIZE, .by = DISP_BUF_SIZE
	});
}

// Clear a section of the screen (does not affect the framebuffer)
void display_clear_section(ScreenRect dim) {
	begin_spi();
	// Set the X bounds
	display_send(0x15);
	spiWriteByte(dim.ax);
	spiWriteByte(dim.bx - 1);
	// Set the Y bounds
	display_send(0x75);
	spiWriteByte(dim.ay);
	spiWriteByte(dim.by - 1);

	// Begin sending data
	display_send(0x5C);
	for (int32_t i = dim.ay; i < dim.by; ++i) {
		for (int32_t j = dim.ax; j < dim.bx; ++j) {
			spiWriteByte(0);
			spiWriteByte(0);
		}
	}

	end_spi();
}

// Update the entire screen to reflect the framebuffer
void display_update_screen() {
	display_update_section((ScreenRect) {
		.ax = 16, .ay = 0, .bx = 16 + DISPLAY_SIZE, .by = DISPLAY_SIZE
	});
}

// Update only a section of the screen
void display_update_section(ScreenRect dim) {
	begin_spi();
	// Set the X bounds
	display_send(0x15);
	spiWriteByte(dim.ax);
	spiWriteByte(dim.bx - 1);
	// Set the Y bounds
	display_send(0x75);
	spiWriteByte(dim.ay);
	spiWriteByte(dim.by - 1);

	// Begin sending data
	display_send(0x5C);
	for (int32_t i = dim.ay; i < dim.by; ++i) {
		for (int32_t j = dim.ax; j < dim.bx; ++j) {
			spiWriteByte(pixels[i * DISP_BUF_SIZE + j] >> 8);
			spiWriteByte(pixels[i * DISP_BUF_SIZE + j] & 0xFF);
		}
	}

	end_spi();
}

void display_set_pixel(uint8_t x, uint8_t y, uint16_t color) {
	pixels[y * DISP_BUF_SIZE + x + 0x10] = color;
}
