
#include "common.h"
#include "include.h"
#include "motor.h"

int16  NumPulseFTM, NumPulselptmr;//��������Ĵ�����ֵ
int32  RightMotorPulseAccumulate;
int32  LeftMotorPulseAccumulate;
int32  GetRightMotorPules, GetLeftMotorPules; //��õ�һ����������������PID��������ݴ���



/****�����ʼ��******/

void MotorInit(void)
{
	gpio_init(PTB18, GPO, 0);//0��ת��1��ת
	ftm_pwm_init(FTM0, FTM_CH0, 1000, 0);//PTC1���ҵ��

	gpio_init(PTB9, GPO, 0);
	ftm_pwm_init(FTM0, FTM_CH1, 1000, 0);//PTC2,����  
}



#if OpenLoop//�������Ʒ�ʽ

int32  OpendLoopPwmL; //������ߵ����PWMֵ
int32  OpendLoopPwmR; //�����ұߵ����PWMֵ


//�������Ʒ�Ϊ������λ��ͨ��3�Ų��뿪��ѡ��
//1.ռ�ձȲ����Ŀ�������  2.�������٣�����ƫ�����

void MotorControlOpenLoop(void)
{
	int ErrorError = 0;

	if (DialSwitch_3)
	{
		ErrorError = Error*Error;

		if (Error*Error >= 300)  ErrorError = 300;

		OpendLoopPwmR = 650 - (ErrorError);
		OpendLoopPwmL = 650 - (ErrorError);

		//����
		OpendLoopPwmL = (int)(OpendLoopPwmL - (OpendLoopPwmL*Error*000.1));
		if (OpendLoopPwmL >= 890)  OpendLoopPwmL = 890;
		if (OpendLoopPwmL <= 200)    OpendLoopPwmL = 200;
		OpendLoopPwmR = (int)(OpendLoopPwmR + OpendLoopPwmR*Error*000.1);

		if (OpendLoopPwmR >= 890)  OpendLoopPwmR = 890;
		if (OpendLoopPwmR <= 200)    OpendLoopPwmR = 200;

		ftm_pwm_duty(FTM0, FTM_CH0, OpendLoopPwmR);
		ftm_pwm_duty(FTM0, FTM_CH1, OpendLoopPwmL); //PTC2,���� 
	}
	else if (!DialSwitch_3)
	{
		OpendLoopPwmR = 300;
		OpendLoopPwmL = 300;

		//����
		OpendLoopPwmL = (int)(OpendLoopPwmL - (OpendLoopPwmL*Error*000.1));

		if (OpendLoopPwmL >= 890)  OpendLoopPwmL = 890;
		if (OpendLoopPwmL <= 200)    OpendLoopPwmL = 200;

		OpendLoopPwmR = (int)(OpendLoopPwmR + (OpendLoopPwmR*Error*000.1));

		if (OpendLoopPwmR >= 890)  OpendLoopPwmR = 890;
		if (OpendLoopPwmR <= 200)    OpendLoopPwmR = 200;

		ftm_pwm_duty(FTM0, FTM_CH0, OpendLoopPwmR);
		ftm_pwm_duty(FTM0, FTM_CH1, OpendLoopPwmL); //PTC2,���� 

	}

}


#endif





#if CloseLoop //�ջ�����



int32 LSpeedSet = 0;//60;//2.5M/S
int32 RSpeedSet = 0;
int32 SpeedSet = 0;

float SpeedErrorL = 0;
float SpeedErrorR = 0;

float SpeedLastErrorL = 0;
float SpeedLastErrorR = 0;

float SpeedPerErrorL = 0;
float SpeedPerErrorR = 0;

int Error1 = 0, Error1Last = 0;
int SlowSpeed = 0;
int Slowing = 0;

#if 1

float  SpeedP = 75.0;//50.0;40
float  SpeedI = 0.0006;//16.0;50,0.0006
float  SpeedD = 9.0;//1.3,10.0

#endif


float  Differential_P = 0.0;//0.0250,0.1050

int MotorPwmL = 0;
int MotorPwmR = 0;

int MotorPwmRight = 0;
int MotorPwmLeft = 0;

float LeftMotorPwmAdd = 0;
float RightMotorPwmAdd = 0;



