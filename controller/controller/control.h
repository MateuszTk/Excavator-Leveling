#ifndef CONTROL_H_
#define CONTROL_H_

#include "servo.h"
#include "config.h"

void set_speed(int8_t speed_a, int8_t speed_b){
	servo_set_angle_minus_90_90(speed_a, 1);
	servo_set_angle_minus_90_90(speed_b, 2);
}

void set_normalized_speed(config_t* config, float speed_normalized){
	int8_t speed_a = 0;
	int8_t speed_b = 0;

	float speed_0_1 = (speed_normalized + 1.0f) / 2.0f; 
	int8_t position_floor = (int8_t)(speed_0_1 * 4);
	int8_t position_ceil = position_floor + 1;
	float position_blend = (speed_0_1 * 4.0f) - position_floor;

	if (position_ceil > 4){
		position_ceil = 4;
		position_floor = 4;
	}

	int8_t speed_a_floor = config->positions_a[position_floor];
	int8_t speed_a_ceil = config->positions_a[position_ceil];
	int8_t speed_b_floor = config->positions_b[position_floor];
	int8_t speed_b_ceil = config->positions_b[position_ceil];

	speed_a = (int8_t)(speed_a_floor + (speed_a_ceil - speed_a_floor) * position_blend);
	speed_b = (int8_t)(speed_b_floor + (speed_b_ceil - speed_b_floor) * position_blend);

	servo_set_angle_minus_90_90(speed_a, 1);
	servo_set_angle_minus_90_90(speed_b, 2);
}

#endif /* CONTROL_H_ */