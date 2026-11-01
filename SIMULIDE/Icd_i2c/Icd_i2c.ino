// Section 3.1.4 - LCD

#include <LiquidCrystal_AIP31068_I2C.h>

// Adresse I2C : 0x27, 16 colonnes, 2 lignes
LiquidCrystal_AIP31068_I2C lcd(0x3E, 16, 2);

void setup() {
  lcd.init();       // Initialisation du LCD 

  // PremiÃ¨re ligne 
  lcd.setCursor(1, 0); 
  lcd.print("HELLO");

  // DeuxiÃ¨me ligne 
  lcd.setCursor(8, 1); 
  lcd.print("WORLD");
}

void loop() {
  
}