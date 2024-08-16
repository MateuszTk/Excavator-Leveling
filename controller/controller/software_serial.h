#ifndef SOFTWARESERIAL_H_
#define SOFTWARESERIAL_H_

#include <avr/io.h>
#include <avr/interrupt.h>

#define USE_TIMER2
#ifdef USE_TIMER2
#define TIMER_FREQ 2000000
#else
#define TIMER_FREQ 16000000
#endif

#define BAUD_RATE 9600
#define BIT_DURATION (TIMER_FREQ / BAUD_RATE)

#define RX_PIN PIND2

volatile uint16_t rxBuffer;
volatile uint8_t rxAvailable = 0;

#ifndef USE_TIMER2
// Initialize the software serial
void software_serial_init() {
	// input, pullup
	DDRD &= ~(1 << RX_PIN);
	//PORTD |= (1 << RX_PIN);

	// Configure Timer1
	TCCR1A = 0;
	// CTC mode, no prescaler
	TCCR1B = (1 << WGM12) | (1 << CS10);
	// Compare match value
	OCR1A = BIT_DURATION - 1; 

	// Enable Timer1 compare match A interrupt
	TIMSK1 |= (1 << OCIE1A);
	
	// Enable external interrupt INT0 for RX pin
	cli();
	// Falling edge
	EICRA |= (1 << ISC01); 
	EIMSK |= (1 << INT0);
	sei();
}

// RX pin change
ISR(INT0_vect) {
	// Disable INT0 interrupt
	EIMSK &= ~(1 << INT0);

	// Start Timer1 with phase offset
	TCNT1 = BIT_DURATION / 2;
	// Clear any pending compare match
	TIFR1 |= (1 << OCF1A);
	// Enable Timer1 compare match interrupt
	TIMSK1 |= (1 << OCIE1A);
}

// Timer1 compare
ISR(TIMER1_COMPA_vect) {
	static uint16_t rxByte = 0;
	static uint8_t rxBit = 0;

	if ((PIND & (1 << RX_PIN)) && rxBit > 0) {
		rxByte |= (1 << rxBit - 1);
	}

	rxBit++;
	if (rxBit >= 9) {
		rxBuffer = rxByte;
		rxAvailable = 1;
		rxByte = 0;
		rxBit = 0;

		// Stop Timer1
		TIMSK1 &= ~(1 << OCIE1A);

		// Re-enable INT0 interrupt
		EIFR |= (1 << INTF0);
		EIMSK |= (1 << INT0);
	}
}
#else

void software_serial_init() {
	// input, pullup
	DDRD &= ~(1 << RX_PIN);
	//PORTD |= (1 << RX_PIN);

	// Configure Timer2
	TCCR2A = (1 << WGM21); // CTC mode
	TCCR2B = (1 << CS21); // Prescaler 8
	OCR2A = BIT_DURATION; // Compare match value

	// Enable Timer2 compare match A interrupt
	TIMSK2 |= (1 << OCIE2A);

	// Enable external interrupt INT0 for RX pin
	cli();
	// Falling edge
	EICRA |= (1 << ISC01); 
	EIMSK |= (1 << INT0);
	sei();
}

ISR(TIMER2_COMPA_vect) {
	static uint16_t rxByte = 0;
	static uint8_t rxBit = 0;

	if ((PIND & (1 << RX_PIN)) && rxBit > 0) {
		rxByte |= (1 << rxBit - 1);
	}

	rxBit++;
	if (rxBit >= 9) {
		rxBuffer = rxByte;
		rxAvailable = 1;
		rxByte = 0;
		rxBit = 0;

		// Stop Timer2
		TIMSK2 &= ~(1 << OCIE2A);

		// Re-enable INT0 interrupt
		EIFR |= (1 << INTF0);
		EIMSK |= (1 << INT0);
	}
}

ISR(INT0_vect) {
	// Disable INT0 interrupt
	EIMSK &= ~(1 << INT0);

	// Start Timer2 with phase offset
	TCNT2 = BIT_DURATION / 2;
	// Clear any pending compare match
	TIFR2 |= (1 << OCF2A);
	// Enable Timer2 compare match interrupt
	TIMSK2 |= (1 << OCIE2A);
}

#endif

uint16_t software_serial_read() {
	while (!rxAvailable);

	rxAvailable = 0;
	return rxBuffer;
}

uint8_t software_serial_available() {
	return rxAvailable;
}

#endif /* SOFTWARESERIAL_H_ */