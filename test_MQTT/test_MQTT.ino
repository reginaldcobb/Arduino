#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define wifi_ssid "PBellAire1"
#define wifi_password "miltonflorida1"

#define mqtt_server "192.168.0.223"
#define mqtt_user "mosquitto"
#define mqtt_password "n219022"

#define temperature_topic "sensor/temperature1"  //Office
#define humidity_topic "sensor/humidity1"

char message_buff[100];

long lastMsg = 0;   
long lastRecu = 0;
bool debug = false;  //Display log message if True
   
WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);     
  setup_wifi();
  client.setServer(mqtt_server, 1883);

}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi OK ");
  Serial.print("=> ESP8266 IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {

  while (!client.connected()) {
    Serial.print("Connecting to MQTT broker ...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("OK");
    } else {
      Serial.print("Error : ");
      Serial.print(client.state());
      Serial.println(" Wait 5 seconds before retry");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
   }
  client.loop();

  long now = millis();
  if (now - lastMsg > 1000 * 60) {
    lastMsg = now;
    int h = 44;
    int t = 55;
     
    if (debug) {
      Serial.print("Temperature : ");
      Serial.print(t);
      Serial.print(" | Humidity : ");
      Serial.println(h);
    }
    digitalWrite(2,HIGH); 
    client.publish(temperature_topic, String(t).c_str(), true);   // Publish temperature
    client.publish(humidity_topic, String(h).c_str(), true);      // Publish humidity
    delay(2000);
    digitalWrite(2,LOW);
  }
}
