#include <DS1307RTC.h>
#include <Wire.h>
#include <TimeLib.h>
#include <UTFT.h> 
#include <URTouch.h>
#define LED16_En  8
#define LED24_En  9
#define LED32_En  10
#define PWM1  11
#define PWM2  12
extern unsigned int erkamlogo[];
int doz;
int sure=0; 
//int sayi;
int sayi_birler;
int sayi_onlar;
int sayi_yuzler;
int sayi_toplam;

int xR16=38;
int xG16=38;
int xB16=38;
int xR24=38;
int xG24=38;
int xB24=38;
int xR32=38;
int xG32=38;
int xB32=38;
int sure16=0;
int sure24=0;
int sure32=0;
//==== Creating Objects
UTFT    myGLCD(SSD1289,38,39,40,41); //Parameters should be adjusted to your Display/Schield model
URTouch  myTouch( 6, 5, 4, 3, 2);
uint16_t startAddr = 0x0000;            // Start address to store in the NV-RAM
uint16_t lastAddr;                      // new address for storing in NV-RAM
uint16_t TimeIsSet = 0xaa55;            // Helper that time must not set again

//==== Defining Variables
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];

int x, y, a,b,x2,y2 ;
char currentPage ;
const int button = 14;
int buttonState = 0;
unsigned long eskiZaman=0;
unsigned long yeniZaman;
int girisKontrol = 0;
int giris=0;

char stCurrent[20]="";
int stCurrentLen=0;
char stLast[20]="";
//***************************************
char stcurrent[20]="";  //dozyukle fonksiyonu için updatestr 
int stcurrentlen=0;     //fonksiyonunu kopyaladık. Kolaylık açısından sadece
char stlast[20]="";     //değişkenlerin harflerini küçülttük (ör: stCurrentLen->stcurrentlen)
//***************************************
  tmElements_t tm;
void setup() {
  /*
  Serial.begin(9600);
 while (!Serial) ; // wait for serial
  delay(200);
  Serial.println("DS1307RTC Read Test");
  Serial.println("-------------------");
*/
  // Initial setup
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);


  pinMode(LED16_En, OUTPUT); // VCC
  pinMode(LED24_En, OUTPUT); // Sets the trigPin as an Output
  pinMode(LED32_En, OUTPUT); // Sets the echoPin as an Input
  pinMode(PWM1, OUTPUT);
  pinMode(PWM2, OUTPUT);

   myGLCD.drawBitmap(10,75,304,43,erkamlogo);
   delay(1000);
  for(int i=0;i<=320;i++){
  myGLCD.setColor(255,0,0);
  myGLCD.fillRect(i,0,320,240);
  }
  delay(5000);
  myGLCD.clrScr();
 

  
animasyonScreen();
  drawHomeScreen();
  currentPage = '1'; // Indicates that we are at Home Screen

}

