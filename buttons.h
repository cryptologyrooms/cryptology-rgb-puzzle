#ifndef _BUTTONS_H_
#define _BUTTONS_H_

void buttons_setup(BinaryOutput * pButtonSelect);
void buttons_tick(int32_t fake_button, uint8_t max_level);
uint8_t const * buttons_get_levels();
void buttons_reset_level(uint8_t level);

#endif
