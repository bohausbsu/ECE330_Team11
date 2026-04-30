#include "main.h"
#include "usb_host.h"
#include "seg7.h"

#define VERTICAL 0
#define HORIZONTAL 1

typedef struct {
  uint32_t boats_vert;  // Bits 31-16: top vert, 15-0, bot vert
  uint32_t boats_horiz; // Byte 2: Top row, Byte 1: Mid row, Byte 0: bot row
  uint32_t shots_vert;  // Record shots fired at vertical segments
  uint32_t shots_horiz; // Record shots fired at orizontal segments
  int hits_received;
} PlayerMap;

extern uint8_t display_buffer[8];
extern int frame_counter; // 0-9 for PWM

void SetHorizontalBoat(uint32_t *map, int x, int y) {
    // y = 0 is top row (bit 16-23)
    // y = 1 is mid row (bit 8-15)
    // y = 2 is bot row (bit 0-7)
    int shift = (2 - y) * 8 + x;
    *map |= (1UL << shift);
}

void SetVerticalBoat(uint32_t *map, int x, int y, int side) {
    // y = 0 is top vert (bits 16-31)
    // y = 1 is bot vert (bits 0-15)
    // Side 0 is left, 1 is right
    int row_offset = (1 - y) * 16;
    int side_offset = (1 - side) * 8;
    *map |= (1UL << (row_offset + side_offset + x)); 
}