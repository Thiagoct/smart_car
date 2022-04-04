#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>  
#include "separador_digitos.h"
//#include "SSD1306_Settings.h" 
#include "SSD1306.h"
#include "SSD1306.c"
#include "Font5x8.h"
//#include "TWI_Settings.h"
#include "TWI.h"
#include "TWI.c"
//#include "IO_Macros.h"
#include <util/delay.h>
#include <avr/eeprom.h>

//Variáveis globais
uint8_t palheta         = 0;
uint8_t farol           = 0;
uint8_t temperatura_ant   = 0;
uint8_t temperatura_atual;
uint8_t motor;
uint16_t bateria;
uint16_t sonar;
uint32_t t_up           = 0;
uint32_t t_delta        = 0;
uint32_t cont_pos       = 0;
uint32_t tempo_100us    = 0; //armazena a contagem de 100us
uint16_t tempo_s        ; //armazena a quantidade de segundos que passaram
uint16_t vel_med        = 0; //armazena a velocidade média
uint16_t diametro_pneu  ;//armazena o diametro do pneu
uint16_t distancia      ; //armazena a distancia percorrida
uint16_t RPS            = 0; //Rotações por segundo
uint8_t  digito_0       = 0; //variaveis para os displays de 7 segmentos
uint8_t  digito_1       = 0; 
uint8_t  digito_2       = 0;
uint8_t  aux            = 0;
uint8_t  aux_adc        = 0;

//Funções auxiliares************************************

void set_lcd(int diam, int rpm, int dist, int sonar, int bateria, int temperatura)
{

    //21 caracteres por linha
    GLCD_Clear(); //Apagar buffer
    
    GLCD_GotoXY(0, 4); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintString(" Computador de Bordo"); //Setar essa string no buffer
    GLCD_GotoXY(2, 16); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintInteger(rpm);
    GLCD_GotoXY(27, 16); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintString("rpm"); //Setar essa string no buffer
    GLCD_GotoXY(2, 27); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintString("Sonar:"); //Setar essa string no buffer
    GLCD_GotoXY(37, 27); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintInteger(sonar);
    GLCD_GotoXY(55, 27); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintString("cm"); //Setar essa string no buffer
    GLCD_GotoXY(2, 40); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintString("D. Pneu:"); //Setar essa string no buffer
    GLCD_GotoXY(55, 40); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintInteger(diam);
    GLCD_GotoXY(70, 40); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintString("cm"); //Setar essa string no buffer
    GLCD_GotoXY(45, 53); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintInteger(dist);
    GLCD_GotoXY(65, 53); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintString("km"); //Setar essa string no buffer
    GLCD_DrawRectangle(30,48, 90, 63, GLCD_Black);
    GLCD_GotoXY(117, 53); //Coloca o cursor na posição x e y, referencial superior esquerdo
    //Adicionar if para D, R, P
    if((PIND & 0b10100000) == 0b10100000)
        GLCD_PrintString("R"); //Setar essa string no buffer
    else if((PIND & 0b10100000) == 0b10000000)
        GLCD_PrintString("D"); //Setar essa string no buffer
    else
        GLCD_PrintString("P"); //Setar essa string no buffer
    GLCD_DrawRectangle(112,48, 126, 63, GLCD_Black);
    GLCD_GotoXY(102, 20); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintInteger(bateria);
    GLCD_GotoXY(120, 20); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintString("%"); //Setar essa string no buffer
    GLCD_GotoXY(102, 33); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintInteger(temperatura);
    GLCD_GotoXY(120, 33); //Coloca o cursor na posição x e y, referencial superior esquerdo
    GLCD_PrintString("C"); //Setar essa string no buffer
    GLCD_DrawRectangle(100,16, 126, 45, GLCD_Black);
    GLCD_Render(); //Passa os dados do buffer para a tela
}
void canal_adc(uint8_t a)
{
    if(a==0)
    {
        ADMUX  = 0b01100000;//Referência VCC, ajuste a esquerda ,sinal vindo do ADC0  
        ADCSRA = 0b11100111;
        ADCSRB = 0b00000000;
        DIDR0  = 0b00111110; //habilita pino ADC0 
    }
    else if(a==1)
    {
        ADMUX  = 0b01100001;//Referência VCC, ajuste a esquerda ,sinal vindo do ADC1
        ADCSRA = 0b11100111;
        ADCSRB = 0b00000000; 
        DIDR0  = 0b00111101; //habilita pino ADC1
    }
    else
    {
        //Configuração do ADC
        ADMUX  = 0b01100010;//Referência VCC, ajuste a esquerda ,sinal vindo do ADC2
        ADCSRA = 0b11100111;
        ADCSRB = 0b00000000;
        DIDR0  = 0b00111011; //habilita pino ADC2
    }
}
void executa_a_cada_1_segundo(uint32_t TEMPO_100us)
{
    static uint32_t TEMPO_100us_anterior = 0;
    static uint16_t r_ant = 0;
    if((TEMPO_100us - TEMPO_100us_anterior) >= 10000)
    {
        tempo_s++;
        TEMPO_100us_anterior = TEMPO_100us;

        RPS = cont_pos - r_ant;
        r_ant = cont_pos;

        distancia = (int)vel_med*tempo_s*0.000277778;
        sonar = t_delta/58;
        set_lcd(diametro_pneu, RPS*60, distancia,sonar, bateria, temperatura_atual);
        eeprom_write_word((uint16_t *)2, tempo_s);
        if(temperatura_ant<temperatura_atual)
        {
            temperatura_ant = temperatura_atual;
            eeprom_write_byte((uint8_t *)8, temperatura_atual);
        }
        if(palheta==1)
            PORTD = PORTD ^ 0b00100000;
        if(farol == 1)
            PORTC = PORTC | 0b00001000;
        else 
            PORTC = PORTC & 0b11110111;    


    }
    else if((TEMPO_100us - TEMPO_100us_anterior) >= 5000)
    {
        if(aux_adc==0)
        {
            motor = ADCH;
            aux_adc = 1;
        }
        else if(aux_adc==1)   
        {
            bateria = ADCH*0.39;
            aux_adc = 2;
        }
        else    
        {
            temperatura_atual = 247 - ADCH ;
            aux_adc = 0;
        }
        canal_adc(aux_adc);
    }
}
void USART_Transmit(unsigned char data)
{
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}
//******************************************************
//Tratamento de interrupções****************************
ISR(INT0_vect)
{
    if(diametro_pneu<300)
    {
        diametro_pneu = diametro_pneu + 1;
        eeprom_write_word((uint16_t *)0, diametro_pneu);
    }
}

