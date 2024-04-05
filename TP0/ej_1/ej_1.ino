#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  Serial.println("OLED test");
}

void loop() {
  u8g2.setFont(u8g2_font_ncenB14_tr);
  u8g2.drawStr(15, 15, "HOLA");
  u8g2.drawStr(15, 30, "MUNDO");

  u8g2.sendBuffer();
  delay(1000);
  Serial.println("ACTIVO");
}
