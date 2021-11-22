#ifndef _CHIP8_H
#define _CHIP8_H

#define _CRT_SECURE_NO_WARNINGS

#include <SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t

#define memory_size 4096
#define num_reg 16
#define stack_size 16
#define num_keys 16

#define SCREEN_HEIGHT 32
#define SCREEN_WIDTH 64

struct chip8
{
	u8 memory[memory_size];
	u16 stack[stack_size];
	u8 display[SCREEN_HEIGHT][SCREEN_WIDTH];

	u16 opcode;

	u8 reg[num_reg];
	u16 reg_I; // This register is generally used to store memory addresses
	u16 pc;
	u16 sp;

	uint8_t keyboard[num_keys];
	bool key_pressed;

	u8 delay_timer;
	u8 sound_timer;

	bool draw_flag;

	FILE* rom;
};


void chip_init(struct chip8*);
void chip_load_rom(struct chip8*, const char*);
void chip_execute(struct chip8*);
void chip_input(struct chip8*);

#endif