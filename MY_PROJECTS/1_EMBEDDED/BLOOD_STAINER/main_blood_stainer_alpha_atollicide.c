#include "stm32f10x.h"
#include <stddef.h>
#include <stdio.h>
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_i2c.h"
#include <string.h>
#include "misc.h"



TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  		 TIM_OCInitStructure;
GPIO_InitTypeDef   		 GPIO_InitStructure;
DMA_InitTypeDef    		 DMA_InitStructure;
USART_InitTypeDef        USART_InitStructure;
NVIC_InitTypeDef         NVIC_InitStructure;
GPIO_InitTypeDef         i2c_gpio;
I2C_InitTypeDef          i2c;


#define  Led1                GPIO_Pin_10  //PortC
#define  Led2                GPIO_Pin_15  //PortA
#define  Buzzer              GPIO_Pin_9   //PortB
#define  PompaDolum          GPIO_Pin_10  //PortC
#define  PompaBosalt         GPIO_Pin_9   //PortB
#define  Air                 GPIO_Pin_2   //PortD
#define  LCDBackLight        GPIO_Pin_13  //PortC

#define  ZClk      			 GPIO_Pin_3   //GPIOB
#define  ZDir      			 GPIO_Pin_4   //GPIOB
#define  ZEn                 GPIO_Pin_5   //GPIOB
#define  ZLimitSW            GPIO_Pin_6   //GPIOB
#define  ALimitSW            GPIO_Pin_7   //GPIOB
#define  AClk      			 GPIO_Pin_12  //GPIOA
#define  ADir      			 GPIO_Pin_11  //GPIOA

#define  AMotorPort          GPIOA
#define  ZMotorPort          GPIOB


#define  VBatPin             GPIO_Pin_4   //GPIOA
#define  StepPot             GPIO_Pin_5   //GPIOA
#define  SuSeviye            GPIO_Pin_0   //GPIOA

#define  StartButton         GPIO_Pin_0   //GPIOB
#define  ResetButton         GPIO_Pin_1   //GPIOB

#define  KeyScanRow1         GPIO_Pin_0  //GPIOC
#define  KeyScanRow2         GPIO_Pin_1  //GPIOC
#define  KeyScanRow3         GPIO_Pin_2  //GPIOC
#define  KeyScanRow4         GPIO_Pin_3  //GPIOC
#define  KeyScanRow5         GPIO_Pin_4  //GPIOc

#define  KeyPin1             GPIO_Pin_0  //GPIOA
#define  KeyPin2             GPIO_Pin_1  //GPIOA
#define  KeyPin3             GPIO_Pin_2  //GPIOA
#define  KeyPin4             GPIO_Pin_3  //GPIOA

#define  KeyPadReadPort      GPIOA



#define  KapPositionTrim     3

static   __IO uint32_t      usTicks;

#define  MVolt          	0.0008056640625

#define  ADC1_DR_Address    ((uint32_t)0x4001244C)
#define  AStepTime          1
#define  ZStepTime          1500


#define  Up                 11500
#define  HalfUp             5000
#define  Down               1000
#define  DolumTime          125
#define  BosaltTime         160

#define  AirON              GPIOD->BSRR=Air
#define  AirOFF             GPIOD->BRR=Air

#define  PompaDolumON       GPIOA->BSRR=PompaDolum
#define  PompaDolumOFF      GPIOA->BRR=PompaDolum

#define  PompaBosaltON      GPIOA->BSRR=PompaBosalt
#define  PompaBosaltOFF     GPIOA->BRR=PompaBosalt




// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

typedef struct
{
  uint8_t Addr;
  uint8_t displayfunction;
  uint8_t displaycontrol;
  uint8_t displaymode;
  uint8_t numlines;
  uint8_t cols;
  uint8_t rows;
  uint8_t backlightval;
} LiquidCrystal_I2C_Def;

unsigned char TRChr[8][8]=
{
	{ 0,14,16,16,17,14, 4, 0},//ç
	{ 0, 0,12, 4, 4, 4,14, 0},//I
	{10, 0,14,17,17,17,14, 0},//ö
	{ 0,15,16,14, 1,30, 4, 0},//$
	{10, 0,17,17,17,17,14, 0},//ü
	{2, 6,14,30,14, 6,  2, 0},//<
	{ 0, 4, 4, 4, 4, 4, 4, 0},//|
	{ 0, 16, 8, 4, 2, 1, 0,0} //\//
};

unsigned char KeyPadMatrix[5][4]=
{
	{'F','M','#','*'},
	{'1','2','3','U'},
	{'4','5','6','A'},
	{'7','8','9','X'},
	{'L','0','R','E'}
};

const uint16_t  ScanPort[5]={KeyScanRow5,KeyScanRow4,KeyScanRow3,KeyScanRow2,KeyScanRow1};
const unsigned char  Text1[]=("--OTOMATIK BOYAMA --");
const unsigned char  Text2[]=("   Bitirme Projesi  ");
const unsigned char  Text3[]=("      Savaþ         ");
const unsigned char  Text4[]=("      Nehri         ");

volatile char buffer[80] = {'\0'};
const unsigned int  KapPozition[]={0,914,1828,2800,3990};//
const unsigned int  LCDADDR[4]   ={0x80,0xC0,0x94,0xD4};//
      unsigned char Sort[10]	 ={1,2,4,3,0,0,0,0,0,0};


typedef enum{FAILED=0,PASSED=!FAILED} TestStatus;
volatile FLASH_Status FLASHStatus=FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus =PASSED;

