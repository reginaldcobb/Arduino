/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
#include "esp_camera.h"
#include "ESP.h"

#include <EEPROM.h>            // read and write from flash memo
#define EEPROM_SIZE 1

//* ESP32CAM - PIR
//* GPIO13 - OUT
gpio_num_t pirPin = GPIO_NUM_13; // signal pin of the PIR sensor



const char* ssid = "PBellAire1";
const char* password = "miltonflorida1";

String serverName = "192.168.0.208";   // REPLACE WITH YOUR Unbuntu Server (milton)
//String serverName = "192.168.0.223";   // REPLACE WITH YOUR Raspberry Pi IP ADDRESS
//String serverName = "192.168.1.XXX";   // REPLACE WITH YOUR Raspberry Pi IP ADDRESS
//String serverName = "example.com";   // OR REPLACE WITH YOUR DOMAIN NAME

String serverPath = "/upload.php";     // The default serverPath should be upload.php

const int serverPort = 80;

WiFiClient client;

// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

const int timerInterval = 30000;    // time between each HTTP POST image
unsigned long previousMillis = 0;   // last time image was sent


//////////   Procedures for NTP    /////////

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);


String formattedDate;
String dayStamp;
String timeStamp;


//////////   Procedures for NTP    /////////


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("ESP32-CAM IP Address: ");
  Serial.println(WiFi.localIP());



  //////////   Setup for NTP    /////////
  // Initialize a NTPClient to get time
  timeClient.begin();
  // Set offset time in seconds to adjust for your timezone, for example:
  // GMT +1 = 3600
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  //  timeClient.setTimeOffset(3600);

  //////////   Setup for NTP    /////////

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // init with high specs to pre-allocate larger buffers
  if (psramFound()) {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 40;  //0-63 lower number means higher quality
//    config.jpeg_quality = 10;  //0-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_CIF;
    config.jpeg_quality = 50;  //0-63 lower number means higher quality
//    config.jpeg_quality = 12;  //0-63 lower number means higher quality
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    delay(1000);
    ESP.restart();
  }

  sendPhoto();


  // Turns off the ESP32-CAM white on-board LED (flash) connected to GPIO 4

  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  digitalWrite(4, HIGH);  // flash white led
  delay(500);
  digitalWrite(4, LOW);
  //  rtc_gpio_hold_en(GPIO_NUM_4);

  //  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, 1); //1 = High, 0 = Low
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_12, RISING); //1 = High, 0 = Low

  // PIR sensor
  //  esp_sleep_enable_ext0_wakeup(pirPin, RISING);

  delay(2000);
  Serial.println("Going to sleep now");
  delay(3000);
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= timerInterval) {
    sendPhoto();
    previousMillis = currentMillis;
  }
}

void array_to_string(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}


String sendPhoto() {
  String getAll;
  String getBody;

  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }

  ///// Debug   ////////
  uint8_t chipid[6];
  esp_efuse_read_mac(chipid);
  Serial.print(" Chip ID = ");
  //    Serial.println( String(chipid)).c_str() );
  Serial.printf("%X\n", chipid);
  Serial.print(" MAC Address info= ");
  Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", chipid[0], chipid[1], chipid[2], chipid[3], chipid[4], chipid[5]);

  Serial.print("ESP Board MAC Address:  ");
  Serial.println(WiFi.macAddress());


  /// NTP Code
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
   unsigned long epoch = timeClient.getEpochTime ();
  Serial.println();
  Serial.print("Epoch time = ");
  Serial.println(epoch);
//  Serial.println(timeClient.getEpochTime ());
  Serial.println();

  /// NTP Code

  ///// Debug   ////////

  Serial.println("Connecting to server: " + serverName);

  if (client.connect(serverName.c_str(), serverPort)) {
    Serial.println("Connection successful!");
    //    String head1 = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam" ;
    String head1 = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"" ;
    char str[32] = "";
    array_to_string(chipid, 6, str);
    //    Serial.println(str);

    String head4 = str;
    String head3 = head1 + head4 ;
    String head2 =  ".jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String head = head1 + str + "__" +  String(epoch).c_str() + head2 ;

    //    String head = "--RandomNerdTutorials\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"esp32-cam.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--RandomNerdTutorials--\r\n";

    uint32_t imageLen = fb->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;

    client.println("POST " + serverPath + " HTTP/1.1");
    client.println("Host: " + serverName);
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    client.println();
    client.print(head);

    ///// Debug   ////////
    //    Serial.print("head1 = ");
    //    Serial.println(head1);
    //
    //    Serial.print("head2 = ");
    //    Serial.println(head2);
    //
    //    Serial.print("head3 = ");
    //    Serial.println(head3);
    //
    //    Serial.print("head4 = ");
    //    Serial.println(head4);
    //
    //    Serial.print("head = ");
    //    Serial.println(head);
    //
    //    Serial.print("tail = ");
    //    Serial.println(tail);

    Serial.println("POST " + serverPath + " HTTP/1.1");
    Serial.println("Host: " + serverName);
    Serial.println("Content-Length: " + String(totalLen));
    Serial.println("Content-Type: multipart/form-data; boundary=RandomNerdTutorials");
    Serial.println();
    Serial.print(head);
    Serial.println();

    //
    //    uint8_t chipid[6];
    //    esp_efuse_read_mac(chipid);
    //    Serial.print(" Chip ID = ");
    //    //    Serial.println( String(chipid)).c_str() );
    //    Serial.printf("%X\n", chipid);
    //    Serial.print(" MAC Address info= ");
    //    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n", chipid[0], chipid[1], chipid[2], chipid[3], chipid[4], chipid[5]);
    //
    //    Serial.print("ESP Board MAC Address:  ");
    //    Serial.println(WiFi.macAddress());

    ///// Debug   ////////

    uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    for (size_t n = 0; n < fbLen; n = n + 1024) {
      if (n + 1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      }
      else if (fbLen % 1024 > 0) {
        size_t remainder = fbLen % 1024;
        client.write(fbBuf, remainder);
      }
    }
    client.print(tail);

    esp_camera_fb_return(fb);

    int timoutTimer = 10000;
    long startTimer = millis();
    boolean state = false;

    while ((startTimer + timoutTimer) > millis()) {
      Serial.print(".");
      delay(100);
      while (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (getAll.length() == 0) {
            state = true;
          }
          getAll = "";
        }
        else if (c != '\r') {
          getAll += String(c);
        }
        if (state == true) {
          getBody += String(c);
        }
        startTimer = millis();
      }
      if (getBody.length() > 0) {
        break;
      }
    }
    Serial.println();
    client.stop();
    Serial.println(getBody);
  }
  else {
    getBody = "Connection to " + serverName +  " failed.";
    Serial.println(getBody);
  }
  return getBody;
}