void loop() { 

//--------------------------->RTC<--------------
  tmElements_t tm;
/*
  if (RTC.read(tm)) {
    //myGLCD.printNumI(tm.Day, CENTER, 41);
    Serial.print("Ok, Time = ");
    print2digits(tm.Hour);
    Serial.write(':');
    print2digits(tm.Minute);
    Serial.write(':');
    print2digits(tm.Second);
    Serial.print(", Date (D/M/Y) = ");
    Serial.print(tm.Day);
    Serial.write('/');
    Serial.print(tm.Month);
    Serial.write('/');
    Serial.print(tmYearToCalendar(tm.Year));
    Serial.println();
  } else {
    if (RTC.chipPresent()) {
      Serial.println("The DS1307 is stopped.  Please run the SetTime");
      Serial.println("example to initialize the time and begin running.");
      Serial.println();
    } else {
      Serial.println("DS1307 read error!  Please check the circuitry.");
      Serial.println();
    }
    delay(9000);
  }
  delay(1000);
  */
 //----------------------------------------------------------------------------------------------------------------------------------------------------------
//                                          ==================>CURRENTPAGE SEÇİMİ ve GERİ BUTONU<===============
 // Home Screen
 if (currentPage == '1') {
    if (myTouch.dataAvailable()) {
      myTouch.read();
      x=myTouch.getX(); // X coordinate where the screen has been pressed
      y=myTouch.getY(); // Y coordinates where the screen has been pressed
      // If we press the LED16 Button 
      if ((x>=35) && (x<=285) && (y>=90) && (y<=130)) {
        drawFrame(35, 90, 285, 130); // Custom Function -Highlighs the buttons when it's pressed
        currentPage = '2'; // Indicates that we are the first example
        myGLCD.clrScr(); // Clears the screen
        LED16(); // It is called only once, because in the next iteration of the loop, this above if statement will be false so this funtion won't be called. This function will draw the graphics of the first example.
      }
      // If we press the LED24 Button 
      if ((x>=35) && (x<=285) && (y>=140) && (y<=180)) {
        drawFrame(35, 140, 285, 180);
        currentPage = '3';
        myGLCD.clrScr();
        LED24();
      }  
      // If we press the LED32 Button
      if ((x>=35) && (x<=285) && (y>=190) && (y<=230)) {
        drawFrame(35, 190, 285, 230);
        currentPage = '4';
        myGLCD.clrScr();
        LED32();
        delay(1000);
      }
    }
 
  //myGLCD.print("Saat:", LEFT, 41);
  if (RTC.read(tm)) {
  myGLCD.setBackColor(0,0,0);
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(SmallFont); // Sets the font to small
    myGLCD.printNumI(tm.Hour, 0, 41);
    myGLCD.print(":", 15, 41);
    myGLCD.printNumI(tm.Minute, 20, 41);
    myGLCD.print(":", 35, 41);
    myGLCD.printNumI(tm.Second, 40, 41);
      delay(1000);
     myGLCD.drawLine( 0,57,50,57);
    myGLCD.printNumI(tm.Day, 0, 61);
    myGLCD.print("/", 15, 61);
    myGLCD.printNumI(tm.Month, 23, 61);
    myGLCD.print("/", 30, 61);
    myGLCD.printNumI(tmYearToCalendar(tm.Year), 37, 61);
    //  delay(1000);

      
   // myGLCD.printNumI(tm.Second, 55, 41);
    }
  }     

  /*                                         ----> Çalışmıyor
  if (currentPage == '5') {
    sayiGir();
    if (myTouch.dataAvailable()) {
        myTouch.read();
        x=myTouch.getX();
        y=myTouch.getY();
        
        //Back button 
        if ((x>=160) && (x<=300) &&(y>=130) && (y<=180)) {
          drawFrame(160, 300, 130, 180);
          currentPage = '2';
          myGLCD.clrScr();
          LED16 ();
        }
    }
}       */                     
  // Distance Sensor Example             ==============>GERİ BUTONLARI<==========
  if (currentPage == '2') {    
      getLED16_En(); // Gets distance from the sensor and this function is repeatedly called while we are at the first example in order to print the lasest results from the distance sensor
    if (myTouch.dataAvailable()) {
        myTouch.read();
        x=myTouch.getX();
        y=myTouch.getY();
//******************************************************************************************************************
       if ((x<250)&& (x>180) && (y<60) && (y>40)) {
        drawFrame(220,55 ,260, 45);
        currentPage = '5';
        myGLCD.clrScr();
        drawButtons();
        sayiGir();                                                                    //sayigir() ve dozyukle() 
                                                                                      //fonksiyonlarının çağırıldığı yer.
} 
  if ((x<200)&& (x>120) && (y<300) && (y>120)) {
        drawFrame(120,120 ,200, 300);
        currentPage = '5';
        myGLCD.clrScr();
        drawButtons();
        dozyukle();
        //Geri button
  }
  //***************************************************************************************************************
        if ((x>=10) && (x<=60) &&(y>=10) && (y<=36)) {
          drawFrame(10, 10, 60, 36);
          currentPage = '1';
          myGLCD.clrScr();
          drawHomeScreen();
          // Turns the LED off

        }
    }
  }
  
  // RGB LED Control 
  if (currentPage == '3') {
    getLED24_En();
    if (myTouch.dataAvailable()) {
        myTouch.read();
        x=myTouch.getX();
        y=myTouch.getY();
        
        //Back button
        if ((x>=10) && (x<=60) &&(y>=10) && (y<=36)) {
          drawFrame(10, 10, 60, 36);
          currentPage = '1';
          myGLCD.clrScr();
          drawHomeScreen();
        }
    }
  }

  if (currentPage == '4') {
    getLED32_En();
    if (myTouch.dataAvailable()) {
        myTouch.read();
        x=myTouch.getX();
        y=myTouch.getY();
        
        //Back button 
        if ((x>=10) && (x<=60) &&(y>=10) && (y<=36)) {
          drawFrame(10, 10, 60, 36);
          currentPage = '1';
          myGLCD.clrScr();
          drawHomeScreen();
        }
    }
}





//if (myTouch.dataAvailable()) {
//        myTouch.read();
//        x=myTouch.getX();
//        y=myTouch.getY();
//        if ((x>=10) && (x<=60) &&(y>=10) && (y<=36)) {
//          drawFrame(10, 10, 60, 36);
//          currentPage = '1';
//          myGLCD.clrScr();
//          drawHomeScreen();
//
//        }
//    }
}
//-----------------------------------------------------------------------------------------------------------------------------------------------------------
//                                            ===================>ANİMASYON SAYFASI<================
// ====== Custom Funtions ======
void animasyonScreen() {



  /*
       for (int i=0; i<100; i++)
        {
          myGLCD.setColor(random(255), random(255), random(255));
          x=2+random(316);
          y=16+random(207);
          x2=2+random(316);
          y2=16+random(207);
          myGLCD.drawRoundRect(x, y, x2, y2);
        }
  delay(2000);
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);

    for (int i=0; i<100; i++)
        {
          myGLCD.setColor(random(255), random(255), random(255));
          x=2+random(316);
          y=16+random(209);
          x2=2+random(316);
          y2=16+random(209);
          myGLCD.drawLine(x, y, x2, y2);
        }
  delay(2000);
  myGLCD.setColor(0,0,0);
  myGLCD.fillRect(1,15,318,224);
  */
}


