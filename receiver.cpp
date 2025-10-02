#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

const String symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_,.() ";
char message[16];
int index = 0;

int read5BitWithFrame() {
  // Wait for start bit
  while (Serial.available() == 0);
  int startBit = Serial.read();
  if (startBit != 0) return -1; // error, ignore

  int val = 0;
  for (int i = 0; i < 5; i++) {
    while (Serial.available() == 0); // wait for data bit
    int bit = Serial.read();
    val = (val << 1) | (bit & 1);
  }

  // Wait for stop bit
  while (Serial.available() == 0);
  int stopBit = Serial.read();
  if (stopBit != 1) return -1; // error, ignore

  return val;
}

void print5Bit(int val) {
  for (int i = 4; i >= 0; i--) {
    lcd.print((val >> i) & 1);
  }
}

void setup() {
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.noBlink();
  message[0] = '\0';
}

void loop() {
  int val = read5BitWithFrame();
  if (val >= 0 && index < 15) {
    message[index++] = symbols[val];
    message[index] = '\0';
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.setCursor(index, 0);
  lcd.cursor(); // static cursor

  if (index > 0) {
    lcd.setCursor(0, 1);
    print5Bit(val); // 5-bit of last character
  }
}