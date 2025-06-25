#ifndef LDC1614_H_
#define LDC1614_H_

#include <Arduino.h>
#include <Wire.h>

#define LDC1614_DATA_MSB_CH0  0x00//定义LDC1614四个通道的位置
#define LDC1614_DATA_LSB_CH0  0x01
#define LDC1614_DATA_MSB_CH1  0x02
#define LDC1614_DATA_LSB_CH1  0x03
#define LDC1614_DATA_MSB_CH2  0x04
#define LDC1614_DATA_LSB_CH2  0x05
#define LDC1614_DATA_MSB_CH3  0x06
#define LDC1614_DATA_LSB_CH3  0x07
#define CH0_FIN_DIVIDER  1  
#define CH1_FIN_DIVIDER  1 
#define CH2_FIN_DIVIDER  1 
#define CH3_FIN_DIVIDER  1  
#define CH0_FREF_DIVIDER  2
#define CH1_FREF_DIVIDER  2
#define CH2_FREF_DIVIDER  2
#define CH3_FREF_DIVIDER  2
#define CH0_OFFSET   0         //CHx_OFFSET设置为0
#define CH1_OFFSET   0  
#define CH2_OFFSET   0  
#define CH3_OFFSET   0  


class LDC1614
{
  public:
    void writeRegister(uint8_t reg, uint16_t data);
    int readRegister(uint8_t reg, uint16_t * data);
    int8_t readChannel(uint8_t channel, uint32_t *data);
    void init(void);
    uint32_t readData(uint8_t Channel);
    float readf_SEN(uint8_t Channel);
    float readL(uint8_t Channel);
    void getData(int n);

    uint8_t f_CLKIN =  40; //外部参考晶振40MHz。
    uint8_t f_REF   =  20; //分频后的参考频率20MHz
    uint8_t C       = 140; //单位pF

    uint32_t data[4]  = {0}; //传感器原始数据
    float    f_SEN[4] = {0}; //传感器的频率(传感线圈谐振频率)
    float    L[4]     = {0}; //传感线圈的电感值

};



#endif
