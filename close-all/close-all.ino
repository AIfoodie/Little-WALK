#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <NewPing.h>
#include <LedControl.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
#define TRIG_PIN 2
#define ECHO_PIN 3
#define MAX_DISTANCE 200
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
#define DATA_IN_PIN 11
#define CLOCK_PIN 13
#define LOAD_PIN 10
LedControl lc = LedControl(DATA_IN_PIN, CLOCK_PIN, LOAD_PIN, 1);

void setup() {
  lcd.begin(16, 2);
  lcd.print("Initializing...");
  
  sonar.ping();  // 初始化 HC-SR04
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, false); // 關閉所有 LED
    }
  }

  delay(2000);
}

void loop() {
  // 假設我們要關閉設備：
  
  // 1. 關閉 LCD 顯示背光
  lcd.noBacklight();

  // 2. 停止 HC-SR04 發送觸發信號
  digitalWrite(TRIG_PIN, LOW);

  // 3. 關閉 8x8 LED 點陣
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      lc.setLed(0, i, j, false);  // 關閉所有 LED
    }
  }

  // 如果想進入低功耗模式，可以使用 LowPower 库
  // LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  // 讓 Arduino 休眠 8 秒

  delay(5000);  // 延遲 5 秒再關閉
}
