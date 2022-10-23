#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif


#define D4 eS_PORTC4
#define D5 eS_PORTC5
#define D6 eS_PORTC6
#define D7 eS_PORTC7
#define RS eS_PORTB0
#define EN eS_PORTB2

#include <avr/io.h>
#include <util/delay.h>
#include<avr/interrupt.h>
#include "lcd.h"


#include <string.h>
#include <stdlib.h>

void init_adc(void);
unsigned int read_adc(unsigned char channel);
int Weight(void);

volatile unsigned char ms=0,mini=0,sec=0,hour=0,Val_ms=0;
char buffer[10]; 
int pecentage;

float Voltage;
float x = 107;          // when empty cylinder 
float y = 151 ;        // when full cylinter 
int z =0;             // precentage of the weight   
float ad_val=0;

int main(void){ 
    
   DDRD |=(1<<4);       // DDRD 4th bit set as output
   DDRD &= ~(1<<5);     // DDRD 5th bit set as input

   sei();
   
   TCCR0 = 1<<WGM01 |1<<CS01 |1<<CS00  ;         //pre scler set 64
   TIMSK = 1<<OCIE0;                             // every 1ms cll the interupt 
   OCR0= 124;
   TCNT0=0;
   
   GICR |= 1<<INT0;                 // button of buzzer off set as external inturrupt 
   MCUCR |= 1<<ISC01 | 1<<ISC00;    // rising edge

   init_adc();
   pecentage = Weight();
  
   while(1){
                
	
		if ((PIND & (1<<5)) == 0 ){         // gas senseor digital pin in PD5
		
			PORTD|=(1<<4);                  //turn on the buzzer
		
		}
		
		
		if ((PIND & (1<<0))){                // LCD ON/OFF buttton presss  (hold on buuton)
			DDRC = 0xFF;
			DDRB = 0xFF;
		
			Lcd4_Init();
			Lcd4_Set_Cursor(1,1);
			Lcd4_Write_String("Gas Level Is");
			    
			Lcd4_Set_Cursor(2,4);        
			itoa(pecentage, buffer, 10);         // display the gas amount as pecentage
			Lcd4_Write_String(buffer);
		
			Lcd4_Set_Cursor(2,7);
			Lcd4_Write_String("%");
   
		
		}else{
			PORTC = 0x00;	
			PORTB = 0x00;
		}
	  	  
   }
   return 0;
}

//...timer........................................................

ISR(TIMER0_COMP_vect){
	if(++Val_ms==100){
		Val_ms=0;
		if(++ms==10){
			ms=0;
			if(++sec==60){
				sec=0;
				if(++mini==30){
					mini=0;
			        pecentage = Weight();
				
				}
			}
		}
	}
}
 


// when press the button buzzer off...........................................................
ISR(INT0_vect){
	PORTD &= ~(1<<4); //buzzerr off
}  

                   

// initilize the ADC.................................................................................. 

void init_adc(void){
    DDRA &= ~(1<<2);			             /* Make ADC port pinA2 as input */
	ADMUX = 1<< REFS0;
	ADCSRA = 1<<ADEN | 1<<ADPS2 | 1<<ADPS1; /* make pre scaler in 64...*/  
}

// read value from chanel 2(PA2)................................................................

unsigned int read_adc(unsigned char channel){
	
	ADMUX = ADMUX | (channel & 0x0f);             // set input chanel to read 
	ADCSRA = ADCSRA | 1<<ADSC;                   // start convercation
	while(ADCSRA & (1<<ADSC)){                  // monitor end of convercasion intereupt
	}
	return ADC;
}


// calculat the weight.................................
int Weight(void){

	ad_val= read_adc(2);
	Voltage=((ad_val*5)/1024)*10000;
		
	z = ((Voltage- x)*100)/(y-x); // the function of calculate the weight as precentage 
	return z;
}