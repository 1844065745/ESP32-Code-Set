#include "myTimer.h"

//定时器中断服务函数，为使编译器将代码分配到IRAM内，中断处理程序应该具有 IRAM_ATTR 属性
void IRAM_ATTR TimerEvent()
{
  static unsigned short cnt=0;
  if(cnt++%2==0)
  {
    Serial.println("interrupt1");
    //digitalWrite(8, HIGH); //IO8与IIC引脚冲突
  }else{
    Serial.println("interrupt0");
    //digitalWrite(8, LOW);
  }

}

void timer_init()
{
  hw_timer_t *timer = NULL; //注册定时器中断

  // timer = timerBegin(0, 80, true); //预分频系数80
  // timerAttachInterrupt(timer, &TimerEvent, true);
  // timerAlarmWrite(timer, 1000000, true); //1000000us
  // timerAlarmEnable(timer); //使能定时器

  timer = timerBegin(1000000);
  timerAttachInterrupt(timer, &TimerEvent);
  timerAlarm(timer, 1000000, true, 0);
}