static vu16      ADCVal[3];
static uint16_t  Step,AMotorPozition,ZMotorPozition,Wait;
       uint32_t  i,j,SoundTimer,DolumTimer,BosaltTimer;
       uint8_t   Power=0,SoundTime,PowerTime,SaveTime,BandIndex,ChannelIndex,PowerFlag,SaveFlag=1,KeyBeepFlag;
       uint8_t   NoSound,ALimitFlag=0,ParkFlag,ResetFlag,SoundFlag,StartFlag,SoundTime,DolumFlag,BosaltFlag;
       uint8_t   StringBuffer[6],WaitTime;
       uint8_t   AirTime,WasherTime,Stain1Time,Stain2Time,Time,TimSec,TimeDak,Key;
unsigned char 	 Loop,SaveTimer=0,SaveFlag,Tick,Dakika,Saniye,ProcessLoop,Sayac,Adres=64,Mode;
unsigned int     SaveBuffer[256],Timer3;
float            ADCValue,VBat;


void LCDI2C_WriteChar(uint8_t value);
void LCDI2C_init(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows);
void LCDI2C_begin(uint8_t cols, uint8_t rows);
void LCD_Clear();
void LCD_Home();
void LCDI2C_noDisplay();
void LCDI2C_display();
void LCDI2C_noBlink();
void LCDI2C_blink();
void LCDI2C_noCursor();
void LCDI2C_cursor();
void LCDI2C_scrollDisplayLeft();
void LCDI2C_scrollDisplayRight();
void LCDI2C_printLeft();
void LCDI2C_printRight();
void LCDI2C_leftToRight();
void LCDI2C_rightToLeft();
void LCDI2C_shiftIncrement();
void LCDI2C_shiftDecrement();
void LCDI2C_noBacklight();
void LCDI2C_backlight();
void LCDI2C_autoscroll();
void LCDI2C_noAutoscroll();
void LCDI2C_CreateChar(uint8_t location, uint8_t charmap[]);
void LCD_XY(uint8_t col, uint8_t row);
void LCD_WriteString(char* str);
void LCDI2C_command(uint8_t value);
////compatibility API function aliases
void LCDI2C_blink_on();						// alias for blink()
void LCDI2C_blink_off();       					// alias for noBlink()
void LCDI2C_cursor_on();      	 					// alias for cursor()
void LCDI2C_cursor_off();      					// alias for noCursor()
void LCDI2C_setBacklight(uint8_t new_val);				// alias for backlight() and nobacklight()
void LCDI2C_load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar()
void LCDI2C_printstr(const char[]);

//void LCDI2C_init_priv();
void LCDI2C_send(uint8_t, uint8_t);
void LCDI2C_write4bits(uint8_t);
void LCDI2C_expanderWrite(uint8_t);
void LCDI2C_pulseEnable(uint8_t);

void I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t data);
void I2C_StartTransmission(I2C_TypeDef* I2Cx, uint8_t transmissionDirection,  uint8_t slaveAddress);
void init_I2C1(void);
void DelayMC(uint32_t mc);
void Delay(uint32_t ms);

/*************************************************************************************************************************/
void LCDI2C_WriteChar(uint8_t value)
{
	switch (value)
		{
			case 'Ç' : value=0x00; break;
			case 'Ý' : value=0x01; break;
			case 'Ö' : value=0x02; break;
			case 'Þ' : value=0x03; break;
			case 'Ü' : value=0x04; break;
			case 'ç' : value=0x00; break;
			case 'ý' : value=0x01; break;
			case 'ö' : value=0x02; break;
			case 'þ' : value=0x03; break;
			case 'ü' : value=0x04; break;
			default : break;
		}
	LCDI2C_send(value, Rs);
}

/*************************************************************************************************************************/
LiquidCrystal_I2C_Def lcdi2c;

