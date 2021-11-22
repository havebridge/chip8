#include "chip8.h"
#include "window.h"

struct chip8 chip;
struct sdl_window screen;

void usage(int argc, char* argv[])
{
	printf("Usage: chip8 <ROM file>\n");
}

int main(int argc, char* argv[])
{
	srand(time(NULL));

	if (argc < 2)
	{
		usage(argc, argv);
		return 1;
	}

	window_init(&screen);

	chip_init(&chip);
	chip_load_rom(&chip, argv[1]);

	uint32_t* pixel_buffer = malloc(sizeof(uint32_t) * (WINDOW_HEIGHT * WINDOW_WIDTH));

	while (1)
	{
		chip_execute(&chip);

		if (chip.draw_flag == true)
		{
			chip.draw_flag = false;
			window_buffer(&screen, pixel_buffer, &chip);
			window_update(&screen, pixel_buffer);
		}

		chip_input(&chip);
	}

	window_close(&screen);
	return 0;
}