//----------------------------------------------------------------------------------------------------------------------------------------------------------
//                                       ===========> GİRİŞ SAYFASI <=================
// drawHomeScreen - Custom Function 
void drawHomeScreen() {
  // Title
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(VGA_AQUA); // Sets color to white
  myGLCD.setFont(BigFont); // Sets font to big
  myGLCD.print("ERKAM Fototerapi", CENTER, 10); // Prints the string on the screen
  myGLCD.setColor(255, 0, 0); // Sets color to red
  myGLCD.drawLine(0,32,319,32); // Draws the red line
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(SmallFont); // Sets the font to small
   /*  if (RTC.read(tm)) {
    myGLCD.printNumI(tm.Hour, 10, 41);
    myGLCD.print(":", 25, 41);
    myGLCD.printNumI(tm.Minute, 30, 41);
    myGLCD.print(":", 45, 41);
    myGLCD.printNumI(tm.Second, 55, 41);
      delay(1000);
    myGLCD.print("  ", 55, 41);
    delay(1000);
    myGLCD.printNumI(tm.Second, 55, 41);
    }
    */
 myGLCD.print("EFT 1.0.0", CENTER, 41); // Prints the string
 // myGLCD.printNumI(tm.Second, CENTER, 49);
  myGLCD.setFont(BigFont);
  myGLCD.print("Mod Seciniz", CENTER, 64);
  
  // Button - LED16
  myGLCD.setColor(16, 167, 103); // Sets green color
  myGLCD.fillRoundRect (35, 90, 285, 130); // Draws filled rounded rectangle
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.drawRoundRect (35, 90, 285, 130); // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.setFont(BigFont); // Sets the font to big
  myGLCD.setBackColor(16, 167, 103); // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.print("16 LED", CENTER, 102); // Prints the string
  
  // Button - LED24
  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (35, 140, 285, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (35, 140, 285, 180);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.print("24 LED", CENTER, 152);

  //  Button - LED32
  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (35, 190, 285, 230);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (35, 190, 285, 230);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.print("32 LED", CENTER, 202);
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
// Highlights the button when pressed
void drawFrame(int x1, int y1, int x2, int y2) {
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (x1, y1, x2, y2);
}

//**************************************************************************************************************************************************************
 
//====================================================
void LED16() {
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRoundRect (10, 10, 60, 36);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 10, 60, 36);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("<", 12, 15);
  myGLCD.setFont(SmallFont);
  myGLCD.print("Geri", 28, 18);
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(BigFont); 
  myGLCD.print("DOZ AYARI ", CENTER, 50);
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawLine(0,75,319,75); 
  myGLCD.setColor(VGA_YELLOW);
  myGLCD.print("Doz= ",87,100);
  myGLCD.printNumI(doz,152, 100);
  myGLCD.setColor(0, 255, 0);
 myGLCD.print("Gonder", 200, 200);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(BigFont); 
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("SURE AYARI ", CENTER, 150);
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawLine(0,175,319,175); 
  myGLCD.setColor(VGA_YELLOW);
  myGLCD.print("Sure= ",87,200);
  myGLCD.printNumI(sure,166, 200);
  
  
 // myGLCD.print(sure,152, 200);
 
/*  myGLCD.print("DOZ:", 10, 95);
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRoundRect (150, 10, 210, 36);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (150, 10, 210, 36);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("SURE", 10, 170);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.print("D", 10, 135);
  myGLCD.print("S", 10, 215);
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawLine(0,75,319,75); 
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect(30, 138, 310, 148); // R - Slider
  myGLCD.drawRect(30, 218, 310, 228);
  */
//  myGLCD.drawRect(30, 218, 310, 228);   
}
//====================================================
//===== getDistance - Custom Function
void getLED16_En() {
  if (myTouch.dataAvailable()) {
    myTouch.read();
    x=myTouch.getX();
    y=myTouch.getY();   
    /*
if ((x<200)&& (x>140) && (y<60) && (y>40)) {
        drawFrame(140,40 ,200, 60);
        currentPage = '5';
        myGLCD.clrScr();
        drawButtons();
        sayiGir();
}
if ((x<200)&& (x>120) && (y<300) && (y>120)) {
        drawFrame(120,120 ,200, 300);
       currentPage = '5';
        myGLCD.clrScr();
        drawButtons();
        dozyukle();
        */

    if ((x<250)&& (x>200) && (y<220) && (y>180)) {
        drawFrame(200,180 ,250, 220);
       
          //PWM gönderme
          while(true) {
        int doz_pwm = map(doz,0,100,0,255);
        digitalWrite(LED16_En, HIGH);
         analogWrite(PWM1, doz_pwm);
         delay(sure*60000);
           if (myTouch.dataAvailable()) {
    myTouch.read();
    x=myTouch.getX();
    y=myTouch.getY();
     if ((x<250)&& (x>200) && (y<160) && (y>120))
     drawFrame(200,120 ,250, 160);
     break;
           }
          }
         analogWrite(PWM1,0);
         
    }
  }


         /*
    // Area of the Red color slider
    if( (y>=130) && (y<=156)) {
      xR16=x; // Stores the X value where the screen has been pressed in to variable xR
      if (xR16<=38) { // Confines the area of the slider to be above 38 pixels
        xR16=38;
      }
      if (xR16>=303){ /// Confines the area of the slider to be under 310 pixels
        xR16=303;
      }
    }
    // Area of the Green color slider
    if( (y>=170) && (y<=196)) {
      xG16=x;
      if (xG16<=38) {
        xG16=38;
      }
      if (xG16>=303){
        xG16=303;
      }
    }
    // Area of the Blue color slider
    if( (y>=210) && (y<=236)) {
      xB16=x;
      if (xB16<=38) {
        xB16=38;
      }
      if (xB16>=303){
        xB16=303;
      }
    } 
     if( (y>=10) && (y<=36)&&(x>=60) && (y<=36)) {
        giris=1;
        }
      if( (y>=10) && (y<=36)&&(x>=170) && (x<=180)) {
         drawButtons();
//        sayiGir(); 
        }       
 */
  
  // Maps the values of the X - Axis from 38 to 0 and 310 to 255, because we need values from 0 to 255 for turning on the led
 /* int xRC16 = map(xR16,38,310,0,255);
  int xGC16 = map(xG16,38,310,0,255);
  unsigned long xBC16 = map(xB16,38,310,0,600);
  int led16_reg=0;
  int PWM1_reg=0;
  */
 // Girişe basıldı mı?
 /*
yeniZaman = millis();
  /* bir önceki turdan itibaren 1000 milisaniye geçmiş mi
  yani yeniZaman ile eskiZaman farkı 1000den büyük mü */
  /*if(giris==1){
         
  if(yeniZaman-eskiZaman >=  (xBC16*1000*60)){
        eskiZaman = yeniZaman;
        giris=0;
        led16_reg=0;
        PWM1_reg=0;   

         }
      else {
//        eskiZaman = yeniZaman;
        led16_reg=255;
        PWM1_reg=xRC16;
           }    

  } 
             digitalWrite(LED16_En, led16_reg);
            analogWrite(PWM1, PWM1_reg);
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setFont(BigFont);
  myGLCD.printNumI(round(xRC16*0.39),80, 94,3);
  myGLCD.printNumI(xBC16,80, 170,3);
  // Draws a rectangle with the latest color combination 
  myGLCD.setColor(xRC16, xGC16, xBC16);
  myGLCD.fillRoundRect(175, 87, 310, 119);
  */

  /*
  // Draws the positioners
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(xR16,139,(xR16+4),147); // Positioner
  myGLCD.setColor(xRC16, 0, 0);
  myGLCD.fillRect(31, 139, (xR16-1), 147);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect((xR16+5), 139, 309, 147);
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(xB16,219,(xB16+4),227);
  myGLCD.setColor(0, 0, xBC16);
  myGLCD.fillRect(31, 219, (xB16-1), 227);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect((xB16+5), 219, 309, 227);
  

//  myGLCD.setColor(255, 255, 255);
//  myGLCD.fillRect(xB,219,(xB+4),227);
//  myGLCD.setColor(0, 0, xBC);
//  myGLCD.fillRect(31, 219, (xB-1), 227);
////  myGLCD.setColor(0, 0, 0);
////  myGLCD.fillRect((xB+5), 219, 309, 227);
*/
}
//====================================================
void LED24() {
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRoundRect (10, 10, 60, 36);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 10, 60, 36);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("<-", 18, 15);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SmallFont);
  myGLCD.print("Geri", 70, 18);
  myGLCD.setFont(BigFont);
  myGLCD.print("DOZ AYAR ", CENTER, 50);
  myGLCD.print("DOZ:", 10, 95);
  myGLCD.print("SURE:", 10, 170);
  myGLCD.print("D", 10, 135);
  myGLCD.print("S", 10, 215);
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawLine(0,75,319,75); 
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect(30, 138, 310, 148); // R - Slider
  myGLCD.drawRect(30, 218, 310, 228);  
}
//====================================================
//============= setLedColor() - Custom Funtion
void getLED24_En() {
  if (myTouch.dataAvailable()) {
    myTouch.read();
    x=myTouch.getX();
    y=myTouch.getY();        
    // Area of the Red color slider
    if( (y>=130) && (y<=156)) {
      xR24=x; // Stores the X value where the screen has been pressed in to variable xR
      if (xR24<=38) { // Confines the area of the slider to be above 38 pixels
        xR24=38;
      }
      if (xR24>=303){ /// Confines the area of the slider to be under 310 pixels
        xR24=303;
      }
    }
    // Area of the Green color slider
    if( (y>=170) && (y<=196)) {
      xG24=x;
      if (xG24<=38) {
        xG24=38;
      }
      if (xG24>=303){
        xG24=303;
      }
    }
    // Area of the Blue color slider
    if( (y>=210) && (y<=236)) {
      xB24=x;
      if (xB24<=38) {
        xB24=38;
      }
      if (xB24>=303){
        xB24=303;
      }
    }    
  }
  // Maps the values of the X - Axis from 38 to 0 and 310 to 255, because we need values from 0 to 255 for turning on the led
  int xRC24 = map(xR24,38,310,0,255);
  int xGC24 = map(xG24,38,310,0,255);
  int xBC24 = map(xB24,38,310,0,600);

  
  // Sends PWM signal to the pins of the led
  digitalWrite(LED24_En, HIGH);
  analogWrite(PWM2, xRC24);
  
  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setFont(BigFont);
  myGLCD.printNumI(round(xRC24*0.39),80, 94,3);
  myGLCD.printNumI(xBC24,80, 170,3);
  // Draws a rectangle with the latest color combination 
  myGLCD.setColor(xRC24, xGC24, xBC24);
  myGLCD.fillRoundRect(175, 87, 310, 119);
  

  
  // Draws the positioners
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(xR24,139,(xR24+4),147); // Positioner
  myGLCD.setColor(xRC24, 0, 0);
  myGLCD.fillRect(31, 139, (xR24-1), 147);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect((xR24+5), 139, 309, 147);
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(xB24,219,(xB24+4),227);
  myGLCD.setColor(0, 0, xBC24);
  myGLCD.fillRect(31, 219, (xB24-1), 227);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect((xB24+5), 219, 309, 227);

}
//====================================================
void LED32() {
   myGLCD.setColor(100, 155, 203);
  myGLCD.fillRoundRect (10, 10, 60, 36);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 10, 60, 36);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print("<-", 18, 15);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SmallFont);
  myGLCD.print("Geri", 70, 18);
  myGLCD.setFont(BigFont);
  myGLCD.print("DOZ AYAR", CENTER, 50);
  myGLCD.print("DOZ:", 10, 95);
  myGLCD.print("SURE:", 10, 170);
  myGLCD.print("D", 10, 135);
  myGLCD.print("S", 10, 215);
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawLine(0,75,319,75); 
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRect(30, 138, 310, 148); // R - Slider
  myGLCD.drawRect(30, 218, 310, 228);  
 
  
}
void getLED32_En( ) {
  if (myTouch.dataAvailable()) {
    myTouch.read();
    x=myTouch.getX();
    y=myTouch.getY();        
    // Area of the Red color slider
    if( (y>=130) && (y<=156)) {
      xR32=x; // Stores the X value where the screen has been pressed in to variable xR
      if (xR32<=38) { // Confines the area of the slider to be above 38 pixels
        xR32=38;
      }
      if (xR32>=303){ /// Confines the area of the slider to be under 310 pixels
        xR32=303;
      }
    }
    // Area of the Green color slider
    if( (y>=170) && (y<=196)) {
      xG32=x;
      if (xG32<=38) {
        xG32=38;
      }
      if (xG32>=303){
        xG32=303;
      }
    }
    // Area of the Blue color slider
    if( (y>=210) && (y<=236)) {
      xB32=x;
      if (xB32<=38) {
        xB32=38;
      }
      if (xB32>=303){
        xB32=303;
      }
    }    
  }
  // Maps the values of the X - Axis from 38 to 0 and 310 to 255, because we need values from 0 to 255 for turning on the led
  int xRC32 = map(xR32,38,310,0,255);
  int xGC32 = map(xG32,38,310,0,255);
  int xBC32 = map(xB32,38,310,0,600);
  
  // Sends PWM signal to the pins of the led
  digitalWrite(LED32_En, HIGH);
  analogWrite(PWM2, xRC32);

  myGLCD.setBackColor(0,0,0); // Sets the background color of the area where the text will be printed to black
  myGLCD.setColor(255, 255, 255); // Sets color to white
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setFont(BigFont);
  myGLCD.printNumI(round(xRC32*0.39),80, 94,3);
  myGLCD.printNumI(xBC32,80, 170,3);
  // Draws a rectangle with the latest color combination 
  myGLCD.setColor(xRC32, xGC32, xBC32);
  myGLCD.fillRoundRect(175, 87, 310, 119);
  

  
  // Draws the positioners
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(xR32,139,(xR32+4),147); // Positioner
  myGLCD.setColor(xR32, 0, 0);
  myGLCD.fillRect(31, 139, (xR32-1), 147);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect((xR32+5), 139, 309, 147);
  
  myGLCD.setColor(255, 255, 255);
  myGLCD.fillRect(xB32,219,(xB32+4),227);
  myGLCD.setColor(0, 0, xBC32);
  myGLCD.fillRect(31, 219, (xB32-1), 227);
  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect((xB32+5), 219, 309, 227);
 delay(sure32);
 analogWrite(xBC32, 0);
}

