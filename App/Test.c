#include "common.h"
#include "include.h"

#if BuzzerOpen //��������˷������궨��

//��������ʼ������
void BuzzerInit()
{
	gpio_init(PTB19, GPO, 0);//��������ʼ��
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

//Led��ʼ��
void LedInit()
{
	gpio_init(PTB20, GPO, 1);//����
	gpio_init(PTB21, GPO, 1);//����
	gpio_init(PTB22, GPO, 1);//����
	gpio_init(PTB23, GPO, 1);//����
}

//�����Ͻǵ�ledȥ���Ա�־λ
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

//�����½ǵ�ledȥ���Ա�־λ

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

//�����Ͻǵ�ledȥ���Ա�־λ
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

//�����½ǵ�ledȥ���Ա�־λ

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