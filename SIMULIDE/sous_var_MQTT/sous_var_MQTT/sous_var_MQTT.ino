#include <Wire.h>
#include <rgb_lcd.h>
#include <DHT.h>
#include <RTC.h>
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <ArduinoMqttClient.h>

#define TYPE_DHT DHT22
#define PIN_DHT22 A0
#define PIN_LED_VIE 2

// ---------- WiFi ----------
char ssid[] = "IoT IMT Nord Europe";   
char pass[] = "72Hin@R*";                 

// ---------- NTP ----------
const char* NTP_SERVER = "192.168.1.254";   
const long  UTC_OFFSET = 7200;             
const char* VERSION    = "V1.0";

// ---------- MQTT ----------
const char* MQTT_BROKER = "192.168.1.1";  // IP du broker
const int   MQTT_PORT   = 1883;
const char* MQTT_USER   = "mos_user";
const char* MQTT_PASS   = "mos_user";

// ---------- Identification ----------
const char* ID_PC   = "42902";
const char* GROUPE = "/iot/";
String capteurId;
String topicVersion, topicDate, topicTemp, topicHumi;
String topicCommand, topicBargraph, topicBlink;

// ---------- Objets ----------
rgb_lcd lcd;
DHT dht(PIN_DHT22, TYPE_DHT);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, UTC_OFFSET);
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// ---------- Variables ----------
uint8_t tick = 0;
float temp = 0;
float humi = 0;
bool wifiOk = false;
bool ntpOk = false;

// Variables pour la partie 6.1.5
int valeurBargraph = 0;
unsigned long periodeBlink = 1000; // Multiple de 100 ms

// =====================================================
// WiFi
// =====================================================
bool setup_wifi() {
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Module Wi-Fi introuvable !");
    return false;
  }

  WiFi.begin(ssid, pass);

  unsigned long debut = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - debut < 10000)) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("IP : ");
    Serial.println(WiFi.localIP());
    return true;
  }
  Serial.println("Echec connexion WiFi");
  return false;
}

// =====================================================
// NTP -> RTC
// =====================================================
void synchroNTP() {
  if (ntpOk || !wifiOk) return;
  if (timeClient.forceUpdate()) {
    RTCTime t;
    t.setUnixTime(timeClient.getEpochTime());
    RTC.setTime(t);
    ntpOk = true;
    Serial.println("RTC synchronisee par NTP");
  } else {
    Serial.println("NTP : pas de reponse");
  }
}

// =====================================================
// PARTIE 6.1.5 : Fonction de callback (Réception MQTT)
// =====================================================
void onMqttMessage(int messageSize) {
  // 1. Acquisition du topic
  String topic = mqttClient.messageTopic();
  
  // 2. Acquisition du message
  String payload = "";
  while (mqttClient.available()) {
    payload += (char)mqttClient.read();
  }
  
  // 3. Conversion du message en entier
  int valeurEntiere = payload.toInt();

  // 4. Traitement selon le topic reçu
  if (topic.indexOf("COMMAND") != -1) {
    Serial.print("Commande reçue : ");
    Serial.println(valeurEntiere);
    // Traiter le code commande ici si besoin
  } 
  else if (topic.indexOf("BARGRAPH") != -1) {
    valeurBargraph = valeurEntiere;
    Serial.print("Bargraph mis à jour : ");
    Serial.println(valeurBargraph);
  } 
  else if (topic.indexOf("BLINK") != -1) {
    // Sauvegarder la période (doit être un multiple de 100 ms)
    if (valeurEntiere >= 100) {
      periodeBlink = valeurEntiere;
      Serial.print("Période Blink mise à jour : ");
      Serial.println(periodeBlink);
    }
  }
}

// =====================================================
// MQTT : Initialisation, Connexion et Souscriptions (6.1.5 b)
// =====================================================
int setup_mqtt() {
  capteurId    = String("IdO_Client_") + ID_PC;
  topicVersion = String(GROUPE) + "VERSION_"     + ID_PC;  
  topicDate    = String(GROUPE) + "DATE_"        + ID_PC;  
  topicTemp    = String(GROUPE) + "TEMPERATURE_" + ID_PC;
  topicHumi    = String(GROUPE) + "HUMIDITE_"    + ID_PC;  

  // Topics de souscription (6.1.5)
  topicCommand  = String(GROUPE) + "COMMAND_"  + ID_PC;
  topicBargraph = String(GROUPE) + "BARGRAPH_" + ID_PC;
  topicBlink    = String(GROUPE) + "BLINK_"    + ID_PC;

  mqttClient.setId(capteurId);
  if (strlen(MQTT_USER) > 0) {
    mqttClient.setUsernamePassword(MQTT_USER, MQTT_PASS);
  }

  if (!mqttClient.connect(MQTT_BROKER, MQTT_PORT)) {
    int err = mqttClient.connectError();
    Serial.print("Echec MQTT, code erreur : ");
    Serial.println(err);
    return err;
  }

  // Souscription aux 3 topics demandés au paragraphe 3.2 / 6.1.5
  mqttClient.subscribe(topicCommand);
  mqttClient.subscribe(topicBargraph);
  mqttClient.subscribe(topicBlink);

  Serial.println("MQTT connecte et abonne aux topics !");
  return 0;
}

