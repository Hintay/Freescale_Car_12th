#ifndef __TEST_H__
#define __TEST_H__

#define BuzzerOpen  1 //打开蜂鸣器

#define LedOpen     1 //打开Led

extern void BuzzerInit();
extern void LedInit();
extern void BuzzerTest(unsigned char flag);
extern void LeftUpLedTest(unsigned char flag);
extern void LeftUnderLedTest(unsigned char flag);
extern void RightUnderLedTest(unsigned char flag);
extern void RightUpLedTest(unsigned char flag);


#define  BuzzerRing           gpio_set   (PTB19, 1) //蜂鸣器响
#define  BuzzerQuiet          gpio_set   (PTB19, 0) //蜂鸣器不响

#define  LeftUpLedBright      gpio_set   (PTB20, 0) //左上的Led亮 
#define  LeftUpLedDark        gpio_set   (PTB20, 1) //左上的Led暗 

#define  LeftUnderLedBright   gpio_set   (PTB21, 0) //左下的Led亮 
#define  LeftUnderLedDark     gpio_set   (PTB21, 1) //左下的Led暗 

#define  RightUnderLedBright  gpio_set   (PTB22, 0) //右下的Led亮 
#define  RightUnderLedDark    gpio_set   (PTB22, 1) //右下的Led暗 

#define  RightUpLedBright     gpio_set   (PTB23, 0) //右上的Led亮 
#define  RightUpLedDark       gpio_set   (PTB23, 1) //右上的Led暗 


#endif