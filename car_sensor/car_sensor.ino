/*
  Car Sensor

*/

//////////  Headers /////////
#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

//#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_TSL2591.h"


#include "Adafruit_SHT4x.h"

///////////   DEBUG SECTION  //////////

const bool DEBUG = true;
//const bool DEBUG = false;

///////////   DEBUG SECTION  //////////


///////////   Temp/Hum Init SECTION  //////////

Adafruit_SHT4x sht4 = Adafruit_SHT4x();

///////////   Temp/Hum Init SECTION  //////////

///////////   PIR Motion Init SECTION  //////////

int inputPin = 8;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status

///////////   PIR Motion Init SECTION  //////////


///////////   Light Sensor Init SECTION  //////////


Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591); // pass in a number for the sensor identifier (for your use later)

/**************************************************************************/
/*
    Displays some basic information on this sensor from the unified
    sensor API sensor_t type (see Adafruit_Sensor for more information)
*/
/**************************************************************************/
void displaySensorDetails(void)
{
  sensor_t sensor;
  tsl.getSensor(&sensor);
  if (DEBUG) {
    Serial.println(F("------------------------------------"));
    Serial.print  (F("Sensor:       ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:   ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:    ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:    ")); Serial.print(sensor.max_value); Serial.println(F(" lux"));
    Serial.print  (F("Min Value:    ")); Serial.print(sensor.min_value); Serial.println(F(" lux"));
    Serial.print  (F("Resolution:   ")); Serial.print(sensor.resolution, 4); Serial.println(F(" lux"));
    Serial.println(F("------------------------------------"));
    Serial.println(F(""));
  }
  delay(500);
}

/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2591
*/
/**************************************************************************/
void configureSensor(void)
{
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  tsl.setGain(TSL2591_GAIN_MED);      // 25x gain
  //tsl.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  //tsl.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  // tsl.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)

  /* Display the gain and integration time for reference sake */
  if (DEBUG) {
    Serial.println(F("------------------------------------"));
    Serial.print  (F("Gain:         "));
  }
  tsl2591Gain_t gain = tsl.getGain();
  switch (gain)
  {
    case TSL2591_GAIN_LOW:
      if (DEBUG) {
        Serial.println(F("1x (Low)"));
      }
      break;
    case TSL2591_GAIN_MED:
      if (DEBUG) {
        Serial.println(F("25x (Medium)"));
      }
      break;
    case TSL2591_GAIN_HIGH:
      if (DEBUG) {
        Serial.println(F("428x (High)"));
      }
      break;
    case TSL2591_GAIN_MAX:
      if (DEBUG) {
        Serial.println(F("9876x (Max)"));
      }
      break;
  }
  if (DEBUG) {
    Serial.print  (F("Timing:       "));
    Serial.print((tsl.getTiming() + 1) * 100, DEC);
    Serial.println(F(" ms"));
    Serial.println(F("------------------------------------"));
    Serial.println(F(""));
  }
}


/**************************************************************************/
/*
    Shows how to perform a basic read on visible, full spectrum or
    infrared light (returns raw 16-bit ADC values)
*/
/**************************************************************************/
void simpleRead(void)
{
  // Simple data read example. Just read the infrared, fullspecrtrum diode
  // or 'visible' (difference between the two) channels.
  // This can take 100-600 milliseconds! Uncomment whichever of the following you want to read
  uint16_t x = tsl.getLuminosity(TSL2591_VISIBLE);
  //uint16_t x = tsl.getLuminosity(TSL2591_FULLSPECTRUM);
  //uint16_t x = tsl.getLuminosity(TSL2591_INFRARED);

  if (DEBUG) {
    Serial.print(F("[ "));
    Serial.print(millis());
    Serial.print(F(" ms ] "));
    Serial.print(F("Luminosity: "));
    Serial.println(x, DEC);
  }
}

/**************************************************************************/
/*
    Show how to read IR and Full Spectrum at once and convert to lux
*/
/**************************************************************************/
void advancedRead(void)
{
  // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
  // That way you can do whatever math and comparisons you want!
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  if (DEBUG) {
    Serial.print(F("[ "));
    Serial.print(millis());
    Serial.print(F(" ms ] "));
    Serial.print(F("IR: "));
    Serial.print(ir);
    Serial.print(F("  "));
    Serial.print(F("Full: "));
    Serial.print(full);
    Serial.print(F("  "));
    Serial.print(F("Visible: "));
    Serial.print(full - ir);
    Serial.print(F("  "));
    Serial.print(F("Lux: "));
    Serial.println(tsl.calculateLux(full, ir), 6);
  }
}

