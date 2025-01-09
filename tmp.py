# -*- coding: utf-8 -*-

import sys
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QPushButton, QVBoxLayout
from PyQt5.QtCore import QTimer
import serial

# 設置 Arduino 的串口，根據實際情況調整 COM 端口
# 在 Windows 上的端口：'COM3'，在 Mac 上的端口：'/dev/cu.usbmodem144201'
# Connect to the Arduino serial port (adjust the port based on your system)
# ser = serial.Serial('COM3', 9600)  # For Windows, use COM3
ser = serial.Serial('/dev/cu.usbmodem143201', 9600)  # For Mac, use the specific USB port

class RobotControlApp(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        # 設置應用程式的標題和窗口尺寸
        # Set the application window title and geometry
        self.setWindowTitle("Robot Control")
        self.setGeometry(300, 300, 300, 200)

        # 創建佈局，垂直排列所有元件
        # Create a layout and arrange all widgets vertically
        layout = QVBoxLayout()

        # 創建狀態顯示標籤
        # Create a label to display the current status
        self.statusLabel = QLabel("Status: Ready", self)
        layout.addWidget(self.statusLabel)

        # 創建並設置 "Move Forward" 按鈕
        # Create the "Move Forward" button and connect it to the appropriate function
        self.moveForwardBtn = QPushButton("Move Forward", self)
        self.moveForwardBtn.clicked.connect(lambda: self.sendCommand("Move Forward"))
        layout.addWidget(self.moveForwardBtn)

        # 創建並設置 "Move Backward" 按鈕
        # Create the "Move Backward" button and connect it to the appropriate function
        self.moveBackwardBtn = QPushButton("Move Backward", self)
        self.moveBackwardBtn.clicked.connect(lambda: self.sendCommand("Move Backward"))
        layout.addWidget(self.moveBackwardBtn)

        # 創建並設置 "Move Left" 按鈕
        # Create the "Move Left" button and connect it to the appropriate function
        self.moveLeftBtn = QPushButton("Move Left", self)
        self.moveLeftBtn.clicked.connect(lambda: self.sendCommand("Move Left"))
        layout.addWidget(self.moveLeftBtn)

        # 創建並設置 "Move Right" 按鈕
        # Create the "Move Right" button and connect it to the appropriate function
        self.moveRightBtn = QPushButton("Move Right", self)
        self.moveRightBtn.clicked.connect(lambda: self.sendCommand("Move Right"))
        layout.addWidget(self.moveRightBtn)

        # 創建並設置 "Stop" 按鈕
        # Create the "Stop" button and connect it to the appropriate function
        self.stopBtn = QPushButton("Stop", self)
        self.stopBtn.clicked.connect(lambda: self.sendCommand("Stop"))
        layout.addWidget(self.stopBtn)

        # 創建並設置 "Reboot" 按鈕
        # Create the "Reboot" button and connect it to the appropriate function
        self.rebootBtn = QPushButton("Reboot", self)
        self.rebootBtn.clicked.connect(lambda: self.sendCommand("Reboot"))
        layout.addWidget(self.rebootBtn)

        # 創建並設置 "Get Distance" 按鈕
        # Create the "Get Distance" button and connect it to the appropriate function
        self.distanceBtn = QPushButton("Get Distance", self)
        self.distanceBtn.clicked.connect(self.getDistance)
        layout.addWidget(self.distanceBtn)

        # 設置窗口的佈局
        # Set the layout for the window
        self.setLayout(layout)

    def sendCommand(self, command):
        # 通過串口發送命令至 Arduino
        # Send the command to Arduino via serial
        ser.write((command + '\n').encode('utf-8'))  # 命令以 UTF-8 編碼發送
        # 更新狀態顯示為發送的命令
        # Update the status label to show the command sent
        self.statusLabel.setText(f"Status: {command} sent")

    def getDistance(self):
        # 發送 "Get Distance" 命令至 Arduino
        # Send the "Get Distance" command to Arduino
        ser.write(b"Get Distance\n")
        # 檢查是否有返回數據
        # Check if data is available to read from the serial port
        if ser.in_waiting > 0:
            # 讀取並解碼返回的距離數據
            # Read and decode the returned distance data
            distance = ser.readline().decode('utf-8').strip()
            # 更新顯示狀態為返回的距離
            # Update the status label with the received distance
            self.statusLabel.setText(f"Distance: {distance} cm")

if __name__ == "__main__":
    # 創建 PyQt 應用程式並啟動
    # Create the PyQt application and start the event loop
    app = QApplication(sys.argv)
    window = RobotControlApp()
    window.show()
    sys.exit(app.exec_())