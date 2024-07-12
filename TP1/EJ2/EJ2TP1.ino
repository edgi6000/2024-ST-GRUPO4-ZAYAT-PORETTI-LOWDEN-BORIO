//grupo 4 Poretti, Borio, Lowden y Zayat
#include <WiFi.h>
#include "time.h"
#include <U8g2lib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

int GMT = -3;
int minutos;
int hora;
float tempActual = 0;

char stringTempActual[5];
char stringHora[5];
char stringMinutos[5];
char stringGMT[5];

struct tm timeinfo;
const char* ssid = "ORT-IoT";
const char* password = "OrtIOTnew22$2";

const char* ntpServer = "pool.ntp.org";
long gmtOffset_sec = GMT * 3600;
const int daylightOffset_sec = 0;

#define DHTPIN 23
#define SW1 35
#define SW2 34
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Estados
#define P1 0
#define P2 1
#define ESPERA1 2
#define ESPERA2 3
#define GMTA 4
#define GMTD 5
int estado = 0;


void setup() {
  Serial.begin(115200);
  u8g2.begin();
  dht.begin();

  //pinMode(DHTPIN, INPUT);
  pinMode(SW1, INPUT);
  pinMode(SW2, INPUT);

  // Connect to Wi-Fi
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
}

void loop() {
  maquinaEstado();
  Serial.println(estado);
}

void maquinaEstado() {
  switch (estado) {

    case P1:
      // Init and get the time
      gmtOffset_sec = GMT * 3600;
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

      //mostrar temp y hora

      if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
        //return;
      }
      minutos = timeinfo.tm_min;
      hora = timeinfo.tm_hour;
      tempActual = dht.readTemperature();
      displayPrint();
      if (digitalRead(SW1) == LOW && digitalRead(SW2) == LOW) {
        estado = ESPERA1;
      }
      break;

    case ESPERA1:
      if (digitalRead(SW1) == HIGH && digitalRead(SW2) == HIGH) {
        estado = P2;
      }
      break;

    case P2:
      displayGMT();
      if (digitalRead(SW1) == LOW) {
        estado = GMTA;
      }
      if (digitalRead(SW2) == LOW) {
        estado = GMTD;
      }
       if (digitalRead(SW1) == LOW && digitalRead(SW2) == LOW) {
      estado = ESPERA2;
      }
      break;

    case ESPERA2:
     if (digitalRead(SW1) == HIGH && digitalRead(SW2) == HIGH) {
      estado = P1;
      }
      break;

    case GMTA:
      if (digitalRead(SW1) == HIGH) {
        GMT = GMT + 1;
        if (GMT > 12) {
          GMT = -12;
        }
        estado = P2;
      }
       if (digitalRead(SW2) == LOW){
        estado = ESPERA2;
      }
      break;

    case GMTD:
      if (digitalRead(SW2) == HIGH) {
        GMT = GMT - 1;
        if (GMT < -12) {
          GMT = 12;
        }
        estado = P2;
      }
      if (digitalRead(SW1) == LOW){
        estado = ESPERA2;
      }
      break;
  }
}

void displayPrint() {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 15, "Temp: ");
  sprintf(stringTempActual, "%.1f", tempActual);
  u8g2.drawStr(65, 15, stringTempActual);
  u8g2.drawStr(105, 15, "C");
  u8g2.drawStr(0, 40, "Hora: ");
  sprintf(stringHora, "%d", hora);
  u8g2.drawStr(60, 40, stringHora);
  u8g2.drawStr(85, 40, ":");
  sprintf(stringMinutos, "%d", minutos);
  u8g2.drawStr(100, 40, stringMinutos);

  u8g2.sendBuffer();
}

void displayGMT() {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(0, 15, "GMT: ");
  sprintf(stringGMT, "%.2d", GMT);
  u8g2.drawStr(65, 15, stringGMT);

  u8g2.sendBuffer();
}