//********************************************************************************************************************************************************************


//====================================================

void print2digits(int number) {
  if (number >= 0 && number < 10) {
    Serial.write('0');
  }
  Serial.print(number);
}


/*
int tamsayi_cevir (char a)
{
 const int sayi;
  switch (a) {
    case '0': sayi==0; break;
    case '1': sayi==1; break;
    case '2': sayi==2; break;
    case '3': sayi==3; break;
    case '4': sayi==4; break;
    case '5': sayi==5; break;
    case '6': sayi==6; break;
    case '7': sayi==7; break;
    case '8': sayi==8; break;
    case '9': sayi==9; break;
    }
    */
    /*
      if (stCurrentLen=1) {
    sayi_birler=sayi;
  }
    if (stCurrentLen=2) {
    sayi_onlar=sayi;
  }
    if (stCurrentLen=3) {
    sayi_yuzler=sayi;
  }

  sayi_toplam=(sayi_birler*1)+(sayi_onlar*10)+(sayi_yuzler*100);
  return sayi_toplam;
 
  return sayi;
  
  }
   */
  /*
  void topla(int a) {
        if (stCurrentLen=1) {
    sayi_birler=a;
  }
    if (stCurrentLen=2) {
    sayi_onlar=a;
  }
    if (stCurrentLen=3) {
    sayi_yuzler=a;
  }

  sayi_toplam=(sayi_birler*1)+(sayi_onlar*10)+(sayi_yuzler*100);
  return sayi_toplam;
  }
*/
void drawButtons()
{
// Draw the upper row of buttons
  for (x=0; x<5; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*60), 10, 60+(x*60), 60);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*60), 10, 60+(x*60), 60);
    myGLCD.printNumI(x+1, 27+(x*60), 27);
  }
