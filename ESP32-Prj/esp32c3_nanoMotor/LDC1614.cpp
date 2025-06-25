#include "ldc1614.h"


void LDC1614::writeRegister(uint8_t reg, uint16_t data)//一个写寄存器函数，可以将配置信息写入LDC1614
{
  Wire.beginTransmission(0x2A);//将数据传送到设备0X2A
  Wire.write(reg);
  Wire.write(data >> 8);//将十六位data右移
  Wire.write(data);
  delay(2);
  Wire.endTransmission(); //停止传送
}

int LDC1614::readRegister(uint8_t reg, uint16_t * data)//读寄存器函数
{
  Wire.beginTransmission(0x2A);
  Wire.write(reg);
  Wire.endTransmission();

  delayMicroseconds(2);

  int bytes = Wire.requestFrom(0x2A, 2);//从设备0x2a请求2个字节
  
  if(bytes != 2)
    return -1; // Error

  uint8_t MSB = Wire.read();  //接受并读取从设备发来的一个字节的数据
  uint8_t LSB = Wire.read();

  *data =  (MSB << 8) | LSB;   

  return 0;
}

int8_t LDC1614::readChannel(uint8_t channel, uint32_t *data)//从0到3通道读寄存器
{ 
  uint8_t error = 0;
  if(channel > 3)
    return -1;  
  uint16_t MSB, LSB;
  readRegister(LDC1614_DATA_MSB_CH0 + 2*channel, &MSB);
  readRegister(LDC1614_DATA_LSB_CH0 + 2*channel, &LSB);
  error = MSB >> 12;
  *data = (((uint32_t)(MSB & 0x0FFF)) << 16) | LSB;
  
  return error;
}

uint32_t LDC1614::readData(uint8_t Channel)
{
  readChannel(Channel, &data[Channel]); 
  return data[Channel];
}

float LDC1614::readf_SEN(uint8_t Channel)
{
  f_SEN[Channel] = f_REF*readData(Channel)/268435456.0; //2^28=268435456
  return f_SEN[Channel];
}

float LDC1614::readL(uint8_t Channel)
{
  L[Channel] = 1000000.0/( C*pow(2.0*3.14*f_SEN[Channel], 2) ); //uH
  return L[Channel];
}

void LDC1614::getData(int n)
{
  for(int i=0;i<n;i++)
  {
    readChannel(i, &data[i]); 
    f_SEN[i] = f_REF*data[i]/268435456.0; //2^28=268435456
    L[i] = 1000000.0/( C*pow(2.0*3.14*f_SEN[i], 2) );
  }

}

void LDC1614::init()
{
  Wire.begin();  
  //进入睡眠模式，以开始初始化
  writeRegister(0x1C , 0x8000); 
  //配置多通道模式，共四个通道
  writeRegister(0x1A , 0x3681);//通道0
  writeRegister(0x1B , 0x820D);//通道1  
  writeRegister(0x1B , 0xA20D);//通道2
  writeRegister(0x1B , 0xC20D);//通道3
  //设置分频（Frequency Divider）.FIN_DIVIDER与Fref_DIVIDER都设置为1 (前1后2)
  writeRegister(0x14 , 0x1002);//通道0
  writeRegister(0x15 , 0x1002);//通道1  
  writeRegister(0x16 , 0x1002);//通道2
  writeRegister(0x17 , 0x1002);//通道3
  //设置驱动电流，drive current code=30
  writeRegister(0x1E , 0xF000);//通道0
  writeRegister(0x1F , 0xF000);//通道1  
  writeRegister(0x20 , 0xF000);//通道2
  writeRegister(0x21 , 0xF000);//通道3
  //SettleCount 准备计数为100个周期
  writeRegister(0x10 , 0x00FF);//通道0
  writeRegister(0x11 , 0x00FF);//通道1
  writeRegister(0x12 , 0x00FF);//通道2
  writeRegister(0x13 , 0x00FF);//通道3
  //Referece Count 采样计数为4096个周期
  writeRegister(0x08 , 0x2000);//通道0
  writeRegister(0x09 , 0x2000);//通道1  
  writeRegister(0x0A , 0x2000);//通道2 
  writeRegister(0x0B , 0x2000);//通道3 
  //设置频率偏置。默认值为0
  writeRegister(0x0C , 0x0000);//通道0
  writeRegister(0x0D , 0x0000);//通道1  
  writeRegister(0x0E , 0x0000);//通道2
  writeRegister(0x0F , 0x0000);//通道3
  //退出睡眠模式，开始测量数据
  writeRegister(0x1A , 0x5681); 

}



