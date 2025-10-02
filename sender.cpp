#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

const String symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_,.() ";
const char *message = "HELLO WORLD";

int getSymbolIndex(char c) {
  for (int i = 0; i < symbols.length(); i++) {
    if (symbols[i] == c) return i;
  }
  return symbols.length() - 1; // default to space
}

void send5BitWithFrame(char c) {
  int val = getSymbolIndex(c);

  Serial.write(0); // Start bit
  for (int i = 4; i >= 0; i--) {
    int bit = (val >> i) & 1;
    Serial.write(bit); // Data bits
  }
  Serial.write(1); // Stop bit
}

void print5Bit(char c) {
  int val = getSymbolIndex(c);
  for (int i = 4; i >= 0; i--) {
    lcd.print((val >> i) & 1);
  }
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.noBlink();
}

void loop() {
  for (int i = 0; message[i] != '\0'; i++) {
    char currentChar = message[i];

    send5BitWithFrame(currentChar);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
    lcd.setCursor(i, 0);
    lcd.cursor(); // static cursor

    lcd.setCursor(0, 1);
    print5Bit(currentChar);

    delay(1000); // slower transmission
  }
  while (1); // stop after sending
}