#include "DHT.h"

#define DHTPIN A0       // Capteur DHT22 branché sur A0
#define DHTTYPE DHT22   // Type de capteur

DHT dht(DHTPIN, DHTTYPE);

const int ledVerte = 3;   // LED verte sur la broche 3
const int ledRouge = 12;  // LED rouge sur la broche 12

void setup() {
  pinMode(ledVerte, OUTPUT);
  pinMode(ledRouge, OUTPUT);
  dht.begin();
}

void loop() {
  // 1. Lecture automatique de la température en cours
  float temperature = dht.readTemperature();

  // 2. Automatisation de la logique selon la température
  if (temperature < 20.0) {
    // Si inférieur à 20°C : Vert actif, Rouge éteint
    digitalWrite(ledVerte, HIGH);
    digitalWrite(ledRouge, LOW);
  } 
  else {
    // Si supérieur ou égal à 20°C : Vert éteint, Rouge actif
    digitalWrite(ledVerte, LOW);
    digitalWrite(ledRouge, HIGH);
  }

  // 3. Pause d'une seconde avant la prochaine actualisation automatique
  delay(1000);
}