void LCDI2C_init(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
{
  lcdi2c.Addr = lcd_Addr;
  lcdi2c.cols = lcd_cols;
  lcdi2c.rows = lcd_rows;
  lcdi2c.backlightval = LCD_NOBACKLIGHT;

  init_I2C1();
  lcdi2c.displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
  LCDI2C_begin(lcd_cols, lcd_rows);
  LCDI2C_CreateChar(0, TRChr[0]);
  LCDI2C_CreateChar(1, TRChr[1]);
  LCDI2C_CreateChar(2, TRChr[2]);
  LCDI2C_CreateChar(3, TRChr[3]);
  LCDI2C_CreateChar(4, TRChr[4]);
  LCDI2C_CreateChar(5, TRChr[5]);
  LCDI2C_CreateChar(6, TRChr[6]);
  LCDI2C_CreateChar(7, TRChr[7]);
}
/*************************************************************************************************************************/
void LCDI2C_begin(uint8_t cols, uint8_t lines)
{//, uint8_t dotsize) {
	if (lines > 1)
	{
		lcdi2c.displayfunction |= LCD_2LINE;
	}
	lcdi2c.numlines = lines;
	Delay(50);
	LCDI2C_expanderWrite(lcdi2c.backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	Delay(10);
    LCDI2C_write4bits(0x03 << 4);
    DelayMC(4500); // wait min 4.1ms
    LCDI2C_write4bits(0x03 << 4);
    DelayMC(4500); // wait min 4.1ms
    LCDI2C_write4bits(0x03 << 4);
    DelayMC(150);
    LCDI2C_write4bits(0x02 << 4);


	// set # lines, font size, etc.
	LCDI2C_command(LCD_FUNCTIONSET | lcdi2c.displayfunction);

	// turn the display on with no cursor or blinking default
	lcdi2c.displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	LCDI2C_display();

	// clear it off
	LCD_Clear();

	// Initialize to default text direction (for roman languages)
	lcdi2c.displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;

	// set the entry mode
	LCDI2C_command(LCD_ENTRYMODESET | lcdi2c.displaymode);

	LCD_Home();

}

/*************************************************************************************************************************/
void LCD_Clear()
{
	LCDI2C_command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	DelayMC(3000);  // this command takes a long time!
}
/*************************************************************************************************************************/
void LCD_Home()
{
	LCDI2C_command(LCD_RETURNHOME);  // set cursor position to zero
	DelayMC(3000);  // this command takes a long time!
}
/*************************************************************************************************************************/
void LCD_XY(uint8_t row, uint8_t col)
{
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > lcdi2c.numlines ) {
		row = lcdi2c.numlines-1;
	}
	LCDI2C_command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

/*************************************************************************************************************************/
void LCDI2C_noDisplay()
{
	lcdi2c.displaycontrol &= ~LCD_DISPLAYON;
	LCDI2C_command(LCD_DISPLAYCONTROL | lcdi2c.displaycontrol);
}
/*************************************************************************************************************************/
void LCDI2C_display()
{
	lcdi2c.displaycontrol |= LCD_DISPLAYON;
	LCDI2C_command(LCD_DISPLAYCONTROL | lcdi2c.displaycontrol);
}
/*************************************************************************************************************************/
void LCDI2C_noCursor()
{
	lcdi2c.displaycontrol &= ~LCD_CURSORON;
	LCDI2C_command(LCD_DISPLAYCONTROL | lcdi2c.displaycontrol);
}
/*************************************************************************************************************************/
void LCDI2C_cursor()
{
	lcdi2c.displaycontrol |= LCD_CURSORON;
	LCDI2C_command(LCD_DISPLAYCONTROL | lcdi2c.displaycontrol);
}
/*************************************************************************************************************************/
void LCDI2C_noBlink()
{
	lcdi2c.displaycontrol &= ~LCD_BLINKON;
	LCDI2C_command(LCD_DISPLAYCONTROL | lcdi2c.displaycontrol);
}
/*************************************************************************************************************************/
void LCDI2C_blink()
{
	lcdi2c.displaycontrol |= LCD_BLINKON;
	LCDI2C_command(LCD_DISPLAYCONTROL | lcdi2c.displaycontrol);
}
/*************************************************************************************************************************/
// These commands scroll the display without changing the RAM
void LCDI2C_scrollDisplayLeft(void)
{
	LCDI2C_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
/*************************************************************************************************************************/
void LCDI2C_scrollDisplayRight(void)
{
	LCDI2C_command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}
/*************************************************************************************************************************/
// This is for text that flows Left to Right
void LCDI2C_leftToRight(void)
{
	lcdi2c.displaymode |= LCD_ENTRYLEFT;
	LCDI2C_command(LCD_ENTRYMODESET | lcdi2c.displaymode);
}
/*************************************************************************************************************************/
// This is for text that flows Right to Left
void LCDI2C_rightToLeft(void)
{
	lcdi2c.displaymode &= ~LCD_ENTRYLEFT;
	LCDI2C_command(LCD_ENTRYMODESET | lcdi2c.displaymode);
}
/*************************************************************************************************************************/
// This will 'right justify' text from the cursor
void LCDI2C_autoscroll(void)
{
	lcdi2c.displaymode |= LCD_ENTRYSHIFTINCREMENT;
	LCDI2C_command(LCD_ENTRYMODESET | lcdi2c.displaymode);
}
/*************************************************************************************************************************/
// This will 'left justify' text from the cursor
void LCDI2C_noAutoscroll(void)
{
	lcdi2c.displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	LCDI2C_command(LCD_ENTRYMODESET | lcdi2c.displaymode);
}
/*************************************************************************************************************************/
// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LCDI2C_CreateChar(uint8_t location, uint8_t charmap[])
{
	location &= 0x7; // we only have 8 locations 0-7
	LCDI2C_command(LCD_SETCGRAMADDR | (location << 3));
	int i;
	for (i=0; i<8; i++) {
		LCDI2C_WriteChar(charmap[i]);
	}
}
/*************************************************************************************************************************/
// Turn the (optional) backlight off/on
void LCDI2C_noBacklight(void)
{
	lcdi2c.backlightval=LCD_NOBACKLIGHT;
	LCDI2C_expanderWrite(0);
}
/*************************************************************************************************************************/
void LCDI2C_backlight(void)
{
	lcdi2c.backlightval=LCD_BACKLIGHT;
	LCDI2C_expanderWrite(0);
}
/*************************************************************************************************************************/
void LCDI2C_command(uint8_t value)
{
	LCDI2C_send(value, 0);
}
/*************************************************************************************************************************/
void LCDI2C_send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    LCDI2C_write4bits((highnib)|mode);
	LCDI2C_write4bits((lownib)|mode);
}
/*************************************************************************************************************************/
void LCDI2C_write4bits(uint8_t value) {
	LCDI2C_expanderWrite(value);
	LCDI2C_pulseEnable(value);
}
/*************************************************************************************************************************/
void LCDI2C_expanderWrite(uint8_t _data){
	I2C_StartTransmission (I2C2, I2C_Direction_Transmitter, lcdi2c.Addr); //Wire.beginTransmission(_Addr);
	I2C_WriteData(I2C2, (int)(_data) | lcdi2c.backlightval);  //printIIC((int)(_data) | _backlightval);
	I2C_GenerateSTOP(I2C2, ENABLE); //Wire.endTransmission();
}
/*************************************************************************************************************************/
void LCDI2C_pulseEnable(uint8_t _data){
	LCDI2C_expanderWrite(_data | En);	// En high
	DelayMC(1);		// enable pulse must be >450ns

	LCDI2C_expanderWrite(_data & ~En);	// En low
	DelayMC(50);		// commands need > 37us to settle
}
/*************************************************************************************************************************/
void LCDI2C_cursor_on()
{
	LCDI2C_cursor();
}
/*************************************************************************************************************************/
void LCDI2C_cursor_off(){
	LCDI2C_noCursor();
}
/*************************************************************************************************************************/
void LCDI2C_blink_on(){
	LCDI2C_blink();
}
/*************************************************************************************************************************/
void LCDI2C_blink_off(){
	LCDI2C_noBlink();
}
/*************************************************************************************************************************/
void LCDI2C_load_custom_character(uint8_t char_num, uint8_t *rows){
		LCDI2C_CreateChar(char_num, rows);
}
/*************************************************************************************************************************/
void LCDI2C_setBacklight(uint8_t new_val)
{
	if(new_val){
		LCDI2C_backlight();		// turn backlight on
	}else{
		LCDI2C_noBacklight();		// turn backlight off
	}
}
/*************************************************************************************************************************/
void LCD_WriteString(char* str)
{
  uint8_t i=0;
  while(str[i])
  {
	  LCDI2C_WriteChar(str[i]);
    i++;
  }
}
/*************************************************************************************************************************/
void Delay(uint32_t ms)
{
        volatile uint32_t nCount;
        RCC_ClocksTypeDef RCC_Clocks;
        RCC_GetClocksFreq (&RCC_Clocks);

        nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
        for (; nCount!=0; nCount--);
}

/*************************************************************************************************************************/

void DelayMC(uint32_t mc)
{
        volatile uint32_t nCount;
        RCC_ClocksTypeDef RCC_Clocks;
        RCC_GetClocksFreq (&RCC_Clocks);

        nCount=(RCC_Clocks.HCLK_Frequency/10000000)*mc;
        for (; nCount!=0; nCount--);
}
/*************************************************************************************************************************/
void DelayInit()
{
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000);
}
/*************************************************************************************************************************/
void DelayMs(uint32_t Ms)
{
	usTicks = Ms;
	while (usTicks);
}
/*************************************************************************************************************************/
void NVIC_Configuration(void)
{

   NVIC_InitTypeDef NVIC_InitStructure;
   NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);
}
/*************************************************************************************************************************/
void GPIOConfig(void)
{

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin =Buzzer|ZClk|ZDir|ZEn;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin =Buzzer;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin =ZLimitSW|ALimitSW|ResetButton|StartButton;;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStructure.GPIO_Pin =Led1|PompaDolum|KeyScanRow1|KeyScanRow2|KeyScanRow3|KeyScanRow4|KeyScanRow5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	GPIO_InitStructure.GPIO_Pin =Air;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin =Led2|AClk|ADir|PompaBosalt|PompaDolum;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |  RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin =StepPot|VBatPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =KeyPin1|KeyPin2|KeyPin3|KeyPin4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
}
/*************************************************************************************************************************/
void init_I2C1(void)
{

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    i2c.I2C_ClockSpeed = 100000;
    i2c.I2C_Mode = I2C_Mode_I2C;
    i2c.I2C_DutyCycle = I2C_DutyCycle_2;
    i2c.I2C_OwnAddress1 = 0x15;
    i2c.I2C_Ack = I2C_Ack_Enable;
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C2, &i2c);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    I2C_Cmd(I2C2, ENABLE);
}

