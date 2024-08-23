
#ifndef SERVO_H_
#define SERVO_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define SERVO_1_DDR DDRB
#define SERVO_1_PORT PORTB
#define SERVO_1_PIN PB1

#define SERVO_2_DDR DDRB
#define SERVO_2_PORT PORTB
#define SERVO_2_PIN PB2

#define SERVO_MIN 500
#define SERVO_MAX 2500

void servo_init(){
    SERVO_1_DDR |= (1 << SERVO_1_PIN);
    SERVO_2_DDR |= (1 << SERVO_2_PIN);
    
    // Configure Timer1, Fast PWM, prescaler 8, clear OC1A on compare match, set OC1A at BOTTOM
    // 16MHz / 8 = 2MHz
    TCCR1A = (1 << COM1A1) | (1 << WGM11) | (1 << COM1B1);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    
    ICR1 = 39999;

    OCR1A = SERVO_MIN * 2;
    OCR1B = SERVO_MIN * 2;
}

void servo_1_set(uint16_t value){
    if (value < SERVO_MIN){
        value = SERVO_MIN;
    }
    else if (value > SERVO_MAX){
        value = SERVO_MAX;
    }
    
    OCR1A = value * 2;
}

void servo_2_set(uint16_t value){
    if (value < SERVO_MIN){
        value = SERVO_MIN;
    }
    else if (value > SERVO_MAX){
        value = SERVO_MAX;
    }
    
    OCR1B = value * 2;
}

void servo_set_angle_180(uint8_t angle, uint8_t servo){
    if (angle > 180){
        angle = 180;
    }
    
    if (servo == 1){
        servo_1_set(SERVO_MIN + (((uint32_t)angle) * (SERVO_MAX - SERVO_MIN) / 180));
    }
    else{
        servo_2_set(SERVO_MIN + (((uint32_t)angle) * (SERVO_MAX - SERVO_MIN) / 180));
    }
}

void servo_set_angle_minus_90_90(int8_t angle, uint8_t servo){
    if (angle < -90){
        angle = -90;
    }
    else if (angle > 90){
        angle = 90;
    }
    
    servo_set_angle_180(angle + 90, servo);
}

#endif /* SERVO_H_ */
