#ifndef CONFIG_SCREEN_H_
#define CONFIG_SCREEN_H_

#include "lcd.h"
#include "buttons.h"
#include "config.h"
#include <util/delay.h>
#include "control.h"


#define MODE_RUNNING 0
#define MODE_SETUP 1

void setup_exit(uint8_t* mode, config_t* config, uint8_t* focused){
    *mode = MODE_RUNNING;
    *focused = 0;
    //config_save(config);
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Exit");
    _delay_ms(1000);
    lcd_clear();
}

void setup_case(const char* text, uint8_t* focused, uint8_t* up_pressed, uint8_t* down_pressed, int16_t* config_value){
    lcd_set_cursor(0, 0);
    lcd_print(text);
    lcd_set_cursor(1, 0);
    lcd_print_int(*config_value, 10);
    if (*focused){
        if (*up_pressed){
            *config_value += 1;
        }
        if (*down_pressed){
            *config_value -= 1;
        }
    }
}

void setup_mode(config_t* config, uint8_t* mode){
	static uint8_t setup_position = 0;
	static uint8_t focused = 0;

	if (button_ok()){
		focused = !focused;
		if (focused){
			lcd_set_cursor(1, 15);
			lcd_print("<");
		}
		else{
			lcd_set_cursor(1, 15);
			lcd_print(" ");
		}
		_delay_ms(200);
	}
	if (!focused){
		if (button_up() && button_down() && !button_ok()){
			setup_exit(mode, config, &focused);
			return;
		}
		if (button_up()){
			setup_position = (setup_position + 1) % 16;
			_delay_ms(200);
			lcd_clear();
		}
		if (button_down()){
			setup_position = (setup_position + 15) % 16;
			_delay_ms(200);
			lcd_clear();
		}
	}

	uint8_t up_pressed = button_up();
	uint8_t down_pressed = button_down();

	if (focused && (up_pressed || down_pressed)){
		lcd_set_cursor(1, 0);
		lcd_print("               ");
	}
	// TODO: ask if servo preview should be enabled
    uint8_t servo_preview = true;

	switch (setup_position){
        case 0:
            setup_case("1.valid_sample_timeout", &focused, &up_pressed, &down_pressed, &config->valid_sample_timeout);
            break;

        case 1:
            setup_case("2.positions_a_-2", &focused, &up_pressed, &down_pressed, &config->positions_a[0]);
            if (focused && servo_preview) set_speed(config->positions_a[0], config->positions_b[0]);
            break;

        case 2:
            setup_case("3.positions_b_-2", &focused, &up_pressed, &down_pressed, &config->positions_b[0]);
            if (focused && servo_preview)  set_speed(config->positions_a[0], config->positions_b[0]);
            break;

        case 3:
            setup_case("4.positions_a_-1", &focused, &up_pressed, &down_pressed, &config->positions_a[1]);
            if (focused && servo_preview) set_speed(config->positions_a[1], config->positions_b[1]);
            break;

        case 4:
            setup_case("5.positions_b_-1", &focused, &up_pressed, &down_pressed, &config->positions_b[1]);
            if (focused && servo_preview) set_speed(config->positions_a[1], config->positions_b[1]);
            break;

        case 5:
            setup_case("6.positions_a_0", &focused, &up_pressed, &down_pressed, &config->positions_a[2]);
            if (focused && servo_preview) set_speed(config->positions_a[2], config->positions_b[2]);
            break;

        case 6:
            setup_case("7.positions_b_0", &focused, &up_pressed, &down_pressed, &config->positions_b[2]);
            if (focused && servo_preview) set_speed(config->positions_a[2], config->positions_b[2]);
            break;

        case 7:
            setup_case("8.positions_a_1", &focused, &up_pressed, &down_pressed, &config->positions_a[3]);
            if (focused && servo_preview) set_speed(config->positions_a[3], config->positions_b[3]);
            break;

        case 8:
            setup_case("9.positions_b_1", &focused, &up_pressed, &down_pressed, &config->positions_b[3]);
            if (focused && servo_preview) set_speed(config->positions_a[3], config->positions_b[3]);
            break;

        case 9:
            setup_case("10.positions_a_2", &focused, &up_pressed, &down_pressed, &config->positions_a[4]);
            if (focused && servo_preview) set_speed(config->positions_a[4], config->positions_b[4]);
            break;

        case 10:
            setup_case("11.positions_b_2", &focused, &up_pressed, &down_pressed, &config->positions_b[4]);
            if (focused && servo_preview) set_speed(config->positions_a[4], config->positions_b[4]);
            break;

        case 11:
            setup_case("12.release_position_a", &focused, &up_pressed, &down_pressed, &config->release_position_a);
            if (focused && servo_preview) set_speed(config->release_position_a, config->release_position_b);
            break;

        case 12:
            setup_case("13.release_position_b", &focused, &up_pressed, &down_pressed, &config->release_position_b);
            if (focused && servo_preview) set_speed(config->release_position_a, config->release_position_b);
            break;

		case 13:
			lcd_set_cursor(0, 0);
			lcd_print("14.step_up");
			lcd_set_cursor(1, 0);
			lcd_print_float(config->step_up);
			if (focused){
				if (up_pressed){
					config->step_up += 0.001f;
				}
				if (down_pressed){
					config->step_up -= 0.001f;
				}
			}
			break;

		case 14:
			lcd_set_cursor(0, 0);
			lcd_print("15.step_down");
			lcd_set_cursor(1, 0);
			lcd_print_float(config->step_down);
			if (focused){
				if (up_pressed){
					config->step_down += 0.001f;
				}
				if (down_pressed){
					config->step_down -= 0.001f;
				}
			}
			break;

		case 15:
			lcd_set_cursor(0, 0);
			lcd_print("16.Exit");
			if (focused){
				*mode = MODE_RUNNING;
				focused = 0;
				setup_position = 0;
				lcd_clear();
				return;
			}
			break;
		default:
			break;
	}

	if ((up_pressed || down_pressed) && focused){
		_delay_ms(200);
	}
}

#endif /* CONFIG_SCREEN_H_ */