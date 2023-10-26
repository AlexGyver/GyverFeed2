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
#define STEPS_FRW 19        // шаги вперёд
#define STEPS_BKW 12        // шаги назад
#define STEP_PIN 3          // пин шага
#define DIR_PIN 4           // пин направления

// =========================================================
#include <EEPROM.h>
#include "microDS3231.h"
MicroDS3231 rtc;

#include "EncButton.h"
EncButton<EB_TICK, BTN_PIN> btn;
int feedAmount = 100;

void setup() {
  rtc.begin();
  if (EEPROM.read(0) != EE_RESET) {   // первый запуск
    EEPROM.write(0, EE_RESET);
    EEPROM.put(1, feedAmount);
    rtc.setTime(BUILD_SEC, BUILD_MIN, BUILD_HOUR, BUILD_DAY, BUILD_MONTH, BUILD_YEAR);
  }
  EEPROM.get(1, feedAmount);
  for (byte i = 0; i < 4; i++) pinMode(drvPins[i], OUTPUT);   // пины выходы
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
}

void loop() {
  static uint32_t tmr = 0;

  if (millis() - tmr > 500) {           // два раза в секунду
    static byte prevMin = 0;
    tmr = millis();
    DateTime now = rtc.getTime();
    if (prevMin != now.minute) {
      prevMin = now.minute;
      for (byte i = 0; i < sizeof(feedTime) / 2; i++)    // для всего расписания
        if (feedTime[i][0] == now.hour && feedTime[i][1] == now.minute) feed();
    }
  }

  btn.tick();
  if (btn.click()) feed();

  if (btn.hold()) {
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
  for (int i = 0; i < feedAmount; i++) oneRev();
  disableMotor();
}

// выключаем ток на мотор
void disableMotor() {
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
}

void oneRev() {
  for (int i = 0; i < STEPS_BKW; i++) runMotor(HIGH);
  for (int i = 0; i < STEPS_FRW; i++) runMotor(LOW);
}

void runMotor(bool direction) {
  digitalWrite(DIR_PIN, direction);
  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(FEED_SPEED);
  digitalWrite(STEP_PIN, LOW);
}
