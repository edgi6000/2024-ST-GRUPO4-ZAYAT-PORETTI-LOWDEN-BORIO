#include <U8g2lib.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>

#define LED 25
#define BOTON1 35
#define BOTON2 34
#define DHTPIN 23

#define MODO1 0
#define MODO2 1
#define ESPERA1 2
#define ESPERA2 3
#define VUA 4
#define VUD 5
int estado = 0;
int temperaturaActual;
char stringTemperaturaActual[5];  //lo que lee el tht
int temperaturaUmbral = 30;       //es la maxima establecida(30)
char stringTemperaturaUmbral[5];

#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void maquinaEstados(void);

void setup() {
  Serial.begin(115200);
  dht.begin();
  u8g2.begin();
  pinMode(BOTON1, INPUT_PULLUP);
  pinMode(BOTON2, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
}

void loop() {
  maquinaEstados();
  delay(1000);
}

void maquinaEstados(void) {
  u8g2.clearBuffer();
  temperaturaActual = dht.readTemperature();
  maquinaTemperatura();
  u8g2.sendBuffer();
}

void maquinaTemperatura() {
  switch (estado) {
    case MODO1:
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 15, "Temperatura Actual: ");
      sprintf(stringTemperaturaActual, "%d", temperaturaActual);
      u8g2.drawStr(17, 30, stringTemperaturaActual);
      u8g2.drawStr(50, 30, "°C");

      u8g2.drawStr(0, 50, "Temperatura umbral: ");
      sprintf(stringTemperaturaUmbral, "%d", temperaturaUmbral);
      u8g2.drawStr(80, 50, stringTemperaturaUmbral);
      u8g2.drawStr(100, 50, "°C");

      if (temperaturaActual > temperaturaUmbral) {
        digitalWrite(LED, HIGH);
      } else {
        digitalWrite(LED, LOW);
      }
      if ((digitalRead(BOTON1) == LOW) && (digitalRead(BOTON2) == LOW)) {
        estado = ESPERA1;
      }
      break;

    case MODO2:

      u8g2.drawStr(0, 50, "Temperatura umbral: ");
      sprintf(stringTemperaturaUmbral, "%d", temperaturaUmbral);
      u8g2.drawStr(80, 50, stringTemperaturaUmbral);
      u8g2.drawStr(100, 50, "°C");

      if (digitalRead(BOTON1) == LOW) {
        estado = VUA;
      }
      if (digitalRead(BOTON2) == LOW) {
        estado = VUD;
      }

      if ((digitalRead(BOTON1) == LOW) && (digitalRead(BOTON2) == LOW)) {
        estado = ESPERA2;
      }
      break;

    case ESPERA1:
      if ((digitalRead(BOTON1) == HIGH) && (digitalRead(BOTON2) == HIGH)) {
        estado = MODO2;
      }
      break;

    case ESPERA2:
      if ((digitalRead(BOTON1) == HIGH) && (digitalRead(BOTON2) == HIGH)) {
        estado = MODO1;
      }
      break;
    case VUA:
    if (digitalRead(BOTON1) == LOW){
      temperaturaUmbral = temperaturaUmbral + 1;
    }
    if (digitalRead(BOTON2) == LOW){
      estado = ESPERA2;
    }
      break;
    case VUD:
     if (digitalRead(BOTON2) == LOW){
      temperaturaUmbral = temperaturaUmbral - 1;
    }
     if (digitalRead(BOTON1) == LOW){
      estado = ESPERA2;
    }
      break;
  }
}