// Draw the center row of buttons
  for (x=0; x<5; x++)
  {
    myGLCD.setColor(0, 0, 255);
    myGLCD.fillRoundRect (10+(x*60), 70, 60+(x*60), 120);
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (10+(x*60), 70, 60+(x*60), 120);
    if (x<4)
      myGLCD.printNumI(x+6, 27+(x*60), 87);
  }
  myGLCD.print("0", 267, 87);
// Draw the lower row of buttons
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (10, 130, 150, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 130, 150, 180);
  myGLCD.print("Clear", 40, 147);
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (160, 130, 300, 180);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (160, 130, 300, 180);
  myGLCD.print("Enter", 190, 147);
  myGLCD.setBackColor (0, 0, 0);
}

void updateStr(char val)
{
  if (stCurrentLen<20)
  { 
    stCurrent[stCurrentLen]=val;
    stCurrent[stCurrentLen+1]='\0';
    stCurrentLen++;
    myGLCD.setColor(0, 255, 0);
    myGLCD.print(stCurrent, LEFT, 224);
    doz=atoi(stCurrent);                                                      //********************

  }
  
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.print("FAZLA KARAKTER!", CENTER, 192);
    delay(500);
    myGLCD.print("                  ", CENTER, 192);
    delay(500);
    myGLCD.print("FAZLA KARAKTER!", CENTER, 192);
    delay(500);
    myGLCD.print("                 ", CENTER, 192);
    myGLCD.setColor(0, 255, 0);
  }
}
//----------------------------------------------------Doz için-----------

