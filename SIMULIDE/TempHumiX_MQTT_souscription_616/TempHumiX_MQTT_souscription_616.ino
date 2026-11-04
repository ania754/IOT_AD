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

// ---------- Bargraphe : 10 LED sur D3 à D12 ----------
#define NB_LED_BAR 10
const uint8_t PINS_BAR[NB_LED_BAR] = {3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
#define LED_ON  HIGH 
#define LED_OFF LOW 
#define TEST_BARGRAPH 0 

// ---------- WiFi ----------
char ssid[] = "IoT IMT Nord Europe";
char pass[] = "72Hin@R*";

// ---------- NTP ----------
const char* NTP_SERVER = "192.168.1.254";
const long  UTC_OFFSET = 3600; 
const char* VERSION    = "1.0";

// ---------- MQTT ----------
const char* MQTT_BROKER = "192.168.1.1";
const int   MQTT_PORT   = 1883;
const char* MQTT_USER   = "mos_user";
const char* MQTT_PASS   = "mos_user";

// ---------- Identification ----------
const char* ID_PC  = "42904"; // Mets ton vrai numéro d'étiquette ici !
const char* GROUPE = "/iot/";
String idUnique; 
String capteurId;
String topicVersion, topicTemp, topicHumi, topicDate;
String topicCommand, topicBargraph, topicBlink;
// ---------- Objets ----------
rgb_lcd lcd;
DHT dht(PIN_DHT22, TYPE_DHT);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_SERVER, UTC_OFFSET);
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// ---------- Variables ----------
uint8_t tick = 0;                          // 0..19 (cycle de 2 s, base 100 ms)
float temp = 0;
float humi = 0;
bool wifiOk = false;
bool ntpOk = false;
float lastTemp = -999;
float lastHumi = -999;

bool mesureActive = true;                  // commandes 20 / 30
unsigned long periodeBlink = 500;          // période courante de la LED de vie (ms)
unsigned long periodePerso = 500;          // dernière valeur reçue sur BLINK_
bool blinkPerso = false;                   // true si la commande 60 est active
bool cmdSyncNTP = false;                   // demandée par la commande 10

