#include "chip8.h"
#include "fontset.h"

uint8_t key[16] = {
	SDLK_x, 
	SDLK_1, 
	SDLK_2, 
	SDLK_3, 
	SDLK_q, 
	SDLK_w, 
	SDLK_e, 
	SDLK_a, 
	SDLK_s, 
	SDLK_d, 
	SDLK_z, 
	SDLK_c, 
	SDLK_4, 
	SDLK_r, 
	SDLK_f,
	SDLK_v  
};

void chip_init(struct chip8* chip)
{
	chip->pc = 0x200;
	chip->opcode = 0;
	chip->reg_I = 0;
	chip->sp = 0;

	memset(chip->memory, 0, sizeof(chip->memory));
	memset(chip->reg, 0, sizeof(chip->reg));
	memset(chip->stack, 0, sizeof(chip->stack));
	memset(chip->keyboard, 0, sizeof(chip->keyboard));

	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			chip->display[y][x] = 0;
		}
	}

	for (unsigned i = 0; i < FONTSET_SIZE; i++)
	{
		chip->memory[i] = chip8_fontset[i];
	}

	chip->draw_flag = false;
	chip->key_pressed = false;

	chip->delay_timer = 0;
	chip->sound_timer = 0;
}

void chip_load_rom(struct chip8* chip, const char* name)
{
	chip->rom = fopen(name, "rb");
	if (!chip->rom)
	{
		perror("Opening file failed: ");
		exit(EXIT_FAILURE);
	}

	fseek(chip->rom, 0, SEEK_END);
	long file_size = ftell(chip->rom);
	rewind(chip->rom);

	fread(chip->memory + 0x200, sizeof(uint8_t), (size_t)file_size, chip->rom);

	fclose(chip->rom);
}

