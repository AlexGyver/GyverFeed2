#include "display.h"

/*
  Скетч к проекту "Автокормушка 2"
  - Страница проекта (схемы, описания): https://alexgyver.ru/gyverfeed2/
  - Исходники на GitHub: https://github.com/AlexGyver/GyverFeed2/
  Проблемы с загрузкой? Читай гайд для новичков: https://alexgyver.ru/arduino-first/
  AlexGyver, AlexGyver Technologies, 2020
*/

// v2.1 - исправлен баг с невыключением мотора

// Клик - внеочередная кормёжка
// Удержание - задаём размер порции
// const byte feedTime[][2] = {
//   {7, 0},       // часы, минуты. НЕ НАЧИНАТЬ ЧИСЛО С НУЛЯ
//   {12, 0},
//   {17, 0},
//   {21, 0},
// };

#define EE_RESET 12         // любое число 0-255. Измени, чтобы сбросить настройки и обновить время
// #define FEED_SPEED 3000     // задержка между шагами мотора (мкс)
// #define BTN_PIN 2           // кнопка
#define ENCODER_PIN1 D5
#define ENCODER_PIN2 D6
#define ENCODER_SW D7
// #define STEPS_FRW 19        // шаги вперёд
// #define STEPS_BKW 12        // шаги назад
// const byte drvPins[] = {3, 4, 5, 6};  // драйвер (фазаА1, фазаА2, фазаВ1, фазаВ2)

// =========================================================
#include <EEPROM.h>
// #include "microDS3231.h"
// MicroDS3231 rtc;

// #include "EncButton.h"
// EncButton<EB_TICK, BTN_PIN> btn;

#include <EncButton2.h>
EncButton2<EB_ENCBTN> enc(INPUT, ENCODER_PIN1, ENCODER_PIN2, ENCODER_SW);  // энкодер с кнопкой

int feedAmount = 100;

void setup() {
  Serial.begin(115200);

  enc.setEncType(EB_HALFSTEP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;) {
      Serial.println(F("SSD1306 allocation failed"));
    }; // Don't proceed, loop forever
  }

  // display.display();

  // rtc.begin();
  if (EEPROM.read(0) != EE_RESET) {   // первый запуск
    EEPROM.write(0, EE_RESET);
    EEPROM.put(1, feedAmount);
    // rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
  }
  EEPROM.get(1, feedAmount);
  // for (byte i = 0; i < 4; i++) pinMode(drvPins[i], OUTPUT);   // пины выходы
}

void loop() {
  // static uint32_t tmr = 0;
  // if (millis() - tmr > 1000) {           // два раза в секунду
    // static byte prevMin = 0;
    // tmr = millis();
    // DateTime now = rtc.getTime();
    // if (prevMin != now.minute) {
    //   prevMin = now.minute;
    //   for (byte i = 0; i < sizeof(feedTime) / 2; i++)    // для всего расписания
    //     if (feedTime[i][0] == now.hour && feedTime[i][1] == now.minute) feed();
    // }

  //   display.clearDisplay();

  //   display.setTextSize(1);             // Normal 1:1 pixel scale
  //   display.setTextColor(SSD1306_WHITE);        // Draw white text
  //   display.setCursor(0, 0);             // Start at top-left corner
  //   display.println(tmr);

  //   display.display();
  //   Serial.println(tmr);
  // }

  enc.tick();                       // опрос происходит здесь

  if (enc.turn()) {
    // Serial.println("turn");

    // можно опросить ещё:
    // Serial.println(enc.counter);  // вывести счётчик
    // Serial.println(enc.fast());   // проверить быстрый поворот
    // Serial.println(enc.dir()); // направление поворота

    display.clearDisplay();

    display.setTextSize(4);             // Normal 1:1 pixel scale
    display.setTextColor(SSD1306_WHITE);        // Draw white text
    display.setCursor(0, 0);             // Start at top-left corner

    // display.println(enc.dir());
    display.println(enc.counter);

    // if (enc.left()) {
    //   Serial.println("left");
    //   display.println(F("left"));     // поворот налево
    // }

    // if (enc.right()) {
    //   Serial.println("right");
    //   display.println(F("right"));   // поворот направо
    // }

    display.display();
  }

  // if (btn.click()) feed();

  // if (btn.hold()) {
  //   int newAmount = 0;
  //   while (btn.isHold()) {
  //     btn.tick();
  //     oneRev();
  //     newAmount++;
  //   }
  //   disableMotor();
  //   feedAmount = newAmount;
  //   EEPROM.put(1, feedAmount);
  // }
}

// void feed() {
//   for (int i = 0; i < feedAmount; i++) oneRev();
//   disableMotor();
// }

// выключаем ток на мотор
// void disableMotor() {
//   for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], 0);
// }

// void oneRev() {
//   for (int i = 0; i < STEPS_BKW; i++) runMotor(-1);
//   for (int i = 0; i < STEPS_FRW; i++) runMotor(1);
// }

// const byte steps[] = {0b1010, 0b0110, 0b0101, 0b1001};
// void runMotor(int8_t dir) {
//   static byte step = 0;
//   for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], bitRead(steps[step & 0b11], i));
//   delayMicroseconds(FEED_SPEED);
//   step += dir;
// }
