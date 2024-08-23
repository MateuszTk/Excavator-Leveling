#ifndef CONFIG_SCREEN_H_
#define CONFIG_SCREEN_H_

#include "lcd.h"
#include "buttons.h"
#include "config.h"
#include <util/delay.h>
#include "control.h"


#define MODE_RUNNING 0
#define MODE_SETUP 1

#define clamp(x, min, max) ((x < min) ? min : (x > max) ? max : x)

void setup_exit(uint8_t* mode, config_t* config, uint8_t* focused){
    *mode = MODE_RUNNING;
    *focused = 0;
    lcd_clear();
    lcd_set_cursor(0, 0);
    lcd_print("Save config?");
	lcd_set_cursor(1, 1);
	lcd_print("Yes");
	lcd_set_cursor(1, 6);
	lcd_print("No");
	uint8_t save = false;
    while (!button_ok()){
		if (button_up()){
			save = false;
			lcd_set_cursor(1, 0);
			lcd_print(" ");
			lcd_set_cursor(1, 5);
			lcd_print(">");
		}	
		if (button_down()){
			save = true;
			lcd_set_cursor(1, 0);
			lcd_print(">");
			lcd_set_cursor(1, 5);
			lcd_print(" ");
		}	
	}
	if (save){
		config_save(config);
	}
    lcd_clear();
}

void setup_case(const char* text, uint8_t* focused, uint8_t* up_pressed, uint8_t* down_pressed, int16_t* config_value, int16_t min, int16_t max, int8_t hold){
    lcd_set_cursor(0, 0);
    lcd_print(text);
    lcd_set_cursor(1, 0);
    lcd_print_int(*config_value, 10);
	lcd_print(" ");
    if (*focused){
        if (*up_pressed){
            *config_value += (hold == 1) ? 2 : 1;
        }
        if (*down_pressed){
            *config_value -= (hold == -1) ? 2 : 1;
        }
		*config_value = clamp(*config_value, min, max);
    }
}

