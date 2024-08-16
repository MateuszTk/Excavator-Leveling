#ifndef BUTTONS_H_
#define BUTTONS_H_

#include <avr/io.h>

#define BUTTON_UP_DDR DDRD
#define BUTTON_UP_PORT PORTD
#define BUTTON_UP_PIN PIND
#define BUTTON_UP PD5

#define BUTTON_DOWN_DDR DDRD
#define BUTTON_DOWN_PORT PORTD
#define BUTTON_DOWN_PIN PIND
#define BUTTON_DOWN PD4

#define BUTTON_OK_DDR DDRD
#define BUTTON_OK_PORT PORTD
#define BUTTON_OK_PIN PIND
#define BUTTON_OK PD6

void buttons_init(){
    BUTTON_UP_DDR &= ~(1 << BUTTON_UP);
    BUTTON_UP_PORT |= (1 << BUTTON_UP);
    
    BUTTON_DOWN_DDR &= ~(1 << BUTTON_DOWN);
    BUTTON_DOWN_PORT |= (1 << BUTTON_DOWN);
    
    BUTTON_OK_DDR &= ~(1 << BUTTON_OK);
    BUTTON_OK_PORT |= (1 << BUTTON_OK);
}

uint8_t button_up(){
    return !(BUTTON_UP_PIN & (1 << BUTTON_UP));
}

uint8_t button_down(){
    return !(BUTTON_DOWN_PIN & (1 << BUTTON_DOWN));
}

uint8_t button_ok(){
    return !(BUTTON_OK_PIN & (1 << BUTTON_OK));
}

#endif /* BUTTONS_H_ */