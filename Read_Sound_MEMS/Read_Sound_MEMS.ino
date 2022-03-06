/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

////////// Initialize for MEMS Sound chip //////////
const int SAMPLE_TIME = 10;
unsigned long millisCurrent;
unsigned long millisLast = 0;
unsigned long millisElapsed = 0;
const int SOUND_PIN = A1;
int ambientValue;
////////// Initialize for MEMS Sound chip //////////

int min = 1023 ;
int max = 0 ;
int delta = 0 ;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  ambientValue = get_ambient_MEMS(100) ; //initialize ambient value of sound to get baseline

}


// the loop routine runs over and over again forever:
void loop() {

  int sensorValue = read_sound_MEMS();
  delta = abs(ambientValue - sensorValue) ;

  // print out the value you read:
  Serial.print("ambient ");
  Serial.print(ambientValue);
  Serial.print("    ");
  Serial.print(sensorValue);
  Serial.print("   delta ");
  Serial.print(delta);
  Serial.print("   min ");
  Serial.print(min);
  Serial.print("   max ");
  Serial.println(max);

  if (delta > max) {
    max = delta ;
  }
  if (delta < min) {
    min = delta ;
  }

}

int read_sound_MEMS () {
  // read the input on analog pin :
  int sensorValue = analogRead(SOUND_PIN);
  delay(10);        // delay in between reads for stability
  return (sensorValue) ;
}

//void read_sound_MEMS () {
//  millisCurrent = millis();
//  millisElapsed = millisCurrent - millisLast;
//  if (millisElapsed > SAMPLE_TIME) {
//    // read the input on analog pin :
//    int sensorValue = analogRead(SOUND_PIN);
//    // print out the value you read:
////    Serial.print("sound ");
////    Serial.println(sensorValue);
//    millisLast = millisCurrent;
//  }
//  //  delay(10);        // delay in between reads for stability
//
//}

int get_ambient_MEMS (int samples) {

  float sound_total = 0;
  int i ;

  for ( i = 0; i < samples; i++) {
    sound_total += analogRead(SOUND_PIN) ;
    delay(10);        // delay in between reads for stability
  }
  //  Serial.print ("samples=");
  //  Serial.print (samples) ;
  //  Serial.print ("    ");
  //  Serial.print ("i=");
  //  Serial.print (i) ;
  //  Serial.print ("    ");
  //  Serial.print ("sound total=");
  //  Serial.print (sound_total) ;
  //  Serial.print ("    ");
  //  Serial.print ("sound average=");
  //  Serial.println (sound_total / i) ;

  return (sound_total / i) ; // return average value
}
