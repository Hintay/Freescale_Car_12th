#include "common.h"
#include "include.h"

uint8 imgbuff[CAMERA_SIZE];            //定义存储接收图像的数组
uint8 img[CAMERA_H][CAMERA_W];

//函数声明
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void HardWare_Init(void);
void PIT0_IRQHandler();
void PORTE_IRQHandler();
void set_vector_handler(VECTORn_t, void pfunc_handler(void));      //设置中断函数到中断向量表里



void HardWare_Init(void)
{
	DisableInterrupts;

	camera_init(imgbuff);//摄像头初始化
	set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler);   //设置 PORTA 的中断服务函数为 PORTA_IRQHandler
	set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler);     //设置 DMA0 的中断服务函数为 PORTA_IRQHandler

#if DEBUG_MODE
	// 设置串口中断
	set_vector_handler(UART4_RX_TX_VECTORn, uart_handler);
	uart_rx_irq_en(VCAN_PORT);
#endif

#if 1										      
	ftm_quad_init(FTM2);  //FTM2  PTA10 （ PTA11 ）正交解码初始化
	ftm_quad_clean(FTM2);//计数寄存器清零
	lptmr_pulse_init(LPT0_ALT2, 0xffff, LPT_Rising); //PTC5
	lptmr_pulse_clean();
#endif

#if 1										
	pit_init_ms(PIT0, 1);//PIT初始化  1ms                  
	set_vector_handler(PIT0_VECTORn, PIT0_IRQHandler);   // 设置中断复位函数到中断向量表
	enable_irq(PIT0_IRQn);                  // 使能PIT0中断
#endif    


	NVIC_SetPriorityGrouping(4);            //设置优先级分组,4bit 抢占优先级,没有亚优先
	NVIC_SetPriority(PORTA_IRQn, 0);          //配置优先级  图像场中断  PORTA
	NVIC_SetPriority(DMA0_IRQn, 1);          //配置优先级  图像  DMA0
	NVIC_SetPriority(PIT0_IRQn, 2);          //配置优先级  编码器   PIT0 

#if    OpenDialSwitch

	DialSwitchInit();//拨码开关初始化

#endif   

#if    BuzzerOpen  

	BuzzerInit();//蜂鸣器初始化

#endif  

#if     LedOpen

	LedInit();

#endif 


	SteerInit();//舵机初始化

	MotorInit();//电机初始化

	if (DialSwitch_1)
	{
		LCD_Init();// LCD_ST7735S 液晶初始化,不能初始化在摄像头前面

	}
	EnableInterrupts;//使能总中断

}


void  main(void)
{
#if DEBUG_MODE
	if (MC_SRSH & MC_SRSH_SW_MASK) printf("Software Reset\n");
	if (MC_SRSH & MC_SRSH_LOCKUP_MASK) printf("Core Lockup Event Reset\n");
	if (MC_SRSH & MC_SRSH_JTAG_MASK) printf("JTAG Reset\n");
	if (MC_SRSL & MC_SRSL_POR_MASK) printf("Power-on Reset\n");
	if (MC_SRSL & MC_SRSL_PIN_MASK) printf("External Pin Reset\n");
	if (MC_SRSL & MC_SRSL_COP_MASK) printf("Watchdog(COP) Reset\n");
	if (MC_SRSL & MC_SRSL_LOC_MASK) printf("Loss of Clock Reset\n");
	if (MC_SRSL & MC_SRSL_LVD_MASK) printf("Low-voltage Detect Reset\n");
	if (MC_SRSL & MC_SRSL_WAKEUP_MASK) printf("LLWU Reset\n");
#endif

	HardWare_Init();
	if (DialSwitch_4)
	{ // 盲跑
		ftm_pwm_duty(FTM0, FTM_CH0, 400);//右电机
		ftm_pwm_duty(FTM0, FTM_CH1, 400); //左电机
		DELAY_MS(500);//延时500ms
	}
	//int8_t osc_array[6];
	while (1)
	{
		if (*read_datapacket)
		{
			zjgm_getcrotroldata();
			*read_datapacket = 0; // 重置
		}

		camera_get_img();//（耗时13.4ms）图像采集
		img_extract(img, imgbuff);
		//vcan_sendimg(imgbuff,CAMERA_SIZE);//将图像发送到上位机
		SearchCenterBlackline();
		GetBlackEndParam();//获取黑线截止行
		CrossRecognition(&Cross);//十字识别
		CrossConduct();//十字补线
		LoopRecognition(&Loop);//圆环识别
		FindLoopOptimalExit();//寻找圆环出口
		LoopIntoRepair();//圆环补线
		ClearLoopControl(); //出圆环标志位清零
		TrackType();

		/*osc_array[0] = StartLineEnd;
		osc_array[1] = StartLineStart + 60;
		osc_array[2] = EndLineFlagCount - 10;
		osc_array[3] = StartLinenNextClear - 5;
		osc_array[4] = GetRightMotorPules;
		osc_array[5] = GetLeftMotorPules;
		vcan_sendware(osc_array, 6); // 把数据发送到上位机 虚拟示波器*/

# if ObstacleOpen  //如果不需要避障碍，将这个宏定义置0即可
		RecognitionObstacle();
#endif

		SteerControl();

#if OpenLoop
		MotorControlOpenLoop();
#endif

#if CloseLoop
		MotorControl();
#endif

		if (DialSwitch_1)
		{
			LCDDisplay();//液晶显示
		}
	}

}

void PORTA_IRQHandler()
{
	uint8  n;    //引脚号
	uint32 flag;

	while (!PORTA_ISFR);
	flag = PORTA_ISFR;
	PORTA_ISFR = ~0;                                   //清中断标志位

	n = 29;                                             //场中断
	if (flag & (1 << n))                                 //PTA29触发中断
	{
		camera_vsync();
	}
#if ( CAMERA_USE_HREF == 1 )                            //使用行中断
	n = 28;
	if (flag & (1 << n))                                 //PTA28触发中断
	{
		camera_href();
	}
#endif


}


void DMA0_IRQHandler()
{
	camera_dma();
}





