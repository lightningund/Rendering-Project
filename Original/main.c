#include <math.h>
#include <sam.h>
#include <stdlib.h>
#include "vec.h"
#include "ray.h"
#include "cell.h"
#include "display.h"
#include "thumbstick.h"
#include "map.h"
#include "defines.h"
#include "textures.h"
#include "task.h"

// Number of cycles in a single ms
#define MS_TO_CYCLES 20

// Number of ticks in a cycle (1/20th of a second)
#define CYCLE_TICKS (120000/MS_TO_CYCLES)

// How long (nominally) a frame should be
#define UPDATE_CYCLES (50 * MS_TO_CYCLES)

#define WIDTH 96
#define HEIGHT 96
// Field of View (in degrees)
#define FOV 110.0

// How much to turn whenever you press a button (in radians)
#define TURN_AMNT (PI / 8)

struct _Column {
	int extent;
	int prev_extent;
	uint16_t col;
	uint8_t tex_id;
	float t;
	int shade;
} typedef Column;

#define NUM_TEX 4

bool wall_textured = true;

static color_t textures[] = {RED, GREEN, BLUE, WHITE};

float ang_height;
float ang_height_sq;
float ang_height_sq_inv;

Vec cam_pos = (Vec) {.x = 10, .y = 5};
float cam_ang = 0;

Ray rays[WIDTH];
Column cols[WIDTH];

bool to_be_generated;

volatile cycles_t cycles = 0;

void print_bitset(idx_t val) {
	#ifndef NDEBUG
		char bits[sizeof(idx_t) * 8 + 1];
		bits[sizeof(idx_t) * 8] = '\0';
		for (int i = 0; i < sizeof(idx_t) * 8; ++i) {
			bits[i] = ((val >> i) & 1) ? '1' : '0';
		}

		write_str(bits);
	#endif
}

float radians(float deg) {
	return (deg / 180.0) * PI;
}

void enable_fpu() {
	uint32_t* cpacr = (uint32_t*)0xE000ED88;
	// "Set bits 20-23 to enable CP10 and CP11 coprocessors"
	*cpacr |= 0xF << 20;
}

void overclock() {
	// set generator 2 to use DFLL48M as source; with a divider of 48 that gives us 1MHz
	GCLK_REGS->GCLK_GENCTRL[2] = GCLK_GENCTRL_DIV(48) | GCLK_GENCTRL_SRC_DFLL | GCLK_GENCTRL_GENEN_Msk;
	WAIT_FOR_NOT_VAL(GCLK_REGS->GCLK_SYNCBUSY, GCLK_SYNCBUSY_GENCTRL_GCLK2);

	// set DPLL0 peripheral to use generator 2 as its clock
	// see page 156 of data sheet for GCLK array offsets
	GCLK_REGS->GCLK_PCHCTRL[1] = GCLK_PCHCTRL_GEN_GCLK2 | GCLK_PCHCTRL_CHEN_Msk;
	WAIT_FOR_VAL(GCLK_REGS->GCLK_PCHCTRL[1], GCLK_PCHCTRL_CHEN_Msk);

	// DPLL in integer mode: multiply generator clk by 120, giving us 120MHz
	OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLCTRLB = OSCCTRL_DPLLCTRLB_FILTER(0) | OSCCTRL_DPLLCTRLB_LTIME(0)| OSCCTRL_DPLLCTRLB_REFCLK(0);
	OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLRATIO = OSCCTRL_DPLLRATIO_LDRFRAC(0) | OSCCTRL_DPLLRATIO_LDR(119);
	WAIT_FOR_NOT_VAL(OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLSYNCBUSY, OSCCTRL_DPLLSYNCBUSY_DPLLRATIO_Msk);

	// Enable DPLL0
	OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLCTRLA = OSCCTRL_DPLLCTRLA_ENABLE_Msk;
	WAIT_FOR_NOT_VAL(OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLSYNCBUSY, OSCCTRL_DPLLSYNCBUSY_ENABLE_Msk);

	// Wait for "ready state"
	WAIT_FOR_VAL(OSCCTRL_REGS->DPLL[0].OSCCTRL_DPLLSTATUS, (OSCCTRL_DPLLSTATUS_LOCK_Msk | OSCCTRL_DPLLSTATUS_CLKRDY_Msk));

	// define our main generic clock, which drives everything, to be 120MHz from the PLL
	MCLK_REGS->MCLK_CPUDIV = MCLK_CPUDIV_DIV_DIV1;
	// Wait for main clock to be ready
	WAIT_FOR_VAL(MCLK_REGS->MCLK_INTFLAG, MCLK_INTFLAG_CKRDY_Msk);

	GCLK_REGS->GCLK_GENCTRL[0] = GCLK_GENCTRL_DIVSEL_DIV1 | GCLK_GENCTRL_SRC_DPLL0 | GCLK_GENCTRL_GENEN_Msk;
	WAIT_FOR_NOT_VAL(GCLK_REGS->GCLK_SYNCBUSY, GCLK_SYNCBUSY_GENCTRL_GCLK0);
}

