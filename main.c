/*
 * temperature_i2c_sht21.c
 *
 * Created: 07-Aug-22 10:03:32 AM
 * Author : admin
 */ 

#undef	F_CPU
#define F_CPU		16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <compat/deprecated.h>
#include <stdio.h>
#include <string.h>
#include "uartnaval.h"
#include "uartnaval.c"
#include "lcdgdheerajat8.h"
#include "lcdgdheerajat8.c"

#define SHT21_W_ADD				0x80
#define SHT21_R_ADD				0x81
#define TEMP_HOLD_CMD			0xE3
#define HUMIDITY_HOLD_CMD		0xE5
#define TEMP_NO_HOLD_CMD		0xF3
#define HUMIDITY_NO_HOLD_CMD	0xF5

// below variables for task scheduling at given time

volatile int flag_100ms=0;
volatile int flag_500ms=0;

// Timer0 ISR for setting task flag

ISR(TIMER0_OVF_vect)
{
	static	int count_100ms=0;
	static	int count_500ms=0;
	
	if(count_100ms == 100)
	{
		flag_100ms = 1;
		count_100ms = 0;
	}
	
	if(count_500ms == 500)
	{
		flag_500ms = 1;
		count_500ms = 0;
	}
	
	count_500ms++;
	count_100ms++;
	TCNT0 = 240;		// COUNT FOR 1 MS
}

void timer0_init()
{
	TCCR0 = 0X05;			//	PRESCALAR AS 1024
	TCNT0 = 240;			//	COUNT FOR 1 MS
	TIMSK |= (1 << TOIE0);  //	Unmask Timer 0 overflow interrupt.
}

void i2c_init(void)
{
	TWSR=0x00;
	TWBR=0x48;
	TWCR=0x00;
}

void i2c_start(void)
{
	TWCR=(1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	while((TWCR&(1<<TWINT))==0);
}

void i2c_write(unsigned char data)
{
	TWDR=data;
	TWCR=(1<<TWINT)|(1<<TWEN);
	while((TWCR&(1<<TWINT))==0);
}

unsigned char i2c_read(unsigned char ackVal)
{
	TWCR=(1<<TWINT)|(1<<TWEN)|(ackVal<<TWEA);
	while(!(TWCR & (1<<TWINT)));
	return TWDR;
}


void i2c_stop()
{
	TWCR=(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	//for(int x=0;x<10;x++);	//wait for sometime
	_delay_ms(100);
}

void sht21_init(void)
{
	i2c_init();
}

float sht21_getTemp()
{

	unsigned char i[2];
	unsigned short a=0;  /// digit range upto 65535....
	i2c_start();
	i2c_write(SHT21_W_ADD); // address of device in write mode
	i2c_write(TEMP_NO_HOLD_CMD); // address for no hold get temperature
	_delay_ms(100);
	i2c_start();
	_delay_ms(100);
	i2c_write(SHT21_R_ADD);
	i[0]=i2c_read(1);
	i[1]=i2c_read(1);
	i[2]=i2c_read(0);
	i2c_stop();
	
	//i[1] = i[1] & ~(1 << 0);
	//i[1] = i[1] & ~(1 << 1);
	
	a=(i[0]<<8)|i[1];
	
	float temp = -46.85 + 175.72*(a/65535.0);
	
	return temp;

}

float sht21_getHum()
{

	unsigned char i[2];
	unsigned short a=0;
	i2c_start();
	i2c_write(SHT21_W_ADD); // address of device in write mode
	i2c_write(HUMIDITY_NO_HOLD_CMD); // address for no hold get humidity
	_delay_ms(100);
	i2c_start();
	_delay_ms(100);
	i2c_write(SHT21_R_ADD);
	i[0]=i2c_read(1);
	i[1]=i2c_read(1);
	i[2]=i2c_read(0);
	i2c_stop();
	
	//i[1] = i[1] & ~(1 << 0);
	//i[1] = i[1] & ~(1 << 1);
	
	a=(i[0]<<8)|(i[1]>>4);
	
	float hum = -6 + 125*(a/65535.0);
	
	return hum;

}

int main(void)
{
	/* Replace with your application code */
	
	unsigned char st1[50],st[50],st2[50];
	
	sbi(DDRB,0);
	sbi(DDRB,1);
	
	uart_init(9600);			// UART INITIALIZE AT 9600 AS DEFAULT BAUD RATE
	
	timer0_init();
	
	sht21_init();
	
	lcd_init(LCD_DISP_ON);

	sei();
	
	while(1)
	
	if(flag_100ms == 1)
	{
		sbi(PORTB,0);
		cbi(PORTB,1);
		flag_100ms = 0;
	}
	
	else if(flag_500ms == 1)
	{
		dtostrf(sht21_getTemp(),2,2,st1);
		dtostrf(sht21_getHum(),2,2,st2);
		sprintf(st,"Temp is %s",st1); // making string with value of temperature
		lcd_putsxy(0,0,st); 
		lcd_putsxy(14,0,"*C");
		uart_txstr(st);
		uart_txstr(" °C");
		uart_tx(32);
		sprintf(st,"Hum is %s",st2);  // making string with value of humidity
		lcd_putsxy(0,1,st);
		lcd_putsxy(13,1,"%RH");
		uart_txstr(st);
		uart_txstr("%RH");
		uart_tx('\r');
		
		cbi(PORTB,0);
		sbi(PORTB,1);
		flag_500ms = 0;
	}
	
}