/**************************************************************************/
/*
    Performs a read using the Adafruit Unified Sensor API.
*/
/**************************************************************************/
void unifiedSensorAPIRead(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  tsl.getEvent(&event);

  /* Display the results (light is measured in lux) */
  if (DEBUG) {
    Serial.print(F("[ "));
    Serial.print(event.timestamp);
    Serial.print(F(" ms ] "));
  }
  if ((event.light == 0) |
      (event.light > 4294966000.0) |
      (event.light < -4294966000.0))
  {
    /* If event.light = 0 lux the sensor is probably saturated */
    /* and no reliable data could be generated! */
    /* if event.light is +/- 4294967040 there was a float over/underflow */
    if (DEBUG) {
      Serial.println(F("Invalid data (adjust gain or timing)"));
    }
  }
  else
  {
    if (DEBUG) {
      Serial.print(event.light);
      Serial.print(F(" lux"));
    }
  }
}


/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/

///////////   Light Sensor Init SECTION  //////////

///////////   OLED Init Section  //////////
U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // pin remapping with ESP8266 HW I2C

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}
//
void u8g2_box_title(uint8_t a) {
  u8g2.drawStr( 10 + a * 2, 5, "U8g2");
  u8g2.drawStr( 10, 20, "GraphicsTest");

  u8g2.drawFrame(0, 0, u8g2.getDisplayWidth(), u8g2.getDisplayHeight() );
}

void u8g2_box_frame(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawBox");
  u8g2.drawBox(5, 10, 20, 10);
  //  u8g2.drawBox(10 + a, 15, 30, 7);
  //  u8g2.drawStr( 0, 30, "drawFrame");
  //  u8g2.drawFrame(5, 10 + 30, 20, 10);
  //  u8g2.drawFrame(10 + a, 15 + 30, 30, 7);
}

void u8g2_disc_circle(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawDisc");
  u8g2.drawDisc(10, 18, 9);
  //  u8g2.drawDisc(24 + a, 16, 7);
  //  u8g2.drawStr( 0, 30, "drawCircle");
  //  u8g2.drawCircle(10, 18 + 30, 9);
  //  u8g2.drawCircle(24 + a, 16 + 30, 7);
}

void u8g2_r_frame(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawRFrame/Box");
  u8g2.drawRFrame(5, 10, 40, 30, a + 1);
  u8g2.drawRBox(50, 10, 25, 40, a + 1);
}

void u8g2_string(uint8_t a) {
  u8g2.setFontDirection(0);
  u8g2.drawStr(30 + a, 31, " 0");
  //  u8g2.setFontDirection(1);
  //  u8g2.drawStr(30, 31 + a, " 90");
  //  u8g2.setFontDirection(2);
  //  u8g2.drawStr(30 - a, 31, " 180");
  //  u8g2.setFontDirection(3);
  //  u8g2.drawStr(30, 31 - a, " 270");
}

void u8g2_line(uint8_t a) {
  u8g2.drawStr( 0, 0, "drawLine");
  u8g2.drawLine(7 + a, 10, 40, 55);
  //  u8g2.drawLine(7 + a * 2, 10, 60, 55);
  //  u8g2.drawLine(7 + a * 3, 10, 80, 55);
  //  u8g2.drawLine(7 + a * 4, 10, 100, 55);
}

void u8g2_triangle(uint8_t a) {
  uint16_t offset = a;
  u8g2.drawStr( 0, 0, "drawTriangle");
  u8g2.drawTriangle(14, 7, 45, 30, 10, 40);
  //  u8g2.drawTriangle(14 + offset, 7 - offset, 45 + offset, 30 - offset, 57 + offset, 10 - offset);
  //  u8g2.drawTriangle(57 + offset * 2, 10, 45 + offset * 2, 30, 86 + offset * 2, 53);
  //  u8g2.drawTriangle(10 + offset, 40 + offset, 45 + offset, 30 + offset, 86 + offset, 53 + offset);
}

void u8g2_ascii_1() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr( 0, 0, "ASCII page 1");
  for ( y = 0; y < 6; y++ ) {
    for ( x = 0; x < 16; x++ ) {
      s[0] = y * 16 + x + 32;
      u8g2.drawStr(x * 7, y * 10 + 10, s);
    }
  }
}

