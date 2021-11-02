/**************************************************************************
 * PingYun's mini_KeyBoard application programme
 * 
 * mode:2.0
 * 
 * new added:
 *    1: ws2812RGBLEDs are supported in PingYun's mini_KeyBoard
 *    2: I fixed the top common leds and let them show the mode
 *    3: lighting sensors can working correctly on the concers of the keyboard
 *    4: A new key is supported (linked to the A3)
 * 
 * designed by YuQingyang in BJFU(BeiJingForestyUniversity) 2020.12
 **************************************************************************/
#include <Keyboard.h>
#include <Mouse.h>
#include <EEPROM.h>
#include <FastLED.h>
#define touch_time 500        //if touching time more than this time ,do second procress
#define delay_time 10         //depending on the speed of your computer,not your arduino_leonardo
#define LED_PIN     10        //ws2812_RGB_led is linked to pin_10 
#define NUM_LEDS    16        //this mini_keyboard has 16 ws2812_RGB_leds
#define LIGHTING_BASE 100    //the value compared to the linhting sensor.When arrived,turn on the RGB lights
CRGB leds[NUM_LEDS];          //to setup ,then you can use "leds[i]" and "CRGB".

#define test 0                //if you want to test,please change it to '1',then you can check the raw code from serial port

char analysis_RAM1;           //to save the val of "analysis()" for first time.
char analysis_RAM2;           //to save the val of "analysis()" for second time.
int keyboardMode=0;           //to save the working mode.including C programme(1),C++programme(2),arduino IDE(3) ...
int led_testMode=0;           //save the mode to let leds blink one by one when keyboard is in the test mode
int switchKey1=0;             //to decided whether change analysis_RAM2 or not. Being used in loop()
int switchKey2=0;             //to insure output one char
int switchKey3=0;             //to insure output one char
int common_led_Mode=0;        //to save common led modes,including showing keyboard modes(1) and showing RGB modes(2)
int RGB_blink_Mode=0;         //include 0,1 and 2.To save ws2812_RGB_leds blink modes.
long int led_RAM[10];         //to save the vals about leds(let leds blink)
long int timeRAM_1=0;         //to save press time
long int timeRAM_2=0;         //to save press time
long int commonLEDtimeRAM=0;  //when RGBleds' blinking mode is changed ,let commom led delay 3s .
long int RGBautoON_timeRAM=0; //delete the RGB shaking

void showKeyboard();          //to test raw code.
char analysis();              //to analysis which key is pressing.
void myROM_refresh();               //refresh EEPROM
void C_programme(char inputVal,int mode);    //C programme(1)
void CCC_programme(char inputVal,int mode);  //C++programme(2)
void arduinoIED(char inputVal,int mode);     //arduino IDE(3)
void delLastChar();                          //delete the lase char
void keys_scaning();                         //refresh mode_pin to change defferent modes
void LEDs_setup();                           //to setup the RGB LEDs and the common leds
void keys_setup();                           //to setup the keys on the right side of the keyboard
void leds_fresh();                           //to fresh leds working mode

void setup()
{
  Serial.begin(9600);
  Keyboard.begin();
  Mouse.begin();
  LEDs_setup();                              //to setup the RGB LEDs and the common leds
  keys_setup();                              //to setup the keys on the right side of the keyboard
  for(int i=2;i<=6;i++)pinMode(i,INPUT);     //set up keyboard scaner pin
  if(!test)while(analysis()!='Z'){}          //prevent accidental touch
  if(test)EEPROM.write(7,1);                 //be careful to this test function!!!         (keyboard working mode)
  if(test)EEPROM.write(8,0);                 //be careful to this test function!!!         (RGBleds blinking mode)
  myROM_refresh();                           //refresh the keyboard mode
}