void setup_mode(config_t* config, uint8_t* mode, const config_t* default_config){
	static uint8_t setup_position = 0;
	static uint8_t focused = 0;

	uint8_t up_pressed = button_up();
	uint8_t down_pressed = button_down();
	uint8_t servo_preview = switch_armed();

	lcd_set_cursor(1, 15);
	lcd_print((servo_preview) ? "A" : "D");
	if (!servo_preview) {
		set_speed(config->release_position_a, config->release_position_b);
	}

	if (button_ok()){
		focused = !focused;
		if (focused){
			lcd_set_cursor(1, 14);
			lcd_print("<");
		}
		else{
			lcd_set_cursor(1, 14);
			lcd_print(" ");
		}
		_delay_ms(200);
	}
	if (!focused){
		if (up_pressed && down_pressed && !button_ok()){
			setup_exit(mode, config, &focused);
			return;
		}
		if (up_pressed){
			setup_position = (setup_position + 1) % 17;
			_delay_ms(200);
			lcd_clear();
		}
		if (down_pressed){
			setup_position = (setup_position + 16) % 17;
			_delay_ms(200);
			lcd_clear();
		}
	}

	const hold_delay = 10;
	static int8_t hold_counter = 0;
	int8_t hold = 0;
	if (focused && (up_pressed || down_pressed)){
		lcd_set_cursor(1, 0);
		lcd_print("              ");
		if (up_pressed){
			hold_counter = clamp(hold_counter + 1, 0, hold_delay);
		}
		if (down_pressed){
			hold_counter = clamp(hold_counter - 1, -hold_delay, 0);
		}
	}
	else{
		hold_counter = 0;
	}
	hold = (hold_counter == hold_delay) ? 1 : ((hold_counter == -hold_delay) ? -1 : 0);

	switch (setup_position){
        case 0:
            setup_case("1.valid_sample_timeout", &focused, &up_pressed, &down_pressed, &config->valid_sample_timeout, 1, 1000, hold);
            break;

        case 1:
            setup_case("2.positions_a_-2", &focused, &up_pressed, &down_pressed, &config->positions_a[0], -90, 90, hold);
            if (focused && servo_preview) set_speed(config->positions_a[0], config->positions_b[0]);
            break;

        case 2:
            setup_case("3.positions_b_-2", &focused, &up_pressed, &down_pressed, &config->positions_b[0], -90, 90, hold);
            if (focused && servo_preview)  set_speed(config->positions_a[0], config->positions_b[0]);
            break;

        case 3:
            setup_case("4.positions_a_-1", &focused, &up_pressed, &down_pressed, &config->positions_a[1], -90, 90, hold);
            if (focused && servo_preview) set_speed(config->positions_a[1], config->positions_b[1]);
            break;

        case 4:
            setup_case("5.positions_b_-1", &focused, &up_pressed, &down_pressed, &config->positions_b[1], -90, 90, hold);
            if (focused && servo_preview) set_speed(config->positions_a[1], config->positions_b[1]);
            break;

        case 5:
            setup_case("6.positions_a_0", &focused, &up_pressed, &down_pressed, &config->positions_a[2], -90, 90, hold);
            if (focused && servo_preview) set_speed(config->positions_a[2], config->positions_b[2]);
            break;

        case 6:
            setup_case("7.positions_b_0", &focused, &up_pressed, &down_pressed, &config->positions_b[2], -90, 90, hold);
            if (focused && servo_preview) set_speed(config->positions_a[2], config->positions_b[2]);
            break;

        case 7:
            setup_case("8.positions_a_1", &focused, &up_pressed, &down_pressed, &config->positions_a[3], -90, 90, hold);
            if (focused && servo_preview) set_speed(config->positions_a[3], config->positions_b[3]);
            break;

        case 8:
            setup_case("9.positions_b_1", &focused, &up_pressed, &down_pressed, &config->positions_b[3], -90, 90, hold);
            if (focused && servo_preview) set_speed(config->positions_a[3], config->positions_b[3]);
            break;

        case 9:
            setup_case("10.positions_a_2", &focused, &up_pressed, &down_pressed, &config->positions_a[4], -90, 90, hold);
            if (focused && servo_preview) set_speed(config->positions_a[4], config->positions_b[4]);
            break;

        case 10:
            setup_case("11.positions_b_2", &focused, &up_pressed, &down_pressed, &config->positions_b[4], -90, 90, hold);
            if (focused && servo_preview) set_speed(config->positions_a[4], config->positions_b[4]);
            break;

        case 11:
            setup_case("12.release_pos_a", &focused, &up_pressed, &down_pressed, &config->release_position_a, -90, 90, hold);
            if (focused && servo_preview) set_speed(config->release_position_a, config->release_position_b);
            break;

        case 12:
            setup_case("13.release_pos_b", &focused, &up_pressed, &down_pressed, &config->release_position_b, -90, 90, hold);
            if (focused && servo_preview) set_speed(config->release_position_a, config->release_position_b);
            break;

		case 13:
			lcd_set_cursor(0, 0);
			lcd_print("14.step_up");	
			if (focused){
				if (up_pressed){
					config->step_up += (hold == 1) ? 0.005f : 0.001f;
				}
				if (down_pressed){
					config->step_up -= (hold == -1) ? 0.005f : 0.001f;
				}
				config->step_up = clamp(config->step_up, 0.001f, 0.500f);
			}
			lcd_set_cursor(1, 0);
			lcd_print_float(config->step_up);
			break;

		case 14:
			lcd_set_cursor(0, 0);
			lcd_print("15.step_down");
			if (focused){
				if (up_pressed){
					config->step_down += (hold == 1) ? 0.005f : 0.001f;
				}
				if (down_pressed){
					config->step_down -= (hold == -1) ? 0.005f : 0.001f;
				}
				config->step_down = clamp(config->step_down, 0.001f, 0.500f);
			}
			lcd_set_cursor(1, 0);
			lcd_print_float(config->step_down);
			break;

		case 15:
			lcd_set_cursor(0, 0);
			lcd_print("16.Reset");
			if (focused){
				focused = 0;
				config_clear();
				*config = *default_config;
				lcd_set_cursor(1, 0);
				lcd_print("Done!");
				_delay_ms(1000);
				lcd_set_cursor(1, 0);
				lcd_print("               ");
			}
			break;

		case 16:
			lcd_set_cursor(0, 0);
			lcd_print("17.Exit");
			if (focused){
				setup_exit(mode, config, &focused);
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