void u8g2_ascii_2() {
  char s[2] = " ";
  uint8_t x, y;
  u8g2.drawStr( 0, 0, "ASCII page 2");
  for ( y = 0; y < 6; y++ ) {
    for ( x = 0; x < 16; x++ ) {
      s[0] = y * 16 + x + 160;
      u8g2.drawStr(x * 7, y * 10 + 10, s);
    }
  }
}

void u8g2_extra_page(uint8_t a)
{
  //  u8g2.drawStr( 0, 0, "Unicode");
  //  u8g2.setFont(u8g2_font_unifont_t_symbols);
  //  u8g2.setFontPosTop();
  //  u8g2.drawUTF8(0, 24, "☀ ☁");
  //  switch (a) {
  //    case 0:
  //    case 1:
  //    case 2:
  //    case 3:
  //      u8g2.drawUTF8(a * 3, 36, "☂");
  //      break;
  //    case 4:
  //    case 5:
  //    case 6:
  //    case 7:
  //      u8g2.drawUTF8(a * 3, 36, "☔");
  //      break;
  //  }
}

void u8g2_xor(uint8_t a) {
  uint8_t i;
  u8g2.drawStr( 0, 0, "XOR");
  u8g2.setFontMode(1);
  u8g2.setDrawColor(2);
  for ( i = 0; i < 5; i++)
  {
    u8g2.drawBox(10 + i * 16, 18 + (i & 1) * 4, 21, 31);
  }
  u8g2.drawStr( 5 + a, 19, "XOR XOR XOR XOR");
  u8g2.setDrawColor(0);
  u8g2.drawStr( 5 + a, 29, "CLR CLR CLR CLR");
  u8g2.setDrawColor(1);
  u8g2.drawStr( 5 + a, 39, "SET SET SET SET");
  u8g2.setFontMode(0);

}
//
//#define cross_width 24
//#define cross_height 24
//static const unsigned char cross_bits[] U8X8_PROGMEM  = {
//  0x00, 0x18, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x42, 0x00,
//  0x00, 0x42, 0x00, 0x00, 0x42, 0x00, 0x00, 0x81, 0x00, 0x00, 0x81, 0x00,
//  0xC0, 0x00, 0x03, 0x38, 0x3C, 0x1C, 0x06, 0x42, 0x60, 0x01, 0x42, 0x80,
//  0x01, 0x42, 0x80, 0x06, 0x42, 0x60, 0x38, 0x3C, 0x1C, 0xC0, 0x00, 0x03,
//  0x00, 0x81, 0x00, 0x00, 0x81, 0x00, 0x00, 0x42, 0x00, 0x00, 0x42, 0x00,
//  0x00, 0x42, 0x00, 0x00, 0x24, 0x00, 0x00, 0x24, 0x00, 0x00, 0x18, 0x00,
//};
//
//#define cross_fill_width 24
//#define cross_fill_height 24
//static const unsigned char cross_fill_bits[] U8X8_PROGMEM  = {
//  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x18, 0x64, 0x00, 0x26,
//  0x84, 0x00, 0x21, 0x08, 0x81, 0x10, 0x08, 0x42, 0x10, 0x10, 0x3C, 0x08,
//  0x20, 0x00, 0x04, 0x40, 0x00, 0x02, 0x80, 0x00, 0x01, 0x80, 0x18, 0x01,
//  0x80, 0x18, 0x01, 0x80, 0x00, 0x01, 0x40, 0x00, 0x02, 0x20, 0x00, 0x04,
//  0x10, 0x3C, 0x08, 0x08, 0x42, 0x10, 0x08, 0x81, 0x10, 0x84, 0x00, 0x21,
//  0x64, 0x00, 0x26, 0x18, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//};
//
//#define cross_block_width 14
//#define cross_block_height 14
//static const unsigned char cross_block_bits[] U8X8_PROGMEM  = {
//  0xFF, 0x3F, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
//  0xC1, 0x20, 0xC1, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20, 0x01, 0x20,
//  0x01, 0x20, 0xFF, 0x3F,
//};