void heart_init() {
	NVIC_EnableIRQ(SysTick_IRQn);

	SysTick_Config(CYCLE_TICKS);
}

void SysTick_Handler() {
	++cycles;
}

uint32_t buttons;

// Setup all the buttons
void init_buttons() {
	// Board Button
	PORT_REGS->GROUP[0].PORT_DIRCLR |= PORT_PA15;
	PORT_REGS->GROUP[0].PORT_OUTSET |= PORT_PA15;
	PORT_REGS->GROUP[0].PORT_PINCFG[15] = PORT_PINCFG_INEN_Msk | PORT_PINCFG_PULLEN_Msk;
}

void read_btns() {
	// Read the raw button state
	uint32_t btn_state = PORT_REGS->GROUP[0].PORT_IN;

	// Filter out anything but buttons that are on now that weren't before
	uint32_t btn_rising = (buttons ^ btn_state) & btn_state;

	// Update the previous state
	buttons = btn_state;

	// Board Button
	if (btn_rising & (1 << 15)) {
		wall_textured = !wall_textured;
	}
}

void draw_render() {
	for (int y = 0; y < HEIGHT / 2; ++y) {
		for (int x = 0; x < WIDTH; ++x) {
			if (cols[x].extent < y && cols[x].extent < (HEIGHT / 2) - y) {
				if (wall_textured) {
					color_t color = get_tex(cols[x].tex_id, cols[x].extent, y, cols[x].t);
					display_set_pixel(x, y, fast_color_scale(color, cols[x].shade));
				} else {
					display_set_pixel(x, y, cols[x].col);
				}
			} else if (cols[x].prev_extent < y && cols[x].prev_extent < (HEIGHT / 2) - y) {
				display_set_pixel(x, y, 0);
			}
		}
	}

	for (int i = 0; i < WIDTH; ++i) {
		cols[i].prev_extent = cols[i].extent;
	}

	display_update_section((ScreenRect) {
		.ax = 16,
		.ay = 0,
		.bx = 112,
		.by = 96 / 2
	});
}

uint8_t get_ray_wall(const Ray* ray) {
	return cell_wall_section(*ray->cell_hit->walls, ray->cell_wall);
}

// Calculate the height and shade of a single pixel column based on the ray
void update_column(const Ray ray, int x) {
	static const float H4 = HEIGHT / 4;
	static const float W2 = WIDTH / 2;

	uint8_t ray_wall = get_ray_wall(&ray);

	// If it's fake then just blank it out and leave
	if (cell_wall_is_fake(ray_wall, 0)) {
		cols[x].extent = WIDTH;
		return;
	}

    // float a = atan2(x - (WIDTH / 2), ang_height);
    // float d = cos(a) * ray.dist;
    // float diff = (HEIGHT / 4) - (atan(1 / d) * 200);
	float diff = H4 - 200 * atan(sqrt(1 + ((x - W2) * (x - W2) * ang_height_sq_inv)) / ray.dist);
	cols[x].extent = (int)diff;
	float shade = 1 - (3 * ray.dist / WIDTH);
	// Make the shade more pronounced
	shade *= shade;
	// shade *= shade;
	// Make the walls always at least a little bit visible
	if (shade < 0.05) shade = 0.05;
	// Limit it just in case
	if (shade > 1) shade = 1;
	if (wall_textured) {
		cols[x].shade = 1 / shade;
		cols[x].tex_id = cell_wall_texture(ray_wall, 0) % NUM_TEX;
		cols[x].t = ray.t;
	} else {
		cols[x].col = textures[cell_wall_texture(ray_wall, 0) % NUM_TEX];
		cols[x].col = color_mult(cols[x].col, shade);
	}
}

