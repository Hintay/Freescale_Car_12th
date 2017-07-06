
#include "common.h"
#include "include.h"
#include "motor.h"

int16  NumPulseFTM, NumPulselptmr;//脉冲计数寄存器的值
int32  RightMotorPulseAccumulate;
int32  LeftMotorPulseAccumulate;
int32  GetRightMotorPules, GetLeftMotorPules; //获得的一次脉冲总数（用于PID电机的数据处理）



/****电机初始化******/

void MotorInit(void)
{
	gpio_init(PTB18, GPO, 0);//0正转，1反转
	ftm_pwm_init(FTM0, FTM_CH0, 1000, 0);//PTC1，右电机

	gpio_init(PTB9, GPO, 0);
	ftm_pwm_init(FTM0, FTM_CH1, 1000, 0);//PTC2,左电机  
}



#if OpenLoop//开环控制方式

int32  OpendLoopPwmL; //开环左边电机的PWM值
int32  OpendLoopPwmR; //开环右边电机的PWM值


//开环控制分为两个档位，通过3号拨码开关选择
//1.占空比不会变的开环匀速  2.开环变速，根据偏差拟和

void MotorControlOpenLoop(void)
{
	int ErrorError = 0;

	if (DialSwitch_3)
	{
		ErrorError = Error*Error;

		if (Error*Error >= 300)  ErrorError = 300;

		OpendLoopPwmR = 650 - (ErrorError);
		OpendLoopPwmL = 650 - (ErrorError);

		//差速
		OpendLoopPwmL = (int)(OpendLoopPwmL - (OpendLoopPwmL*Error*000.1));
		if (OpendLoopPwmL >= 890)  OpendLoopPwmL = 890;
		if (OpendLoopPwmL <= 200)    OpendLoopPwmL = 200;
		OpendLoopPwmR = (int)(OpendLoopPwmR + OpendLoopPwmR*Error*000.1);

		if (OpendLoopPwmR >= 890)  OpendLoopPwmR = 890;
		if (OpendLoopPwmR <= 200)    OpendLoopPwmR = 200;

		ftm_pwm_duty(FTM0, FTM_CH0, OpendLoopPwmR);
		ftm_pwm_duty(FTM0, FTM_CH1, OpendLoopPwmL); //PTC2,左电机 
	}
	else if (!DialSwitch_3)
	{
		OpendLoopPwmR = 300;
		OpendLoopPwmL = 300;

		//差速
		OpendLoopPwmL = (int)(OpendLoopPwmL - (OpendLoopPwmL*Error*000.1));

		if (OpendLoopPwmL >= 890)  OpendLoopPwmL = 890;
		if (OpendLoopPwmL <= 200)    OpendLoopPwmL = 200;

		OpendLoopPwmR = (int)(OpendLoopPwmR + (OpendLoopPwmR*Error*000.1));

		if (OpendLoopPwmR >= 890)  OpendLoopPwmR = 890;
		if (OpendLoopPwmR <= 200)    OpendLoopPwmR = 200;

		ftm_pwm_duty(FTM0, FTM_CH0, OpendLoopPwmR);
		ftm_pwm_duty(FTM0, FTM_CH1, OpendLoopPwmL); //PTC2,左电机 

	}

}


#endif





#if CloseLoop //闭环控制



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

	if (DialSwitch_2)//二号拨码开关往上波
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
		if (ABS(Error) >= 10) //偏差大于某个值才进行差速
		{
			Differential_P = 0.058; //(float)(30.0/((60-(LastLine+4))*(60-(LastLine+4))));//30
			LSpeedSet = (int32)(SpeedSet - (Differential_P*Error*SpeedSet));//左轮差速
			if (LSpeedSet <= 40)    LSpeedSet = 40;
			if (LSpeedSet >= 350)  LSpeedSet = 350;
			RSpeedSet = (int32)(SpeedSet + (Differential_P*Error*SpeedSet));//右轮差速
			if (RSpeedSet <= 40)    RSpeedSet = 40;
			if (RSpeedSet >= 350)  RSpeedSet = 350;
		}
		else
		{
			LSpeedSet = SpeedSet;
			RSpeedSet = SpeedSet;
		}

	}

	else if (!DialSwitch_2)//二号拨码开关往上波
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

			Differential_P = 0.0380;//调差速，调太大会跳轮
			LSpeedSet = (int32)(SpeedSet - (Differential_P*Error*SpeedSet));//左轮差速
			if (LSpeedSet <= 40)    LSpeedSet = 40;
			if (LSpeedSet >= 350)  LSpeedSet = 350;
			RSpeedSet = (int32)(SpeedSet + (Differential_P*Error*SpeedSet));//右轮差速
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
	//    BuzzerQuiet;//触发了清环道控制标志的条件，这里就会不响了
	//  }
	//      
}