void dozyukle(char value)
{
  if (stcurrentlen<20)
  { 
    stcurrent[stcurrentlen]=value;
    stcurrent[stcurrentlen+1]='\0';
    stcurrentlen++;
    myGLCD.setColor(0, 255, 0);
    myGLCD.print(stcurrent, LEFT, 224);
    sure=atoi(stcurrent);                                                      //********************

  }
  
  else
  {
    myGLCD.setColor(255, 0, 0);
    myGLCD.print("FAZLA KARAKTER!", CENTER, 192);
    delay(500);
    myGLCD.print("                  ", CENTER, 192);
    delay(500);
    myGLCD.print("FAZLA KARAKTER!", CENTER, 192);
    delay(500);
    myGLCD.print("                 ", CENTER, 192);
    myGLCD.setColor(0, 255, 0);
  }
}
//----------------------------------------------------

// Draw a red frame while a button is touched
void waitForIt(int x1, int y1, int x2, int y2)
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  
}

//sayiGir
void sayiGir()
{ 
  stCurrentLen=0;
      
  while (true)
  {
    
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      
      if ((y>=10) && (y<=60))  // Upper row
      {
        if ((x>=10) && (x<=60))  // Button: 1
        {
          waitForIt(10, 10, 60, 60);
          updateStr('1');
          //tamsayi_cevir('1');
        }
        if ((x>=70) && (x<=120))  // Button: 2
        {
          waitForIt(70, 10, 120, 60);
          updateStr('2');
          //tamsayi_cevir('2');
        }
        if ((x>=130) && (x<=180))  // Button: 3
        {
          waitForIt(130, 10, 180, 60);
          updateStr('3');
         // tamsayi_cevir('3');
        }
        if ((x>=190) && (x<=240))  // Button: 4
        {
          waitForIt(190, 10, 240, 60);
          updateStr('4');
          //tamsayi_cevir('4');
        }
        if ((x>=250) && (x<=300))  // Button: 5
        {
          waitForIt(250, 10, 300, 60);
          updateStr('5');
          //tamsayi_cevir('5');
        }
      }

      if ((y>=70) && (y<=120))  // Center row
      {
        if ((x>=10) && (x<=60))  // Button: 6
        {
          waitForIt(10, 70, 60, 120);
          updateStr('6');
          //tamsayi_cevir('6');
        }
        if ((x>=70) && (x<=120))  // Button: 7
        {
          waitForIt(70, 70, 120, 120);
          updateStr('7');
          //tamsayi_cevir('7');
     
        }
        if ((x>=130) && (x<=180))  // Button: 8
        {
          waitForIt(130, 70, 180, 120);
          updateStr('8');
          //tamsayi_cevir('8');
        }
        if ((x>=190) && (x<=240))  // Button: 9
        {
          waitForIt(190, 70, 240, 120);
          updateStr('9');
          //tamsayi_cevir('9');
        }
        if ((x>=250) && (x<=300))  // Button: 0
        {
          waitForIt(250, 70, 300, 120);
          updateStr('0');
         // tamsayi_cevir('0');
          //topla(tamsayi_cevir);
        }
        
      }


      if ((y>=130) && (y<=180))  // Upper row
      {
        if ((x>=10) && (x<=150))  // Button: Clear
        {
          waitForIt(10, 130, 150, 180);
          stCurrent[0]='\0';
          stCurrentLen=0;
          myGLCD.setColor(0, 0, 0);
          myGLCD.fillRect(0, 224, 319, 239);
        }
        if ((x>=160) && (x<=300))  // Button: Enter
        {    
          myGLCD.setColor(0, 0, 0);
          myGLCD.fillRect(0, 224, 319, 239);
          
          currentPage = '2';
          myGLCD.clrScr();
          LED16 (); 
            // waitForIt(160, 130, 300, 180);
          break;
          if (stCurrentLen>0)
          {
           
            for (x=0; x<stCurrentLen+1; x++)
            {
              stLast[x]=stCurrent[x];
            }
      
        
            /*
            stCurrent[0]='\0';
            stCurrentLen=0;
            myGLCD.setColor(0, 0, 0);
            myGLCD.fillRect(0, 208, 319, 239);
            myGLCD.setColor(0, 255, 0);
            myGLCD.print(stLast, LEFT, 208);
          */
          }
          else
          {
            myGLCD.setColor(255, 0, 0);
            myGLCD.print("SAYI GIRILMEDI!", CENTER, 192);
            delay(500);
            myGLCD.print("               ", CENTER, 192);
            delay(500);
            myGLCD.print("SAYI GIRILMEDI!", CENTER, 192);
            delay(500);
            myGLCD.print("               ", CENTER, 192);
            myGLCD.setColor(0, 255, 0);
          }
          
        }
      }
    }
    
 }

}
//-------------------------------------------Doz girme fonksiyonu------------------------------

