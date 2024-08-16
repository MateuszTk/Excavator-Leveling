#ifndef CONFIG_SERIAL_H_
#define CONFIG_SERIAL_H_

#include "config.h"
#include "USART.h"

void process_serial(uint8_t data, config_t* config){
	// config
	if (data == 'c'){
		USART_print("1 - Print config\n");
		//USART_print("2 - Edit config\n");
		USART_print("3 - Save config\n");
		USART_print("Enter option:\n");

		int option = USART_read_int();
		switch (option){
			case 1:
				config_print(config);
				break;
			/*case 2:
			{
				USART_print("Enter position:\n");
				int position = USART_read_int();
				switch (position){
					case 1:
						USART_print("Enter valid_sample_timeout:\n");
						config->valid_sample_timeout = USART_read_int();
						break;
					case 2:
						USART_print("Enter speeds:\n");
						for (int i = 0; i < 5; i++){
							config->speeds[i] = USART_read_int();
						}
						break;
					case 3:
						USART_print("Enter uncertain_speed_mult:\n");
						config->uncertain_speed_mult = USART_read_int() / 1000.0f;
						break;
					case 4:
						USART_print("Enter max_uncertain_steps:\n");
						config->max_uncertain_steps = USART_read_int();
						break;
					default:
						USART_print("Invalid position\n");
						break;
				}
				config_print(config);
				break;
			}*/
			case 3:
				config_save(config);
				USART_print("Config saved\n");
				break;
			default:
				USART_print("Invalid option\n");
				break;
		}
	}
}

#endif /* CONFIG_SERIAL_H_ */