/*
 * controller.c
 *
 * Created: 2024-08-01 20:37:47
 * Author : mateu
 */ 

#define F_CPU 16000000
#define BAUD 57600
#define MYUBRR F_CPU/16/BAUD-1

#define true 1
#define false 0

#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include "usart.h"
#include "software_serial.h"
#include "config.h"
#include "servo.h"
#include "lcd.h"
#include "buttons.h"
#include "config_screen.h"
#include "config_serial.h"
#include "control.h"

void transmit_binary(uint16_t data, uint8_t bits){
	int mask = 1 << (bits - 1);
	for (int i = 0; i < bits; i++){
		USART_transmit((data & mask) ? '1' : '0');
		mask >>= 1;
	}
}

int8_t bit_index(uint8_t data){
	int8_t state_index = -1;
	for (uint8_t bit = 0; bit < 5; bit++){
		if (data & (1 << bit)){
			if (state_index == -1){
				state_index = bit;
			}
			else{
				// if state_index is already set, it means that there are more than one non zero bit in the sample
				// it is considered invalid state
				state_index = -2;
			}
		}
	}
	return state_index;
}

void process_sample(uint8_t data, const config_t* config, uint8_t* release_out, float* speed_out_normalized){
	static uint8_t prev_state = 0;
	static uint16_t state_rep = 0;
	
	//static uint8_t prev_non_zero_state = 0;
	//static uint16_t uncertain_steps = 0;

	static float target_speed_normalized = 0;
	
	if ((data >> 5) == 0b010){
		if (data != prev_state){
			if (state_rep < config->valid_sample_timeout){
				//if high frequency samples are being received
				transmit_binary(data, 5);
				USART_transmit('\n');
				
				int8_t state_index = bit_index(data);
				if (state_index >= 0){
					//uncertain_steps = 0;
					//target_speed_normalized = config->speeds[state_index];
					// map from 0-4 to -1 to 1
					target_speed_normalized = (float)state_index / 2.0f - 1.0f;
					*release_out = false;
					USART_print("Target speed: ");
					USART_print_int(target_speed_normalized, 10);
					USART_print("\n");
				}
				else if (state_index == -2){
					USART_print("None-control data: ");
					transmit_binary(data, 5);
					USART_print("\n");
				}
			}
			else{		
				// samples are not frequent enough to be sure
				target_speed_normalized = 0.0f;
				if (*speed_out_normalized < 0.0001f && *speed_out_normalized > -0.0001f){
					// release if speed is already at 0
					*release_out = true;
				}

				/*if (uncertain_steps <= config->max_uncertain_steps){
					uncertain_steps++;
					int8_t state_index = bit_index(prev_non_zero_state);
					*speed_out = (int16_t)(config->speeds[state_index] * config->uncertain_speed_mult);
					*release_out = false;
					USART_print("Fine Speed: ");	
					USART_print_int(*speed_out, 10);
					USART_print("\n");
					is_uncertain = true;
				}
				else{
					*release_out = true;
					*speed_out = 0;
					USART_print("Out of range\n");
					is_uncertain = true;
				}*/
			}
			//if (data & 0b11111){
			//	prev_non_zero_state = data;
			//}
			prev_state = data;
			state_rep = 0;
		}
		else{
			state_rep++;
		}

		if (target_speed_normalized != *speed_out_normalized){
			if (*speed_out_normalized > 0){
				if (target_speed_normalized > *speed_out_normalized){
					if (target_speed_normalized - *speed_out_normalized < config->step_up){
						*speed_out_normalized = target_speed_normalized;
					}
					else{
						*speed_out_normalized += config->step_up;
					}
				}
				else{
					if (*speed_out_normalized - target_speed_normalized < config->step_down){
						*speed_out_normalized = target_speed_normalized;
					}
					else{
						*speed_out_normalized -= config->step_down;
					}
				}
			}
			else{
				if (target_speed_normalized > *speed_out_normalized){
					if (target_speed_normalized - *speed_out_normalized < config->step_down){
						*speed_out_normalized = target_speed_normalized;
					}
					else{
						*speed_out_normalized += config->step_down;
					}
				}
				else{
					if (*speed_out_normalized - target_speed_normalized < config->step_up){
						*speed_out_normalized = target_speed_normalized;
					}
					else{
						*speed_out_normalized -= config->step_up;
					}
				}
			}
		}
	}
	else{
		*release_out = true;
		target_speed_normalized = 0.0f;
		USART_print("Bad data\n");
	}
}

int main(void){
	// default config
	const config_t default_config = {
		.valid_sample_timeout = 20,
		.positions_a = {
			45, 45/2, 0, -45/2, -45
		},
		.positions_b = {
			45, 45/2, 0, -45/2, -45
		},
		.release_position_a = 45,
		.release_position_b = -45,
		.step_up = 0.05f,
		.step_down = 0.01f
	};

	config_t config = default_config;
	
	USART_init(MYUBRR);
	
	// try to load config from EEPROM
	uint8_t result = config_load(&config);
	if (result){
		USART_print("Config loaded\n");
	}
	else{
		USART_print("Using default config\n");
	}
	config_print(&config);
		
	servo_init();
	software_serial_init();
	lcd_init();
	buttons_init();
	
	float speed_normalized = 0;
	uint8_t release = true;
	uint8_t raw_sensor_data = 0;

	uint8_t mode = MODE_RUNNING;
	
	// TODO: reset speed after timeout (in case sensor got disconnected)
	
    while (1){
		if (USART_avaliable()){
			char data = USART_read();
			process_serial(data, &config);
		}

		if (mode == MODE_RUNNING){
			uint8_t armed = switch_armed();
			if (software_serial_available()){
				uint8_t data = software_serial_read();
				process_sample(data, &config, &release, &speed_normalized);
				raw_sensor_data = data;
			}
			
			if (!armed){
				release = true;
			}
			if (release){
				set_speed(config.release_position_a, config.release_position_b);
			}
			else{
				set_normalized_speed(&config, speed_normalized);
			}

			lcd_set_cursor(0, 0);
			for (int i = 4; i >= 0; i--){
				if (raw_sensor_data & (1 << i)){
					lcd_print("#");
				}
				else{
					lcd_print("-");
				}
			}
			
			if (armed){
				lcd_set_cursor(1, 16 - 5);
				lcd_print("Armed");
				lcd_set_cursor(1, 0);
				if (release){
					lcd_print("Release ");
				}
				else{
					lcd_print_float(speed_normalized);
					lcd_print("      ");
				}
			}
			else{
				lcd_set_cursor(1, 0);
				lcd_print("Disarmed        ");
			}

			if (button_up() && button_down() && !button_ok()){
				mode = MODE_SETUP;
				lcd_clear();
				lcd_print("Setup mode");
				_delay_ms(1000);
				lcd_clear();
			}
		}

		if (mode == MODE_SETUP){
			setup_mode(&config, &mode, &default_config);
		}
    }
}

