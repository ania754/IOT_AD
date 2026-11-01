#include <Wire.h>
#include <LiquidCrystal_AIP31068_I2C.h>
#include "DHT.h"

// Configuration du capteur DHT22 sur la broche A0[cite: 4]
#define DHTPIN A0
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Initialisation de l'écran LCD I2C avec la bibliothèque spécifique (16 colonnes, 2 lignes)
LiquidCrystal_AIP31068_I2C lcd(0x3E, 16, 2); // Remplace 0x27 par 0x3F si l'écran ne s'allume pas

void setup() {
  // Démarrage de l'écran et du capteur
  lcd.init();          
  dht.begin();
}

void loop() {
  // Lecture de l'humidité et de la température
  float humidite = dht.readHumidity();
  float temperature = dht.readTemperature();

 

  // Affichage de la température sur la première ligne
  lcd.setCursor(1, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print(" C   "); 

  // Affichage de l'humidité sur la deuxième ligne
  lcd.setCursor(8, 1);
  lcd.print("Hum:  ");
  lcd.print(humidite);
  lcd.print(" %   ");

  // Actualisation toutes les 2 secondes
  delay(2000);
}