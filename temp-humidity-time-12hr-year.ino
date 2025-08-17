//Display date time in PM with year on display with temperature and humidity

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Adafruit_SHT31.h"
#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// LCD (First I2C Bus: Wire)
#define LCD_SDA 26
#define LCD_SCL 27
LiquidCrystal_I2C lcd(0x27, 20, 4);

// SHT31 (Second I2C Bus)
#define SHT_SDA 21
#define SHT_SCL 22
Adafruit_SHT31 sht31;

// WiFi and NTP
const char* ssid = "User-name";
const char* password = "Password";
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800);

// Time variables
unsigned long lastSync = 0;
unsigned long syncAttempts = 0;
const unsigned long syncInterval = 3600000; // 1 hour in ms
const unsigned long recheckInterval = 30000; // 30 seconds for rechecks
const char* monthAbbr[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", 
                             "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char* dayNames[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

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
  sht31 = Adafruit_SHT31(&Wire1);
  if (!sht31.begin(0x44)) {
    lcd.clear();
    lcd.print("SHT31 Error!");
    while (1);
  }

  // Setup initial display
  lcd.clear();
  lcd.print("Temp:    --.- ");
  lcd.setCursor(0, 1);
  lcd.print("Humidity:  -- %");
  lcd.setCursor(0, 2);
  lcd.print("Day Time: --:-- --");
  lcd.setCursor(0, 3);
  lcd.print("Date: --- --, ----");
}

void connectWiFi() {
  lcd.setCursor(0, 3);
  lcd.print("Connecting WiFi...");
  
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 10) {
    delay(500);
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 3);
    lcd.print("WiFi Failed!       ");
    return;
  }

  timeClient.begin();
  if (timeClient.update()) {
    lastSync = millis();
    syncAttempts++;
    lcd.setCursor(0, 3);
    lcd.print("Time synced!      ");
  } else {
    lcd.setCursor(0, 3);
    lcd.print("NTP Failed!       ");
  }
  
  delay(1000);
  WiFi.disconnect(true);
}

void updateTimeDisplay() {
  if (lastSync == 0) return;
  
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  
  // Convert to 12-hour format with AM/PM
  int hour12 = ptm->tm_hour % 12;
  hour12 = hour12 ? hour12 : 12; // Convert 0 to 12
  const char* ampm = ptm->tm_hour < 12 ? "AM" : "PM";
  
  // Create time string
  char timeStr[20];
  snprintf(timeStr, sizeof(timeStr), "%s %d:%02d %s", 
           dayNames[ptm->tm_wday], hour12, ptm->tm_min, ampm);
  
  // Create date string
  char dateStr[20];
  snprintf(dateStr, sizeof(dateStr), "%s %d %d", 
           monthAbbr[ptm->tm_mon], ptm->tm_mday, ptm->tm_year + 1900);

  // Update display
  lcd.setCursor(0, 2);
  lcd.print(timeStr);
  lcd.print("        "); // Clear any leftover characters
  
  lcd.setCursor(0, 3);
  lcd.print(dateStr);
  lcd.print("    "); // Clear any leftover characters
}

void loop() {
  // Check if time for NTP sync (initial or periodic)
  unsigned long currentMillis = millis();
  if ((lastSync == 0) || 
      (syncAttempts < 3 && currentMillis - lastSync > recheckInterval) ||
      (currentMillis - lastSync > syncInterval)) {
    connectWiFi();
  }

  // Read sensors
  float temp = sht31.readTemperature();
  float hum = sht31.readHumidity();

  // Update temperature display
  if (!isnan(temp)) {
    lcd.setCursor(7, 0);
    lcd.print(String(temp, 1) + " C");
  }

  // Update humidity display
  if (!isnan(hum)) {
    lcd.setCursor(10, 1);
    lcd.print(String(hum, 1) + " %");
  }

  // Show sensor error if needed
  if (isnan(temp) || isnan(hum)) {
    lcd.setCursor(0, 2);
    lcd.print("Sensor Error!      ");
    lcd.setCursor(0, 3);
    lcd.print("Check Connection! ");
  } else {
    updateTimeDisplay();
  }

  delay(2000);
}
