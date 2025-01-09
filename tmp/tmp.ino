#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <LedControl.h>

// MAX7219 LED 點陣
// Initialize MAX7219 LED Matrix controller
LedControl lc = LedControl(11, 13, 10, 1); // DIN=11, CLK=13, CS=10
int robotX = 3, robotY = 3; // 初始機器人位置 (2x2 的中心位置)
// Initial robot position (center of the 2x2 block)

// I2C LCD 顯示器
// Initialize I2C LCD with address 0x27 and 16x2 size
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 地址: 0x27

// 超聲波模組
// Ultrasonic Module
const int trigPin = 2; // 定義觸發腳位
// Define the trigger pin
const int echoPin = 3; // 定義回波腳位
// Define the echo pin
long distance; // 測量的距離值
// Measured distance value

// 蜂鳴器腳位
// Buzzer Pin
const int buzzerPin = 9; // 定義蜂鳴器的腳位
// Define the buzzer pin

// 狀態變數
// Robot's status variables
bool moving = false;  // 是否移動中？
// Is the robot moving?
bool avoiding = false; // 是否正在避障？
// Is the robot avoiding an obstacle?
bool happy = true;     // 是否顯示開心的表情？
// Is the robot displaying a happy expression?

// 距離測量函數
// Distance Measurement Function
// This function triggers the ultrasonic sensor and calculates the distance based on the reflected signal time.
long getDistance() {
  digitalWrite(trigPin, LOW); // 清空觸發腳信號，確保初始狀態為低
  // Clear the trigger pin signal to ensure initial state is LOW
  delayMicroseconds(2); // 延時 2 微秒，為觸發信號準備
  // Delay 2 microseconds to prepare the trigger signal
  digitalWrite(trigPin, HIGH); // 發送觸發信號，開始測距
  // Send the trigger signal to start distance measurement
  delayMicroseconds(10); // 信號持續 10 微秒，保證有效性
  // Hold the signal for 10 microseconds to ensure effectiveness
  digitalWrite(trigPin, LOW); // 關閉觸發信號，結束發送
  // Turn off the trigger signal to finish sending

  long duration = pulseIn(echoPin, HIGH); // 讀取從 echoPin 返回的脈衝持續時間
  // Read the pulse duration from echoPin
  // 計算距離，聲波速度為 0.034 cm/µs，往返時間需除以 2
  // Calculate the distance: Speed of sound = 0.034 cm/µs, divide round-trip time by 2
  return duration * 0.034 / 2; 
}

// 更新 LED 顯示
// Update LED Display
// Update the LED display to show the robot's position
void updateLED() {
  lc.clearDisplay(0); // 清除顯示，確保畫面乾淨
  // Clear the display to ensure a clean screen

  // 確保機器人位置在有效範圍內
  // Ensure robot position is within valid range
  robotX = constrain(robotX, 0, 6);  // 限制 robotX 在範圍 0-6 內 (最多顯示 2x2 塊)
  robotY = constrain(robotY, 0, 6);  // 限制 robotY 在範圍 0-6 內 (最多顯示 2x2 塊)

  // 顯示 2x2 方塊
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      lc.setLed(0, robotY + i, robotX + j, true); // 根據 robotX 和 robotY 設置 LED 點亮
      // Light up LEDs based on robotX and robotY
    }
  }
}

// 避障功能
// Obstacle Avoidance Function
// Avoid obstacles based on the distance measured by the ultrasonic sensor
void avoidObstacle() {
  distance = getDistance(); // 獲取當前距離，檢測是否有障礙物
  // Get the current distance to detect obstacles
  if (distance < 10) { // 如果距離小於 10 公分，觸發避障行為
    // If the distance is less than 10 cm, trigger obstacle avoidance behavior
    avoiding = true; // 將避障狀態設置為真
    // Set the avoiding status to true
    moving = false; // 停止移動，避免碰撞
    // Stop moving to avoid collision
    happy = false;  // 顯示難過表情，表明有障礙物
    // Display a sad expression to indicate an obstacle
    tone(buzzerPin, 1000, 200); // 發出短音效 (頻率 1000Hz，持續 200 毫秒)
    // Emit a short sound (frequency 1000Hz, duration 200ms)
    // robotX = 3; robotY = 3; // 重置機器人位置到起始點 (可以不重置，保持當前位置)
    // Reset robot position to starting point (you can skip resetting, keep current position)
    updateLED(); // 更新 LED 顯示器，反映新的位置
    // Update the LED display to reflect the new position
  } else {
    avoiding = false; // 如果沒有障礙物，停止避障行為
    // If no obstacle, stop avoidance behavior
  }
}

