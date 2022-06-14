/*
 * Phototherapy.c
 *
 * Created: 10/20/2019 6:01:36 PM
 *  Author: Salih Yalçın
 */

#ifndef F_CPU
#define F_CPU 16000000UL  // 
#endif

#define FOSC 16000000
#define BAUD 115200
#define MYUBRR FOSC/16/BAUD-1

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "keypad.h"

#define ON  1
#define OFF 0

#define TYPE_MICRO   1
//#define TYPE_GRILL 2  // BURAYI BEN DEĞİŞTİRDİM

/*
#define UBRR_VAL	51
/ * UART Buffer Defines * /
#define UART_RX_BUFFER_SIZE 16    / * 2,4,8,16,32,64,128 or 256 bytes * /
#define UART_TX_BUFFER_SIZE 16
#define UART_RX_BUFFER_MASK (UART_RX_BUFFER_SIZE - 1)

#if (UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK)
#error RX buffer size is not a power of 2
#endif

#define UART_TX_BUFFER_MASK (UART_TX_BUFFER_SIZE - 1)
#if (UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK)
#error TX buffer size is not a power of 2
#endif*/

void Init();
void SetMicro(uint8_t state);

void SetTime();
void CountDown();

void SetBuzzer(uint8_t);
void Beep();


volatile long int h=0;    // BUNU BEN EKLEDİM SAAT AYARI YAPABİLMEK İÇİN!
volatile long int m=0;      // ÖNCEDEN h ve m uint8_t İDİ ŞİMDİ LONG INT YAPTIM
volatile uint8_t s=0;

volatile uint8_t timer_running=0;
volatile int DutyCycle=0;  // ÖNCEDEN SADECE VOLATILE VARDI!
volatile int DutyCycle_gecici=0;
volatile double sayac=0;

static  uint8_t buzzer[6] = {0xA5,0x5A,0x03,0x80,0x02,0x10};
static  uint8_t buzzer_300ms_clear[6] = {0xA5,0x5A,0x03,0x80,0x02,0x30};
static  uint8_t buzzer_3ms[6] = {0xA5,0x5A,0x03,0x80,0x02,0x03};	
	
static  uint8_t operasyon_son[8] = {0xA5,0x5A,0x05,0x82,0x00,0x01,0x00,0x00};
static  uint8_t operasyon_yan[8] = {0xA5,0x5A,0x05,0x82,0x00,0x01,0x00,0x01};
	
static  uint8_t pause_son[8] = {0xA5,0x5A,0x05,0x82,0x00,0x25,0x00,0x00};
static  uint8_t pause_yan[8] = {0xA5,0x5A,0x05,0x82,0x00,0x25,0x00,0x01};	
	
static  uint8_t pwm_yan0[8] = {0xA5,0x5A,0x05,0x82,0x00,0x07,0x00,0x00};	
static  uint8_t pwm_yan1[8] = {0xA5,0x5A,0x05,0x82,0x00,0x07,0x00,0x01};
static  uint8_t pwm_yan2[8] = {0xA5,0x5A,0x05,0x82,0x00,0x07,0x00,0x02};		
static  uint8_t pwm_yan3[8] = {0xA5,0x5A,0x05,0x82,0x00,0x07,0x00,0x03};
static  uint8_t pwm_yan4[8] = {0xA5,0x5A,0x05,0x82,0x00,0x07,0x00,0x04};
static  uint8_t pwm_yan5[8] = {0xA5,0x5A,0x05,0x82,0x00,0x07,0x00,0x05};	
	
static uint8_t min_son[8] = {0xA5,0x5A,0x05,0x82,0x00,0x21,0x00,0x00};
static uint8_t min_yan[8] = {0xA5,0x5A,0x05,0x82,0x00,0x21,0x00,0x01};
	
static uint8_t saat_simge_son[8] = {0xA5,0x5A,0x05,0x82,0x00,0x02,0x00,0x00};
static uint8_t saat_simge_yan[8] = {0xA5,0x5A,0x05,0x82,0x00,0x02,0x00,0x01};
	
static uint8_t saniye_goster0[8] = {0xA5,0x5A,0x05,0x82,0x00,0x08,0x00,0x00};
	
static uint8_t touch_off[6] = {0xA5,0x5A,0x03,0x80,0x0B,0x00};
static uint8_t touch_on[6] = {0xA5,0x5A,0x03,0x80,0x0B,0xFF};

	
	
/*
static  unsigned char UART_TxBuf[UART_TX_BUFFER_SIZE];
static volatile unsigned char UART_TxHead;
static volatile unsigned char UART_TxTail;*/

//void TransmitByte(unsigned char data);
void UART_putc(unsigned char data);
void UART_putsas(uint8_t s[]);
void UART_putsasbuz(uint8_t s[]);
void USART_Init(unsigned int ubrr);
void UART_putsas2(uint8_t s[]);


