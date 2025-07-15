#include <WiFi.h>

#include "LDC1614.h"

#define WIFI_SSID "mywifi"
#define WIFI_PASS "wangyueyang"

WiFiClient client;
WiFiUDP udp;

LDC1614 ldc1614_H(0x2B);
LDC1614 ldc1614_L(0x2A);

float data_send[4] = {0.0};
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 1;

unsigned long packet_id = 0;

IPAddress serverIP(100,64,144,87);
uint16_t serverPort = 10000;


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
  Serial.println("[INFO] begin");

  // ldc1614_H.init();
  ldc1614_L.init();

  wifi_connect();
}

void loop() {
  // if (!server_discovered) return;

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

    ldc1614_L.getData(4);
    for (int i = 0; i < 4; i++) {
      data_send[i] = ldc1614_L.L[i];
    }

    String payload = "Ldata," + String(packet_id++) + "," + floatToString(data_send, 4);
    payload += "," + floatToString(data_send, 4);
    payload += "," + floatToString(data_send, 4);
    payload += "," + floatToString(data_send, 4);
    payload += "\n";
    client.print(payload);
    client.flush();  // 强制立刻发送

    Serial.print("[SEND] ");
    Serial.print(payload);
  }
}
