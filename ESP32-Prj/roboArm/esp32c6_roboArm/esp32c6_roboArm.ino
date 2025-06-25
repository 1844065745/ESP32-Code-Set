#include <Wire.h>
#include "LDC1614.h"
#include "myTimer.h"


LDC1614 ldc1614;


int   L_diff[3]  = {0};
float L_0[3]   = {0.0};
float L_1[3]   = {0.0};
float param_bias[3] = {10.0, 15.0, 20.0};


void setup() {
  Serial.begin(9600);

  ldc1614.init();   //初始化LDC1614
  //timer_init();   //开启定时器中断

  Serial.println("Init Finished!");


  // 线圈初始位置数据
  ldc1614.getData(3);
  for(int i=0;i<3;i++)
  {
    L_0[i] = ldc1614.L[i];
    Serial.printf("%.3f ", L_0[i]);
  }
  Serial.println();

  delay(5); // 适当间隔一定时间

}

void loop() {

  // 采集线圈数据
  ldc1614.getData(3);
  for(int i=0;i<3;i++)
  {
    L_1[i] = ldc1614.L[i];
    Serial.printf("%.3f,", L_1[i]);
  }
  Serial.println();



  // //计算差值
  // for(int i=0;i<3;i++){
  //   L_diff[i] = L_1[i] - L_0[i];
  //   //Serial.printf("%.3f ", L_diff[i]);
  // }
  // //Serial.println();

  // if(L_diff[0]<0 && L_diff[1]>0 && L_diff[2]>0) // 向前
  // {
  //   Serial.print(1);
  // }
  // else if(L_diff[0]>0 && L_diff[1]<0 && L_diff[2]<0) // 向后
  // {
  //   Serial.print(2);
  // }

  // else if(L_diff[0]>0 && L_diff[1]<0 && L_diff[2]>0) // 向左
  // {
  //   Serial.print(3);
  // }
  // else if(L_diff[0]<0 && L_diff[1]>0 && L_diff[2]<0) // 向右
  // {
  //   Serial.print(4);
  // }
  // else
  // {
  //   Serial.print(0);
  // }

  delay(200); // 适当间隔一定时间

}



