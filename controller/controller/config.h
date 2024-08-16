#ifndef CONFIG_H_
#define CONFIG_H_

#include "eeprom.h"
#include "usart.h"

#define CONFGIG_ADDR 0x0
#define MAGIC_NUMBER (sizeof(config_t))

typedef struct {
	uint16_t valid_sample_timeout;
	int16_t positions_a[5];
	int16_t positions_b[5];
	int16_t release_position_a;
	int16_t release_position_b;
	float step_up;
	float step_down;
} config_t;

void config_print(config_t* config){
	USART_print("1. valid_sample_timeout: ");
	USART_print_int(config->valid_sample_timeout, 10);
	
	USART_print("\n2. positions_a: ");
	for (int i = 0; i < 5; i++){
		USART_print_int(config->positions_a[i], 10);
		USART_print(" ");
	}

	USART_print("\n3. positions_b: ");
	for (int i = 0; i < 5; i++){
		USART_print_int(config->positions_b[i], 10);
		USART_print(" ");
	}

	USART_print("\n4. release_position_a: ");
	USART_print_int(config->release_position_a, 10);

	USART_print("\n5. release_position_b: ");
	USART_print_int(config->release_position_b, 10);

	USART_print("\n6. step_up: ");
	USART_print_float(config->step_up);

	USART_print("\n7. step_down: ");
	USART_print_float(config->step_down);
	
	USART_print("\n");
}

void config_save(const config_t* config){
	cli();
	uint8_t* config_data = (uint8_t*)config;
	
	for (int i = 0; i < sizeof(config_t) + 1; i++){
		if (i == 0){
			EEPROM_write(CONFGIG_ADDR, MAGIC_NUMBER);
		}
		else{
			EEPROM_write(CONFGIG_ADDR + i, config_data[i - 1]);
		}
	}
	sei();
}

uint8_t config_load(config_t* config){
	cli();
	uint8_t success = EEPROM_read(CONFGIG_ADDR) == MAGIC_NUMBER;
	if (success){
		uint8_t* config_data = (uint8_t*)config;
		
		for (int i = 0; i < sizeof(config_t); i++){
			config_data[i] = EEPROM_read(CONFGIG_ADDR + 1 + i);
		}
	}
	sei();
	
	return success;
}

#endif /* CONFIG_H_ */
