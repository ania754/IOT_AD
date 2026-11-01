#include <Wire.h>
#include <rgb_lcd.h>
#include <DHT.h>
#include <RTClib.h>
#include <WiFiS3.h>
#include <WiFiSSLClient.h>
#include <ArduinoMqttClient.h>
#include <WiFiUDP.h>
#include <NTPClient.h>

const char* NUMERO_PC = "42904";      // Votre numéro de PC
const char* ID_CAPTEUR = "IdO_Client_42904"; // L'ID de votre capteur
const char* mqtt_server = "192.168.1.254"; // IP du broker MQTT
const int mqtt_port = 1883;                 // Port sécurisé XCA

rgb_lcd lcd;
DHT dht(A0, DHT22);
RTC_Millis rtc;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "192.168.1.1", 3600);
WiFiSSLClient wifiClient;
MqttClient mqttClient(wifiClient);

int setup_mqtt() {
  mqttClient.setId(String(NUMERO_PC) + "_" + String(ID_CAPTEUR));
  mqttClient.setUsernamePassword("mos_user", "mos_user");
  if (!mqttClient.connect(mqtt_server, mqtt_port)) {
    return mqttClient.connectError();
    //return mqttClient.connect(mqtt_server, mqtt_port);
  }
  return 0; // Succès
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  lcd.begin(20, 4);
  lcd.setRGB(0, 255, 0);

  // Connexion Wi-Fi
  while (WiFi.begin("IoT IMT Nord Europe", "72Hin@R*") != WL_CONNECTED) {
    delay(4000);
  }

  // Synchro NTP
  timeClient.begin();
  if (timeClient.update()) rtc.adjust(DateTime(timeClient.getEpochTime()));
  else rtc.begin(DateTime(F(__DATE__), F(__TIME__)));

  // Test MQTT
  lcd.setCursor(0, 0);
  int res = setup_mqtt();
  lcd.print(res == 0 ? "MQTT OK (0)" : "Erreur MQTT: " + String(res));
  delay(2000);
  lcd.clear();
}

void loop() {
  timeClient.update();
  if (!mqttClient.connected()) setup_mqtt();

  char buf[] = "DD/MM/YYYY hh:mm:ss";
  rtc.now().toString(buf);
  lcd.setCursor(0, 0); lcd.print(buf);
  lcd.setCursor(0, 1); lcd.print("Temp: " + String(dht.readTemperature(), 1) + " C");
  lcd.setCursor(0, 2); lcd.print("Humi: " + String(dht.readHumidity(), 1) + " %");
  lcd.setCursor(0, 3); lcd.print(WiFi.status() == WL_CONNECTED ? "En ligne   " : "Deconnecte ");
  delay(1000);
}