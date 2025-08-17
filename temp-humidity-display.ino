//This shows the temprature and humidity outout to display

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_SHT31.h"

// LCD (First I2C Bus: Wire)
#define LCD_SDA 26
#define LCD_SCL 27
LiquidCrystal_I2C lcd(0x27, 20, 4);

// SHT31 (Second I2C Bus)
#define SHT_SDA 21
#define SHT_SCL 22
Adafruit_SHT31 sht31;

void setup() {
  Serial.begin(115200);

  // Initialize LCD
  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");

  // Initialize SHT31 on second I2C bus
  Wire1.begin(SHT_SDA, SHT_SCL);
  sht31 = Adafruit_SHT31(&Wire1);  // Create instance with Wire1
  if (!sht31.begin(0x44)) {
    lcd.clear();
    lcd.print("SHT31 Error!");
    while (1);
  }

  lcd.clear();
  lcd.print("Temp:    --.- ");
  lcd.setCursor(0, 1);
  lcd.print("Humidity:  -- %");
}

void loop() {
  float temp = sht31.readTemperature();
  float hum = sht31.readHumidity();

  if (!isnan(temp) && !isnan(hum)) {
    lcd.setCursor(7, 0);
    lcd.print(String(temp, 1) + " C");
    lcd.setCursor(10, 1);
    lcd.print(String(hum, 1) + " %");
  } else {
    lcd.setCursor(0, 3);
    lcd.print("Sensor Error!");
  }
  delay(2000);
}
