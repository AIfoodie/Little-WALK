#include <Wire.h>               // 引入 I2C 通訊庫 / Include I2C communication library
#include <LiquidCrystal_I2C.h>   // 引入 I2C LCD 顯示器控制庫 / Include I2C LCD display control library
#include <LedControl.h>          // 引入 MAX7219 LED 控制庫 / Include MAX7219 LED control library

// MAX7219 LED 點陣顯示設定 / MAX7219 LED matrix display settings
LedControl lc = LedControl(11, 13, 10, 1); // DIN=11, CLK=13, CS=10
int robotX = 3, robotY = 3; // 初始機器人位置 / Initial robot position

// I2C LCD 顯示器設定 / I2C LCD display settings
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD 地址: 0x27 / LCD address: 0x27

// 超聲波模組設定 / Ultrasonic sensor settings
const int trigPin = 2; // 定義觸發腳位 / Define trigger pin
const int echoPin = 3; // 定義回波腳位 / Define echo pin
long distance; // 測量的距離值 / Variable to store the measured distance

// 狀態變數 / Robot's state variables
bool moving = false;  // 是否移動中？ / Is the robot moving?
bool happy = true;     // 是否顯示開心的表情？ / Is the robot displaying a happy face?

// 距離測量函數 / Distance measurement function
long getDistance() {
  digitalWrite(trigPin, LOW); // 清空觸發腳信號，確保初始狀態為低 / Clear the trigger pin signal to ensure the initial state is LOW
  delayMicroseconds(2); // 延時 2 微秒，為觸發信號準備 / Delay 2 microseconds to prepare for the trigger signal
  digitalWrite(trigPin, HIGH); // 發送觸發信號，開始測距 / Send the trigger signal to start measuring distance
  delayMicroseconds(10); // 信號持續 10 微秒，保證有效性 / Hold the signal for 10 microseconds to ensure validity
  digitalWrite(trigPin, LOW); // 關閉觸發信號，結束發送 / Turn off the trigger signal to end sending

  long duration = pulseIn(echoPin, HIGH); // 讀取從 echoPin 返回的脈衝持續時間 / Measure the pulse duration from the echoPin
  return duration * 0.034 / 2; // 計算距離，聲波速度為 0.034 cm/µs，往返時間需除以 2 / Calculate the distance, sound speed is 0.034 cm/µs, divide by 2 for round-trip time
}

// 更新 LED 顯示 / Update the LED matrix to show the robot's position
void updateLED() {
  lc.clearDisplay(0); // 清除顯示，確保畫面乾淨 / Clear the display to ensure the screen is clean

  // 確保機器人位置在有效範圍內 / Ensure the robot's position is within the valid range
  robotX = constrain(robotX, 0, 6);  // 限制 robotX 在範圍 0-6 內 (最多顯示 2x2 塊) / Constrain robotX within the range 0-6 (maximum 2x2 block)
  robotY = constrain(robotY, 0, 6);  // 限制 robotY 在範圍 0-6 內 (最多顯示 2x2 塊) / Constrain robotY within the range 0-6 (maximum 2x2 block)

  // 顯示 2x2 方塊 / Display a 2x2 block to represent the robot's position
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      lc.setLed(0, robotY + i, robotX + j, true); // 根據 robotX 和 robotY 設置 LED 點亮 / Turn on LEDs at the robot's position
    }
  }
}

// 反向移動功能 / Reverse the robot's movement if an obstacle is detected within 100 cm
void reverseMove() {
  distance = getDistance(); // 獲取當前距離 / Get the current distance measurement
  if (distance < 100 && moving) { // 如果距離小於 100 公分，並且正在移動 / If the distance is less than 100 cm and the robot is moving
    if (robotY > 0) robotY--; // 向上移動 / Move the robot up
    else if (robotY < 6) robotY++; // 如果已經到達頂部，則向下移動 / If at the top, move down
    else if (robotX > 0) robotX--; // 向左移動 / Move the robot left
    else if (robotX < 6) robotX++; // 如果已經到達左邊，則向右移動 / If at the left edge, move right
    happy = false;  // 顯示難過表情，表示遇到障礙物 / Set the robot to display a sad face (obstacle encountered)
    updateLED(); // 更新 LED 顯示 / Update the LED display
  }
}