void loop()
{
  leds_fresh();
  if(test)showKeyboard();                           //to show raw code
  keys_scaning();                                   //refresh mode_pin to change defferent modes
  if(analysis()!='Z')                               //the keyboard button is pressed
  {
    delay(10);                                      //delete the shaking
    analysis_RAM1=analysis();
    if(switchKey1==0)
    {
      analysis_RAM2=analysis_RAM1;
      switchKey1=1;            //close the door
    }
    if(switchKey2==0)
    {
      switchKey2=1;
      switch(keyboardMode)
      {
         case 1:C_programme(analysis_RAM2,1);break;
         case 2:CCC_programme(analysis_RAM2,1);break;
         case 3:arduinoIED(analysis_RAM2,1);break;
      }
    }
    if((millis()-timeRAM_1>touch_time)&&(switchKey3==0))
    {
      timeRAM_1=millis();
      switchKey3=1;
      switch(keyboardMode)
      {
         case 1:C_programme(analysis_RAM2,2);break;
         case 2:CCC_programme(analysis_RAM2,2);break;
         case 3:arduinoIED(analysis_RAM2,2);break;
      }
    }
  }
  else
  {
    timeRAM_1=millis();
    switchKey1=0;
    switchKey2=0;
    switchKey3=0;
  }
  //if(test)Serial.print(millis());
  //if(test)Serial.print("--");
  //if(test)Serial.println(millis()-timeRAM_1);
  //if(millis>5123)resetFunc();
}
void C_programme(char inputVal,int mode)
{
  char c='"';                             //c is "
  if(inputVal=='E')
  {
      Keyboard.press(KEY_RETURN);
      delay(delay_time);
      Keyboard.releaseAll();
  }
  if((mode==1)&&(inputVal!='E'))
  {
    Keyboard.print(inputVal);
  }
  if((mode==2)&&(inputVal!='E'))
  {
    switch(inputVal)
    {
      case '7':
        delLastChar();
        Keyboard.print("#include<stdio.h>\nint main()\n{\n");
        break;
      case '9':
        delLastChar();
        Keyboard.print("return 0;");
        break;
      case '6':
        delLastChar();
        Keyboard.print("printf();");
        Keyboard.press(KEY_LEFT_ARROW);
        delay(delay_time);
        Keyboard.releaseAll();
        Keyboard.press(KEY_LEFT_ARROW);
        delay(delay_time);
        Keyboard.releaseAll();
        Keyboard.print(c);                            //c is "
        break;
      case '4':
        delLastChar();
        Keyboard.print("scanf();");
        Keyboard.press(KEY_LEFT_ARROW);
        delay(delay_time);
        Keyboard.releaseAll();
        Keyboard.press(KEY_LEFT_ARROW);
        delay(delay_time);
        Keyboard.releaseAll();
        Keyboard.print(",&");
        Keyboard.press(KEY_LEFT_ARROW);
        delay(delay_time);
        Keyboard.releaseAll();
        Keyboard.press(KEY_LEFT_ARROW);
        delay(delay_time);
        Keyboard.releaseAll();
        Keyboard.print(c);                            //c is "
        Keyboard.print("%");
        break;
      case '-':
        delLastChar();
        Keyboard.print("&&");
        break;
      case '+':
        delLastChar();
        Keyboard.print("||");
        break;
      case '*':
        delLastChar();
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('s');
        delay(delay_time);
        Keyboard.releaseAll();
        break;
    }
    
  }
}
void CCC_programme(char inputVal,int mode)
{
  char c='"';                             //c is "
  if(inputVal=='E')
  {
      Keyboard.press(KEY_RETURN);
      delay(delay_time);
      Keyboard.releaseAll();
  }
  if((mode==1)&&(inputVal!='E'))
  {
    Keyboard.print(inputVal);
  }
  if((mode==2)&&(inputVal!='E'))
  {
    switch(inputVal)
    {
      case '7':
        delLastChar();
        Keyboard.print("#include<iostream>\n");
        Keyboard.print("using namespace std;\n\n");
        Keyboard.println("int main()\n{\n");
        break;
      case '9':
        delLastChar();
        Keyboard.print("return 0;");
        break;
      case '6':
        delLastChar();
        Keyboard.print("cout<<<<endl;");
        for(int i=0;i<7;i++)
        {
          Keyboard.press(KEY_LEFT_ARROW);
          delay(delay_time);
          Keyboard.releaseAll();
        }
        break;
      case '5':
        delLastChar();
        Keyboard.print("<<");
        break;
      case '4':
        delLastChar();
        Keyboard.print("cin>>;");
        Keyboard.press(KEY_LEFT_ARROW);
        delay(delay_time);
        Keyboard.releaseAll();
        break;
      case '-':
        delLastChar();
        Keyboard.print("&&");
        break;
      case '+':
        delLastChar();
        Keyboard.print("||");
        break;
      case '*':
        delLastChar();
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('s');
        delay(delay_time);
        Keyboard.releaseAll();
        break;
    }
    
  }
}
void arduinoIED(char inputVal,int mode)
{
  char c='"';                             //c is "
  if(inputVal=='E')
  {
      Keyboard.press(KEY_RETURN);
      delay(delay_time);
      Keyboard.releaseAll();
  }
  if((mode==1)&&(inputVal!='E'))
  {
    Keyboard.print(inputVal);
  }
  if((mode==2)&&(inputVal!='E'))
  {
    switch(inputVal)
    {
      case '7':
        delLastChar();
        Keyboard.print("digitalWrite(,);");
        for(int i=0;i<3;i++)
        {
          Keyboard.press(KEY_LEFT_ARROW);
          delay(delay_time);
          Keyboard.releaseAll();
        }
        break;
      case '9':
        delLastChar();
        Keyboard.print("digitalRead()");
        for(int i=0;i<1;i++)
        {
          Keyboard.press(KEY_LEFT_ARROW);
          delay(delay_time);
          Keyboard.releaseAll();
        }
        break;
        case '8':
        delLastChar();
        Keyboard.print("pinMode(,OUTPUT);");
        for(int i=0;i<9;i++)
        {
          Keyboard.press(KEY_LEFT_ARROW);
          delay(delay_time);
          Keyboard.releaseAll();
        }
        break;
      case '4':
        delLastChar();
        Keyboard.print("analogWrite(,);");
        for(int i=0;i<3;i++)
        {
          Keyboard.press(KEY_LEFT_ARROW);
          delay(delay_time);
          Keyboard.releaseAll();
        }
        break;
      case '5':
        delLastChar();
        Keyboard.print("()");
        Keyboard.press(KEY_LEFT_ARROW);
        delay(delay_time);
        Keyboard.releaseAll();
        break;
      case '6':
        delLastChar();
        Keyboard.print("analogRead()");
        for(int i=0;i<1;i++)
        {
          Keyboard.press(KEY_LEFT_ARROW);
          delay(delay_time);
          Keyboard.releaseAll();
        }
        break;
      case '-':
        delLastChar();
        Keyboard.println("Serial.begin(9600);");
        break;
      case '+':
        delLastChar();
        Keyboard.print("Serial.println();");
        for(int i=0;i<2;i++)
        {
          Keyboard.press(KEY_LEFT_ARROW);
          delay(delay_time);
          Keyboard.releaseAll();
        }
        break;
      case '*':
        delLastChar();
        Keyboard.press(KEY_LEFT_CTRL);
        Keyboard.press('u');
        delay(delay_time);
        Keyboard.releaseAll();
        break;
    }
  }
}
void showKeyboard()     //to test raw code
{
  Serial.print(digitalRead(2));Serial.print("-");
  Serial.print(digitalRead(3));Serial.print("-");
  Serial.print(digitalRead(4));Serial.print("-");
  Serial.print(digitalRead(5));Serial.print("-");
  Serial.print(digitalRead(6));Serial.print("-restlt:");
  Serial.print(analysis());Serial.print(" -keys:");
  Serial.print(digitalRead(A0));Serial.print("-");
  Serial.print(digitalRead(A1));Serial.print("-");
  Serial.print(digitalRead(A2));Serial.print("-");
  Serial.print(digitalRead(A3));Serial.print("-");
  Serial.print("-SensorRead:");
  Serial.print(analogRead(A4));
  Serial.print("-");
  Serial.println(analogRead(A5));
}
char analysis()
{
  if((digitalRead(2)==0)&&(digitalRead(3)==0)&&(digitalRead(4)==0)&&(digitalRead(5)==0)&&(digitalRead(6)==0))return 'Z';
  if((digitalRead(2)==0)&&(digitalRead(3)==0)&&(digitalRead(4)==0)&&(digitalRead(5)==1)&&(digitalRead(6)==0))return '0';
  if((digitalRead(2)==1)&&(digitalRead(3)==0)&&(digitalRead(4)==0)&&(digitalRead(5)==1)&&(digitalRead(6)==0))return '3';
  if((digitalRead(2)==1)&&(digitalRead(3)==1)&&(digitalRead(4)==0)&&(digitalRead(5)==0)&&(digitalRead(6)==0))return '2';
  if((digitalRead(2)==1)&&(digitalRead(3)==0)&&(digitalRead(4)==1)&&(digitalRead(5)==0)&&(digitalRead(6)==0))return '1';
  if((digitalRead(2)==1)&&(digitalRead(3)==0)&&(digitalRead(4)==0)&&(digitalRead(5)==0)&&(digitalRead(6)==1))return '6';
  if((digitalRead(2)==0)&&(digitalRead(3)==1)&&(digitalRead(4)==1)&&(digitalRead(5)==0)&&(digitalRead(6)==0))return '5';
  if((digitalRead(2)==0)&&(digitalRead(3)==1)&&(digitalRead(4)==0)&&(digitalRead(5)==1)&&(digitalRead(6)==0))return '4';
  if((digitalRead(2)==1)&&(digitalRead(3)==0)&&(digitalRead(4)==0)&&(digitalRead(5)==0)&&(digitalRead(6)==0))return '7';
  if((digitalRead(2)==0)&&(digitalRead(3)==1)&&(digitalRead(4)==0)&&(digitalRead(5)==0)&&(digitalRead(6)==0))return '8';
  if((digitalRead(2)==0)&&(digitalRead(3)==0)&&(digitalRead(4)==1)&&(digitalRead(5)==0)&&(digitalRead(6)==0))return '9';
  if((digitalRead(2)==1)&&(digitalRead(3)==1)&&(digitalRead(4)==1)&&(digitalRead(5)==0)&&(digitalRead(6)==0))return '+';
  if((digitalRead(2)==1)&&(digitalRead(3)==1)&&(digitalRead(4)==0)&&(digitalRead(5)==1)&&(digitalRead(6)==0))return '-';
  if((digitalRead(2)==1)&&(digitalRead(3)==1)&&(digitalRead(4)==0)&&(digitalRead(5)==0)&&(digitalRead(6)==1))return '*';
  if((digitalRead(2)==1)&&(digitalRead(3)==0)&&(digitalRead(4)==1)&&(digitalRead(5)==1)&&(digitalRead(6)==0))return '/';
  if((digitalRead(2)==0)&&(digitalRead(3)==0)&&(digitalRead(4)==0)&&(digitalRead(5)==0)&&(digitalRead(6)==1))return '.';
  if((digitalRead(2)==1)&&(digitalRead(3)==0)&&(digitalRead(4)==1)&&(digitalRead(5)==0)&&(digitalRead(6)==1))return 'E';  //enter
 }
 void LEDs_setup()
{
  led_RAM[1] = millis();                        //let led_RAM[1] save time,to make leds blink come ture
  led_RAM[2] = millis();                        //let led_RAM[1] save time,to make leds blink come ture
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  for(int i=7;i<=9;i++)pinMode(i,OUTPUT);     //common leds are linked to pin7,8 and 9
  for(int i=7;i<=9;i++)digitalWrite(i,LOW);   //to test whether common leds are working rightly
  for (int i = 0; i < NUM_LEDS; i++) {        //to test whether ws2812_RGB_leds are working rightly
    leds[i] = CRGB ( 255, 50, 80);
    FastLED.show();
    delay(40);
  }
  for (int i = 0; i < NUM_LEDS; i++) {        //to turn down ws2812_RGB_leds
    leds[i] = CRGB ( 0, 0, 0);
    FastLED.show();
    delay(40);
  }
  for(int i=7;i<=9;i++)digitalWrite(i,HIGH);  //to turn down common leds
}
void myROM_refresh()
{
  keyboardMode=EEPROM.read(7);
  RGB_blink_Mode=EEPROM.read(8);
}
void GObaiduzyb()   //custom-made mode.
{
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB ( 100, 0, 0);FastLED.show();
  delay(1000);
  for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB ( 0, 250, 0);FastLED.show();
  Mouse.move(0, 0);
  while(1)
  {
    Mouse.move(20, 80);
    delay(100);
    Mouse.click(MOUSE_LEFT);delay(10);Mouse.release(MOUSE_LEFT);
    Mouse.move(-20, -80);
    delay(900);
    Mouse.click(MOUSE_LEFT);delay(10);Mouse.release(MOUSE_LEFT);
  }
}
void keys_scaning()
{
  if((digitalRead(A0)==1)&&(digitalRead(A1)==0)&&(digitalRead(A2)==1))GObaiduzyb();
  
  if((digitalRead(A0)==1)&&(digitalRead(A1)==0)&&(digitalRead(A2)==0))EEPROM.write(7,1);
  if((digitalRead(A0)==0)&&(digitalRead(A1)==1)&&(digitalRead(A2)==0))EEPROM.write(7,2);
  if((digitalRead(A0)==0)&&(digitalRead(A1)==0)&&(digitalRead(A2)==1))EEPROM.write(7,3);
  if((digitalRead(A0)==1)||(digitalRead(A1)==1)||(digitalRead(A2)==1))myROM_refresh();
  if(digitalRead(A3)==1)
  {
    RGB_blink_Mode++;
    if(RGB_blink_Mode==3)RGB_blink_Mode=0;
    EEPROM.write(8,RGB_blink_Mode);
    commonLEDtimeRAM=millis();
    common_led_Mode=1;
    while(digitalRead(A3)==1);
    myROM_refresh();
  }
}
void delLastChar()
{
  Keyboard.press(KEY_BACKSPACE);
  delay(50);
  Keyboard.releaseAll();                     //delete the last char
}
void keys_setup()                             //to setup the keys on the right side of the keyboard
{
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,OUTPUT);
  pinMode(A4,INPUT);
  pinMode(A5,INPUT);
  digitalWrite(A3,LOW);
}
void leds_fresh()                            //to fresh leds working mode
{
  if(test)                                   //to let the leds blink one by one,which means keyboard is in the test mode
  {
    if(millis()-led_RAM[1]>500)
    {
      led_testMode++;
      led_RAM[1] = millis();
    }
    if(led_testMode == 4)led_testMode = 1;
    switch(led_testMode)
    {
      case 1:digitalWrite(7,LOW);digitalWrite(8,HIGH);digitalWrite(9,HIGH);break;
      case 2:digitalWrite(8,LOW);digitalWrite(7,HIGH);digitalWrite(9,HIGH);break;
      case 3:digitalWrite(9,LOW);digitalWrite(8,HIGH);digitalWrite(7,HIGH);break;
    }
  }
  else 
  {
    if(common_led_Mode==0)
      switch(keyboardMode)
      {
        case 1:digitalWrite(7,LOW);digitalWrite(8,HIGH);digitalWrite(9,HIGH);break;
        case 2:digitalWrite(8,LOW);digitalWrite(7,HIGH);digitalWrite(9,HIGH);break;
        case 3:digitalWrite(9,LOW);digitalWrite(8,HIGH);digitalWrite(7,HIGH);break;
      }
    if(common_led_Mode==1)
    {
      switch(RGB_blink_Mode)
      {
        case 0:digitalWrite(7,HIGH);digitalWrite(8,LOW);digitalWrite(9,LOW);break;
        case 1:digitalWrite(8,HIGH);digitalWrite(7,LOW);digitalWrite(9,LOW);break;
        case 2:digitalWrite(9,HIGH);digitalWrite(8,LOW);digitalWrite(7,LOW);break;
        default:digitalWrite(9,LOW);digitalWrite(8,LOW);digitalWrite(7,LOW);break;
      }
      if(millis()-commonLEDtimeRAM>2000)common_led_Mode=0;
    }
    if(RGB_blink_Mode==0)
    {
      for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB ( 255, 255, 50);FastLED.show();
    }
    if(RGB_blink_Mode==1)
    {
      if(millis()-led_RAM[2]>200)
      {
        led_RAM[2] = millis();
        if(timeRAM_2>239)timeRAM_2=0;
        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB ( timeRAM_2++, i*11, 255-timeRAM_2);FastLED.show();
      }
    }
    if(RGB_blink_Mode==2)
    {
      if((analogRead(A4)<LIGHTING_BASE)&&(analogRead(A5)<LIGHTING_BASE)&&(millis()-RGBautoON_timeRAM>500))
      {
        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB ( 255, i*11, 255-i*11);FastLED.show();
        RGBautoON_timeRAM=millis();
      }
      else if(((analogRead(A4)>LIGHTING_BASE+80)||(analogRead(A5)>LIGHTING_BASE+80))&&(millis()-RGBautoON_timeRAM>500))
      {
        for (int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB ( 0, 0, 0);FastLED.show();
        RGBautoON_timeRAM=millis();
      }
    }
  }
}