// 更新 LCD 顯示
// Update LCD Display
// Display the robot's mood and the measured distance on the LCD
void updateLCD() {
  lcd.clear(); // 清除 LCD 顯示，防止疊加內容
  // Clear the LCD to prevent overlapping content
  if (happy) { // 判斷機器人是否開心
    // Check if the robot is happy
    lcd.setCursor(0, 0); // 設置光標到第一行
    // Set the cursor to the first line
    lcd.print("^ w ^/"); // 顯示開心表情
    // Display a happy expression
  } else {
    lcd.setCursor(0, 0); // 設置光標到第一行
    // Set the cursor to the first line
    lcd.print(". _ ."); // 顯示難過表情
    // Display a sad expression
  }

  lcd.setCursor(0, 1); // 設置光標到第二行
  // Set the cursor to the second line
  lcd.print("Distance: "); // 顯示文字 "Distance:"
  // Display the text "Distance:"
  lcd.print(distance); // 顯示當前測量的距離值
  // Display the current measured distance value
  lcd.print(" cm"); // 單位顯示為公分
  // Show the unit as cm
}

// 接收指令並執行
// Handle Received Commands
// Perform actions based on commands received through serial communication
void handleCommand(String command) {
  if (command == "Move Forward") { // 指令為前進
    // Command to move forward
    if (robotY > 0) robotY--; // 如果尚未到頂部，向上移動
    // Move up if not at the top
    moving = true; // 設置移動狀態
    // Set moving status

  } else if (command == "Move Backward") { // 指令為後退
    // Command to move backward
    if (robotY < 6) robotY++; // 如果尚未到底部，向下移動
    // Move down if not at the bottom
    moving = true;

  } else if (command == "Move Left") { // 指令為向左移動
    // Command to move left
    if (robotX > 0) robotX--; // 如果尚未到左側，向左移動
    // Move left if not at the left edge
    moving = true;

  } else if (command == "Move Right") { // 指令為向右移動
    // Command to move right
    if (robotX < 6) robotX++; // 如果尚未到右側，向右移動
    // Move right if not at the right edge
    moving = true;

  } else if (command == "Stop") { // 指令為停止
    // Command to stop
    moving = false; // 停止機器人移動
    // Stop the robot's movement

  } else if (command == "Reboot") { // 指令為重啟
    // Command to reboot
    robotX = 3; robotY = 3; // 重置機器人位置到起始點
    // Reset robot position to starting point
    moving = false; // 停止移動
    // Stop moving
    avoiding = false; // 停止避障行為
    // Stop obstacle avoidance
    happy = true; // 恢復開心表情
    // Restore happy expression
    updateLED(); // 更新 LED 顯示器
    // Update LED display
    updateLCD(); // 更新 LCD 顯示
    // Update LCD display
  }

  updateLED(); // 根據新狀態更新 LED 顯示
  // Update the LED display based on the new state
  updateLCD(); // 根據新狀態更新 LCD 顯示
  // Update the LCD display based on the new state
}

// 初始化
// Initialization
void setup() {
  lc.shutdown(0, false); // 啟動 MAX7219 LED 顯示控制器
  // Initialize the MAX7219 LED matrix controller
  lc.setIntensity(0, 8);  // 設置顯示亮度為中等
  // Set display brightness to medium
  lc.clearDisplay(0);     // 清除所有顯示內容
  // Clear all display contents

  pinMode(trigPin, OUTPUT); // 設置超聲波觸發腳為輸出模式
  // Set ultrasonic trigger pin to output mode
  pinMode(echoPin, INPUT);  // 設置超聲波回聲腳為輸入模式
  // Set ultrasonic echo pin to input mode
  pinMode(buzzerPin, OUTPUT); // 設置蜂鳴器腳為輸出模式
  // Set buzzer pin to output mode

  lcd.init(); // 初始化 LCD 顯示器
  // Initialize the LCD display
  lcd.backlight(); // 開啓 LCD 背光燈
  // Turn on the LCD backlight

  Serial.begin(9600); // 初始化串口通信，波特率為 9600
  // Initialize serial communication with baud rate 9600
}

// 主循環
// Main loop
// Continuously check for commands and obstacles
void loop() {
  // 檢查是否有來自串口的數據
  // Check if there's data available from the serial port

  if (Serial.available() > 0) { 
    // 讀取指令直到換行符，並儲存為命令字串
    // Read the command string until a newline character is encountered
    String command = Serial.readStringUntil('\n'); 
    handleCommand(command); // 處理接收到的指令
                             // Handle the received command
  }

  // 持續檢測障礙物，並根據情況進行避障
  // Continuously check for obstacles and avoid them
  avoidObstacle(); 
}