uint8_t type=TYPE_MICRO;

int main(void)
{

   
   DDRB|=(1<<PB3)|(1<<PB5); // BURAYI BEN EKLEDİM(BUZZER) // BUNLARDA HERHANGİ BİR SORUN ÇIKMAZSA INIT KISMINA AKTARACAĞIM.
   TCCR2A = (1<<COM2A1)|(1<<WGM21)|(1<<WGM20); // TOP OLARAK 0xFF OLARAK AYARLANDI. FAST PWM NON INVERTING MODE!
   TIMSK2=(1<<TOIE2);
   TCNT2=0;
   
   
   //ICR2=300; BUNU NASIL AYARLARIM BUL.FREKANS
   
//   OCR2A = (DutyCycle/100.0)*255.0;
   
   OCR2B = 250;  // ZAMAN AYARI İÇİN BURAYI MI DEĞİŞTİRMEK GEREKİYOR!.. DAHA ÖNCE 250 İDİ TIMER1_OVF DAHA UZUN SÜRMESİ İÇİN 400 OLABİLİR Mİ DİYE DENEME YAPMAK İSTİYORDUM!
   
   TCCR2B=(1<<CS22);  // 64 PRE-SCALER OLARAK AYARLADIK.
   
   EIMSK = (1<<INT0); // OPERASYON LAMBASI İÇİN YAZDIK BUNU
   EICRA = (1<<ISC01)|(1<<ISC11);  // OPERASYON LAMBASI BASIP BASILI TUTUNCA YANIP SÖNMESİN DİYE YAZDIK.
   
   
   Init();
   USART_Init(8);
   
   UART_putsasbuz(touch_off);   
   
   sei();  // INITIN İÇİNDEN ALDIM BUNU BURAYA KOYDUM!
   PORTB&=~(PORTB5);  // BURADA OP: LAMBASINI SIFIRLIYORUM ÇÜNKÜ KENDİLİĞİNDEN YANABİLİYOR BAZEN BAŞLANGIÇTA!!!   
 
   while(1)
   {
	   
      TIFR2&=~(1<<TOV2);   /// FLAGLER İŞE YARAMIYOR OLABİLİr
	  
	  uint8_t k=GetKeypadCmd(1);
	  
      switch(k)
      {
         case KEY_MICRO:
         {
			_delay_us(100);
			UART_putsas(saat_simge_yan);
			UART_putsas(pause_son);
            UART_putsasbuz(buzzer);
            type=TYPE_MICRO;
            SetTime();
            timer_running=1;

            if(type==TYPE_MICRO)
			{
				SetMicro(ON);
			}
               
            CountDown();

            break;
         }
         
         case KEY_START:  
         {
			 _delay_us(100);
			 
			 UART_putsas(pause_son);
			 UART_putsas(saat_simge_son);
			 _delay_us(3);
			 UART_putsasbuz(buzzer);
			 
			 DutyCycle+=20;
		  
          if(DutyCycle>100)
		  {
			  DutyCycle=20;
		  }
		  
		  
		  
		  if (DutyCycle==0)
		  {
			  
			  UART_putsas(pwm_yan0);
			  _delay_us(3);
			  
		  }
		  else if(DutyCycle==20)
		  {
			  
			  UART_putsas(pwm_yan1);
			  _delay_us(3);
			  UART_putsasbuz(buzzer_3ms);
			  _delay_us(3);
		  }
		  else if (DutyCycle==40)
		  {
			  UART_putsas(pwm_yan2);
			  _delay_us(3);
			  UART_putsasbuz(buzzer_3ms);
			  _delay_us(3);
		  }
		  else if (DutyCycle==60 )
		  {
			 
			  UART_putsas(pwm_yan3);
			  _delay_us(3);
			  UART_putsasbuz(buzzer_3ms);
		  }
		  else if (DutyCycle==80)
		  {
			  
			  UART_putsas(pwm_yan4);
			  _delay_us(3);
			  UART_putsasbuz(buzzer_3ms);
			  _delay_us(3);
			  
		  }
		  else if (DutyCycle==100)
		  {
			 
			  UART_putsas(pwm_yan5);
			  _delay_us(3);
			  UART_putsasbuz(buzzer_3ms);
			  _delay_us(3);
		  }
		  
		  
		 
		  TIFR2 = (1<<TOV2);
		  
			 
			 break;
			
	     }
		 case KEY_STOP:  // BUNU SONRADAN BEN EKLEDİM!
		 { 
		   _delay_us(100);
		   UART_putsas(pause_yan);
		   UART_putsas(pwm_yan0);
		   UART_putsasbuz(buzzer);
		   DutyCycle=0;
	
		   TIFR2 = (1<<TOV2);
		   break;
		  // PORTB=0;
		 }
      }

   }
}

