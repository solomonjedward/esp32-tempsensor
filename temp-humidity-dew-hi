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

// Min / max tracking since boot
float minTemp = 999, maxTemp = -999;
float minHum  = 999, maxHum  = -999;

// ---- Helper calculations ----

float computeDewPoint(float t, float rh) {
  const float a = 17.27;
  const float b = 237.7;
  float alpha = (a * t) / (b + t) + log(rh / 100.0);
  return (b * alpha) / (a - alpha);
}

float computeHeatIndex(float t, float rh) {
  float T = t * 9.0 / 5.0 + 32.0;
  float R = rh;
  float HI = 0.5 * (T + 61.0 + ((T - 68.0) * 1.2) + (R * 0.094));
  if (HI > 80.0) {
    HI = -42.379 + 2.04901523 * T + 10.14333127 * R
         - 0.22475541 * T * R - 0.00683783 * T * T
         - 0.05481717 * R * R + 0.00122874 * T * T * R
         + 0.00085282 * T * R * R - 0.00000199 * T * T * R * R;
  }
  return (HI - 32.0) * 5.0 / 9.0;
}

// Helper: print a string padded to a fixed width (overwrites leftovers)
void printPadded(int col, int row, const String &s, int width) {
  lcd.setCursor(col, row);
  lcd.print(s);
  for (int i = s.length(); i < width; i++) lcd.print(' ');
}

void setup() {
  Serial.begin(115200);

  Wire.begin(LCD_SDA, LCD_SCL);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("Initializing...");

  Wire1.begin(SHT_SDA, SHT_SCL);
  sht31 = Adafruit_SHT31(&Wire1);
  if (!sht31.begin(0x44)) {
    lcd.clear();
    lcd.print("SHT31 Error!");
    while (1);
  }

  // Static labels only — dynamic values will be overwritten each loop
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print("Temp: ");
  lcd.setCursor(0, 1); lcd.print("Humidity: ");
  lcd.setCursor(0, 2); lcd.print("T ");
  lcd.setCursor(0, 3); lcd.print("Dew: ");
  lcd.setCursor(11, 3); lcd.print("HI: ");
}

void loop() {
  float temp = sht31.readTemperature();
  float hum  = sht31.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    printPadded(0, 2, "Sensor Error!", 20);
    printPadded(0, 3, "Check Connection!", 20);
    delay(2000);
    return;
  }

  // --- Current readings ---
  // Row 0: value area is columns 6..19 (14 chars wide) — plenty of padding
  printPadded(6, 0, String(temp, 1) + " C", 14);

  // Row 1: value area is columns 10..19 (10 chars wide)
  printPadded(10, 1, String(hum, 1) + " %", 10);

  // --- Min / Max since boot ---
  if (temp < minTemp) minTemp = temp;
  if (temp > maxTemp) maxTemp = temp;
  if (hum  < minHum)  minHum  = hum;
  if (hum  > maxHum)  maxHum  = hum;

  // Row 2: T nn.n/nn.nC H nn/nn%  (up to ~24 chars, but 20 is what we have)
  String minmax = "T " + String(minTemp, 1) + "/" + String(maxTemp, 1) + "C"
                + " H " + String((int)minHum) + "/" + String((int)maxHum) + "%";
  // Truncate to 20 chars max just in case
  if (minmax.length() > 20) minmax = minmax.substring(0, 20);
  printPadded(0, 2, minmax, 20);

  // --- Dew point + Heat index ---
  float dew = computeDewPoint(temp, hum);
  float hi  = computeHeatIndex(temp, hum);

  // Row 3 layout: "Dew: nn.nC" at col 0, "HI: nn.nC" at col 11
  printPadded(5, 3, String(dew, 1) + "C", 6);   // cols 5..10
  printPadded(14, 3, String(hi, 1) + "C", 6);   // cols 14..19

  delay(2000);
}
