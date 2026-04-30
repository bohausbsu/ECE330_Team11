/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : seg7.c
  * @brief          : Main program body
  ******************************************************************************
  *
  *
  * Copyright (c) 2023 Boise State University
  * All rights reserved.
  *
  * This file provides a library for outputting characters to the 7 segment
  * displays on the STM32F4 Discovery Peripheral Board for ECE330L lab.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "seg7.h"

extern void Seven_Segment_Digit (unsigned char digit, unsigned char hex_char, unsigned char dot);
extern void Seven_Segment_ASCII (unsigned char digit, unsigned char ascii_char, unsigned char dot);
extern void Seven_Segment(unsigned int HexValue);

#define COMMON_ANODE 1
/* Character set Common Anode */
#if COMMON_ANODE == 1

// Characters assigned numerically based on 0-9, A-Z, plus special characters
char _7SEG[] =
		{~(0x3F),~(0x06),~(0x5B),~(0x4F),~(0x66),~(0x6D),~(0x7D),
		~(0x07),~(0x7F),~(0x6F),~(0x77),~(0x7C),~(0x39),~(0x5E),
		~(0x79),~(0x71),~(0x3D),~(0x76),~(0x06),~(0x1E),~(0x7A),
		~(0x38),~(0x15),~(0x54),~(0x3F),~(0x73),~(0x67),~(0x50),
		~(0x6D),~(0x78),~(0x3E),~(0x62),~(0x2A),~(0x64),~(0x6E),
		~(0x5B),~(0x80),~(0x48),~(0x01),~(0x02),~(0x04),~(0x08),
		~(0x10),~(0x20),~(0x40),~(0x00)};

// Characters assigned based on ASCII character codes
unsigned char _7SEG_ASCII[]=
{
	['0'] = ~(0x3F),
	['1'] = ~(0x06),
	['2'] = ~(0x5B),
	['3'] = ~(0x4F),
	['4'] = ~(0x66),
	['5'] = ~(0x6D),
	['6'] = ~(0x7D),
	['7'] = ~(0x07),
	['8'] = ~(0x7F),
	['9'] = ~(0x6F),
	['A'] = ~(0x77),
	['B'] = ~(0x7C),
	['C'] = ~(0x39),
	['D'] = ~(0x5E),
	['E'] = ~(0x79),
	['F'] = ~(0x71),
	['G'] = ~(0x3D),
	['H'] = ~(0x76),
	['I'] = ~(0x06),
	['J'] = ~(0x1E),
	['K'] = ~(0x7A),
	['L'] = ~(0x38),
	['M'] = ~(0x15),
	['N'] = ~(0x54),
	['O'] = ~(0x3F),
	['P'] = ~(0x73),
	['Q'] = ~(0x67),
	['R'] = ~(0x50),
	['S'] = ~(0x6D),
	['T'] = ~(0x78),
	['U'] = ~(0x3E),
	['V'] = ~(0x62),
	['W'] = ~(0x2A),
	['X'] = ~(0x64),
	['Y'] = ~(0x6E),
	['Z'] = ~(0x5B),
	['a'] = ~(0x77),
	['b'] = ~(0x7C),
	['c'] = ~(0x39),
	['d'] = ~(0x5E),
	['e'] = ~(0x79),
	['f'] = ~(0x71),
	['g'] = ~(0x3D),
	['h'] = ~(0x76),
	['i'] = ~(0x06),
	['j'] = ~(0x1E),
	['k'] = ~(0x7A),
	['l'] = ~(0x38),
	['m'] = ~(0x15),
	['n'] = ~(0x54),
	['o'] = ~(0x3F),
	['p'] = ~(0x73),
	['q'] = ~(0x67),
	['r'] = ~(0x50),
	['s'] = ~(0x6D),
	['t'] = ~(0x78),
	['u'] = ~(0x3E),
	['v'] = ~(0x62),
	['w'] = ~(0x2A),
	['x'] = ~(0x64),
	['y'] = ~(0x6E),
	['z'] = ~(0x5B),
	['.'] = ~(0x80),
	['='] = ~(0x48),
	[' '] = ~(0x00),
	['-'] = ~(0x20),
	['_'] = ~(0x04),
	[127] = 0,
};


#else