// =====================================================
// Publication des topics
// =====================================================
void publier(const String& topic, const String& valeur) {
  mqttClient.beginMessage(topic);
  mqttClient.print(valeur);
  mqttClient.endMessage();
}

void publierTopics() {
  if (!mqttClient.connected()) return;

  publier(topicVersion, VERSION);
  publier(topicTemp, String(temp, 1));
  publier(topicHumi, String(humi, 1));

  if (ntpOk) {   
    RTCTime now;
    RTC.getTime(now);
    char buf[21];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d %02d:%02d:%02d",
             now.getDayOfMonth(), Month2int(now.getMonth()), now.getYear(),
             now.getHour(), now.getMinutes(), now.getSeconds());
    publier(topicDate, String(buf));
  }
}

// =====================================================
// Affichage LCD
// =====================================================
void ecranAccueil() {
  RTCTime now;
  RTC.getTime(now);

  char l0[21];
  snprintf(l0, sizeof(l0), "%s  %02d/%02d/%04d", VERSION,
           now.getDayOfMonth(), Month2int(now.getMonth()), now.getYear());

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(l0);
  lcd.setCursor(0, 1);
  if (wifiOk) lcd.print(WiFi.localIP());
  else lcd.print("WiFi KO");
}

void ecranNormal() {
  RTCTime now;
  RTC.getTime(now);

  char l0[21];
  if (ntpOk) {
    snprintf(l0, sizeof(l0), "%02d/%02d/%04d %02d:%02d:%02d",
             now.getDayOfMonth(), Month2int(now.getMonth()), now.getYear(),
             now.getHour(), now.getMinutes(), now.getSeconds());
  } else {
    snprintf(l0, sizeof(l0), "NTP non synchronise");
  }
  lcd.setCursor(0, 0);
  lcd.print(l0);

  lcd.setCursor(0, 1);
  lcd.print("T ");
  lcd.print(temp, 1);
  lcd.print("oC/H ");
  lcd.print(humi, 1);
  lcd.print("%  ");
}

// =====================================================
// setup / loop
// =====================================================
void setup() {
  Serial.begin(9600);
  Serial.println("Demarrage...");
  pinMode(PIN_LED_VIE, OUTPUT);
  dht.begin();

  lcd.begin(20, 4);
  lcd.setRGB(255, 255, 255);

  RTC.begin();

  wifiOk = setup_wifi();
  if (wifiOk) {
    timeClient.begin();
    synchroNTP();           

    // a. Initialiser le callback sur la fonction onMqttMessage (6.1.5 b)
    mqttClient.onMessage(onMqttMessage);
    
    setup_mqtt();
  }

  ecranAccueil();
  delay(3000);
  lcd.clear();
}

void loop() {
  // Maintient la connexion et écoute les messages des topics souscrits
  mqttClient.poll();         

  // LED de vie
  digitalWrite(PIN_LED_VIE, HIGH);
  delay(250);
  digitalWrite(PIN_LED_VIE, LOW);
  delay(250);

  // Toutes les 2000 ms (1 tick sur 4)
  if (tick % 4 == 0) {
    temp = dht.readTemperature();
    humi = dht.readHumidity();
    Serial.print("Temperature : ");
    Serial.println(temp, 1);
    Serial.print("Humidite : ");
    Serial.println(humi, 1);

    synchroNTP();

    static uint8_t cptMqtt = 0;
    if (wifiOk && !mqttClient.connected()) {
      cptMqtt++;
      if (cptMqtt >= 5) {
        cptMqtt = 0;
        setup_mqtt();
      }
    }

    publierTopics();         
  }

  // Affichage toutes les secondes
  if (tick % 2 == 0) {
    ecranNormal();
  }

  tick++;
  if (tick >= 4) tick = 0;
}