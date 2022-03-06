// Arduino Idle Sleep Mode Example. 
// The following code automatically enables and disables the Idle Sleep Mode.
// increasing the sleep time using the For Loop. 

#include <LowPower.h>


void setup() {
  Serial.begin(9600);
  pinMode(9,OUTPUT);
  digitalWrite(9,LOW);
}
void loop() {

  digitalWrite(9,HIGH);
  delay(2000);
  digitalWrite(9,LOW);
  Serial.println("Going to sleep for 24 seconds");
  delay(100);
  for(int i = 0; i<= 2; i++)
  {
  LowPower.idle(SLEEP_8S, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_OFF,SPI_OFF, USART0_OFF, TWI_OFF);
  }
  delay(1000);
  Serial.println("Arduino: Hey I just Woke up");
  Serial.println("");
  delay(2000);
}
