#ifndef _BUTTONS_H_
#define _BUTTONS_H_

void buttons_setup(BinaryOutput * pButtonSelect);
void buttons_tick();
uint8_t const * buttons_get_levels();

#endif