void Init()
{
   


   

   //Initialize keypad lib
   KeypadInit();

   //Init TIMER1 for main timer operation
   TCCR1B=(1<<WGM12)|(1<<CS12)|(1<<CS10); //CTC Mode pre-scaler 1024
  // OCR1A=976;
   OCR1A = 1000;
   
   TIMSK1 = (1<<OCIE1A);
   
}

ISR(TIMER2_OVF_vect) //
{		
	OCR2A=(DutyCycle/100.0)*255.0;	// _delay_ms  verince ÇALIŞMIYOR.AYRICA KOD UZUN OLUNCA DA ÇALIŞMIYOR OLABİLİR!
}

ISR(INT0_vect)   // 
{
	_delay_ms(20);
	
	if(bit_is_set(PINB,5))
	{
		UART_putsas(operasyon_son);
		_delay_us(3);
		UART_putsasbuz(buzzer_3ms);
	}	
	
	if(bit_is_clear(PINB,5))
	{
		UART_putsas(operasyon_yan);
		_delay_us(3);
		UART_putsasbuz(buzzer);
	}
		
	PORTB^=(1<<PB5);

	
	
}


ISR(TIMER1_COMPA_vect)
{
	
   if(!timer_running)
   {
      return;
   }
   
   sayac++;  // BUNUNLA SAYAÇ HIZLI SAYMA SORUNUNU HALLETTİK.
   if(sayac==16)
   {
	   sayac=0;
	   if(s==0)
	   {
		   if(m==0)
		   {
			   if(h==0)  // BUNU BEN SONRADAN EKLEDİM.
			   {
				   //off
				   timer_running=0;
			   }
			   else
			   {
				   h--;
				   m=59;
				   s=59; // BUNU SONRADAN EKLEDİM BUG ÇIKABİLRİ DİKKATLİ OLMAK LAZIM!!!!
			   }
			   
		   }
		   else
		   {
			   
			   m--;
			   
			   s=59;
		   }
	   }
	   
	   else
	   {
		   s--;
	   }
	   
   }
   
   

}

/*
ISR(USART_UDRE_vect)
{
	unsigned char tmptail;

	/ * Check if all data is transmitted * /
	if (UART_TxHead != UART_TxTail) 
	{
		/ * Calculate buffer index * /
		tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK;
		/ * Store new index * /
		UART_TxTail = tmptail;
		/ * Start transmission * /
		UDR0 = UART_TxBuf[tmptail];
	} 
	
	else 
	{
		/ * Disable UDRE interrupt * /
		UCSR0B &= ~(1<<UDRIE0);
	}
}*/

/*
void TransmitByte(unsigned char data)
{
	unsigned char tmphead;
	
	/ * Calculate buffer index * /
	tmphead = (UART_TxHead + 1) & UART_TX_BUFFER_MASK;
	/ * Wait for free space in buffer * /
	while (tmphead == UART_TxTail);
	/ * Store data in buffer * /
	UART_TxBuf[tmphead] = data;
	/ * Store new index * /
	UART_TxHead = tmphead;
	/ * Enable UDRE interrupt * /
	UCSR0B |= (1<<UDRIE0);
}*/




void SetMicro(uint8_t state)
{
   if(state==ON )
   {
      MICRO_KEY_PORT|=(1<<MICRO_KEY_POS);
   }
   else
   {
      MICRO_KEY_PORT&=~(1<<MICRO_KEY_POS);
	  DutyCycle=0;
   }
}




