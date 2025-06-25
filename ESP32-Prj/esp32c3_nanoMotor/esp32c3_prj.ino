#include <Wire.h>


int GT9764AF_read(uint8_t reg, uint8_t * data)
{
  uint8_t byte;

  Wire.beginTransmission(0x0C);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) { // 保持总线活跃
    Serial.println("I2C Byte Error");
    return -1;
  }

  byte = Wire.requestFrom(0x0C, 1);
  //Serial.printf("request byte num=%x\n", byte);

  uint8_t DATA;
  DATA = Wire.read();  //接受并读取从设备发来的一个字节的数据
  *data = DATA;   

  //Serial.println("Read OK");
  return 0;

}

void GT9764AF_init()//一个写寄存器函数，可以将配置信息写入LDC1614
{
  Wire.begin();
  uint8_t init_cmds[][2] = {
    {0x02, 0x00},
    {0x02, 0x02},
    {0x06, 0x40},
    {0x07, 0x78}
  };
  Wire.beginTransmission(0x0C);//将数据传送到设备0X2A
  Wire.write(init_cmds[0], 2);
  Wire.endTransmission(); //停止传送
  delay(2);
}

void GT9764AF_write(uint8_t reg, uint8_t data)//一个写寄存器函数，可以将配置信息写入LDC1614
{

  Wire.beginTransmission(0x0C);//将数据传送到设备0X2A
  Wire.write(reg);
  Wire.write(data);
  Wire.endTransmission(); //停止传送
  delay(2);
}



void setup() {
  Serial.begin(115200);
  Serial.println("begin");

  GT9764AF_init();
  GT9764AF_write(0x03, 0x00);
  GT9764AF_write(0x04, 0x00);
  GT9764AF_write(0x0B, 0x00); //100mA

  uint8_t data;
  GT9764AF_read(0x00, &data);
  Serial.printf("IC Info: %x\n", data);
  GT9764AF_read(0x0B, &data);
  Serial.printf("Control_V: %x\n", data);
  

  GT9764AF_read(0x03, &data);
  Serial.println(data);
  GT9764AF_read(0x04, &data);
  Serial.println(data);
  
}


void loop() {

  GT9764AF_write(0x03, 0x00);
  GT9764AF_write(0x04, 0xC4);
  delay(500);

  GT9764AF_write(0x03, 0x03);
  GT9764AF_write(0x04, 0x3B);
  delay(500);

  // Serial.println("RUN");
  // delay(5000);


}