// =====================================================
// 3.2 Identifiant unique : 4 octets de poids faible de la MAC
// =====================================================
void lireIdentifiant() {
  uint8_t mac[6];
  WiFi.macAddress(mac);

  // Sur UNO R4 WiFi (WiFiS3) : mac[5] = 1er octet de la MAC ... mac[0] = dernier
  char full[18];
  snprintf(full, sizeof(full), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

  char buf[9];
  snprintf(buf, sizeof(buf), "%02X%02X%02X%02X",
           mac[3], mac[2], mac[1], mac[0]);
  idUnique = String(buf);

  Serial.print("MAC complete : ");
  Serial.println(full);
  Serial.print("ID unique    : ");
  Serial.println(idUnique);
}

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
// Bargraphe : 0..1023 -> 0..10 LED allumées de D3 à D12
// =====================================================
void ecrireBargraph(int valeur) {
  if (valeur < 0) valeur = 0;
  if (valeur > 1023) valeur = 1023;
  int nb = (int)(((long)valeur * NB_LED_BAR + 511) / 1023);

  Serial.print("Bargraphe : ");
  Serial.print(valeur);
  Serial.print(" -> ");
  Serial.print(nb);
  Serial.println(" LED");

  for (int i = 0; i < NB_LED_BAR; i++) {
    digitalWrite(PINS_BAR[i], (valeur >> i) & 1);
  }
}

// Test : allume les LED une par une
void testBargraph() {
  Serial.println("=== TEST BARGRAPHE ===");
  for (int i = 0; i < NB_LED_BAR; i++) {
    Serial.print("Broche D");
    Serial.println(PINS_BAR[i]);
    digitalWrite(PINS_BAR[i], LED_ON);
    delay(500);
  }
  for (int i = 0; i < NB_LED_BAR; i++) digitalWrite(PINS_BAR[i], LED_OFF);
  Serial.println("=== FIN TEST ===");
}

// =====================================================
// 6.1.5 a) Réception d'un message MQTT
// =====================================================
void onMqttMessage(int tailleMessage) {
  String topic = mqttClient.messageTopic();

  String msg = "";
  while (mqttClient.available()) {
    msg += (char)mqttClient.read();
  }
  msg.trim();

  int valeur = msg.toInt();

  Serial.print("Recu ");
  Serial.print(topic);
  Serial.print(" = '");
  Serial.print(msg);
  Serial.print("' -> ");
  Serial.println(valeur);

  if (topic.indexOf("COMMAND") >= 0) {
    switch (valeur) {
      case 10: cmdSyncNTP = true;  break;
      case 20: mesureActive = true;  break;
      case 30: mesureActive = false; break;
      case 40: periodeBlink = 1000; blinkPerso = false; break;
      case 50: periodeBlink = 200;  blinkPerso = false; break;
      case 60: periodeBlink = periodePerso; blinkPerso = true; break;
      default:
        Serial.println("Commande inconnue");
        break;
    }
  }
  else if (topic.indexOf("BARGRAPH") >= 0) {
    ecrireBargraph(valeur);
  }
  else if (topic.indexOf("BLINK") >= 0) {
    long p = (valeur / 100) * 100; // Correction de la multiplication
    if (p < 100) p = 100;
    periodePerso = p;
    if (blinkPerso) periodeBlink = p;
  }
}

// =====================================================
// MQTT : retourne 0 si OK, sinon le code erreur
// =====================================================
int setup_mqtt() {
  capteurId     = String("IdO_Client_") + idUnique;
  topicVersion  = String(GROUPE) + "VERSION_"     + ID_PC;
  topicTemp     = String(GROUPE) + "TEMPERATURE_" + ID_PC;
  topicHumi     = String(GROUPE) + "HUMIDITY_"    + ID_PC;
  topicDate     = String(GROUPE) + "GET_DATE_"    + ID_PC;
  topicCommand  = String(GROUPE) + "COMMAND_"     + ID_PC;
  topicBargraph = String(GROUPE) + "BARGRAPH_"    + ID_PC;
  topicBlink    = String(GROUPE) + "BLINK_"       + ID_PC;

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
  Serial.println("MQTT connecte");

  // 6.1.5 b) callback + souscription aux 3 topics
  mqttClient.onMessage(onMqttMessage);
  mqttClient.subscribe(topicCommand);
  mqttClient.subscribe(topicBargraph);
  mqttClient.subscribe(topicBlink);

  return 0;
}

// =====================================================
// Publication
// =====================================================
void publier(const String& topic, const String& valeur) {
  mqttClient.beginMessage(topic);
  mqttClient.print(valeur);
  mqttClient.endMessage();
}

void publierTopics() {
  if (!mqttClient.connected() || !mesureActive) return;

  // 6.1.6 b) Publie seulement si T ou H a changé (au dixième)
  if (lroundf(temp * 10) != lroundf(lastTemp * 10) ||
      lroundf(humi * 10) != lroundf(lastHumi * 10)) {
    lastTemp = temp;
    lastHumi = humi;

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
}

// =====================================================
// Affichage LCD
// =====================================================
void ecranAccueil() {
  RTCTime now;
  RTC.getTime(now);
  char l0[21];
  snprintf(l0, sizeof(l0), "V%s  %02d/%02d/%04d", VERSION,
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
  lcd.print("%   ");
}

// =====================================================
// setup / loop
// =====================================================
void setup() {
  Serial.begin(9600);
  Serial.println("Demarrage... (version bargraphe 10 LED D3-D12)");

  pinMode(PIN_LED_VIE, OUTPUT);
  for (int i = 0; i < NB_LED_BAR; i++) {
    pinMode(PINS_BAR[i], OUTPUT);
    digitalWrite(PINS_BAR[i], LED_OFF);
  }

#if TEST_BARGRAPH
  testBargraph();
#endif

  dht.begin();

  lcd.begin(20, 4);
  lcd.setRGB(255, 255, 255);

  RTC.begin();

  wifiOk = setup_wifi();
  lireIdentifiant();
  if (wifiOk) {
    timeClient.begin();
    synchroNTP();
    setup_mqtt();
  }

  ecranAccueil();
  delay(3000);
  lcd.clear();
}

void loop() {
  mqttClient.poll();                       // reçoit les messages + keep-alive

  // LED de vie : allumée la moitié de la période
  digitalWrite(PIN_LED_VIE, (millis() % periodeBlink) < (periodeBlink / 2));

  // Commande 10 : resynchronisation NTP, puis publication de la version (6.1.6 a)
  if (cmdSyncNTP) {
    cmdSyncNTP = false;
    ntpOk = false;
    synchroNTP();
    if (ntpOk && mqttClient.connected()) {
      publier(topicVersion, VERSION);
    }
  }

  // Toutes les 2 s (20 ticks de 100 ms)
  if (tick == 0) {
    if (mesureActive) {
      float t = dht.readTemperature();
      float h = dht.readHumidity();
      if (!isnan(t) && !isnan(h)) {
        temp = t;
        humi = h;
      }
      Serial.print("Temperature : ");
      Serial.println(temp, 1);
      Serial.print("Humidite : ");
      Serial.println(humi, 1);
    }

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

  // Affichage toutes les secondes (10 ticks)
  if (tick % 10 == 0) {
    ecranNormal();
  }

  tick++;
  if (tick >= 20) tick = 0;
  delay(100);
}