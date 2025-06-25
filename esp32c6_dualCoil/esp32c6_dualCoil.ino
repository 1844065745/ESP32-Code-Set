#include <Wire.h>
#include <WiFi.h>
#include "LDC1614.h"
#include "myTimer.h"


#define AP_SSID "mywifi" 
#define AP_PSW  "wangyueyang"

WiFiClient client; //声明一个ESP32客户端对象，用于与服务器进行连接
LDC1614 ldc1614_H(0X2B);
LDC1614 ldc1614_L(0X2A);

//创建联合体，将数据从float对齐到char
//经测试，转换后默认会在后面加4字节数据
#define CHNN_NUM 8      //采集通道数
typedef union ldc_val{
	float data_float[CHNN_NUM]={0.0};
	char  data_byte[(CHNN_NUM)*4];

}LDC1614_VALUE_t;
LDC1614_VALUE_t ldc1614_val;

const IPAddress serverIP(172,20,10,3); //欲访问的服务端IP地址
uint16_t serverPort = 10000;


void wifi_connect()
{
  WiFi.begin(AP_SSID, AP_PSW);
  Serial.print(String("Connecting to ") + AP_SSID);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println(WiFi.status());
  }
  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP()); //输出station IP地址，这里的IP地址由DHCP分配



}

void setup() {
  Serial.begin(9600);
  Serial.println("begin INIT");

  // pinMode(8, OUTPUT);
  // digitalWrite(8, 0);

  //ldc1614_H.init();   //初始化LDC1614
  ldc1614_L.init();   //初始化LDC1614
  //wifi_connect(); //连接WIFI
  //timer_init();   //开启定时器中断
  
  Serial.println("begin PRJ");
  delay(3000);

}

void loop() {

  delay(100);

  // // 0X2A地址处LDC1614
  // ldc1614_H.getData(1);
  // for(int i=0;i<1;i++)
  // {
  //   Serial.printf("%.3f ", ldc1614_H.L[i]);
  //   Serial.print(" | ");
  // }
  // Serial.println();


  // 0X2B地址处LDC1614
  ldc1614_L.getData(4);
  for(int i=0;i<4;i++)
  {
    Serial.printf("%.3f ", ldc1614_L.L[i]);
  }
  Serial.println();




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
  //       ldc1614_H.getData(4);
  //       for(int i=0;i<4;i++)
  //       {
  //         ldc1614_val.data_float[i] = ldc1614_H.L[i];
  //         Serial.print(ldc1614_val.data_float[i], 3);
  //         Serial.print(" ");
  //         //Serial.print(" | ");
  //       }
  //       ldc1614_L.getData(4);
  //       for(int i=4;i<8;i++)
  //       {
  //         ldc1614_val.data_float[i] = ldc1614_L.L[i-4];
  //         Serial.print(ldc1614_val.data_float[i], 3);
  //         Serial.print(" ");
  //         //Serial.print(" | ");
  //       }
  //       Serial.println();
  //       client.write(0xAA);
  //       //delay(3);
  //       client.write(ldc1614_val.data_byte);
  //     }
  //     delay(50);
      
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



