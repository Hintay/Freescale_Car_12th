#include "common.h"
#include "include.h"

#if BuzzerOpen //如果开启了蜂鸣器宏定义

//蜂鸣器初始化函数
void BuzzerInit()
{
	gpio_init(PTB19, GPO, 0);//蜂鸣器初始化
}

void BuzzerTest(unsigned char flag)
{
	unsigned char Flag = 0;
	Flag = 0;
	Flag = flag;
	if (Flag)
	{
		BuzzerRing;
	}

	else
	{
		BuzzerQuiet;
	}
}

#endif


#if LedOpen

//Led初始化
void LedInit()
{
	gpio_init(PTB20, GPO, 1);//左上
	gpio_init(PTB21, GPO, 1);//左下
	gpio_init(PTB22, GPO, 1);//右下
	gpio_init(PTB23, GPO, 1);//右上
}

//用左上角的led去测试标志位
void  LeftUpLedTest(unsigned char flag)
{

	unsigned char Flag = 0;
	Flag = 0;
	Flag = flag;
	if (Flag)
	{
		LeftUpLedBright;
	}
	else
	{
		LeftUpLedDark;
	}
}

//用左下角的led去测试标志位

void LeftUnderLedTest(unsigned char flag)
{

	unsigned char Flag = 0;
	Flag = 0;
	Flag = flag;
	if (Flag)
	{
		LeftUnderLedBright;
	}
	else
	{
		LeftUnderLedDark;
	}
}

//用右上角的led去测试标志位
void RightUpLedTest(unsigned char flag)
{

	unsigned char Flag = 0;
	Flag = 0;
	Flag = flag;
	if (Flag)
	{
		RightUpLedBright;
	}
	else
	{
		RightUpLedDark;
	}
}

//用右下角的led去测试标志位

void RightUnderLedTest(unsigned char flag)
{

	unsigned char Flag = 0;
	Flag = 0;
	Flag = flag;
	if (Flag)
	{
		RightUnderLedBright;
	}
	else
	{
		RightUnderLedDark;
	}
}

#endif