void dozyukle()
{ 
stcurrentlen=0;
  while (true)
  {
    
    if (myTouch.dataAvailable())
    {
      myTouch.read();
      x=myTouch.getX();
      y=myTouch.getY();
      
      if ((y>=10) && (y<=60))  // Upper row
      {
        if ((x>=10) && (x<=60))  // Button: 1
        {
          waitForIt(10, 10, 60, 60);
          dozyukle('1');
          //tamsayi_cevir('1');
        }
        if ((x>=70) && (x<=120))  // Button: 2
        {
          waitForIt(70, 10, 120, 60);
          dozyukle('2');
          //tamsayi_cevir('2');
        }
        if ((x>=130) && (x<=180))  // Button: 3
        {
          waitForIt(130, 10, 180, 60);
          dozyukle('3');
         // tamsayi_cevir('3');
        }
        if ((x>=190) && (x<=240))  // Button: 4
        {
          waitForIt(190, 10, 240, 60);
         dozyukle('4');
          //tamsayi_cevir('4');
        }
        if ((x>=250) && (x<=300))  // Button: 5
        {
          waitForIt(250, 10, 300, 60);
          dozyukle('5');
          //tamsayi_cevir('5');
        }
      }

      if ((y>=70) && (y<=120))  // Center row
      {
        if ((x>=10) && (x<=60))  // Button: 6
        {
          waitForIt(10, 70, 60, 120);
          dozyukle('6');
          //tamsayi_cevir('6');
        }
        if ((x>=70) && (x<=120))  // Button: 7
        {
          waitForIt(70, 70, 120, 120);
          dozyukle('7');
          //tamsayi_cevir('7');
     
        }
        if ((x>=130) && (x<=180))  // Button: 8
        {
          waitForIt(130, 70, 180, 120);
          dozyukle('8');
          //tamsayi_cevir('8');
        }
        if ((x>=190) && (x<=240))  // Button: 9
        {
          waitForIt(190, 70, 240, 120);
          dozyukle('9');
          //tamsayi_cevir('9');
        }
        if ((x>=250) && (x<=300))  // Button: 0
        {
          waitForIt(250, 70, 300, 120);
          dozyukle('0');
         // tamsayi_cevir('0');
          //topla(tamsayi_cevir);
        }
        
      }


      if ((y>=130) && (y<=180))  // Upper row
      {
        if ((x>=10) && (x<=150))  // Button: Clear
        {
          waitForIt(10, 130, 150, 180);
          stcurrent[0]='\0';
          stcurrentlen=0;
          myGLCD.setColor(0, 0, 0);
          myGLCD.fillRect(0, 224, 319, 239);
        }
        if ((x>=160) && (x<=300))  // Button: Enter
        {    
          myGLCD.setColor(0, 0, 0);
          myGLCD.fillRect(0, 224, 319, 239);
          
          currentPage = '2';
          myGLCD.clrScr();
          LED16 (); 
            // waitForIt(160, 130, 300, 180);
          break;
          if (stcurrentlen>0)
          {
           
            for (x=0; x<stcurrentlen+1; x++)
            {
              stlast[x]=stcurrent[x];
            }
      
        
            /*
            stCurrent[0]='\0';
            stCurrentLen=0;
            myGLCD.setColor(0, 0, 0);
            myGLCD.fillRect(0, 208, 319, 239);
            myGLCD.setColor(0, 255, 0);
            myGLCD.print(stLast, LEFT, 208);
          */
          }
          else
          {
            myGLCD.setColor(255, 0, 0);
            myGLCD.print("SAYI GIRILMEDI!", CENTER, 192);
            delay(500);
            myGLCD.print("               ", CENTER, 192);
            delay(500);
            myGLCD.print("SAYI GIRILMEDI!", CENTER, 192);
            delay(500);
            myGLCD.print("               ", CENTER, 192);
            myGLCD.setColor(0, 255, 0);
          }
          
        }
      }
    }
    
 }

}

