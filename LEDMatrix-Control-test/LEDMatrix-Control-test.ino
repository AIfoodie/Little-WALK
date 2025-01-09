#include <LedControl.h>

// 初始化 MAX7219
LedControl lc = LedControl(11, 13, 10, 1); // DIN, CLK, CS, 設備數量

void setup() {
  lc.shutdown(0, false);   // 啟用顯示
  lc.setIntensity(0, 2);    // 設置亮度（0-15）
  lc.clearDisplay(0);       // 清除顯示
}

void loop() {
  // 點亮中央四顆燈
  lc.setLed(0, 3, 3, true); // 點亮第 3 行，第 3 列
  lc.setLed(0, 3, 4, true); // 點亮第 3 行，第 4 列
  lc.setLed(0, 4, 3, true); // 點亮第 4 行，第 3 列
  lc.setLed(0, 4, 4, true); // 點亮第 4 行，第 4 列

  delay(1000); // 等待 1 秒

  // // 關閉中央四顆燈
  // lc.setLed(0, 3, 3, false); // 關閉第 3 行，第 3 列
  // lc.setLed(0, 3, 4, false); // 關閉第 3 行，第 4 列
  // lc.setLed(0, 4, 3, false); // 關閉第 4 行，第 3 列
  // lc.setLed(0, 4, 4, false); // 關閉第 4 行，第 4 列

  delay(1000); // 等待 1 秒
}
