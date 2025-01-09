# -*- coding: utf-8 -*-

import sys
import serial
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton, QVBoxLayout, QHBoxLayout
from PyQt5.QtCore import Qt

# 設置 Arduino 的串口，根據實際情況調整 COM 端口
# Set the Arduino serial port, adjust according to your system (e.g., COM3 for Windows, /dev/cu.usbmodemXXXX for Mac)
ser = serial.Serial('/dev/cu.usbmodem143201', 9600)  # For Mac, use the specific USB port

class RobotControlApp(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        # 設置應用程式的標題和窗口尺寸
        # Set the application window title and size
        self.setWindowTitle("Robot Control / 機器人控制")
        self.setGeometry(300, 300, 300, 300)

        # 創建主佈局
        # Create the main layout
        main_layout = QVBoxLayout()

        # 創建按鈕佈局
        # Create the button layout
        button_layout = QVBoxLayout()

        # 第一行佈局：前進 | 現在狀態 | 表情
        # First row layout: Move forward | Current status | Mood
        top_layout = QHBoxLayout()
        self.upBtn = QPushButton("前進 / Move Forward", self)  # 按鈕名稱加上中英文對照 / Button label with Chinese and English
        self.upBtn.clicked.connect(lambda: self.sendCommand("Move Forward"))  # 控制前進 / Control move forward

        self.statusDisplay = QLabel("Status: STOP / 狀態: 停止", self)  # 顯示當前狀態，包含中英文對照 / Show current status with Chinese and English
        self.statusDisplay.setAlignment(Qt.AlignCenter)

        self.faceDisplay = QLabel("Mood: ^ w ^ Y / 表情: ^ w ^ Y", self)  # 顯示當前表情，包含中英文對照 / Show current mood with Chinese and English
        self.faceDisplay.setAlignment(Qt.AlignCenter)

        top_layout.addWidget(self.upBtn)
        top_layout.addWidget(self.statusDisplay)
        top_layout.addWidget(self.faceDisplay)
        button_layout.addLayout(top_layout)

        # 第二行佈局：左 | 停止鍵 | 右
        # Second row layout: Left | Stop button | Right
        middle_layout = QHBoxLayout()
        self.leftBtn = QPushButton("左 / Left", self)  # 按鈕名稱加上中英文對照 / Button label with Chinese and English
        self.leftBtn.clicked.connect(lambda: self.sendCommand("Move Left"))  # 控制左移 / Control move left

        self.stopBtn = QPushButton("停止 / Stop", self)  # 按鈕名稱加上中英文對照 / Button label with Chinese and English
        self.stopBtn.clicked.connect(lambda: self.sendCommand("Stop"))  # 停止機器人 / Stop the robot

        self.rightBtn = QPushButton("右 / Right", self)  # 按鈕名稱加上中英文對照 / Button label with Chinese and English
        self.rightBtn.clicked.connect(lambda: self.sendCommand("Move Right"))  # 控制右移 / Control move right
        
        middle_layout.addWidget(self.leftBtn)
        middle_layout.addWidget(self.stopBtn)
        middle_layout.addWidget(self.rightBtn)
        button_layout.addLayout(middle_layout)

        # 第三行佈局：後退 | 重啟一切 | 取得距離
        # Third row layout: Move backward | Reboot everything | Get distance
        bottom_layout = QHBoxLayout()
        self.downBtn = QPushButton("後退 / Move Backward", self)  # 按鈕名稱加上中英文對照 / Button label with Chinese and English
        self.downBtn.clicked.connect(lambda: self.sendCommand("Move Backward"))  # 控制後退 / Control move backward
        
        # 創建並設置 "Reboot" 按鈕
        # Create the "Reboot" button and connect it to the appropriate function
        self.rebootBtn = QPushButton("Reboot", self)  # 按鈕名稱 / Button label
        self.rebootBtn.clicked.connect(lambda: self.sendCommand("Reboot"))  # 觸發發送 Reboot 命令 / Send Reboot command
        bottom_layout.addWidget(self.rebootBtn)

        self.distanceBtn = QPushButton("距離 / Distance", self)  # 按鈕名稱加上中英文對照 / Button label with Chinese and English
        self.distanceBtn.clicked.connect(self.getDistance)  # 獲取距離 / Get distance
        
        bottom_layout.addWidget(self.downBtn)
        bottom_layout.addWidget(self.distanceBtn)
        button_layout.addLayout(bottom_layout)

        # 設置按鈕佈局
        # Set the button layout
        main_layout.addLayout(button_layout)

        # 設置窗口的佈局
        # Set the window layout
        self.setLayout(main_layout)

    def reset(self):
        # 重置表情和狀態顯示為初始值
        # Reset the face and status display to initial values
        self.faceDisplay.setText("Mood: ^ w ^ Y / 表情: ^ w ^ Y")  # 顯示初始開心表情 / Display initial happy face
        self.statusDisplay.setText("Status: STOP / 狀態: 停止")  # 顯示初始狀態 / Display initial status

    def sendCommand(self, command):
        # 通過串口發送指令給 Arduino
        # Send command to Arduino via serial
        ser.write((command + '\n').encode('utf-8'))
        self.statusDisplay.setText(f"Status: {command} / 狀態: {command}")  # 顯示發送的命令 / Display the sent command
        self.getFaceFromArduino()  # 根據 Arduino 返回的狀態更新表情 / Update the face based on the status from Arduino

    def getDistance(self):
        # 向 Arduino 發送取得距離的指令
        # Send command to get the distance from Arduino
        ser.write(b"Get Distance\n")
        if ser.in_waiting > 0:
            distance = ser.readline().decode('utf-8').strip()  # 讀取並解碼距離數據 / Read and decode the distance data
            self.statusDisplay.setText(f"Distance: {distance} cm / 距離: {distance} 公分")  # 顯示返回的距離 / Display the received distance
            self.getFaceFromArduino()  # 根據距離數據更新表情 / Update the face based on the distance data

    def getFaceFromArduino(self):
        # 向 Arduino 發送取得表情的指令
        # Send command to get the face from Arduino
        ser.write(b"Get Face\n")
        if ser.in_waiting > 0:
            face = ser.readline().decode('utf-8').strip()  # 讀取並解碼表情數據 / Read and decode the face data
            if face == "Happy":
                self.faceDisplay.setText("Mood: ^ w ^ Y / 表情: ^ w ^ Y")  # 顯示開心表情 / Display happy face
            elif face == "Sad":
                self.faceDisplay.setText("Mood: . _ . / 表情: . _ .")  # 顯示難過表情 / Display sad face

if __name__ == "__main__":
    # 創建 PyQt 應用程式並啟動
    # Create the PyQt application and start the event loop
    app = QApplication(sys.argv)
    window = RobotControlApp()
    window.show()
    sys.exit(app.exec_())
