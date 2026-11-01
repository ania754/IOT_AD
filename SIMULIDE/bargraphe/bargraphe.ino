#define PIN_LED_3 3
#define PIN_LED_4 4
#define PIN_LED_5 5
#define PIN_LED_6 6
#define PIN_LED_7 7
#define PIN_LED_8 8
#define PIN_LED_9 9
#define PIN_LED_10 10
#define PIN_LED_11 11
#define PIN_LED_12 12

void setup(){
	pinMode(PIN_LED_3, OUTPUT);
	pinMode(PIN_LED_4, OUTPUT);
	pinMode(PIN_LED_5, OUTPUT);
	pinMode(PIN_LED_6, OUTPUT);
	pinMode(PIN_LED_7, OUTPUT);
	pinMode(PIN_LED_8, OUTPUT);
	pinMode(PIN_LED_9, OUTPUT);	
	pinMode(PIN_LED_10, OUTPUT);
	pinMode(PIN_LED_11, OUTPUT);
	pinMode(PIN_LED_12, OUTPUT);
}

void loop(){
	digitalWrite(PIN_LED_3, HIGH);
	delay(100);
	digitalWrite(PIN_LED_3, LOW);
	delay(100);
	
	digitalWrite(PIN_LED_4, HIGH);
	delay(100);
	digitalWrite(PIN_LED_4, LOW);
	delay(100);
	
	digitalWrite(PIN_LED_5, HIGH);
	delay(100);
	digitalWrite(PIN_LED_5, LOW);
	delay(100);
	
	digitalWrite(PIN_LED_6, HIGH);
	delay(100);
	digitalWrite(PIN_LED_6, LOW);
	delay(100);
	
	digitalWrite(PIN_LED_7, HIGH);
	delay(100);
	digitalWrite(PIN_LED_7, LOW);
	delay(100);	
	
	digitalWrite(PIN_LED_8, HIGH);
	delay(100);
	digitalWrite(PIN_LED_8, LOW);
	delay(100);
	
	digitalWrite(PIN_LED_9, HIGH);
	delay(100);
	digitalWrite(PIN_LED_9, LOW);
	delay(100);
	
	digitalWrite(PIN_LED_10, HIGH);
	delay(100);
	digitalWrite(PIN_LED_10, LOW);
	delay(100);	
	
	digitalWrite(PIN_LED_11, HIGH);
	delay(100);
	digitalWrite(PIN_LED_11, LOW);
	delay(100);
	
	digitalWrite(PIN_LED_12, HIGH);
	delay(100);
	digitalWrite(PIN_LED_12, LOW);
	delay(100);
	
}