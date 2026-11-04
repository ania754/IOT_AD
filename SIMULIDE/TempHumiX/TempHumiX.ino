#include <LiquidCrystal_AIP31068_I2C.h>
#include <DHT.h>
#include <RTClib.h>

#define TYPE_DHT DHT22
#define PIN_DHT22 A0
#define PIN_LED_VIE 2

LiquidCrystal_AIP31068_I2C lcd(0x3E, 20, 4);
DHT dht(PIN_DHT22, TYPE_DHT);
RTC_Millis rtc;

int compteur_vie = 0;
int compteur_lecture = 40;
float temp = 0;
float humi = 0;

void setup()
{
  pinMode(PIN_LED_VIE, OUTPUT);
  dht.begin();
  lcd.init();
  rtc.begin(DateTime(F(__DATE__), F(__TIME__)));
  Serial.begin(9600);
}

void loop()
{
  DateTime now = rtc.now();

  // Date et heure sur la ligne 0
  char buf[] = "DD/MM/YYYY hh:mm:ss";
  now.toString(buf);
  lcd.setCursor(0, 0);
  lcd.print(buf);

  if (compteur_lecture == 40) {
    temp = dht.readTemperature();
    humi = dht.readHumidity();
	Serial.print("Temperature : ");
	Serial.println(temp, 1);
	Serial.print("Humidite : ");
	Serial.println(humi, 1);
	Serial.println(buf);
    compteur_lecture = 0;
  }

  // LED de vie
  if (compteur_vie == 5) {
    digitalWrite(PIN_LED_VIE, HIGH);
  }
  if (compteur_vie == 10) {
    digitalWrite(PIN_LED_VIE, LOW);
    compteur_vie = 0;
  }

  // Température et humidité sur la ligne 1
  lcd.setCursor(0, 1);
  lcd.print("T ");
  lcd.print(temp, 1);
  lcd.print("oC/H ");
  lcd.print(humi, 1);
  lcd.print("% ");

  delay(50);
  compteur_vie++;
  compteur_lecture++;
}