void update_columns() {
	for (int i = 0; i < WIDTH; ++i) {
		update_column(rays[i], i);
	}
}

void update_ray(Ray* ray) {
	ray->dist = INFINITY;
	ray->cell_hit = NULL;
	for (int j = 0; j < MAX_CELLS; ++j) {
		if (cell_free(j)) continue;

		ray_cast_cell(ray, &cells[j]);
	}

	if (ray->cell_hit != NULL) {
		ray->cell_hit->hit = true;

		// If we have room to make a new cell
		if (!cells_full()) {
			// If we don't already have another cell to generate
			if (!to_be_generated) {
				GridCell* walls = ray->cell_hit->walls;
				uint8_t dir = ray->cell_wall;
				// If the wall that the ray hit was fake
				if (cell_wall_is_fake(*walls, dir)) {
					// Set the wall on the cell we hit to be actually empty now instead of fake
					make_cell_wall_real(walls, dir);
					make_cell_wall_gone(walls, dir);
					TBG_DIR = dir;
					TBG_X = ray->cell_hit->x + DirOffsets[TBG_DIR][0];
					TBG_Y = ray->cell_hit->y + DirOffsets[TBG_DIR][1];
					to_be_generated = true;
				}
			}
		}
	}
}

void update_rays() {
	for (int i = 0; i < WIDTH; ++i) {
		rays[i].pos = cam_pos;
		rays[i].dir = vec_from_heading(cam_ang + atan2(i - WIDTH / 2, ang_height));
		update_ray(&rays[i]);
	}
}

void draw_map() {
	map_draw(cam_pos);
}

void update_cells() {
	for (int i = 0; i < MAX_CELLS; ++i) {
		if (cell_free(i)) continue;

		// Remove this cell if it wasn't hit by any rays and it's not the cell the player is currently in
		if (!cells[i].hit && !inside_cell(cells[i], cam_pos)) {
			remove_cell(i);
		}

		cells[i].hit = false;
	}
}

void move_cam() {
	Vec stick = sample_stick();
	// Center the range
	stick.x -= 0x800;
	stick.y -= 0x800;
	// Stick deadzone
	if (fabs(stick.x) < 0x100) stick.x = 0;
	if (fabs(stick.y) < 0x100) stick.y = 0;
	// Scale the range
	stick = vec_scale(stick, 1.0 / 0x800);
	// Turn the camera
	cam_ang += stick.x / 16;
	// Move the camera
	cam_pos = vec_add(cam_pos, vec_scale(vec_from_heading(cam_ang), -stick.y));
}

void gen_cell() {
	if (!to_be_generated) return;
	generate_cell();
	to_be_generated = false;
}

void blinky() {
	PORT_REGS->GROUP[0].PORT_OUTTGL = PORT_PA14;
}

