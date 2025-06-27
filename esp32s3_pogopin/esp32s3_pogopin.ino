#include <WiFi.h>
#include <WiFiUdp.h>
#include "LDC1614.h"

#define WIFI_SSID "mywifi"
#define WIFI_PASS "wangyueyang"

WiFiClient client;
WiFiUDP udp;

LDC1614 ldc1614_H(0x2B);
LDC1614 ldc1614_L(0x2A);

float data_send[4] = {0.0};
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 50;

unsigned long packet_id = 0;

IPAddress serverIP(100,64,171,47);
uint16_t serverPort = 10000;

bool server_discovered = false;

// UDP 监听端口（与 Python 广播端口一致）
const int udp_port = 8888;

String floatToString(float *arr, int len) {
  String result = "";
  for (int i = 0; i < len; i++) {
    result += String(arr[i], 3);
    if (i < len - 1) result += ",";
  }
  return result;
}

// 从广播包中提取 IP:Port
bool parseServerAddress(String msg) {
  int sep = msg.indexOf(':');
  if (sep == -1) return false;

  String ipStr = msg.substring(0, sep);
  String portStr = msg.substring(sep + 1);
  serverIP.fromString(ipStr);
  serverPort = portStr.toInt();
  return true;
}

// UDP 监听广播，获取服务端 IP
void discoverServer() {
  Serial.println("[UDP] Listening for server broadcast...");

  udp.begin(udp_port);
  unsigned long start = millis();

  while (!server_discovered && millis() - start < 10000) { // 最多等待 10 秒
    int packetSize = udp.parsePacket();
    if (packetSize) {
      char buffer[64];
      int len = udp.read(buffer, sizeof(buffer) - 1);
      buffer[len] = '\0';

      String msg = String(buffer);
      Serial.print("[UDP] Received broadcast: ");
      Serial.println(msg);

      if (parseServerAddress(msg)) {
        server_discovered = true;
        Serial.print("[INFO] Server IP: ");
        Serial.println(serverIP);
        Serial.print("[INFO] Server Port: ");
        Serial.println(serverPort);
        break;
      }
    }
    delay(200);
  }

  udp.stop();

  if (!server_discovered) {
    Serial.println("[ERROR] Failed to discover server via UDP.");
  }
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
  // discoverServer(); // UDP 获取服务器地址
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
    payload += "\n";
    client.print(payload);
    client.flush();  // 强制立刻发送

    Serial.print("[SEND] ");
    Serial.print(payload);
  }
}