void u8g2_bitmap_overlay(uint8_t a) {
  //  uint8_t frame_size = 28;
  //
  //  u8g2.drawStr(0, 0, "Bitmap overlay");
  //
  //  u8g2.drawStr(0, frame_size + 12, "Solid / transparent");
  //  u8g2.setBitmapMode(false /* solid */);
  //  u8g2.drawFrame(0, 10, frame_size, frame_size);
  //  u8g2.drawXBMP(2, 12, cross_width, cross_height, cross_bits);
  //  if (a & 4)
  //    u8g2.drawXBMP(7, 17, cross_block_width, cross_block_height, cross_block_bits);
  //
  //  u8g2.setBitmapMode(true /* transparent*/);
  //  u8g2.drawFrame(frame_size + 5, 10, frame_size, frame_size);
  //  u8g2.drawXBMP(frame_size + 7, 12, cross_width, cross_height, cross_bits);
  //  if (a & 4)
  //    u8g2.drawXBMP(frame_size + 12, 17, cross_block_width, cross_block_height, cross_block_bits);
}

void u8g2_bitmap_modes(uint8_t transparent) {
  //  const uint8_t frame_size = 24;
  //
  //  u8g2.drawBox(0, frame_size * 0.5, frame_size * 5, frame_size);
  //  u8g2.drawStr(frame_size * 0.5, 50, "Black");
  //  u8g2.drawStr(frame_size * 2, 50, "White");
  //  u8g2.drawStr(frame_size * 3.5, 50, "XOR");
  //
  //  if (!transparent) {
  //    u8g2.setBitmapMode(false /* solid */);
  //    u8g2.drawStr(0, 0, "Solid bitmap");
  //  } else {
  //    u8g2.setBitmapMode(true /* transparent*/);
  //    u8g2.drawStr(0, 0, "Transparent bitmap");
  //  }
  //  u8g2.setDrawColor(0);// Black
  //  u8g2.drawXBMP(frame_size * 0.5, 24, cross_width, cross_height, cross_bits);
  //  u8g2.setDrawColor(1); // White
  //  u8g2.drawXBMP(frame_size * 2, 24, cross_width, cross_height, cross_bits);
  //  u8g2.setDrawColor(2); // XOR
  //  u8g2.drawXBMP(frame_size * 3.5, 24, cross_width, cross_height, cross_bits);
}

//void u8g2_ellipse(uint8_t a) {
void u8g2_ellipse(char *text) {
  //  uint16_t offset = 5;

  //
  //
  //
  ////String s = "Starting Up...." ;
  //String s = String(loopCount) ;
  //
  //Serial.print ("loopCount=");
  //Serial.print (loopCount);
  //Serial.pring ("   string loopCount = ");
  //Serial.println (s);

  //char s = "Initial Sting" ;

  //  u8g2.drawStr( 0, 0, s.c_str());
  //  u8g2.drawStr( 0, 0, "DRAWING TEXT");
  write_text_OLED(15, 1,  text);

}


//
uint8_t draw_state = 0;
//
void draw(void) {
  u8g2_prepare();
  switch (draw_state >> 3) {
    //    case 0: u8g2_box_title(draw_state & 7); break;
    //    case 1: u8g2_box_frame(draw_state & 7); break;
    case 0:  break;
    case 1:  break;
    case 2:  break;
    case 3:  break;

      //
      //    case 0: u8g2_ellipse("Testing 1, 2, 3..."); break;
      //    case 1: u8g2_ellipse("01234567890123456789ABCDEF"); break;
      //    case 2: u8g2_ellipse("This is a long one...."); break;
      //   case 3: u8g2_ellipse("Another write..."); break;

      //    case 2: u8g2_disc_circle(draw_state & 7); break;
      //    case 3: u8g2_r_frame(draw_state & 7); break;
      //    case 4: u8g2_string(draw_state & 7); break;
      //    case 5: u8g2_line(draw_state & 7); break;
      //    case 6: u8g2_triangle(draw_state & 7); break;
      //    case 7: u8g2_ascii_1(); break;
      //    case 8: u8g2_ascii_2(); break;
      //    case 9: u8g2_extra_page(draw_state & 7); break;
      //    case 10: u8g2_xor(draw_state & 7); break;
      //    case 11: u8g2_bitmap_modes(0); break;
      //    case 12: u8g2_bitmap_modes(1); break;
      //    case 13: u8g2_bitmap_overlay(draw_state & 7); break;
  }
}

//////////  OLED Init Section    /////////


////////// Initialize for MEMS Sound chip //////////
const int SAMPLE_TIME = 10;
unsigned long millisCurrent;
unsigned long millisLast = 0;
unsigned long millisElapsed = 0;
const int SOUND_PIN = A1;
int ambientValue = 0;
int min = 1023 ;
int max = 0 ;
int delta = 0 ;
////////// Initialize for MEMS Sound chip //////////


// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  delay (1000) ;

  Serial.write("B"); 
  Serial.write("B"); 
  Serial.write("B"); 
  Serial.write("B"); 
  Serial.write("B"); 
  Serial.write("B"); 
  Serial.write("B"); 
  Serial.write("B"); 
  Serial.write("B"); 
  Serial.write("B"); 



  ////////// Temp/Hum Sesnor Setup //////////
  if (DEBUG) {

    Serial.println("Adafruit SHT4x test");
  }
  if (! sht4.begin()) {
    if (DEBUG) {
      Serial.println("Couldn't find SHT4x");
    }
    while (1) delay(1);
  }
  if (DEBUG) {

    Serial.println("Found SHT4x sensor");
    Serial.print("Serial number 0x");
    Serial.println(sht4.readSerial(), HEX);
  }

  // You can have 3 different precisions, higher precision takes longer
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  switch (sht4.getPrecision()) {
    case SHT4X_HIGH_PRECISION:
      if (DEBUG) {
        Serial.println("High precision");
      }
      break;
    case SHT4X_MED_PRECISION:
      if (DEBUG) {
        Serial.println("Med precision");
      }
      break;
    case SHT4X_LOW_PRECISION:
      if (DEBUG) {
        Serial.println("Low precision");
      }
      break;
  }



  ////////// Temp/Hum Sesnor Setup //////////

  ////////// PIR Motion Sesnor Setup //////////

  pinMode(inputPin, INPUT);     // declare sensor as input

  ////////// PIR Motion Sesnor Setup //////////


  ////////// Light Sesnor Setup //////////
  if (DEBUG) {
    Serial.println(F("Starting Adafruit TSL2591 Test!"));
  }

  if (tsl.begin())
  {
    if (DEBUG) {
      Serial.println(F("Found a TSL2591 sensor"));
    }
  }
  else
  {
    if (DEBUG) {
      Serial.println(F("No sensor found ... check your wiring?"));
    }
    while (1);
  }

  /* Display some basic information on this sensor */
  displaySensorDetails();

  /* Configure the sensor */
  configureSensor();


  ////////// Light Sesnor Setup //////////

  ////////// Setup for OLED //////////

  u8g2.begin();
  //  u8g2.setFont(u8g2_font_ncenB14_tr);
  //  u8g2.setCursor(0, 15);
  //  u8g2.print("Hello World!");

  ////////// Setup for OLED //////////


  ////////// Get Ambient level for MEMS Sound chip //////////

  ambientValue = get_ambient_MEMS(100) ; //initialize ambient value of sound to get baseline by averaging the first X values

  ////////// Get Ambient level for MEMS Sound chip  //////////
}

bool motion = false ;
int loopCount = 0 ;

uint32_t lum = tsl.getFullLuminosity();
uint16_t ir, full;

