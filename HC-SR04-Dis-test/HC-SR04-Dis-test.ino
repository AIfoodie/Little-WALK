const int trigPin = 2;  // Trig 腳位
const int echoPin = 3;  // Echo 腳位

void setup() {
  pinMode(trigPin, OUTPUT);  // 設定 Trig 腳位為輸出
  pinMode(echoPin, INPUT);   // 設定 Echo 腳位為輸入
  Serial.begin(9600);        // 啟動序列監控器
}

void loop() {
  // 觸發超音波模組
  digitalWrite(trigPin, LOW);  // 先將 Trig 設為 LOW，保持 2 微秒
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); // 設為 HIGH，持續 10 微秒
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  // 立即設為 LOW，結束發送

  // 測量 Echo 信號返回的時間
  long duration = pulseIn(echoPin, HIGH);  // 等待 Echo 信號為 HIGH 時計算持續時間

  // 計算距離（距離 = 時間 * 聲速 / 2）
  float distance = (duration * 0.0343) / 2;

  // 輸出距離結果到序列監控器
  Serial.print("Distance: ");
  Serial.print(distance);  // 顯示距離
  Serial.println(" cm");   // 單位為公分

  delay(2000);  // 每秒測一次距離
}