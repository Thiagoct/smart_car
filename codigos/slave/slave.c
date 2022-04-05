#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>  



ISR(USART_RX_vect)
{
    char recebido;
    recebido = UDR0;
    if(recebido == 'f')
    {
        PORTC = 0b00110000;
    }
    if(recebido == 'e')
    {
        PORTC = 0b00000000;
    }
    if(recebido == 'p')
        PORTB |= 0b00100000;
    if(recebido == 'o')
        PORTB &= 0b11011111;    

}  

//******************************************************

int main(void)
{
    DDRC = 0b00110000;
    DDRB = 0b00100110;
    ICR1 = 39999;
    TCCR1A = 0b10100010;
    TCCR1B = 0b00011010;
    //UART
    UBRR0H = (unsigned char)(MYUBRR>>8);
    UBRR0L = (unsigned char)MYUBRR;
    UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
    UCSR0C = (3<<UCSZ00);
    //habilita as interrupções
    sei();
    //Loop infinito
    while(1)
    {    
        for(uint16_t i=2000;i<4000;i+=100)
        {
            OCR1A = i;
            _delay_ms(100);
        }
        for(uint16_t i=4000;i>2000;i-=100)
        {
            OCR1B = i;
            _delay_ms(100);
        }
        
    }
}