void GetTargetSpeed(void)
{

	static int Num = 0;

	if (DialSwitch_2)//���Ų��뿪�����ϲ�
	{
		SpeedP = 16.0;//50.0;40
		SpeedI = 0.0006;//16.0;50,0.0006
		SpeedD = 0.0;//1.3,10.0

		if (Loop.StrightIntoLoop)
		{
			SpeedSet = 70;//50;
		}

		else if (Tracktype.LongStraightaway)
		{
			SpeedSet = 80;//70;
		}

		else if (Tracktype.ShortStraightaway&&Tracktype.LongStraightawayIntoBend == 0)
		{
			SpeedSet = 50;//50;
		}

		else if (Tracktype.ShortStraightaway&&Tracktype.LongStraightawayIntoBend)
		{
			SlowSpeed = 1;
		}


		else
		{
			SpeedSet = 40;//50;
		}

		if (SlowSpeed == 1)
		{
			SpeedSet = 40;//45;

			Num++;
			if (Num >= 20)
			{
				SlowSpeed = 0;
				Num = 0;
				Tracktype.LongStraightawayIntoBend = 0;
			}
		}
		if (ABS(Error) >= 10) //ƫ�����ĳ��ֵ�Ž��в���
		{
			Differential_P = 0.058; //(float)(30.0/((60-(LastLine+4))*(60-(LastLine+4))));//30
			LSpeedSet = (int32)(SpeedSet - (Differential_P*Error*SpeedSet));//���ֲ���
			if (LSpeedSet <= 40)    LSpeedSet = 40;
			if (LSpeedSet >= 350)  LSpeedSet = 350;
			RSpeedSet = (int32)(SpeedSet + (Differential_P*Error*SpeedSet));//���ֲ���
			if (RSpeedSet <= 40)    RSpeedSet = 40;
			if (RSpeedSet >= 350)  RSpeedSet = 350;
		}
		else
		{
			LSpeedSet = SpeedSet;
			RSpeedSet = SpeedSet;
		}

	}

	else if (!DialSwitch_2)//���Ų��뿪�����ϲ�
	{
		SpeedP = 15.0;//50.0;40
		SpeedI = 0.0006;//16.0;50,0.0006
		SpeedD = 0.0;//1.3,10.0


		if (Loop.StrightIntoLoop)
		{
			SpeedSet = 50;
		}
		else
		{
			SpeedSet = 50;

		}

		if (ABS(Error) >= 10)
		{
			//Differential_P=(float)(30.0/((60-(AvaliableLines+10))*(60-(AvaliableLines+10))));
			 //SpeedSet=50;

			Differential_P = 0.0380;//�����٣���̫�������
			LSpeedSet = (int32)(SpeedSet - (Differential_P*Error*SpeedSet));//���ֲ���
			if (LSpeedSet <= 40)    LSpeedSet = 40;
			if (LSpeedSet >= 350)  LSpeedSet = 350;
			RSpeedSet = (int32)(SpeedSet + (Differential_P*Error*SpeedSet));//���ֲ���
			if (RSpeedSet <= 40)    RSpeedSet = 40;
			if (RSpeedSet >= 350)  RSpeedSet = 350;
		}
		else
		{
			LSpeedSet = SpeedSet;
			RSpeedSet = SpeedSet;
		}
	}

	//  if(Tracktype.LongStraightaway)
	//  {
	//      BuzzerRing;
	//  }
	//  else
	//  {
	//    BuzzerQuiet;//�������廷�����Ʊ�־������������ͻ᲻����
	//  }
	//      
}



//�����ٶ�ƫ��

void CalculateMotorSpeedError(float LeftMotorTarget, float RightMotorTarget)
{
	SpeedPerErrorL = SpeedLastErrorL;
	SpeedLastErrorL = SpeedErrorL;
	SpeedErrorL = LeftMotorTarget - GetLeftMotorPules;
	SpeedPerErrorR = SpeedLastErrorR;
	SpeedLastErrorR = SpeedErrorR;
	SpeedErrorR = RightMotorTarget - GetRightMotorPules;
}

//����ʽPID�����㷨

void MotorControl(void)
{
	if (!EndLineFlag)
	{
		GetTargetSpeed();
		CalculateMotorSpeedError(LSpeedSet, RSpeedSet);//�趨Ŀ���ٶȼ���ƫ��
		MotorPwmR += (SpeedP + SpeedI + SpeedD)*SpeedErrorR - (SpeedP + 2 * SpeedD)*SpeedLastErrorR + SpeedD*SpeedPerErrorR;
		MotorPwmRight = (int)(MotorPwmR);
		MotorPwmL += (SpeedP + SpeedI + SpeedD)*SpeedErrorL - (SpeedP + 2 * SpeedD)*SpeedLastErrorL + SpeedD*SpeedPerErrorL;
		MotorPwmLeft = (int)(MotorPwmL);
		if (MotorPwmLeft <= 0)               MotorPwmLeft = 0;
		if (MotorPwmLeft >= 990)             MotorPwmLeft = 990;
		if (MotorPwmRight <= 0)              MotorPwmRight = 0;
		if (MotorPwmRight >= 990)            MotorPwmRight = 990;
		ftm_pwm_duty(FTM0, FTM_CH0, MotorPwmRight);
		ftm_pwm_duty(FTM0, FTM_CH1, MotorPwmLeft); //PTC2,����       
	}
	else
	{

		ftm_pwm_duty(FTM0, FTM_CH0, 0);
		ftm_pwm_duty(FTM0, FTM_CH1, 0); //PTC2,����       
	}
}


#endif







void SpeedGet(void)

{
	NumPulselptmr = lptmr_pulse_get();
	lptmr_pulse_clean();//�����Ĵ�������
	LeftMotorPulseAccumulate += NumPulselptmr;//��ÿһ��PIT��ʱʱ����������ۼӵ�������������pulse_period_L  


	NumPulseFTM = ftm_quad_get(FTM2);//ȡ�Ĵ�����ֵ
	ftm_quad_clean(FTM2);//�����Ĵ�������
	RightMotorPulseAccumulate += NumPulseFTM;//��������    ��������������壩���� 
}


void PIT0_IRQHandler()
{

	static uint16  TimerCnt8ms = 0;

	disable_irq(PIT0_IRQn);            //��PIT�ж�

	PIT_Flag_Clear(PIT0);                           //���жϱ�־λ

	TimerCnt8ms++;

	SpeedGet();                        //ÿ�ν����ۼ�һ���ٶ�

	if (TimerCnt8ms >= MotorControlPeriod)//һ��8ms����ȡһ���ٶȣ����������ٶ�

	{

		GetRightMotorPules = -RightMotorPulseAccumulate;  //���ֺ���β�õ�������10msһ����õ�����
		GetLeftMotorPules = LeftMotorPulseAccumulate;

		TimerCnt8ms = 0;//����

		RightMotorPulseAccumulate = 0;//�ۼ�ֵ���㣬
		LeftMotorPulseAccumulate = 0;
	}

	enable_irq(PIT0_IRQn);//ʹ���ж�	
}





