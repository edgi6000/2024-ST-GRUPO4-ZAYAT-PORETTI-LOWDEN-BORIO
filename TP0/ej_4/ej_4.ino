#include <U8g2lib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

#define DHTPIN 23
#define DHTTYPE DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);

#define LED 25
#define BTN1 35
#define BTN2 34

void setup() {
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);

  pinMode(LED, OUTPUT);

  u8g2.begin();
  // u8g2.setBusClock(4000000);
  u8g2.setFont(u8g2_font_profont17_tf);

  dht.begin();
}

int16_t minTemperature = 28;

sensors_event_t event;

enum {
  ChangeState,
  WaitingForButton1Release,
  WaitingForButton2Release,
  TempScreen,
  Config
} state = TempScreen,
  nextState, lastState = state;

void showTemp() {
  static uint32_t lastFrame = -1;
  if (millis() - lastFrame >= 100) {
    u8g2.clearBuffer();
    u8g2.setColorIndex(1);
    if (isnan(event.temperature)) {
      u8g2.drawStr(5, 14, "Error!!");
    } else {
      char text[16];
      sprintf(text, "%.1f grados", event.temperature);
      u8g2.drawStr(5, 44, text);
      u8g2.drawStr(5, 14, "Temperatura");
    }
    u8g2.sendBuffer();
    lastFrame = millis();
  }
  if (digitalRead(BTN1) == LOW && digitalRead(BTN2) == LOW) {
    nextState = Config;
    state = ChangeState;
  }
}

void configuration() {
  static uint32_t lastFrame = -1;
  if (millis() - lastFrame >= 100) {
    u8g2.clearBuffer();
    u8g2.setColorIndex(1);
    if (isnan(event.temperature)) {
      u8g2.drawStr(5, 22, "Error!!");
    } else {
      char text[16];
      sprintf(text, "%d grados", minTemperature);
      u8g2.drawStr(5, 44, text);
      u8g2.drawStr(5, 14, "Configuracion");
    }
    u8g2.sendBuffer();
    lastFrame = millis();
  }

  if (lastState == WaitingForButton1Release) {
    minTemperature++;
    if (minTemperature >= 60) {
      minTemperature = 60;
    }
    lastState = Config;
  } else if (lastState == WaitingForButton2Release) {
    minTemperature--;
    if (minTemperature <= -20) {
      minTemperature = -20;
    }
    lastState = Config;
  }
  if (digitalRead(BTN1) == LOW) {
    lastState = state;
    nextState = TempScreen;
    state = WaitingForButton1Release;
  }

  if (digitalRead(BTN2) == LOW) {
    lastState = state;
    nextState = TempScreen;
    state = WaitingForButton2Release;
  }
}


void loop() {
  static uint32_t lastSensorReading = -1;
  if (millis() - lastSensorReading >= 2000) {
    dht.temperature().getEvent(&event);
    lastSensorReading = millis();
  }

  if (!isnan(event.temperature) && event.temperature > minTemperature) {
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }

  switch (state) {
    case TempScreen:
      showTemp();
      break;
    case Config:
      configuration();
      break;
    case ChangeState:
      if (digitalRead(BTN1) == HIGH && digitalRead(BTN2) == HIGH) {
        state = nextState;
        lastState = ChangeState;
      }
      break;
    case WaitingForButton1Release:
      if (digitalRead(BTN2) == LOW) {
        state = ChangeState;
        lastState = WaitingForButton1Release;
      } else if (digitalRead(BTN1) == HIGH) {
        state = lastState;
        lastState = WaitingForButton1Release;
      }
      break;
    case WaitingForButton2Release:
      if (digitalRead(BTN1) == LOW) {
        state = ChangeState;
        lastState = WaitingForButton2Release;
      } else if (digitalRead(BTN2) == HIGH) {
        state = lastState;
        lastState = WaitingForButton2Release;
      }
      break;
  }
}
