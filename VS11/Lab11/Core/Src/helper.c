#include "main.h"
#include "usb_host.h"
#include "seg7.h"
#include "map.h"

// For Boat placement and shooting logic

int PlaceBoat(PlayerMap *p) {
    uint32_t *target_map = (cursor_mode == HORIZONTAL) ? &p->boats_horiz : &p->boats_vert;
        uint32_t bit = CalculateBitMask(cursor_x, cursor_y, cursor_mode);
        
        if (!(*target_map & bit)) { // If spot is empty
            *target_map |= bit;
            return 1; 
        }
        return 0; // If spot is occupied
}

int FireShot(PlayerMap *opponent) {
    uint32_t *shot_map = (cursor_mode == HORIZONTAL) ? &opponent->shots_horiz : &opponent->shots_vert;
    uint32_t *boat_map = (cursor_mode == HORIZONTAL) ? &opponent->boats_horiz : &opponent->boats_vert;
    uint32_t bit = CalculateBitMask(cursor_x, cursor_y, cursor_mode);

    if (!(*shot_map & bit)) { // Prevent shooting same spot twice
        *shot_map |= bit;
        if (*boat_map & bit) {
            opponent->hits_received++;
            // Trigger Explosion Sound
        } else {
            // Trigger Splash Sound
        }
        return 1;
    }
    return 0;
}

uint32_t CalculateBitMask(int x, int y, int mode) {
    if (mode == HORIZONTAL) {
        // Row 0: Top, Row 1: Mid, Row 2: Bot
        return (1UL << ((2 - y) * 8 + x));
    } else {
        // Row 0: Top Vertical, Row 1: Bot Vertical
        return (1UL << ((1 - y) * 16 + x));
    }
}
