#ifndef USART_H_
#define USART_H_

void USART_init(unsigned int ubrr){
	// Set baud rate
	UBRR0H = (uint8_t)(ubrr>>8);
	UBRR0L = (uint8_t)ubrr;
	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8data, 1stop bit
	UCSR0C = (3<<UCSZ00); //(1<<USBS0)|//2stop
}

void USART_transmit(uint8_t data){
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1<<UDRE0))) ;
	// Put data into buffer, sends the data
	UDR0 = data;
}

void USART_print(const char* str){
	int i = 0;
	while (str[i] != 0){
		USART_transmit(str[i++]);
	}
}

void USART_print_int(int num, int base){
	char buff[16];
	itoa(num, buff, base);
	USART_print(buff);
}

void USART_print_float(float num){
	char buff[16];
	itoa((int)num, buff, 10);
	USART_print(buff);
	USART_print(".");
	itoa((int)(num * 1000.0f), buff, 10);
	USART_print(buff);
}

uint8_t USART_read(){
	// Wait for data to be received
	while (!(UCSR0A & (1<<RXC0))) ;
	// Get and return received data from buffer
	return UDR0;
}

uint8_t USART_avaliable(){
	return (UCSR0A & (1<<RXC0));
}

void USART_flush(){
	uint8_t dummy;
	while (UCSR0A & (1<<RXC0)) dummy = UDR0;
}

int USART_read_int(){
	char buff[16];
	int i = 0;
	while (1){
		buff[i] = USART_read();
		if (buff[0] == ' ' || buff[0] == '\n' || buff[0] == '\r'){
			i = 0;
			continue;
		}
		if (buff[i] == '\n' || buff[i] == '\r' || buff[i] == ' ' || i == 15){
			buff[i] = 0;
			break;
		}

		i++;
	}
	return atoi(buff);
}

#endif /* USART_H_ */
