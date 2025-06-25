#include <Wire.h>
#include <WiFi.h>
#include "LDC1614.h"
#include "myTimer.h"


#define AP_SSID "mywifi" 
#define AP_PSW  "wangyueyang"

#define N 3
float buffer[N], sum=0, filt_data[4], buffer2[N],filt_data2[4];
const float weights[N] = {0.25,0.5,0.25}; // 对称权重
//const float weights[N] = {0.2,0.3,0.3,0.2}; // 对称权重


WiFiClient client; //声明一个ESP32客户端对象，用于与服务器进行连接
LDC1614 ldc1614;

//创建联合体，将数据从float对齐到char
typedef union ldc_val{
	float data_float[3]={0.0};
	char  data_byte[12];
}LDC1614_VALUE_t;
LDC1614_VALUE_t ldc1614_val;

const IPAddress serverIP(172,20,10,2); //欲访问的服务端IP地址
uint16_t serverPort = 10000;


void wifi_connect()
{
  WiFi.begin(AP_SSID, AP_PSW);
  Serial.print(String("Connecting to ") + AP_SSID);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); //输出station IP地址，这里的IP地址由DHCP分配
}


float moving_average(float new_data) {
  static int index = 0;
  sum += new_data - buffer[index];
  buffer[index] = new_data;
  index = (index + 1) % N;
  return sum / N;
}

float weighted_moving_average(float new_data) {
  static int index = 0;
  buffer2[index] = new_data;
  index = (index + 1) % N;
  
  float sum = 0;
  for(int i=0; i<N; i++) {
      int pos = (index + i) % N; // 从旧到新循环取数据
      sum += buffer2[pos] * weights[i];
  }
  return sum;
}


void setup() {
  Serial.begin(115200);
  Serial.println("begin init");

  ldc1614.init();   //初始化LDC1614
  //wifi_connect(); //连接WIFI
  //timer_init();   //开启定时器中断
  
  Serial.println("init finish");
  delay(3000);

}

void loop() {

  ldc1614.getData(4);
  for(int i=0;i<4;i++)
  {
    Serial.printf("%.3f ", ldc1614.L[i]);
    // filt_data[i] = moving_average(ldc1614.L[i]);
    // filt_data2[i] = weighted_moving_average(ldc1614.L[i]);

  }
  //Serial.printf("%.3f ", ldc1614.L[0]);
  //Serial.printf("%.3f ", filt_data[0]);
  //Serial.printf("%.3f ", filt_data2[0]);
  Serial.println();

  delay(50);



  // //向服务端发送传感器数据
  // Serial.println("尝试访问服务器");
  // if (client.connect(serverIP, serverPort)) //尝试访问目标地址
  // {
  //   Serial.println("访问成功");
  //   client.print("Hello world!");                    //向服务器发送数据
  //   while (client.connected()) //如果已连接或有收到的未读取的数据
  //   {
  //     if (client.available()) //如果有数据可读取
  //     {
  //       String line = client.readStringUntil('\n'); //读取数据到换行符
  //       Serial.print("读取到数据：");
  //       Serial.println(line);
  //       client.write(line.c_str()); //将收到的数据回发
  //     } 
  //     else 
  //     {
  //       ldc1614.getData(3);
  //       for(int i=0;i<3;i++)
  //       {
  //         ldc1614_val.data_float[i] = ldc1614.L[i];
  //         Serial.print(ldc1614_val.data_float[i], 3);
  //         Serial.print(" ");
  //         Serial.print(" | ");
  //       }
  //       Serial.println();
  //       client.write(0xAA);
  //       delay(3);
  //       client.write(ldc1614_val.data_byte);
  //     }
  //     delay(3);
      
  //   }
  //   Serial.println("关闭当前连接");
  //   client.stop(); //关闭客户端
  // }
  // else
  // {
  //     Serial.println("访问失败");
  //     client.stop(); //关闭客户端
  // }
  // delay(2000);


}