/*************************************************************************************************************************/
void I2C_StartTransmission(I2C_TypeDef* I2Cx, uint8_t transmissionDirection,  uint8_t slaveAddress)
{

    while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));

    I2C_GenerateSTART(I2Cx, ENABLE);

    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));


    I2C_Send7bitAddress(I2Cx, slaveAddress<<1, transmissionDirection);

    if(transmissionDirection== I2C_Direction_Transmitter)
    {
    	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
    }
    if(transmissionDirection== I2C_Direction_Receiver)
    {
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));
    }
}
/*************************************************************************************************************************/
void I2C_WriteData(I2C_TypeDef* I2Cx, uint8_t data)
{

    I2C_SendData(I2Cx, data);
    while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
}
/*************************************************************************************************************************/
uint8_t I2C_ReadData(I2C_TypeDef* I2Cx)
{

    while( !I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED) );
    uint8_t data;
    data = I2C_ReceiveData(I2Cx);
    return data;
}

/*************************************************************************************************************************/
void HSIClockConfig(void)
{
	RCC_DeInit();
	RCC_HSICmd(ENABLE);
	RCC_HSEConfig(RCC_HSE_OFF);
	FLASH_SetLatency(FLASH_Latency_2);
	FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	RCC_HCLKConfig(RCC_SYSCLK_Div1);
	RCC_PCLK1Config(RCC_HCLK_Div2);
	RCC_PCLK2Config(RCC_HCLK_Div1);
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_16);
	RCC_PLLCmd( ENABLE);
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}
	RCC_SYSCLKConfig( RCC_SYSCLKSource_PLLCLK);
	while (RCC_GetSYSCLKSource() != 0x08)
	{
	}
}
/*********************************************************************************************************************************/
void HSEClockConfig(void)
{
	ErrorStatus HSEStartUpStatus;

	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);
	HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus == SUCCESS)
	{
	   FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
	   FLASH_SetLatency(FLASH_Latency_2);
	   RCC_HCLKConfig(RCC_SYSCLK_Div1);					    // HCLK = SYSCLK
	   RCC_PCLK2Config(RCC_HCLK_Div1);						// PCLK2 = HCLK
	   RCC_PCLK1Config(RCC_HCLK_Div2);						// PCLK1 = HCLK/2
	   RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // PLLCLK = 8MHz * 9 = 72 MHz
	   RCC_PLLCmd(ENABLE);
	   while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);
	   RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	   while(RCC_GetSYSCLKSource() != 0x08);
	}
}
/*************************************************************************************************************************/
void RCC_Configuration(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
}
/*************************************************************************************************************************/
void Timer3Config()
{
	/*
	 *                  (Prescaller)*(period+1)
	 * interrupt time=-----------------------------
	 *                    systemclock
	 */
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	TIM_TimeBaseStructure.TIM_Prescaler = 200;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period =36000-1;//100 ms
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_UpdateRequestConfig(TIM3,TIM_UpdateSource_Global);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}
/*************************************************************************************************************************/
void SoundConfig()
{
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4 , ENABLE);
	  /* Time Base configuration */
	  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock /30000) - 1;
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	  TIM_TimeBaseStructure.TIM_Period = 50;
	  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	  /* Channel 1, 2,3 and 4 Configuration in PWM mode */
	  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	  TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	  TIM_OCInitStructure.TIM_Pulse = 10;
	  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	  TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	  TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	  TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	  TIM_Cmd(TIM4, ENABLE);
	  TIM_CtrlPWMOutputs(TIM4, ENABLE);
 }