int main() {
	#ifndef NDEBUG
		// Waiting for the debugger to actually connect
		for (int i = 0; i < 100000; ++i) {}
		#ifdef BRK_ON_ENTRY
			__BKPT(0);
		#endif
	#endif

	overclock();

	enable_fpu();

	heart_init();
	init_grid();
	display_init();
	display_clear();
	init_stick();
	init_buttons();

	__enable_irq();

	PM_REGS->PM_SLEEPCFG = PM_SLEEPCFG_SLEEPMODE_IDLE;

	PORT_REGS->GROUP[0].PORT_DIRSET = PORT_PA14;
	PORT_REGS->GROUP[0].PORT_OUTSET = PORT_PA14;

	TBG_X = 0;
	TBG_Y = 0;
	TBG_DIR = 0;
	generate_cell();
	*cells[0].walls = 0xD952; // Only north wall fake
	// TBG_X = 0;
	// TBG_Y = 1;
	// TBG_DIR = 0;
	// generate_cell();
	// *cells[1].walls = 0xD852;
	// TBG_X = 0;
	// TBG_Y = 2;
	// TBG_DIR = 0;
	// generate_cell();
	// *cells[2].walls = 0xD852;
	// TBG_X = 0;
	// TBG_Y = 3;
	// TBG_DIR = 0;
	// generate_cell();
	// *cells[3].walls = 0xD851;

	ang_height = ((WIDTH / 2) * tan(radians(90 - (FOV / 2))));
	ang_height_sq = ang_height * ang_height;
	ang_height_sq_inv = 1.0 / ang_height_sq;

	Task raycast_task = (Task) {
		.fcn = &update_rays,
		.priority = 100,
		.period = UPDATE_CYCLES,
		.deadline = UPDATE_CYCLES * 0.8
	};

	Task update_column_task = (Task) {
		.fcn = &update_columns,
		.priority = 90,
		.period = UPDATE_CYCLES,
		.deadline = UPDATE_CYCLES * 0.8
	};

	Task draw_render_task = (Task) {
		.fcn = &draw_render,
		.priority = 80,
		.period = UPDATE_CYCLES,
		.deadline = UPDATE_CYCLES * 0.8
	};

	Task update_cells_task = (Task) {
		.fcn = &update_cells,
		.priority = 90,
		.period = UPDATE_CYCLES,
		.deadline = UPDATE_CYCLES * 0.8
	};

	Task draw_map_task = (Task) {
		.fcn = &draw_map,
		.priority = 80,
		.period = UPDATE_CYCLES,
		.deadline = UPDATE_CYCLES * 0.8
	};

	// Run every 10ms, regardless of update speed
	Task joystick_task = (Task) {
		.fcn = &move_cam,
		.priority = 120,
		.period = 10 * MS_TO_CYCLES,
		.deadline = UPDATE_CYCLES * 0.1
	};

	Task generate_cell_task = (Task) {
		.fcn = &gen_cell,
		.priority = 150,
		.period = UPDATE_CYCLES,
		.deadline = UPDATE_CYCLES * 0.1
	};

	Task toggle_light_task = (Task) {
		.fcn = &blinky,
		.priority = 200,
		.period = UPDATE_CYCLES * 2,
		.deadline = UPDATE_CYCLES * 0.1
	};

	// Periodic tasks that need to keep being re-added
	Task* peri_tasks[] = {
		&raycast_task, // 19.72ms
		&update_column_task, // 5.125ms
		&draw_render_task, // 13.515ms
		&draw_map_task, // 11.245ms
		&update_cells_task, // <0.05ms
		&joystick_task, // ~0.6ms
		&generate_cell_task, // <0.05ms
		&toggle_light_task // <0.05ms
	};
	// Total time: ~50ms
	int num_periodic_tasks = sizeof(peri_tasks) / sizeof(peri_tasks[0]);

	// // For benchmarking
	// for (int i = 0; i < num_periodic_tasks; ++i) {
	// 	cycles_t start = cycles;
	// 	// Run 10 times for a better average
	// 	for (int j = 0; j < 10; ++j) {
	// 		(*peri_tasks[i]->fcn)();
	// 	}
	// 	cycles_t diff = cycles - start;
	// 	write_u32(&diff, 1);
	// }

	while (true) {
		__WFI();

		// Update the random
		rand();

		// Check the buttons
		read_btns();

		// Re-adding periodic tasks
		// Screenshot the current time
		cycles_t curr_time = cycles;
		for (int i = 0; i < num_periodic_tasks; ++i) {
			Task* t = peri_tasks[i];
			if ((t->period > 0 && curr_time > t->next_add)) {
				if (!t->added) {
					add_task(t, curr_time);
				} else {
					// write_str("Took too long for task ");
					// write_u32(&i, 1);
					// write_char('\n');
				}
			}
		}

		if (!is_empty()) {
			Task* t = pop_task();
			(*t->fcn)();
		}
	}
}