void chip_execute(struct chip8* chip)
{
	chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];
	uint8_t x = (chip->opcode & 0x0F00) >> 8;
	uint8_t y = (chip->opcode & 0x00F0) >> 4;
	uint8_t pixel;

	switch (chip->opcode & 0xF000)
	{
	case 0x0000:
		switch (chip->opcode & 0x000F)
		{
		case 0x0000:
			for (int y = 0; y < SCREEN_HEIGHT; y++)
			{
				for (int x = 0; x < SCREEN_WIDTH; x++)
				{
					chip->display[y][x] = 0;
				}
			}
			chip->draw_flag = true;
			chip->pc += 2;
			break;
		case 0x000E:
			chip->sp--;
			chip->pc = chip->stack[chip->sp];
			chip->pc += 2;
			break;
		default:
			printf("The opcode is not found\n");
			exit(EXIT_FAILURE);
			break;
		}
		break;
	case 0x1000:
		chip->pc = (chip->opcode & 0x0FFF);
		break;
	case 0x2000:
		chip->stack[chip->sp] = chip->pc;
		chip->sp++;
		chip->pc = (chip->opcode & 0x0FFF);
		break;
	case 0x3000:
		if (chip->reg[x] == (chip->opcode & 0x00FF))
		{
			chip->pc += 4;
		}
		else
		{
			chip->pc += 2;
		}
		break;
	case 0x4000:
		if (chip->reg[x] != (chip->opcode & 0x00FF))
		{
			chip->pc += 4;
		}
		else
		{
			chip->pc += 2;
		}
		break;
	case 0x5000:
		if (chip->reg[x] == chip->reg[y])
		{
			chip->pc += 4;
		}
		else
		{
			chip->pc += 2;
		}
		break;
	case 0x6000:
		chip->reg[x] = (chip->opcode & 0x00FF);
		chip->pc += 2;
		break;
	case 0x7000:
		chip->reg[x] = chip->reg[x] + (chip->opcode & 0x00FF);
		chip->pc += 2;
		break;
	case 0x8000:
		switch (chip->opcode & 0x000F)
		{
		case 0x0000:
			chip->reg[x] = chip->reg[y];
			chip->pc += 2;
			break;
		case 0x0001:
			chip->reg[x] = chip->reg[x] | chip->reg[y];
			chip->pc += 2;
			break;
		case 0x0002:
			chip->reg[x] = chip->reg[x] & chip->reg[y];
			chip->pc += 2;
			break;
		case 0x0003:
			chip->reg[x] = chip->reg[x] ^ chip->reg[y];
			chip->pc += 2;
			break;
		case 0x0004:
			if ((chip->reg[x] + chip->reg[y]) > 255)
			{
				chip->reg[0xF] = 1;
			}
			else
			{
				chip->reg[0xF] = 0;
			}

			chip->reg[x] = (chip->reg[x] + chip->reg[y]) & 0xFF;
			chip->pc += 2;
			break;
		case 0x0005:
			if (chip->reg[x] > chip->reg[y])
			{
				chip->reg[0xF] = 1;
			}
			else
			{
				chip->reg[0xF] = 0;
			}

			chip->reg[x] = chip->reg[x] - chip->reg[y];
			chip->pc += 2;
			break;
		case 0x0006:
			if ((chip->reg[x] % 2) == 1)
			{
				chip->reg[0xF] = 1;
			}
			else
			{
				chip->reg[0xF] = 0;
			}
			
			chip->reg[x] = chip->reg[x] >> 1;
			chip->pc += 2;
			break;
		case 0x0007:
			if (chip->reg[y] > chip->reg[x])
			{
				chip->reg[0xF] = 1;
			}
			else
			{
				chip->reg[0xF] = 0;
			}

			chip->reg[x] = chip->reg[y] - chip->reg[x];
			chip->pc += 2;
			break;
		case 0x000E:
			if ((chip->reg[x] % 10000000) == 1)
			{
				chip->reg[0xF] = 1;
			}
			else
			{
				chip->reg[0xF] = 0;
			}

			chip->reg[x] = chip->reg[x] << 1;
			chip->pc += 2;
			break;
		default:
			printf("The opcode is not found\n");
			exit(EXIT_FAILURE);
			break;
		}
		break;
	case 0x9000:
		if (chip->reg[x] != chip->reg[y])
		{
			chip->pc += 4;
		}
		else
		{
			chip->pc += 2;
		}
		break;
	case 0xA000:
		chip->reg_I = (chip->opcode & 0x0FFF);
		chip->pc += 2;
		break;
	case 0xB000:
		chip->pc = (chip->opcode & 0x0FFF) + chip->reg[0];
		break;
	case 0xC000:
		chip->reg[x] = (rand() % 256) & (chip->opcode & 0x00FF);
		chip->pc += 2;
		break;
	case 0xD000:
		chip->reg[0xF] = false;
		for (int y = 0; y < (chip->opcode & 0x000F); y++)
		{
			pixel = chip->memory[chip->reg_I + y];
			for (int x = 0; x < 8; x++)
			{
				if ((pixel & (0x80 >> x)) != 0)
				{
					if (chip->display[chip->reg[y] + y][chip->reg[x] + x] == 1)
					{
						chip->reg[0xF] = true;
					}
					chip->display[chip->reg[y] + y][chip->reg[x] + x] ^= 1;
				}
			}
		}

		chip->draw_flag = true;
		chip->pc += 2;
		break;
	case 0xE000:
		switch (chip->opcode & 0x00FF)
		{
		case 0x009E:
			if (chip->keyboard[chip->reg[x]] != false)
			{
				chip->pc += 4;
			}
			else
			{
				chip->pc += 2;
			}
			break;
		case 0x00A1:
			if (chip->keyboard[chip->reg[x]] == false)
			{
				chip->pc += 4;
			}
			else
			{
				chip->pc += 2;
			}
			break;
 		default:
			printf("The opcode is not found\n");
			exit(EXIT_FAILURE);
			break;
		}
		break;
	case 0xF000:
		switch (chip->opcode & 0x00FF)
		{
		case 0x0007:
			chip->reg[x] = chip->delay_timer;
			chip->pc += 2;
			break;
		case 0x000A:
			chip->key_pressed = false;

			for (int i = 0; i < num_keys; i++)
			{
				if (chip->keyboard[i] != false)
				{
					chip->reg[x] = i;
					chip->key_pressed = true;
				}
			}

			if (!chip->key_pressed)
			{
				return;
			}

			chip->pc += 2;
			break;
		case 0x0015:
			chip->delay_timer = chip->reg[x];
			chip->pc += 2;
			break;
		case 0x0018:
			chip->sound_timer = chip->reg[x];
			chip->pc += 2;
			break;
		case 0x001E:
			chip->reg_I = chip->reg_I + chip->reg[x];
			chip->pc += 2;
			break;
		case 0x0029:
			chip->reg_I = (chip->reg[x] * 0x5);
			chip->pc += 2;
			break;
		case 0x0033:
			chip->memory[chip->reg_I] = chip->reg[x] / 100;
			chip->memory[chip->reg_I + 1] = (chip->reg[x] / 10) % 10;
			chip->memory[chip->reg_I + 2] = (chip->reg[x] % 100) % 10;
			chip->pc += 2;
			break;
		case 0x0055:
			for (uint8_t i = 0; i <= num_reg; i++)
			{
				chip->memory[chip->reg_I + i] = chip->reg[i];
			}

			chip->reg_I = chip->reg_I + (x + 1);

			chip->pc += 2;
			break;
		case 0x0065:
			for (uint8_t i = 0; i <= num_reg; i++)
			{
				chip->reg[i] = chip->memory[chip->reg_I + i];
			}

			chip->reg_I = chip->reg_I + (x + 1);

			chip->pc += 2;
			break;
		default:
			printf("The opcode is not found\n");
			exit(EXIT_FAILURE);	
			break;
		}
		break;
	default:
		printf("The opcode is not found\n");
		exit(EXIT_FAILURE);
		break;
	}

	if (chip->delay_timer > 0)
	{
		chip->delay_timer--;
	}
	if (chip->sound_timer > 0)
	{
		if (chip->sound_timer == 1)
		{
			printf("c", 7);
		}
		chip->sound_timer--;
	}
}

void chip_input(struct chip8* chip)
{
	SDL_Event e;

	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_KEYDOWN)
		{
			switch (e.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				exit(1);
			default:
				break;
			}

			for (uint8_t i = 0; i < num_keys; i++)
			{
				if (e.key.keysym.sym == key[i])
				{
					chip->keyboard[i] = true;
				}
			}
		}

		if (e.type == SDL_KEYUP)
		{
			for (uint8_t i = 0; i < num_keys; i++)
			{
				if (e.key.keysym.sym == key[i])
				{
					chip->keyboard[i] = false;
				}
			}
		}
	}

	printf("Opcode: %X program counter: %d\n", chip->opcode, chip->pc);
}