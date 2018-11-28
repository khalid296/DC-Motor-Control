/************************************************
 *	 File Name: mini_project2.c					*
 * Description: 								*
 *  Created on: Sep 26, 2018					*
 *      Author: Khalid Tarek					*
 ************************************************/

#include"adc.h"
#include"lcd.h"
#include "micro_config.h"
#include<stdio.h>

void PWM_Timer0_Init(uint8 set_duty_cycle){

	TCNT0 = 0; // Timer initial value

	OCR0  = set_duty_cycle; //compare value

	DDRB  = DDRB | (1<<PB3); //set OC0 as output pin --> pin where the PWM signal is generated from MC

	/* Configure timer control register
	 * 1. Fast PWM mode FOC0=0
	 * 2. Fast PWM Mode WGM01=1 & WGM00=1
	 * 3. Clear OC0 when match occurs (non inverted mode) COM00=0 & COM01=1
	 * 4. clock = F_CPU/8 CS00=0 CS01=1 CS02=0
	 */
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<CS01);
}

/* External INT1 Interrupt Service Routine */
ISR(INT2_vect)
{
	PORTD ^= (1<<PD6);
	PORTD ^= (1<<PD7); //toggle Motor state
}

/* External INT0 enable and configuration function */
void INT2_Init(void)
{
	SREG  &= ~(1<<7);                   // Disable interrupts by clearing I-bit
	DDRB  &= (~(1<<PB2));               // Configure INT0/PD2 as input pin
	GICR  |= (1<<INT2);                 // Enable external interrupt pin INT1
	MCUCSR |= (1<<ISC2);   // Trigger INT1 with the raising edge
	SREG  |= (1<<7);                    // Enable interrupts by setting I-bit
}

void main(){

	uint16 potValue=0;
	uint8 duty=0, dutyPercent=0;

	LCD_init();
	ADC_init();
	INT2_Init();
	LCD_displayString("Duty = ");
	LCD_displayStringRowColumn(0,11,"%");
	PWM_Timer0_Init(0);
	//CLEAR_BIT(DDRA,PA0);
	DDRD |= (1<<PD6) | (1<<PD7);


	PORTD &= (~(1<<PD6));
	PORTD |= (1<<PD7);

	while(1)
	{
		potValue = ADC_readChannel(0); /* read channel zero where the potentiometer is connect */
		dutyPercent = potValue*0.098;
		if(dutyPercent<100){
			LCD_displayStringRowColumn(0,9," ");	
		}
		LCD_goToRowColumn(0,7);
		LCD_intgerToString(dutyPercent);
		duty = dutyPercent*2.55;
		PWM_Timer0_Init(duty);
	}
}