// Characters assigned numerically based on 0-9, A-Z, plus special characters
char _7SEG[] =
		{(0x3F),(0x06),(0x5B),(0x4F),(0x66),(0x6D),(0x7D),
		(0x07),(0x7F),(0x6F),(0x77),(0x7C),(0x39),(0x5E),
		(0x79),(0x71),(0x3D),(0x76),(0x06),(0x1E),(0x7A),
		(0x38),(0x15),(0x54),(0x3F),(0x73),(0x67),(0x50),
		(0x6D),(0x78),(0x3E),(0x62),(0x2A),(0x64),(0x6E),
		(0x5B),(0x80),(0x48),(0x01),(0x02),(0x04),(0x08),
		(0x10),(0x20),(0x40),(0x00)};

// Characters assigned based on ASCII character codes
unsigned char _7SEG_ASCII[]=
{
	['0'] = (0x3F),
	['1'] = (0x06),
	['2'] = (0x5B),
	['3'] = (0x4F),
	['4'] = (0x66),
	['5'] = (0x6D),
	['6'] = (0x7D),
	['7'] = (0x07),
	['8'] = (0x7F),
	['9'] = (0x6F),
	['A'] = (0x77),
	['B'] = (0x7C),
	['C'] = (0x39),
	['D'] = (0x5E),
	['E'] = (0x79),
	['F'] = (0x71),
	['G'] = (0x3D),
	['H'] = (0x76),
	['I'] = (0x06),
	['J'] = (0x1E),
	['K'] = (0x7A),
	['L'] = (0x38),
	['M'] = (0x15),
	['N'] = (0x54),
	['O'] = (0x3F),
	['P'] = (0x73),
	['Q'] = (0x67),
	['R'] = (0x50),
	['S'] = (0x6D),
	['T'] = (0x78),
	['U'] = (0x3E),
	['V'] = (0x62),
	['W'] = (0x2A),
	['X'] = (0x64),
	['Y'] = (0x6E),
	['Z'] = (0x5B),
	['a'] = (0x77),
	['b'] = (0x7C),
	['c'] = (0x39),
	['d'] = (0x5E),
	['e'] = (0x79),
	['f'] = (0x71),
	['g'] = (0x3D),
	['h'] = (0x76),
	['i'] = (0x06),
	['j'] = (0x1E),
	['k'] = (0x7A),
	['l'] = (0x38),
	['m'] = (0x15),
	['n'] = (0x54),
	['o'] = (0x3F),
	['p'] = (0x73),
	['q'] = (0x67),
	['r'] = (0x50),
	['s'] = (0x6D),
	['t'] = (0x78),
	['u'] = (0x3E),
	['v'] = (0x62),
	['w'] = (0x2A),
	['x'] = (0x64),
	['y'] = (0x6E),
	['z'] = (0x5B),
	['.'] = (0x80),
	['='] = (0x48),
	[' '] = (0x00),
	['-'] = (0x20),
	['_'] = (0x04),
	[127] = 0,
};
#endif



void Seven_Segment_Digit (unsigned char digit, unsigned char hex_char, unsigned char dot)
{
/*******************************************************************************
Code to mask and bit shift 0-7 value of digit and 0-15 value of hex_char
to output correct bit pattern to GPIO_Output
*******************************************************************************/
	// Set selected digit to 0, all others high, and output 7 segment pattern
	GPIOE->ODR = (0xFF00 | _7SEG[hex_char]) & ~(1<<(digit+8));

	#if COMMON_ANODE == 1
	if (dot > 0) GPIOE->ODR &= ~(1<<7); // Set dot segment to 0 if dot is on
	#else
	if (dot > 0) GPIOE->ODR |= (1<<7); // Set dot segment to 1 if dot is on
	#endif

	// Set all selects high to latch-in character
	GPIOE->ODR |= 0xFF00;
	return;
}

void Seven_Segment_ASCII (unsigned char digit, unsigned char ascii_char, unsigned char dot)
{
	// Set selected digit to 0, all others high, and output 7 segment pattern
	GPIOE->ODR = (0xFF00 | _7SEG_ASCII[ascii_char]) & ~(1<<(digit+8));

	#if COMMON_ANODE == 1
	if (dot > 0) GPIOE->ODR &= ~(1<<7); // Set dot segment to 0 if dot is on
	#else
	if (dot > 0) GPIOE->ODR |= (1<<7); // Set dot segment to 1 if dot is on
	#endif

	// Set all selects high to latch-in character
	GPIOE->ODR |= 0xFF00;
	return;
}

