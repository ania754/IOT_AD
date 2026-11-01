#include <Wire.h>
#include <rgb_lcd.h>
#include <DHT.h>
#include <RTClib.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Définitions des broches et types
#define TYPE_DHT DHT22
#define PIN_DHT22 A0
#define PIN_LED_VIE 2

// Identifiants Wi-Fi
char ssid[] = "IoT IMT Nord Europe"; 
char pass[] = "72Hin@R*"; 
int status = WL_IDLE_STATUS;

// Configuration du client NTP
WiFiUDP ntpUDP;;
// On indique l'adresse IP de votre serveur NTP (192.168.1.254) et le décalage en secondes (ex: 3600 pour UTC+1)
NTPClient timeClient(ntpUDP, "192.168.1.254", 3600);

// Objets
rgb_lcd lcd;
DHT dht(PIN_DHT22, TYPE_DHT);
RTC_Millis rtc;

int compteur_vie = 0;
int compteur_lecture = 40;
float temp = 0;
float humi = 0;

void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_LED_VIE, OUTPUT);
  dht.begin();
  
  lcd.begin(20, 4);
  lcd.setRGB(0, 255, 0); // Vert
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Connexion WiFi...");

  // Connexion au réseau Wi-Fi
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connecte !");
  delay(1500);

  // Démarrage du client NTP
  timeClient.begin();
  
  // Première mise à jour de l'heure depuis votre serveur NTP (192.168.1.254)
  if (timeClient.update()) {
    rtc.adjust(DateTime(timeClient.getEpochTime()));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("NTP Synchro OK !");
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Echec NTP, RTC base");
    rtc.begin(DateTime(F(__DATE__), F(__TIME__))); // Fallback
  }
  
  delay(2000);
  lcd.clear();
}

void loop() {
  // Mise à jour régulière de l'heure via le serveur NTP (tous les ticks de boucle, la lib gère l'intervalle)
  timeClient.update();
  
  // On synchronise notre RTC interne avec l'heure fraîche du NTP
  rtc.adjust(DateTime(timeClient.getEpochTime()));

  DateTime now = rtc.now();

  // 1. Affichage de la date et l'heure sur la ligne 0
  char buf[] = "DD/MM/YYYY hh:mm:ss";
  now.toString(buf);
  lcd.setCursor(0, 0);
  lcd.print(buf);

  // 2. Lecture du capteur DHT22 toutes les 40 boucles
  if (compteur_lecture >= 40) {
    temp = dht.readTemperature();
    humi = dht.readHumidity();
    compteur_lecture = 0;
  }

  // 3. Gestion de la LED de vie
  if (compteur_vie == 5) {
    digitalWrite(PIN_LED_VIE, HIGH);
  }
  if (compteur_vie >= 10) {
    digitalWrite(PIN_LED_VIE, LOW);
    compteur_vie = 0;
  }

  // 4. Affichage de la température et de l'humidité
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temp, 1);
  lcd.print(" C   ");

  lcd.setCursor(0, 2);
  lcd.print("Humi: ");
  lcd.print(humi, 1);
  lcd.print(" %   ");

  // Ligne 3 : Statut Wi-Fi
  lcd.setCursor(0, 3);
  if (WiFi.status() == WL_CONNECTED) {
    lcd.print("Statut: En ligne   ");
  } else {
    lcd.print("Statut: Deconnecte ");
  }

  delay(50);
  compteur_vie++;
  compteur_lecture++;
}