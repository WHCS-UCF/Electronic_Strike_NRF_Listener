/*
 * Electronic_Strike_NRF_Listener.cpp
 *
 * Created: 6/23/2015 1:23:35 PM
 *  Author: Jimmy
 */ 

#define F_CPU 16000000L
#include "RF24.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "MEGA88A_UART_LIBRARY.h"
#include "MEGA88_SPI_LIBRARY.h"

volatile uint8_t receiveBuffer[32];
//ce, cs
RF24 radio(1,2);
// Single radio pipe address for the node to listen on.
const uint64_t pipe = 0xE8E8F0F0E5LL;

//Activated on falling edge.
void enableInt1( void ){
	//Set Int1 to activate on falling edge.
	EICRA |= (1<<ISC11);
	EICRA &= (~(1<<ISC10));
	
	DDRD &= (~(1<<DDD3)); // Set PD3(Int1) pin as input.
	
	PORTD |= (1<<DDD3); //Enable the pull-up resistor on PD3(Int1) pin.
	
	EIMSK |= (1<<INT1);
}

void LED_on() {
	PORTB |= (1<<DDB0);
}

void LED_off() {
	PORTB &= ~(1<<DDB0);
}

void initLED() {
	DDRB |= (1<<DDB0);
	LED_on();
}

void blinkLED() {
	LED_off();
	_delay_ms(1000);
	LED_on();
	_delay_ms(1000);
}

void initStrikePin() {
	DDRC |= (1<<DDC1);
}

void strikeOn() {
	PORTC |= (1<<DDC1);
}

void strikeOff() {
	PORTC &= ~(1<<DDC1);
}

ISR(INT1_vect){
	if(radio.available()){
		radio.read((uint8_t*)receiveBuffer, 1);
	}
}

int main(void)
{
	initLED();
	initUart();
	InitSPI();
	enableInt1();
	initStrikePin();
	sei();
	
	USART_sendString("HELLO");
	
	int i;
	for(i=0; i<3; i++) {
		blinkLED();
	}
	
	radio.begin();
	radio.openReadingPipe(1, pipe);
	radio.startListening();
    while(1)
    {
        if (receiveBuffer[0] == 'O') {
			LED_on();
			strikeOn();
		}
		else {
			LED_off();
			strikeOff();
		}
    }
}