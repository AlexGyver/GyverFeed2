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
const byte feedTime[][2] = {
  {7, 0},       // часы, минуты. НЕ НАЧИНАТЬ ЧИСЛО С НУЛЯ
  {12, 0},
  {17, 0},
  {21, 0},
};

#define EE_RESET 12         // любое число 0-255. Измени, чтобы сбросить настройки и обновить время
#define FEED_SPEED 3000     // задержка между шагами мотора (мкс)
#define BTN_PIN 2           // кнопка
#define STEPS_FRW 18        // шаги вперёд
#define STEPS_BKW 10        // шаги назад
const byte drvPins[] = {3, 4, 5, 6};  // драйвер (фазаА1, фазаА2, фазаВ1, фазаВ2)

// =========================================================
#include "EncButton.h"
#include <EEPROM.h>
#include <RTClib.h>
RTC_DS3231 rtc;
EncButton<BTN_PIN> btn;
int feedAmount = 100;

void setup() {
  rtc.begin();
  if (EEPROM.read(0) != EE_RESET) {   // первый запуск
    EEPROM.write(0, EE_RESET);
    EEPROM.put(1, feedAmount);
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  EEPROM.get(1, feedAmount);
  for (byte i = 0; i < 4; i++) pinMode(drvPins[i], OUTPUT);   // пины выходы
}

void loop() {
  static uint32_t tmr = 0;
  if (millis() - tmr > 500) {           // два раза в секунду
    static byte prevMin = 0;
    tmr = millis();
    DateTime now = rtc.now();
    if (prevMin != now.minute()) {
      prevMin = now.minute();
      for (byte i = 0; i < sizeof(feedTime) / 2; i++)    // для всего расписания
        if (feedTime[i][0] == now.hour() && feedTime[i][1] == now.minute())    // пора кормить
          feed();
    }
  }

  btn.tick();
  if (btn.isClick()) {
    feed();
  }
  if (btn.isHold()) {
    int newAmount = 0;
    while (btn.isHold()) {
      btn.tick();
      oneRev();
      newAmount++;
    }
    disableMotor();
    feedAmount = newAmount;
    EEPROM.put(1, feedAmount);
  }
}

void feed() {
  for (int i = 0; i < feedAmount; i++) oneRev();      // крутим на количество feedAmount
  disableMotor();
}

void disableMotor() {
  for (byte i = 0; i < 4; i++) digitalWrite(drvPins[i], 0); // выключаем ток на мотор
}

void oneRev() {
  static byte val = 0;  
  for (byte i = 0; i < STEPS_BKW; i++) runMotor(val--);
  for (byte i = 0; i < STEPS_FRW; i++) runMotor(val++);
}

void runMotor(byte thisStep) {
  /*static const byte steps[] = {0b1000, 0b1010, 0b0010, 0b0110, 0b0100, 0b0101, 0b0001, 0b1001};
    for (byte i = 0; i < 4; i++)
    digitalWrite(drvPins[i], bitRead(steps[thisStep & 0b111], i));
  */
  static const byte steps[] = {0b1010, 0b0110, 0b0101, 0b1001};
  for (byte i = 0; i < 4; i++)
    digitalWrite(drvPins[i], bitRead(steps[thisStep & 0b11], i));
  delayMicroseconds(FEED_SPEED);
}