void Seven_Segment(unsigned int HexValue)
{
/******************************************************************************
Use a for loop to output HexValue to 7 segment display digits
*******************************************************************************/
	char digit;
	// Send hex values to lower 4 digits
	for (digit=0 ; digit<8 ; digit++)
	{
	   Seven_Segment_Digit(digit,(HexValue >> (digit*4)) & 0xF,0);
	}

	return;
}

/*===========================================================================
 * Battleship Display Engine
 *===========================================================================*/
void Update_Battleship_Display(PlayerBoard *board, PlayerBoard *opponent,
                               uint8_t cx, uint8_t cy, uint8_t cv_h,
                               uint8_t is_shooting)
{
    int i;

    /* 1. Clear both display buffers */
    for (i = 0; i < 8; i++) {
        display_buffer[i]    = 0;
        display_buffer_50[i] = 0;
    }

    /* 2. Determine which segments are 100% bright vs 50% */
    uint32_t v_100 = 0, v_50 = 0;
    uint32_t h_100 = 0, h_50 = 0;

    if (is_shooting) {
        /* Shooting mode: show hits at 100%, misses at 50% */
        v_100 = board->vert_shots  & board->vert_boats;    /* hits  */
        v_50  = board->vert_shots  & ~board->vert_boats;   /* misses */
        h_100 = board->horiz_shots & board->horiz_boats;
        h_50  = board->horiz_shots & ~board->horiz_boats;
    } else {
        /* Placement mode: show all placed boats at 100% */
        v_100 = board->vert_boats;
        h_100 = board->horiz_boats;
    }

    /* 3. Map bits onto 7-segment display buffers */
    for (i = 0; i < 8; i++) {
        /*--- Horizontal segments ---*/
        /* Row 0 → segment a (bit 0) */
        if ((h_100 >> i) & 1)       display_buffer[i]    |= (1 << 0);
        if ((h_50  >> i) & 1)       display_buffer_50[i] |= (1 << 0);

        /* Row 1 → segment g (bit 6) */
        if ((h_100 >> (i + 8)) & 1) display_buffer[i]    |= (1 << 6);
        if ((h_50  >> (i + 8)) & 1) display_buffer_50[i] |= (1 << 6);

        /* Row 2 → segment d (bit 3) */
        if ((h_100 >> (i + 16)) & 1) display_buffer[i]    |= (1 << 3);
        if ((h_50  >> (i + 16)) & 1) display_buffer_50[i] |= (1 << 3);

        /*--- Vertical segments ---*/
        /* Top-left: f (bit 5) — vert bit (16 + 2*i) */
        if ((v_100 >> (16 + 2*i)) & 1)     display_buffer[i]    |= (1 << 5);
        if ((v_50  >> (16 + 2*i)) & 1)     display_buffer_50[i] |= (1 << 5);

        /* Top-right: b (bit 1) — vert bit (16 + 2*i + 1) */
        if ((v_100 >> (16 + 2*i + 1)) & 1) display_buffer[i]    |= (1 << 1);
        if ((v_50  >> (16 + 2*i + 1)) & 1) display_buffer_50[i] |= (1 << 1);

        /* Bot-left: e (bit 4) — vert bit (2*i) */
        if ((v_100 >> (2*i)) & 1)           display_buffer[i]    |= (1 << 4);
        if ((v_50  >> (2*i)) & 1)           display_buffer_50[i] |= (1 << 4);

        /* Bot-right: c (bit 2) — vert bit (2*i + 1) */
        if ((v_100 >> (2*i + 1)) & 1)       display_buffer[i]    |= (1 << 2);
        if ((v_50  >> (2*i + 1)) & 1)       display_buffer_50[i] |= (1 << 2);
    }

    /* 4. Compute cursor digit and segment for blinking (handled by SysTick) */
    if (cv_h == 0) {
        /* Vertical cursor: cx = 0..15, cy = 0..1 */
        cursor_digit = cx / 2;
        if (cy == 0) {
            /* Top row: odd=left(f), even=right(b) */
            cursor_seg = (cx % 2 == 1) ? (1 << 5) : (1 << 1);  /* f or b */
        } else {
            /* Bottom row: odd=left(e), even=right(c) */
            cursor_seg = (cx % 2 == 1) ? (1 << 4) : (1 << 2);  /* e or c */
        }
    } else {
        /* Horizontal cursor: cx = 0..7, cy = 0..2 */
        cursor_digit = cx;
        if (cy == 0)      cursor_seg = (1 << 0);  /* a */
        else if (cy == 1) cursor_seg = (1 << 6);  /* g */
        else              cursor_seg = (1 << 3);  /* d */
    }
}