void SetTime()
{
	
   int hour1=0;
   int hour2=0;
   int min =0;
   int sec=0;   // BU KULLANILMAYACAK AMA 0 OLARAK VERİLMESİ GEREKİYOR.

  
   uint8_t selection=7;   // CURSOR AYARLAMAK İÇİN BU İKİSİNİ EKLEDİK
   
   
   while(1)
   {
      
      uint8_t hour11[8] = {0xA5,0x5A,0x05,0x82,0x00,0x03,0x00,(uint8_t)hour1};
      uint8_t hour22[8] = {0xA5,0x5A,0x05,0x82,0x00,0x04,0x00,(uint8_t)hour2};
	  uint8_t min11[8] =  {0xA5,0x5A,0x05,0x82,0x00,0x06,0x00,(uint8_t)min};
	  
  
	  UART_putsas(hour11);
      UART_putsas(hour22);
      UART_putsas(min11);

	  
     uint8_t key=GetKeypadCmd(1);

      switch(key)
      {
		  
		 case KEY_MICRO:
		 {
			_delay_us(200);
			UART_putsas(saat_simge_yan);
			UART_putsas(pause_son);
			UART_putsasbuz(buzzer_3ms);
			
		    selection++;
			if(selection==9)
			{
				selection++;
			}
			if (selection==11)
			{
				selection=7;
			}
			break;
			
		 }
         
	   
		 case KEY_STOP: 
		 {
			 _delay_us(200);
			 UART_putsas(pause_yan);
			 UART_putsas(pwm_yan0);
			 UART_putsasbuz(buzzer);
		    
			
			 sec=0;
		     min=0;
		     hour1=0;
			 hour2=0;
			 
			 break;
		 }
		 
         case KEY_START:
         {
			 _delay_us(200);
			 
			 UART_putsas(pause_son);
			 UART_putsas(saat_simge_son);
			 UART_putsasbuz(buzzer);
			 
			 
			 
            if(min>0 || sec>0 || hour1>0 || hour2>0)
            {
              
			   
               m=10*min;
               s=sec;
			   h=10*hour1+hour2;
			   
			   
				   _delay_us(100);
				
				   DutyCycle+=20;
				   if(DutyCycle>100)
				   {
					   DutyCycle=20;
				   }
				
				if (DutyCycle==0)
				{
					
					UART_putsas(pwm_yan0);
					_delay_us(3);
					
				}
				else if(DutyCycle==20)
				{
					
					UART_putsas(pwm_yan1);
					_delay_us(3);
					UART_putsasbuz(buzzer_3ms);
					_delay_us(3);
				}
				else if (DutyCycle==40)
				{
					UART_putsas(pwm_yan2);
					_delay_us(3);
					UART_putsasbuz(buzzer_3ms);
					_delay_us(3);
				}
				else if (DutyCycle==60 )
				{
					
					UART_putsas(pwm_yan3);
					_delay_us(3);
					UART_putsasbuz(buzzer_3ms);
				}
				else if (DutyCycle==80)
				{
					
					UART_putsas(pwm_yan4);
					_delay_us(3);
					UART_putsasbuz(buzzer_3ms);
					_delay_us(3);
					
				}
				else if (DutyCycle==100)
				{
					
					UART_putsas(pwm_yan5);
					_delay_us(3);
					UART_putsasbuz(buzzer_3ms);
					_delay_us(3);
				}
				   
				TIFR2|=(1<<TOV2);
			   
               return;

            }
         }
		 case KEY_INC:    // BUNU BEN SONRADAN EKLEDİM! BURASI ÖNCEDEN H İDİ.
		 {
			 _delay_us(200);
			UART_putsasbuz(buzzer_3ms);
			 
			 if (selection==7)
			 {
				 hour1++;
				
				 if(hour1==10)
				 {
					 hour1=0;
				 }
				 
			 }
			 
			 if (selection==8)
			 {
				 hour2++;
			
				 if(hour2==10)
				 {
					 hour2=0;
				 }
				 
			 }
			 
			 if(selection==10)
			 {
				 min++;
				
				 if(min==6)
				 {
					 min=0;
				 }
				 
			 }
			 
			 break;
			 
             
		 }
		 case KEY_DEC:       // BURASI İPTAL EDİLECEK ÇÜNKÜ 10 MIN HASSASLIKTA OLACAK.
		 {
			 _delay_us(200);
			 UART_putsasbuz(buzzer_3ms);
              
			 if (selection==7)
			 {
				 hour1--;
		
				 if(hour1==-1)
				 {
					 hour1=9;
					 
				 }
				
				 
			 }
			 if (selection==8)
			 {
				 hour2--;
			
				 if(hour2==-1)
				 {
					 hour2=9;
				 }
				 
			 }
			 if(selection==10)
			 {
				 min--;
			
				 if(min==-1)
				 {
					 min=5;
				 }
				 
			 }
			  
			 break; 
			 
			 
			 
		 }
     
      }

     
   }
}



void UART_putc(unsigned char data)
{
	// wait for transmit buffer to be empty
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = data;
}



void UART_putsas(uint8_t s[])
{
	uint8_t i;
	for (i=0;i<8;i++)
	{
		UART_putc(s[i]);
	}
	// transmit character until NULL is reached
	//while(*s > 0) UART_putc(*s++);
}



void UART_putsasbuz(uint8_t s[])
{
	uint8_t i;
	for (i=0;i<6;i++)
	{
		UART_putc(s[i]);
	}
	
	
	// transmit character until NULL is reached
	//while(*s > 0) UART_putc(*s++);
}

void USART_Init(unsigned int ubrr)
{
//	unsigned char x;
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	
	UCSR0B = (1<<TXEN0);  // TX INTERRUPT YAPTIK.
	UCSR0C = (1<<UCSZ00)|(1<<UCSZ01);
	
//	x=0;
	
	/*UART_RxTail = x;
	UART_RxHead = x;*/
	/*UART_TxTail = x;
	UART_TxHead = x;*/
	
}