ISR(INT1_vect)
{
    if(diametro_pneu>1)
    {
        diametro_pneu = diametro_pneu - 1;
        eeprom_write_word((uint16_t *)0, diametro_pneu);
    }
    
}
ISR(PCINT2_vect)
{
    if((PIND & 0b00010000) == 0b00010000)
        cont_pos += 1;
}
ISR(TIMER2_COMPA_vect)
{
    tempo_100us++;

    if(aux == 0)
    {
        PORTB = 0b11000000 | (digito_0<<1);
        aux = 1;
    }   
    else if(aux == 1)  
    {
        PORTB = 0b10100000 | (digito_1<<1); 
        aux = 2;
    }
    else
    {
        PORTB = 0b01100000 | (digito_2<<1);
        aux = 0;
    }
}
ISR(TIMER1_CAPT_vect)
{
    if(TCCR1B & (1<<ICES1))
        t_up = ICR1;
    else 
        t_delta = (ICR1 - t_up)*16; 
    TCCR1B ^=(1<<ICES1);       
} 
ISR(USART_RX_vect)
{
    char recebido;
    recebido = UDR0;
    if(recebido == 'l')
    {
        eeprom_write_byte((uint8_t *)8, 0);
        temperatura_ant = 0;
    }
    if(recebido == 'd')
    {
        recebido = eeprom_read_byte((const uint8_t *)8);
        USART_Transmit(recebido);
    }
    if(recebido == 'p')
    {
        palheta = 1;
    }
    if(recebido == 'o')
    {
        palheta = 0;
    }
    if(recebido == 'f')
    {
        farol = 1;
    }
    if(recebido == 'e')
    {
        farol = 0;
    }
}  

//******************************************************

int main(void)
{

    GLCD_Setup(); //Inicia a biblioteca
    GLCD_SetFont(Font5x8, 5, 8, GLCD_Overwrite); //Seta a fonte utilizada
    GLCD_InvertScreen(); //Inverte o fundo para deixar branco
    DDRC  &= 0b11110110; 
    //habilitar pinos B[7:1] como saida
    DDRB   = 0b11111110; 
    //habilitar pinos D como entrada
    DDRD   = 0b00000000;
    //habilita resitores de pull-up
    PORTD  = 0b00001100;
    //Configuração TC0 - Timer 0
    TCCR0A = 0b10000011;//TC0 em operação CTC
    TCCR0B = 0b00000011;//TC0 prescaler=8. 
    //Configuração TC2 Timer 2
    TCCR2A = 0b00000010;//TC0 em operação CTC
    TCCR2B = 0b00000010;//TC0 prescaler=8. 
    OCR2A  = 199;
    TIMSK2 = 0b00000010;//Habilita a interrupção na igualdade com OCR0A -> 100us = (8*(199+1))/16Mhz
    //Configuração TC1 Timer 1
    TCCR1A = 0b00000000;//TC1 em operação normal
    TCCR1B = 0b01000100;//Captura na borda de subida, presclaer = 256
    TIMSK1 = 0b00100000;//Ativar a interrupção para captura de entrada
    //COnfiguração de interrupção para uso do tacômetro
    PCICR  = 0b00000100;
    PCMSK2 = 0b00010000;//Habilita a PCINT20
    //Configuração das interrupções
    EIMSK  = 0b00000011;
    EICRA  = 0b00001010;//sinal vindo do ADC0  
    ADCSRA = 0b11110111;
    ADCSRB = 0b00000000;
    DIDR0  = 0b00111110; //habilita pino ADC0 
    //UART
    UBRR0H = (unsigned char)(MYUBRR>>8);
    UBRR0L = (unsigned char)MYUBRR;
    UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
    UCSR0C = (1<<USBS0)|(3<<UCSZ00);
    //habilita as interrupções
    sei();
    diametro_pneu = eeprom_read_word((const uint16_t *)0);
    tempo_s       = eeprom_read_word((const uint16_t *)2);
    //Loop infinito
    while(1)
    {    
        executa_a_cada_1_segundo(tempo_100us);
        vel_med = (diametro_pneu*3.6*3.14*(RPS*60))/6000;
        digito_0 = separador_primeiro(vel_med);
        digito_1 = separador_segundo(vel_med);
        digito_2 = separador_terceiro(vel_med);
        if(sonar<300 && vel_med>20)
            OCR0A  = 0b00011010;
        else    
            OCR0A  = motor;
        
    }
}