//////////// the loop routine runs over and over again forever://////////
void loop() {



  //String s = "Starting Up...." ;
  String s = String(loopCount) ;
  if (DEBUG) {

    Serial.print ("loopCount=");
    Serial.print (loopCount);
    Serial.print ("   string loopCount = ");
    Serial.print (s);
    Serial.print ("   s.c_str loopCount = ");
    Serial.println (s.c_str());
  }


  loopCount++;
  ////////// Temp/Hum Sesnor code //////////

  sensors_event_t humidity, temp;

  //  uint32_t timestamp = millis();
  sht4.getEvent(&humidity, &temp);// populate temp and humidity objects with fresh data
  //  timestamp = millis() - timestamp;

  long tempC = temp.temperature ;
  long tempF =  ((tempC / 5.0) * 9.0) + 32.0 ;
  long hum = humidity.relative_humidity ;
  if (DEBUG) {

    Serial.print("Temperature: "); Serial.print(tempC); Serial.println(" degrees C");
    Serial.print("Temperature: "); Serial.print(tempF); Serial.println(" degrees F");
    Serial.print("Humidity: "); Serial.print(hum); Serial.println("% rH");
  }
  ////////// Temp/Hum Sesnor code //////////

  ////////// PIR motion Sensor code //////////
  if (get_PIR_motion()) {
    motion = 1;
  }
  else
  {
    motion = 0 ;
  }
  ////////// PIR motion Sensor code //////////


  ////////// Light Sensor code //////////
  //simpleRead();
  //  advancedRead();
  // unifiedSensorAPIRead();
  {
    // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
    // That way you can do whatever math and comparisons you want!
    lum = tsl.getFullLuminosity();
    ir, full;
    ir = lum >> 16;
    full = lum & 0xFFFF;
    if (DEBUG) {
      //    Serial.print(F("[ "));
      //    Serial.print(millis());
      //    Serial.print(F(" ms ] "));
      Serial.print(F("IR: "));
      Serial.print(ir);
      Serial.print(F("  "));
      Serial.print(F("Full: "));
      Serial.print(full);
      Serial.print(F("  "));
      Serial.print(F("Visible: "));
      Serial.print(full - ir);
      Serial.print(F("  "));
      Serial.print(F("Lux: "));
      Serial.println(tsl.calculateLux(full, ir), 6);
    }
  }



  delay(500);

  ////////// Light Sensor code //////////


  int sensorValue = read_sound_MEMS();
  delta = abs(ambientValue - sensorValue) ;

  if (DEBUG) {
    // print out the value you read:
    if (DEBUG) {

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
    }
  }
  if (delta > max) {
    max = delta ;
  }
  if (delta < min) {
    min = delta ;
  }

  ////////// Code for OLED //////////
  // picture loop
  u8g2.firstPage();
  do {
    draw();
  } while ( u8g2.nextPage() );

  // increase the state
  draw_state++;
  //  if ( draw_state >= 14*8 )
  if ( draw_state >= 4 * 8 )
    draw_state = 0;

  // delay between each page
  //  delay(150);
  ////////// Code for OLED //////////


  ////////// print to OLED //////////

  u8g2.firstPage();
  do {

    u8g2.setCursor(0, 0);
    u8g2.print(u8x8_u8toa(loopCount, 3)); // Print Loop Count

    //    Print Temp
    u8g2.setCursor(0, 10);
    u8g2.print(u8x8_u8toa(tempC, 3)); // Print temp C

    u8g2.setCursor(20, 10);
    u8g2.print(u8x8_u8toa(tempF, 3)); // Print temp F

    u8g2.setCursor(40, 10);
    u8g2.print(u8x8_u8toa(hum, 3)); // Print humidity

    //    Print Light
    u8g2.setCursor(0, 20);
    u8g2.print(u8x8_u8toa(ir, 4)); // Print

    u8g2.setCursor(30, 20);
    u8g2.print(u8x8_u8toa(full, 4)); // Print

    u8g2.setCursor(60, 20);
    u8g2.print(u8x8_u8toa((full - ir), 4)); // Print

    u8g2.setCursor(90, 20);
    u8g2.print(u8x8_u8toa(tsl.calculateLux(full, ir), 6)); // Print


    //    Print Sound


  } while ( u8g2.nextPage() );

  ////////// print to OLED //////////

}
////////// End of Loop //////////


//////////  MEMS Sound chip Functions  //////////
int read_sound_MEMS () {
  // read the input on analog pin :
  int sensorValue = analogRead(SOUND_PIN);
  delay(10);        // delay in between reads for stability
  return (sensorValue) ;
}

int get_ambient_MEMS (int samples) {

  float sound_total = 0;
  int i ;

  for ( i = 0; i < samples; i++) {
    sound_total += analogRead(SOUND_PIN) ;
    delay(10);        // delay in between reads for stability
  }
  return (sound_total / i) ; // return average value
}
//////////  MEMS Sound chip Functions  //////////


//////////  PIR Motion sensor Functions  //////////

bool get_PIR_motion () {
  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {
      // we have just turned on
      if (DEBUG) {
        Serial.println("Motion detected!");
        return (true);
      }
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH) {
      // we have just turned of
      if (DEBUG) {
        Serial.println("Motion ended!");
        return (false);
      }
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }

}

//////////  PIR Motion sensor Functions  //////////

//////////  write text to OLED Functions  //////////

//void write_text_OLED( char *text) {
void write_text_OLED(int x, int y, char *text) {
  //  uint16_t offset = 5;
  //  u8g2.drawStr( 0, 0, "drawtext");

  if (DEBUG) {

    Serial.print ("==============  Length of OLED string ===== ") ;
    Serial.println (strlen(text));
  }
  u8g2.setFont(u8g2_font_5x7_tr );

  for  (int i = 0; i < 15; i++) {
    u8g2.setCursor((y - i), x);
    u8g2.print(" ");
  }
  u8g2.setCursor(y, x);
  u8g2.print(text);


}
//////////  write text to OLED Functions  //////////