//计算速度偏差

void CalculateMotorSpeedError(float LeftMotorTarget, float RightMotorTarget)
{
	SpeedPerErrorL = SpeedLastErrorL;
	SpeedLastErrorL = SpeedErrorL;
	SpeedErrorL = LeftMotorTarget - GetLeftMotorPules;
	SpeedPerErrorR = SpeedLastErrorR;
	SpeedLastErrorR = SpeedErrorR;
	SpeedErrorR = RightMotorTarget - GetRightMotorPules;
}

//增量式PID控制算法

void MotorControl(void)
{
	if (!EndLineFlag)
	{
		GetTargetSpeed();
		CalculateMotorSpeedError(LSpeedSet, RSpeedSet);//设定目标速度计算偏差
		MotorPwmR += (SpeedP + SpeedI + SpeedD)*SpeedErrorR - (SpeedP + 2 * SpeedD)*SpeedLastErrorR + SpeedD*SpeedPerErrorR;
		MotorPwmRight = (int)(MotorPwmR);
		MotorPwmL += (SpeedP + SpeedI + SpeedD)*SpeedErrorL - (SpeedP + 2 * SpeedD)*SpeedLastErrorL + SpeedD*SpeedPerErrorL;
		MotorPwmLeft = (int)(MotorPwmL);
		if (MotorPwmLeft <= 0)               MotorPwmLeft = 0;
		if (MotorPwmLeft >= 990)             MotorPwmLeft = 990;
		if (MotorPwmRight <= 0)              MotorPwmRight = 0;
		if (MotorPwmRight >= 990)            MotorPwmRight = 990;
		ftm_pwm_duty(FTM0, FTM_CH0, MotorPwmRight);
		ftm_pwm_duty(FTM0, FTM_CH1, MotorPwmLeft); //PTC2,左电机       
	}
	else
	{

		ftm_pwm_duty(FTM0, FTM_CH0, 0);
		ftm_pwm_duty(FTM0, FTM_CH1, 0); //PTC2,左电机       
	}
}


#endif







void SpeedGet(void)

{
	NumPulselptmr = lptmr_pulse_get();
	lptmr_pulse_clean();//计数寄存器清零
	LeftMotorPulseAccumulate += NumPulselptmr;//把每一次PIT定时时间的脉冲数累加到脉冲总数变量pulse_period_L  


	NumPulseFTM = ftm_quad_get(FTM2);//取寄存器的值
	ftm_quad_clean(FTM2);//计数寄存器清零
	RightMotorPulseAccumulate += NumPulseFTM;//单相脉冲    （正交解码测脉冲）右轮 
}


void PIT0_IRQHandler()
{

	static uint16  TimerCnt8ms = 0;

	disable_irq(PIT0_IRQn);            //关PIT中断

	PIT_Flag_Clear(PIT0);                           //清中断标志位

	TimerCnt8ms++;

	SpeedGet();                        //每次进来累加一次速度

	if (TimerCnt8ms >= MotorControlPeriod)//一个8ms周期取一次速度，当作车的速度

	{

		GetRightMotorPules = -RightMotorPulseAccumulate;  //保持好这次测得的总数，10ms一共测得的脉冲
		GetLeftMotorPules = LeftMotorPulseAccumulate;

		TimerCnt8ms = 0;//清零

		RightMotorPulseAccumulate = 0;//累加值清零，
		LeftMotorPulseAccumulate = 0;
	}

	enable_irq(PIT0_IRQn);//使能中断	
}





