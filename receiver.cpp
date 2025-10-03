#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x20, 16, 2);

const String symbols = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_,.() ";

const int ledPin = 13;
const int MAX_LEN = 64;   // safe buffer size
char message[MAX_LEN];
int index = 0;

// Read framed symbol (Start, Data, Stop)
int read5BitWithFrame() {
  while (Serial.available() == 0);
  int startBit = Serial.read();
  if (startBit != 0) return -1;

  int val = 0;
  for (int i = 0; i < 5; i++) {
    while (Serial.available() == 0);
    int bit = Serial.read();
    val = (val << 1) | (bit & 1);
  }

  while (Serial.available() == 0);
  int stopBit = Serial.read();
  if (stopBit != 1) return -1;

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

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  message[0] = '\0';
}

void loop() {
  if (Serial.available()) {
    // Look if it's a command
    if (Serial.peek() == '/') {
      String cmd = Serial.readStringUntil('\n');
      if (cmd == "/b") {
        index = 0;
        message[0] = '\0';
        digitalWrite(ledPin, LOW);   // Reset LED at start
      }
      else if (cmd == "/s") {
        digitalWrite(ledPin, HIGH);  // Turn LED ON when done
      }
    }
    else {
      // Otherwise framed symbol
      int val = read5BitWithFrame();
      if (val >= 0 && index < MAX_LEN - 1) {  // prevent overflow
        message[index++] = symbols[val];
        message[index] = '\0';  // keep null-terminated string

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(message);     // top line: received message
        lcd.setCursor(index, 0);
        lcd.cursor();

        lcd.setCursor(0, 1);
        print5Bit(val);         // bottom line: binary
      }
    }
  }
}
