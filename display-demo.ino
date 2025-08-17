//This will display a demo output in the display connected 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define I2C_SCL 27  // Custom SCL pin
#define I2C_SDA 26  // Custom SDA pin

LiquidCrystal_I2C MyLCD(0x27, 20, 4);  // Address 0x27, 20 columns, 4 rows

void setup() {
  Serial.begin(115200);  // For debugging
  Wire.begin(I2C_SDA, I2C_SCL);  // Initialize I2C with custom pins

  // Check if LCD is connected
  Wire.beginTransmission(0x27);
  if (Wire.endTransmission() != 0) {
    Serial.println("LCD not found at 0x27! Check wiring or address.");
    while (1);  // Halt if LCD is missing
  }

  MyLCD.init();          // Initialize LCD
  MyLCD.backlight();     // Turn on backlight
  MyLCD.clear();         // Clear the display

  // Display messages
  MyLCD.setCursor(0, 0);
  MyLCD.print("    Hello World!");
  MyLCD.setCursor(0, 1);
  MyLCD.print("    I2C LCD 20x4");
  MyLCD.setCursor(0, 2);
  MyLCD.print("  Arduino Tutorial");
  MyLCD.setCursor(0, 3);
  MyLCD.print("  DeepBlueMbedded");
}

void loop() {
  // Nothing here (static display)
}