/*************************************************************************************************************************/
void ADCinit(void)
{
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO,ENABLE);
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
  ADC_InitTypeDef  ADC_InitStructure;
  RCC_ADCCLKConfig(RCC_PCLK2_Div6);
  ADC_DeInit(ADC1);
  ADC_InitStructure.ADC_ScanConvMode = ENABLE; // tarama modu açýk
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; // sürekli çevirim yapacaðýz
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 3; //
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_13Cycles5);		// 55,5+12,5=68clck=5,6us (12mhz)
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_13Cycles5);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_13Cycles5);
  //ADC_ExternalTrigConvCmd(ADC1, ENABLE);
  ADC_Cmd(ADC1, ENABLE);
  ADC_DMACmd(ADC1,ENABLE);
  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));
  //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
}

/*************************************************************************************************************************/
void ADC1_DMAConfig()
{
  DMA_Cmd(DMA1_Channel1,DISABLE);
  DMA_DeInit ( DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)0x4001244C; // ADC->DR Adresi
  DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADCVal; // hedef adresimiz
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; // ADC kaynak. Veri yönü ADC -> Hafýza
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // ADC adresi sabit kalacak
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // Her deðer alýndýðýna memory adresi 1 artýrýlacak
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  // Kaynaktan alýnacak veri 16 bit
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; // Hedef büyüklüðü 16 bit
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High ; // Kanal Önceliði yüksek. ( bu bize kalmýþ)
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; // hafýzadan hafýzaya transfer kapalý.
  DMA_InitStructure.DMA_BufferSize = 3;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);

  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
  DMA_Cmd(DMA1_Channel1,ENABLE);
}
/*************************************************************************************************************************/



/*************************************************************************************************************************/
void SPI2_Init(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15|GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2 ,ENABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
}
/*************************************************************************************************************************/
void SPI_WriteByte(u8 Byte)
{
	while((SPI2->SR&SPI_I2S_FLAG_TXE)==RESET);		//µÈ´ý·¢ËÍÇø¿Õ
	SPI2->DR=Byte;	 	//·¢ËÍÒ»¸öbyte
	//while((SPIx->SR&SPI_I2S_FLAG_RXNE)==RESET);//µÈ´ý½ÓÊÕÍêÒ»¸öbyte
	//return SPIx->DR;          	     //·µ»ØÊÕµ½µÄÊý¾Ý
}
/*************************************************************************************************************************/
void SPI_SetSpeed(SPI_TypeDef* SPIx,u8 SpeedSet)
{
	SPIx->CR1&=0XFFC7;
	if(SpeedSet==1)//¸ßËÙ
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_8;//Fsck=Fpclk/2
	}
	else//µÍËÙ
	{
		SPIx->CR1|=SPI_BaudRatePrescaler_32; //Fsck=Fpclk/32
	}
	SPIx->CR1|=1<<6; //SPIÉè±¸Ê¹ÄÜ
}

/*************************************************************************************************************************/
void TimeToStr(unsigned char Sayi1,unsigned char Sayi2)
{
  if(Sayi1<10)
  {
    StringBuffer[0]=0+48;
    StringBuffer[1]=Sayi1+48;
  }
  else if(Sayi1<100)
  {
    StringBuffer[0]=(Sayi1/10)+48;
    StringBuffer[1]=(Sayi1%10)+48;
  }

  if(Sayi2<10)
  {
    StringBuffer[3]=0+48;
    StringBuffer[4]=Sayi2+48;
  }
  else if(Sayi2<100)
  {
     StringBuffer[3]=(Sayi2/10)+48;
     StringBuffer[4]=(Sayi2%10)+48;
  }
  StringBuffer[2]=':';
}
/*************************************************************************************************************************/
void HatFlashWrite(unsigned int Address,unsigned char AdresData)
{
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(Address);
	FLASHStatus = FLASH_ProgramWord(Address,AdresData);
	FLASH_LockBank1();
}
/*************************************************************************************************************************/
void FlashWrite(unsigned int Address)
{
	unsigned int Say=0;
	FLASH_UnlockBank1();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);
	FLASH_ErasePage(Address);
	while((Say<0xFF)&&(FLASHStatus==FLASH_COMPLETE))
	{
	 	FLASHStatus = FLASH_ProgramWord((Say*4)+Address,SaveBuffer[Say]);
	    Say++;
	}
	FLASH_LockBank1();
}
/*************************************************************************************************************************/
unsigned char FlashRead(unsigned int Address)
{
	return (*(__IO uint8_t*)Address);
}

/*************************************************************************************************************************/

