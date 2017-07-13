#include "common.h"
#include "include.h"
#include "steer.h"

/*********define for SteerControl**********/

float KP = 0.0;//����������ϵ����Ӱ�����Ĵ�Ƿ�Χ
float KD = 0.0;//10//7.5//�������΢��ϵ��,Ӱ�����Ĵ�Ƿ�Ӧ
float SteerPwmAdd = 0.0;//���pwm����
float Error;//ƫ��ֵ
float LastError;//�ϴε�ƫ��
float WeightSum = 0;
float CenterMeanValue = 0;
float CenterSum = 0;
float J = 0.0300;//����p��ƫ��Ĺ�ϵ��Խ������Խǿ
float  JD=0.0400;//����p��ƫ��Ĺ�ϵ��Խ������Խǿ
float  BasicP=3; //������Pֵ
float BasicD = 6.5;//6.5 //������Pֵ
uint32 SteerPwm = 0, LastSteerSwm = 0;//�����pwmֵ���ϴζ����pwmֵ

float LastLineMaxInLoop = 30;

//��Ȩƽ����Ȩֵ��ѡȡ
#if 0
float Weight[60] = {
	0,0,0,0,0,0,0,0,0,0,						//0-9�У������ò���
	0,0,0,0,0,0,2,2,2,2,						//0-19�У������ò���

	3.8,3.8,1.8,1.8,2.8,3.3,2.3,3.3,3.3,3.3,	//20-29��

	//1.05,1.05,1.05,1.05,1.05,2,2,2,2,2,

	2,2,2,2,2,2.0,2.0,2.5,2.5,2.5,				//30-39��

	2.1,2.1,2.1,1,1,2,1,1,1,1,					//40-49��
	1,0,0,0,0,									//���ʮ��*///��ѹ�ߣ��������
};
#endif

#if 1
float Weight[60] = {
	0,0,0,0,0,0,0,0,0,0,		//0-9�У������ò���

	2,2,2,2,2,1.5,1,1.5,1,		//0-19�У������ò���

	2.5,1,2.2,1,2.5,1,2,2,1,2,	//20-29��

	1,3,2,2,1,2,1,2,2,1,		//30-39��

	3,1,2,3,3,2,3,1,1,			//40-49��

	1,1,1,1,1,1,1,1,1,1,		//���ʮ��
};
#endif




/******************************************************
 *��������SteerInit
 *
 *���ܣ������ʼ��
 *
 * ��ڲ�������
 *
 *���ز�������
 *
 * ���ߣ�XGL
 *
 * ���ڣ�2016-3-01(�Ѳ���)
 *******************************************************/


void SteerInit(void)//�����ʼ��
{
	ftm_pwm_init(FTM1, FTM_CH0, 50, SteerMidle);
}


#if 1
/******************************************************
 *��������CalculateError
 *
 *���ܣ��������
 *
 * ��ڲ�������
 *
 *���ز�������
 *
 * ���ߣ�XGL
 *
 * ���ڣ�2016-3-01(�Ѳ���)
 *******************************************************/


void CalculateError(void)
{
	int i;
	CenterSum = 0;
	CenterMeanValue = 0;
	WeightSum = 0;
	if (Cross.LeftSideling)
	{
		for (i = 58; i > InflectionPointL.InflectionPointRow; i--)
		{
			CenterSum += MiddleLine[i] * Weight[i];
			WeightSum += Weight[i];
		}
	}
	else if (Cross.RightSideling)
	{
		for (i = 58; i > InflectionPointR.InflectionPointRow; i--)
		{
			CenterSum += MiddleLine[i] * Weight[i];
			WeightSum += Weight[i];
		}
	}
	else if (StrightIntoCrossL)
	{
		for (i = 58; i > InflectionPointL.InflectionPointRow; i--)
		{
			CenterSum += MiddleLine[i] * Weight[i];
			WeightSum += Weight[i];
		}
	}
	else if (StrightIntoCrossR)
	{
		for (i = 58; i > InflectionPointR.InflectionPointRow; i--)
		{
			CenterSum += MiddleLine[i] * Weight[i];
			WeightSum += Weight[i];
		}
	}
	else if (StrightIntoCrossLR)
	{
		for (i = 58; i > (InflectionPointR.InflectionPointRow + InflectionPointR.InflectionPointRow) / 2; i--)
		{
			CenterSum += MiddleLine[i] * Weight[i];
			WeightSum += Weight[i];
		}
	}
	//else if(StrightIntoCrossNotLR)
	//{
		// for()
	//}
	else if (Cross.CrossFlag)
	{
		for (i = 45; i > LastLine; i--)
		{
			CenterSum += MiddleLine[i] * Weight[i];
			WeightSum += Weight[i];
		}

	}
	else
	{
		if (Loop.StrightIntoLoop && LastLine < (int)LastLineMaxInLoop)
		{
			LastLine = (int)LastLineMaxInLoop;
		}
		for (i = 57; i > LastLine; i--)

		{
			CenterSum += MiddleLine[i] * Weight[i];

			WeightSum += Weight[i];
		}
	}

	if (WeightSum != 0)
	{
		CenterMeanValue = (CenterSum / WeightSum);//�����Ȩƽ�������ߵ�ֵ
	}

	LastError = Error;

	Error = (40 - CenterMeanValue);// һ��ͼ��ƫ��ֵ 

	if (Error >= 30.0)//ƫ���޷�
		Error = 30.0;

	if (Error <= -30.0)
		Error = -30.0;

	KP = BasicP + (Error* Error)*J;//��̬����pģ��

	if (KP >= 13) KP = 13;//pֵ�޷�
	KD = BasicD + (Error* Error)*JD;

	if (KD >= 20) KD = 20;//pֵ�޷�
}

#endif



/******************************************************
 *��������SteerControl
 *
 *���ܣ��������
 *
 * ��ڲ�������
 *
 *���ز�������
 *
 * ���ߣ�XGL
 *
 * ���ڣ�2016-3-01(�Ѳ���)
 *******************************************************/



void SteerControl(void)
{
	CalculateError();

	SteerPwmAdd = (KP*Error) + KD*(Error - LastError);//�����pd������

	if (SteerPwmAdd >= 120)//120
		SteerPwmAdd = 120;//120

	if (SteerPwmAdd <= -120)//-120
		SteerPwmAdd = -120;//-120

	SteerPwm = (uint32)(SteerPwmAdd + SteerMidle);

	if (SteerPwm >= SteerMax)//�޷�
		SteerPwm = SteerMax;

	if (SteerPwm <= SteerMin)
		SteerPwm = SteerMin;

	ftm_pwm_duty(FTM1, FTM_CH0, SteerPwm);//���pwm����

	LastSteerSwm = SteerPwm;//��¼pwmֵ
}
