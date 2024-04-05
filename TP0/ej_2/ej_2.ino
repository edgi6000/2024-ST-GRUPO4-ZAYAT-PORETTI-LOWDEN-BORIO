#include <U8g2lib.h>
#include <DHT.h>
#define SENSOR_TEMP 23

DHT dht(23,DHT11);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void printBMP_OLED(void);

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  dht.begin();
  pinMode(SENSOR_TEMP,INPUT);
}

void loop() {
  printBMP_OLED();
  delay(1000);
}

void printBMP_OLED(void) {
  int temp = dht.readTemperature();
  Serial.println(temp);
  char stringtemp[5];
  u8g2.clearBuffer();  // limpiamos la memoria interna

  u8g2.setFont(u8g2_font_ncenB08_tr);  // elegimos la fuente correcta
  sprintf(stringtemp, "%d", temp);     /// se convierte el valor de float a string
  u8g2.drawStr(17, 30, stringtemp);
  u8g2.drawStr(50, 30, "C");
  u8g2.sendBuffer();  // transferencia interna en el display
