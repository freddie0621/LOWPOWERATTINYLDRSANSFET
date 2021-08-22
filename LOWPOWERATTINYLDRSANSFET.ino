

/*
 
Ultra Low Power Led Flasher
with AtTiny85 @ 1Mhz
by Luca Soltoggio
06/01/2014
 
http://www.arduinoelettronica.com
 
*/
//sur 1/64 de seconde (wdt (0) donne counter=33814 pour 45 minutes de temps d'un chronomètre externe )
// placer wdt(4) 250 ms pour le résultat final 24 h et plus
#include <avr/sleep.h>
#include <avr/wdt.h>

#include <EEPROM.h>
//#include <avr/eeprom.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define F_CPU 4800000UL // 4.8 MHz 

int pinLed = 4;//PB4 LED
int sensorPin = A3;    // PB3 select the input pin for the potentiometer
int sensorValue = analogRead(A3);
int sensorValue1 = 0;
int delta=0;

volatile boolean f_wdt = 0;
long val=1;
int counter=0;
//uint8_t eenbr EEMEM = 0x00;	// initialisation (fichier .eep)
int jour=1;
int nuit=0;
void setup(){
//CLKPR = (1<<CLKPCE);
//CLKPR = 0; // Divide by 1   
pinMode(pinLed,OUTPUT);
//OSCCAL=131;

//setup_watchdog(0); // approximately 16ms sleep
//setup_watchdog(1); // approximately 32ms sleep
//setup_watchdog(2); // approximately 64ms sleep
//setup_watchdog(3); // approximately 128ms sleep
setup_watchdog(4); // approximately 250ms sleep
//setup_watchdog(5); // approximately 500ms sleep
//setup_watchdog(6); // approximately 1s sleep
//EEPROM.write(1, OSCCAL);

}
 
void loop(){
if (f_wdt==4) {
  // wait for timed out watchdog / flag is set when a watchdog timeout occurs
f_wdt=0;       // reset flag
counter++ ;
//counter=357 ;
//pinMode(A3,INPUT);
sensorValue1 = analogRead(A3);
delta=abs(sensorValue1-sensorValue);
sensorValue=sensorValue1;
//sensorValue=1000;
EEPROMWritelong(5, sensorValue);
//sensorValue avant modif=800
if ((counter >=val)&& (counter <= 1800) && (delta >=30) && (jour==1))//counter=>(1080)*10s=3h
{
  counter=0;
 pinMode(pinLed,OUTPUT);
digitalWrite(pinLed,HIGH); 
 
// let led blink
//delay(30000);
//digitalWrite(pinLed,LOW);
jour=0;

}
else if ((counter >=val)&& (counter <= 1800) && (jour==0))//counter=>(1080)*10s=3h
{
digitalWrite(pinLed,HIGH); 
 
// let led blink
//delay(30000);
//digitalWrite(pinLed,LOW);


}
else if (  (counter > 1805))//8640*10s=86400s=24h 8640-72=8568 72 correction //ORIGINAL VALUE 550
{
  
  counter=0;
  jour=1;
  //pinMode(pinLed,OUTPUT);
  //pinMode(pinLed,OUTPUT);
  //pinMode(pinLed,INPUT);
  //digitalWrite(pinLed,LOW);
  //pinMode(pinLed,INPUT);
  digitalWrite(pinLed,LOW);
  //delay(2000);
  //sensorValue = analogRead(A3);
  
}
else
{//counter=>(3-4-5-6)*10s
  //digitalWrite(pinLed,LOW);
  
  pinMode(pinLed,INPUT);
  system_sleep();
  //delay(2000);
  //sensorValue = analogRead(A3);
  
}
//pinMode(pinLed,INPUT); // set all used port to intput to save power

//system_sleep();
//pinMode(pinLed,OUTPUT); // set all ports into state before sleep
}

system_sleep();
}
 
// set system into the sleep state
// system wakes up when wtchdog is timed out
void system_sleep() {
cbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter OFF
 
set_sleep_mode(SLEEP_MODE_PWR_DOWN); // sleep mode is set here
sleep_enable();
 
sleep_mode();                        // System sleeps here
 
sleep_disable();                     // System continues execution here when watchdog timed out
sbi(ADCSRA,ADEN);                    // switch Analog to Digitalconverter ON
}
 
// 0=16ms, 1=32ms,2=64ms,3=128ms,4=250ms,5=500ms
// 6=1 sec,7=2 sec, 8=4 sec, 9= 8sec
void setup_watchdog(int ii) {
 
byte bb;
int ww;
if (ii > 9 ) ii=9;
bb=ii & 7;
if (ii > 7) bb|= (1<<5);
bb|= (1<<WDCE);
ww=bb;
 
MCUSR &= ~(1<<WDRF);
// start timed sequence
WDTCR |= (1<<WDCE) | (1<<WDE);
// set new watchdog timeout value
WDTCR = bb;
WDTCR |= _BV(WDTIE);// attiny 13
//WDTCR |= _BV(WDTE);// attiny 85
}
 
// Watchdog Interrupt Service / is executed when watchdog timed out
ISR(WDT_vect) {
f_wdt++;  // set global flag
}

void EEPROMWritelong(int address, long value)
      {
        
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
       byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(address , four);
      EEPROM.write(address + 1, three);
      EEPROM.write(address + 2, two);
      EEPROM.write(address + 3, one);
      
      
      
      }
