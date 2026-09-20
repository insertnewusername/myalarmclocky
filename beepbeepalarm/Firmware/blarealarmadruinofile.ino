#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

// ==== DISPLAY PINS ====
#define TFT_SCLK 0
#define TFT_MOSI 1
#define TFT_RST  2
#define TFT_DC   3
#define TFT_CS   4
#define TFT_BL   5

// ==== BUZZER PIN ====
#define BUZZER_PIN 6   

// ==== TIMEKEEPING ====
unsigned long lastMillis = 0;
int currentSecond = 0;
int currentMinute = 0;
int currentHour = 0;

// ==== ALARM SETTINGS ====
int alarmHour = 7;      // 7 AM
int alarmMinute = 30;   // 7:30 AM
bool alarmTriggered = false;

// ==== CUSTOM ST7789 CLASS ====
class MyST7789 : public Adafruit_ST7789 {
public:
  MyST7789(int8_t cs, int8_t dc, int8_t mosi, int8_t sclk, int8_t rst)
    : Adafruit_ST7789(cs, dc, mosi, sclk, rst) {}
  void setOffsets(uint8_t col, uint8_t row) {
    _colstart = _colstart2 = col;
    _rowstart = _rowstart2 = row;
  }
};

MyST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

void drawClock() {
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(4);
  tft.setCursor(0, 0);

  char buffer[16];
  sprintf(buffer, "%02d:%02d:%02d", currentHour, currentMinute, currentSecond);
  tft.print(buffer);

  tft.setTextSize(2);
  tft.setCursor(0, 50);
  tft.print("Alarm: ");
  sprintf(buffer, "%02d:%02d", alarmHour, alarmMinute);
  tft.print(buffer);
}


void triggerAlarm() {
  for (int i = 0; i < 200; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2);
    digitalWrite(BUZZER_PIN, LOW);
    delay(2);
  }
}

// ==== SETUP ====
void setup() {
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, LOW); 

  pinMode(BUZZER_PIN, OUTPUT);

  tft.init(76, 284);
  tft.setOffsets(82, 18);
  tft.invertDisplay(false);
  tft.setRotation(1);

  tft.fillScreen(ST77XX_BLACK);
  drawClock();

  Serial.println("TFT Initialized!");
}

// ==== LOOP ====
void loop() {
  unsigned long now = millis();

  if (now - lastMillis >= 1000) {
    lastMillis = now;
    currentSecond++;

    if (currentSecond >= 60) {
      currentSecond = 0;
      currentMinute++;
    }
    if (currentMinute >= 60) {
      currentMinute = 0;
      currentHour++;
    }
    if (currentHour >= 24) {
      currentHour = 0;
    }

    drawClock();

    if (!alarmTriggered &&
        currentHour == alarmHour &&
        currentMinute == alarmMinute &&
        currentSecond == 0) {

      alarmTriggered = true;
      triggerAlarm();
    }
  }
}
