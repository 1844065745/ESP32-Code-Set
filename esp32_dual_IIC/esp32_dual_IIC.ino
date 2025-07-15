#include <Wire.h>
#include <WiFi.h>
#include "LDC1614.h"

#define WIFI_SSID "mywifi"
#define WIFI_PASS "wangyueyang"

WiFiClient client;
WiFiUDP udp;

const uint8_t NUM_CN = 16;
float data_send[NUM_CN] = {0.0};
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 50;

unsigned long packet_id = 0;

IPAddress serverIP(100,64,144,87);
uint16_t serverPort = 10000;

// 定义第二条I2C总线
TwoWire I2C_1 = TwoWire(1);

// 创建四个LDC1614对象
LDC1614 ldc1(0x2A, &Wire);    // 总线0，地址0x2A
LDC1614 ldc2(0x2B, &Wire);    // 总线0，地址0x2B
LDC1614 ldc3(0x2A, &I2C_1);   // 总线1，地址0x2A
LDC1614 ldc4(0x2B, &I2C_1);   // 总线1，地址0x2B

String floatToString(float *arr, int len) {
  String result = "";
  for (int i = 0; i < len; i++) {
    result += String(arr[i], 3);
    if (i < len - 1) result += ",";
  }
  return result;
}

void wifi_connect() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n[WiFi] Connected. IP: " + WiFi.localIP().toString());
}

void setup() {
  Serial.begin(115200);
  Serial.println("Begin initializing I2C and LDC1614...");

  wifi_connect();

  // 初始化两个I2C总线（根据你的引脚修改）
  Wire.begin(12, 13);         // SDA, SCL for I2C0
  I2C_1.begin(10, 11);        // SDA, SCL for I2C1

  Serial.println("Begin initializing ldc1-->");
  ldc1.init();
  Serial.println("Begin initializing ldc2-->");
  ldc2.init();
  Serial.println("Begin initializing ldc3-->");
  ldc3.init();
  Serial.println("Begin initializing ldc4-->");
  ldc4.init();

  Serial.println("All LDC1614 initialized.");
  delay(3000);
}

void loop() {
  if (!client.connected()) {
    Serial.println("[TCP] Connecting to server...");
    if (client.connect(serverIP, serverPort)) {
      Serial.println("[TCP] Connected.");
    } else {
      Serial.println("[TCP] Failed. Retrying...");
      delay(1000);
      return;
    }
  }

  // 定时发送
  unsigned long now = millis();
  if (now - lastSendTime >= sendInterval) {
    lastSendTime = now;

    int i = 0;
    ldc1.getData(4);
    for (; i< 4; i++) data_send[i] = ldc1.L[i]; 
    ldc2.getData(4);
    for (; i< 8; i++) data_send[i] = ldc2.L[i-4]; 
    ldc3.getData(4);
    for (; i<12; i++) data_send[i] = ldc3.L[i-8]; 
    ldc4.getData(4);
    for (; i<16; i++) data_send[i] = ldc4.L[i-12]; 

    String payload = "Ldata," + String(packet_id++) + "," + floatToString(data_send, NUM_CN);
    payload += "\n";

    client.print(payload);
    client.flush();  // 强制立刻发送

    Serial.print("[SEND] ");
    Serial.print(payload);
  }

  delay(100);
}