// 更新 LCD 顯示 / Update the LCD display to show the robot's face and the measured distance
void updateLCD() {
  lcd.clear(); // 清除 LCD 顯示，防止疊加內容 / Clear the LCD screen to avoid overlapping text
  if (happy) { // 判斷機器人是否開心 / Check if the robot is happy
    lcd.setCursor(0, 0); // 設置光標到第一行 / Set cursor to the first row
    lcd.print("^ w ^ Y"); // 顯示開心表情 / Display a happy face
  } else {
    lcd.setCursor(0, 0); // 設置光標到第一行 / Set cursor to the first row
    lcd.print(". _ ."); // 顯示難過表情 / Display a sad face
  }

  lcd.setCursor(0, 1); // 設置光標到第二行 / Set cursor to the second row
  lcd.print("Distance: "); // 顯示文字 "Distance:" / Display the label "Distance:"
  lcd.print(distance); // 顯示當前測量的距離值 / Display the current distance measurement
  lcd.print(" cm"); // 單位顯示為公分 / Display units in centimeters
}

// 接收指令並執行 / Execute actions based on the command received via serial
void handleCommand(String command) {
  if (command == "Move Forward") { // 指令為前進 / Command to move forward
    if (robotY > 0) robotY--; // 向上移動 / Move up if not at the top
    moving = true; // 設置移動狀態 / Set moving status to true
  } else if (command == "Move Backward") { // 指令為後退 / Command to move backward
    if (robotY < 6) robotY++; // 向下移動 / Move down if not at the bottom
    moving = true;
  } else if (command == "Move Left") { // 指令為向左移動 / Command to move left
    if (robotX > 0) robotX--; // 向左移動 / Move left if not at the left edge
    moving = true;
  } else if (command == "Move Right") { // 指令為向右移動 / Command to move right
    if (robotX < 6) robotX++; // 向右移動 / Move right if not at the right edge
    moving = true;
  } else if (command == "Stop") { // 指令為停止 / Command to stop
    moving = false; // 停止機器人移動 / Stop the robot from moving
    robotX = 3;  // 重置機器人位置 / Reset robot position
    robotY = 3;  // 重置機器人位置 / Reset robot position
  } else if (command == "Reboot") { // 重啟指令
    robotX = 3; robotY = 3; 
    moving = false; 
    happy = true; 
    updateLED(); 
    updateLCD(); 
  }

  updateLED(); // 更新 LED 顯示器 / Update the LED display
  updateLCD(); // 更新 LCD 顯示器 / Update the LCD display
}


// 初始化 / Set up pin modes and initialize components
void setup() {
  pinMode(trigPin, OUTPUT); // 設置超聲波觸發腳位為輸出 / Set the trigger pin as output
  pinMode(echoPin, INPUT);  // 設置超聲波回波腳位為輸入 / Set the echo pin as input
  Serial.begin(9600); // 初始化串口，設置波特率為 9600 / Initialize serial communication with baud rate 9600
  
  lcd.begin(16, 2); // 初始化 LCD 顯示器 / Initialize the LCD display
  lcd.setCursor(0, 0); // 設置光標位置 / Set cursor position
  lcd.print("Mood: ^ w ^ Y"); // 顯示開心的表情 / Display happy face
  lcd.setCursor(0, 1); // 設置光標位置 / Set cursor position
  lcd.print("Status:Ready GO!"); // 顯示狀態為就緒 / Display status "Ready GO!"
  lcd.backlight(); // 開啟 LCD 背光 / Turn on the LCD backlight
  
  updateLED(); // 更新 LED 顯示 / Update the LED display
  lc.shutdown(0, false); // 啟動 LED 顯示 / Activate the LED display
  lc.setIntensity(0, 4); // 設置 LED 顯示亮度 / Set LED display brightness
  // lc.clearDisplay(0); // 清除 LED 顯示內容 / Clear the LED display
}

// 主循環 / Main loop
void loop() {
  if (Serial.available()) { // 如果有串口數據 / If serial data is available
    String command = Serial.readString(); // 讀取指令 / Read the command from the serial buffer
    handleCommand(command); // 處理指令 / Handle the command
  }

  reverseMove(); // 在移動中檢查障礙物 / Check for obstacles and reverse if needed
  delay(100); // 延時 100 毫秒 / Delay for 100 milliseconds
}