/*************************************************************************************************************************/
void PutChar(unsigned char TXData)
{
	USART_SendData(USART1,TXData);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

/*************************************************************************************************************************/
void PutString(const unsigned char *String)
{
	 while(*String)
	       PutChar(*String++);
}
/*************************************************************************************************************************/
void DelayD(long int Wait)
{
	long int Say;

	for(Say=0;Say<Wait;Say++)
	{
	}
}
/*************************************************************************************************************************/
void VBatRead(void)
{
	VBat=(ADCVal[1]*MVolt)*(float)11.1;

}
/*************************************************************************************************************************/
void GotoAMotorPozition(uint16_t  Pozition)
{
	uint16_t StepSay;
	if(Pozition>AMotorPozition)
	{
		AMotorPort->BRR=ADir;//CC
		StepSay=Pozition-AMotorPozition;
	}
	else
	{
		AMotorPort->BSRR=ADir;//CCW
		StepSay=AMotorPozition-Pozition;
	}

    for(Step=0;Step<StepSay;Step++)
	{
    	AMotorPort->ODR^=AClk;
    	DelayMs(AStepTime );
	}
    AMotorPozition=Pozition;
}

/*************************************************************************************************************************/
void GotoZMotorPosition(uint16_t  Pozition)
{
	uint16_t StepSay;
	if(Pozition>ZMotorPozition)
	{
		GPIOB->BSRR=ZDir;//CC
		StepSay=Pozition-ZMotorPozition;
	}
	else
	{
		GPIOB->BRR=ZDir;//CCW
		StepSay=ZMotorPozition-Pozition;
	}

    for(Step=0;Step<StepSay;Step++)
	{
		GPIOB->ODR^=ZClk;
		DelayD(ZStepTime );
	}
    ZMotorPozition=Pozition;
}
/*************************************************************************************************************************/
void StartPozition(void)
{
	for(Step=0;Step<16;Step++)
	{
		AMotorPort->BRR=ADir;//CC
		AMotorPort->ODR^=AClk;
		DelayMs(AStepTime );

	}
	AMotorPozition=0;
}
/*************************************************************************************************************************/
void GotoPark(void)
{

	while(!ParkFlag)
	{
		if(GPIO_ReadInputDataBit(GPIOB,ALimitSW)==0)
		{
			while(GPIO_ReadInputDataBit(GPIOB,ALimitSW)==0)
			{

				AMotorPort->BSRR=ADir;//CCW
				AMotorPort->ODR^=AClk;
				DelayMs(AStepTime );
			}
		}
		else
		{
			while(GPIO_ReadInputDataBit(GPIOB,ALimitSW)==1)
			{

				AMotorPort->BRR=ADir;//CC
				AMotorPort->ODR^=AClk;
				DelayMs(AStepTime );
			}
		}
		DelayMs(200);
		for(Step=0;Step<720;Step++)
		//while(ADCVal[2]<4080)
		{
			AMotorPort->BSRR=ADir;//CCW
			AMotorPort->ODR^=AClk;
			DelayMs(1);

		}
		if(GPIO_ReadInputDataBit(GPIOB,ALimitSW)==1) ParkFlag=1;
	}
}
/*************************************************************************************************************************/
void GotoParkToHome(void)
{
	while(ParkFlag)
	{
		while(GPIO_ReadInputDataBit(GPIOB,ALimitSW)==1)
		{

			AMotorPort->BRR=ADir;//CC
			AMotorPort->ODR^=AClk;
			DelayMs(AStepTime );
		}
		DelayMs(100);
		StartPozition();
		ParkFlag=0;
	}
}
/*************************************************************************************************************************/
void AEksenGoHome(void)
{
	while(!ALimitFlag)
	{

		while(GPIO_ReadInputDataBit(GPIOB,ALimitSW)==0)
		{

			AMotorPort->BSRR=ADir;//CCW
			AMotorPort->ODR^=AClk;
			DelayMs(AStepTime );
		}
		DelayMs(100);
		StartPozition();
		DelayMs(100);
		AMotorPort->BSRR=ADir;//CC
		//ALimitFlag=1;
	}

}
/*************************************************************************************************************************/
void ZEksenGoHome(void)
{
	while(GPIO_ReadInputDataBit(GPIOB,ZLimitSW)==1)
	{
		while(GPIO_ReadInputDataBit(GPIOB,ZLimitSW)==1)
		{

			ZMotorPort->BRR=ZDir;//CCW
			ZMotorPort->ODR^=ZClk;
			DelayD(ZStepTime );
		}

	}

	DelayMs(500);
	for(Step=0;Step<100;Step++)
	{
		ZMotorPort->BSRR=ZDir;//CC
		ZMotorPort->ODR^=ZClk;
		DelayD(ZStepTime );

	}
	DelayMs(5);
	ZMotorPozition=0;
	ZMotorPort->BSRR=ZDir;//CC
}
/*************************************************************************************************************************/
void LCDEkran(void)
{
	LCD_XY(0,0);
	LCD_WriteString(( char * )(Text1));
	LCD_XY(1,0);
	LCD_WriteString(( char * )(Text2));
	LCD_XY(2,0);
	LCD_WriteString(( char * )(Text3));
	LCD_XY(3,0);
	LCD_WriteString(( char * )(Text4));
}
/*************************************************************************************************************************/
void FirstStart(void)
{
	LCD_Clear();
	LCD_XY(0,0);
	LCD_WriteString("MOD:  ");
	LCD_XY(0,4);
	LCDI2C_WriteChar(Mode+48);
	LCD_XY(1,0);
	LCD_WriteString("iþlem: Hazýrlanýyor ");
	ZEksenGoHome();
	DelayMs(1000);
	GotoPark();
	LCD_XY(1,0);
	LCD_WriteString("   Lam Sepetini     ");
	LCD_XY(2,0);
	LCD_WriteString("    Yerlestirin     ");
	LCD_XY(3,0);
	LCD_WriteString("Giriþ Tuþuna Basýn  ");
	DelayMs(1000);
}
/*************************************************************************************************************************/
void GramStainProcess1(void)
{
	LCD_XY(1,0);
    LCD_WriteString("iþlem: Boyama 1     ");
	LCD_XY(2,0);
	LCD_WriteString("                    ");
	LCD_XY(3,0);
	LCD_WriteString("                    ");
	GotoZMotorPosition(Up);
	DelayMs(200);
	GotoAMotorPozition(KapPozition[1]);
	DelayMs(2000);
	for(Loop=0;Loop<4;Loop++)
	{
		GotoZMotorPosition(Down);
		DelayMs(200);
		GotoZMotorPosition(HalfUp);
		DelayMs(200);
	}
	ZEksenGoHome();
	GPIOB->BSRR=ZEn;
	Wait=Stain1Time*60;
	DolumTimer=0;
	DolumFlag=0;
	while(Wait--)
	{
		TimeToStr((char)(Wait/60),(char)(Wait%60));
		LCD_XY(2,0);
		LCD_WriteString("Kalan Zaman:       ");
		LCD_XY(2,14);
		LCD_WriteString((char *)StringBuffer);
		DelayMs(1000);
		if(!BosaltFlag)
		{
			DolumTimer++;
			if(!DolumFlag)
			{
				PompaDolumON;
				DolumFlag=1;
			}

			if(DolumTimer>=DolumTime)
			{
				PompaDolumOFF;
				DolumTimer=0;
			}
		}
	}
	DolumTimer=0;
	GPIOB->BRR=ZEn;
	DelayMs(1);
	GotoZMotorPosition(Up);
	DelayMs(500);
}
/*************************************************************************************************************************/
void GramStainProcess2(void)
{
	LCD_XY(1,0);
    LCD_WriteString("iþlem: Boyama 2     ");
	LCD_XY(2,0);
	LCD_WriteString("                    ");
	LCD_XY(3,0);
	LCD_WriteString("                    ");
	GotoZMotorPosition(Up);
	DelayMs(200);
	GotoAMotorPozition(KapPozition[2]);
	DelayMs(2000);
	for(Loop=0;Loop<4;Loop++)
	{
		GotoZMotorPosition(Down);
		DelayMs(200);
		GotoZMotorPosition(HalfUp);
		DelayMs(200);
	}
	ZEksenGoHome();
	GPIOB->BSRR=ZEn;
	Wait=Stain2Time*60;
	DolumTimer=0;
	DolumFlag=0;
	while(Wait--)
	{
		TimeToStr((char)(Wait/60),(char)(Wait%60));
		LCD_XY(2,0);
		LCD_WriteString("Kalan Zaman:       ");
		LCD_XY(2,14);
		LCD_WriteString((char *)StringBuffer);
		DelayMs(1000);
		if(Mode==2)
		{
			if(!BosaltFlag)
			{
				DolumTimer++;
				if(!DolumFlag)
				{
					PompaDolumON;
					DolumFlag=1;
				}

				if(DolumTimer>=DolumTime)
				{
					PompaDolumOFF;
					DolumTimer=0;
				}
			}
		}
	}
	GPIOB->BRR=ZEn;
	DelayMs(1);
	GotoZMotorPosition(Up);
	DelayMs(500);
}
/*************************************************************************************************************************/
void AirProcess(void)
{
	LCD_XY(1,0);
    LCD_WriteString("iþlem: Kurutma      ");
	LCD_XY(2,0);
	LCD_WriteString("                    ");
	LCD_XY(3,0);
	LCD_WriteString("                    ");
	GotoZMotorPosition(Up);
	DelayMs(200);
	GotoAMotorPozition(KapPozition[3]);
	DelayMs(2000);
	ZEksenGoHome();
	GPIOB->BSRR=ZEn;
	AirON;
	LCD_XY(1,0);
    LCD_WriteString("iþlem: Kurutuluyor  ");
	Wait=AirTime*60;
	while(Wait--)
	{
		TimeToStr((char)(Wait/60),(char)(Wait%60));
		LCD_XY(2,0);
		LCD_WriteString("Kalan Zaman:       ");
		LCD_XY(2,14);
		LCD_WriteString((char *)StringBuffer);
		DelayMs(1000);
	}
	AirOFF;
	GPIOB->BRR=ZEn;
	DelayMs(1);
	GotoZMotorPosition(Up);
	DelayMs(500);
}
/*************************************************************************************************************************/
void WasherProcess(void)
{
	LCD_XY(1,0);
    LCD_WriteString("iþlem: Yýkama       ");
	LCD_XY(2,0);
	LCD_WriteString("                    ");
	LCD_XY(3,0);
	LCD_WriteString("                    ");
	GotoZMotorPosition(Up);
	DelayMs(200);
	GotoAMotorPozition(KapPozition[4]);
	DelayMs(2000);
	for(Loop=0;Loop<4;Loop++)
	{
		GotoZMotorPosition(Down);
		LCD_XY(1,0);
	    LCD_WriteString("iþlem: Yýkanýyor    ");
		DelayMs(1000);
		GotoZMotorPosition(HalfUp);
		DelayMs(200);
	}
	ZEksenGoHome();
	GPIOB->BSRR=ZEn;
	Wait=WasherTime*60;
	while(Wait--)
	{
		TimeToStr((char)(Wait/60),(char)(Wait%60));
		LCD_XY(2,0);
		LCD_WriteString("Kalan Zaman:       ");
		LCD_XY(2,14);
		LCD_WriteString((char *)StringBuffer);
		DelayMs(1000);
	}
	BosaltFlag=1;
	PompaBosaltON;
	GPIOB->BRR=ZEn;
	DelayMs(1);
	LCD_XY(1,0);
    LCD_WriteString("iþlem: Yýkama Bitti ");
	GotoZMotorPosition(Up);
	DelayMs(5000);
}

/*************************************************************************************************************************/
void FindProcessLoop(void)
{
	while(Sort[ProcessLoop])
	{
		ProcessLoop++;
	}

}
/*************************************************************************************************************************/
void Begin(void)
{
	unsigned char Say;

	LCD_XY(0,0);
	LCD_WriteString("MOD: ");
	LCD_XY(0,4);
	LCDI2C_WriteChar(Mode+48);
	LCD_XY(1,0);
	LCD_WriteString("  iþlem Baþlýyor   ");
	LCD_XY(2,0);
	LCD_WriteString("                   ");
	LCD_XY(3,0);
	LCD_WriteString("                   ");
	GotoParkToHome();
	DelayMs(2000);
	for(Say=0;Say<ProcessLoop;Say++)
	{
		if(Sort[Say]==1) GramStainProcess1();
		if(Sort[Say]==2) GramStainProcess2();
		if(Sort[Say]==3) AirProcess();
		if(Sort[Say]==4) WasherProcess();
	}
	LCD_XY(1,0);
    LCD_WriteString("iþlem: Tamam        ");
	GotoAMotorPozition(0);//AEksenGoHome
	DelayMs(2000);
	ZEksenGoHome();
	DelayMs(1000);
	GotoPark();
	LCD_XY(2,0);
	LCD_WriteString("  Lam Sepetini     ");
	LCD_XY(3,0);
	LCD_WriteString("       Alýn        ");
	SoundFlag=1;
	SoundTimer=0;

}
/*************************************************************************************************************************/
uint8_t ReadKeyPad(void)
{
	uint8_t Tus=0,Row,ReadPort;

	for(Row=0;Row<5;Row++)
	{
		GPIOC->ODR=ScanPort[Row];
		ReadPort=KeyPadReadPort->IDR&0x000F;
		if(ReadPort==1) ReadPort=1;
		else
		if(ReadPort==2) ReadPort=2;
		else
		if(ReadPort==4) ReadPort=3;
		else
		if(ReadPort==8) ReadPort=4;
		else ReadPort=0;

		if(ReadPort)
			Tus=KeyPadMatrix[Row][ReadPort-1]; else Tus=0;
		DelayMs(50);
		if(ReadPort) break;
	}
	return Tus;
}

/*************************************************************************************************************************/
int main(void)
{
 	HSEClockConfig();
	RCC_Configuration();
	DelayInit();
	GPIOConfig();
	NVIC_Configuration();
	SoundConfig();
	TIM4->CCR4=0;
	Timer3Config();
	ADCinit();
	ADC1_DMAConfig();
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	for(i=0;i<100000;i++);

	LCDI2C_init(0x27,20,4);
	LCDI2C_backlight();

	ALimitFlag=0;
	ParkFlag=0;
	AirOFF;
	PompaDolumOFF;
	PompaBosaltOFF;

	GPIOC->BRR=Led1;
	GPIOA->BRR=Led2;

	GPIOB->BRR=ZEn;
	GPIOB->BRR=ZDir;

	GPIOA->BSRR=ADir;
	GPIOA->BSRR=AClk;
	Mode=1;
	Stain1Time=10;
	Stain2Time=10;
	WasherTime=1;
	AirTime=10;
	SoundTime=30;
	LCDEkran();
	DelayMs(3000);
	FirstStart();
	StartFlag=0;
	SoundFlag=0;
	SoundTimer=0;
	while(1)
	{
		Key=ReadKeyPad();
		if (Key=='F')
			{
				Mode=1;
				LCD_XY(0,0);
				LCD_WriteString("MOD:  ");
				LCD_XY(0,4);
				LCDI2C_WriteChar(Mode+48);
				Sort[0]=1;
				Sort[1]=2;
				Sort[2]=4;
				Sort[3]=3;
				Sort[4]=0;
				Sort[5]=0;
				Sort[6]=0;
				KeyBeepFlag=1;
			}else
		if (Key=='M')
		{
			Mode=2;
			LCD_XY(0,0);
			LCD_WriteString("MOD:  ");
			LCD_XY(0,4);
			LCDI2C_WriteChar(Mode+48);
			Sort[0]=1;
			Sort[1]=4;
			Sort[2]=2;
			Sort[3]=4;
			Sort[4]=3;
			Sort[5]=0;
			Sort[6]=0;
			KeyBeepFlag=1;
		}else
		if (Key=='E')
			{
				StartFlag=1;
				KeyBeepFlag=1;
			}
		if(!GPIO_ReadInputDataBit(GPIOB,ResetButton))//üst Düðmeye basýldý ise
		{
			WaitTime=0;
			while(!GPIO_ReadInputDataBit(GPIOB,ResetButton))// üst düðmeye  2 saniye  basýldý ise suyu boþalt
			{
				DelayMs(100);
				WaitTime++;
				if(WaitTime>20)
				{
					BosaltFlag=1;
					PompaBosaltON;
					KeyBeepFlag=1;
				}
			}
			WaitTime=0;
		}

		while(StartFlag)
		{
			SoundFlag=0;
			GPIOC->BSRR=Led1;
			GPIOA->BSRR=Led2;
			StartFlag=0;
			FindProcessLoop();
			Begin();

		}

	}
}
/*************************************************************************************************************************/
void TIM3_IRQHandler(void)
{
	 if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	    {
	       TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	       Timer3++;
	       if(BosaltFlag)
	    	   BosaltTimer++;
	       if(BosaltTimer>=BosaltTime*10)
	       {
	    	   PompaBosaltOFF;
	    	   BosaltTimer=0;
	    	   BosaltFlag=0;

	       }

	       if((Timer3%3)==0)
	       {
	    	   SoundTimer++;
	       }
	       if(KeyBeepFlag)
	       {
	    	   TIM4->CCR4=40;
	    	   KeyBeepFlag=0;

	       } else TIM4->CCR4=0;
	       if(SoundFlag)
	       {
			   if(SoundTimer%2) TIM4->CCR4=10; else TIM4->CCR4=0;

			   if(SoundTime<SoundTimer)
			   {
				   SoundFlag=0;
				   TIM4->CCR4=0;
			   }

	       }

	    }
}
/*************************************************************************************************************************/
void SysTick_Handler()
{
	if (usTicks != 0)
	{
		usTicks--;
	}
}
