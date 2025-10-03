#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

// Custom 5-bit symbol table
const String symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_,.() ";

// Change message here (any length up to buffer)
const char *message = "DEAR EVAN HANSEN";

// Find index of character in the symbols table
int getSymbolIndex(char c) {
  for (int i = 0; i < symbols.length(); i++) {
    if (symbols[i] == c) return i;
  }
  return symbols.length() - 1;  // Default to space if not found
}

// Send one symbol as Start(0) + 5 bits + Stop(1)
void send5BitWithFrame(char c) {
  int val = getSymbolIndex(c);

  Serial.write(0);                   // Start bit
  for (int i = 4; i >= 0; i--) {
    int bit = (val >> i) & 1;
    Serial.write(bit);
  }
  Serial.write(1);                   // Stop bit
}

// Print binary of symbol on LCD
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
  Serial.println("/b");  // Begin transmission

  for (int i = 0; message[i] != '\0'; i++) {
    char currentChar = message[i];

    send5BitWithFrame(currentChar);  // Send framed bits

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(message);
    lcd.setCursor(i, 0);
    lcd.cursor();

    lcd.setCursor(0, 1);
    print5Bit(currentChar);

    delay(1000);  // Slow down for visibility
  }

  Serial.println("/s");  // End transmission
  while (1);             // Stop loop
}
