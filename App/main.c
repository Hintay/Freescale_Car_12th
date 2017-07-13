#include "common.h"
#include "include.h"

uint8 imgbuff[CAMERA_SIZE];            //����洢����ͼ�������
uint8 img[CAMERA_H][CAMERA_W];

//��������
void PORTA_IRQHandler();
void DMA0_IRQHandler();
void HardWare_Init(void);
void PIT0_IRQHandler();
void PORTE_IRQHandler();
void set_vector_handler(VECTORn_t, void pfunc_handler(void));      //�����жϺ������ж���������



void HardWare_Init(void)
{
	DisableInterrupts;

	camera_init(imgbuff);//����ͷ��ʼ��
	set_vector_handler(PORTA_VECTORn, PORTA_IRQHandler);   //���� PORTA ���жϷ�����Ϊ PORTA_IRQHandler
	set_vector_handler(DMA0_VECTORn, DMA0_IRQHandler);     //���� DMA0 ���жϷ�����Ϊ PORTA_IRQHandler

#if DEBUG_MODE
	// ���ô����ж�
	set_vector_handler(UART4_RX_TX_VECTORn, uart_handler);
	uart_rx_irq_en(VCAN_PORT);
#endif

#if 1										      
	ftm_quad_init(FTM2);  //FTM2  PTA10 �� PTA11 �����������ʼ��
	ftm_quad_clean(FTM2);//�����Ĵ�������
	lptmr_pulse_init(LPT0_ALT2, 0xffff, LPT_Rising); //PTC5
	lptmr_pulse_clean();
#endif

#if 1										
	pit_init_ms(PIT0, 1);//PIT��ʼ��  1ms                  
	set_vector_handler(PIT0_VECTORn, PIT0_IRQHandler);   // �����жϸ�λ�������ж�������
	enable_irq(PIT0_IRQn);                  // ʹ��PIT0�ж�
#endif    


	NVIC_SetPriorityGrouping(4);            //�������ȼ�����,4bit ��ռ���ȼ�,û��������
	NVIC_SetPriority(PORTA_IRQn, 0);          //�������ȼ�  ͼ���ж�  PORTA
	NVIC_SetPriority(DMA0_IRQn, 1);          //�������ȼ�  ͼ��  DMA0
	NVIC_SetPriority(PIT0_IRQn, 2);          //�������ȼ�  ������   PIT0 

#if    OpenDialSwitch

	DialSwitchInit();//���뿪�س�ʼ��

#endif   

#if    BuzzerOpen  

	BuzzerInit();//��������ʼ��

#endif  

#if     LedOpen

	LedInit();

#endif 


	SteerInit();//�����ʼ��

	MotorInit();//�����ʼ��

	if (DialSwitch_1)
	{
		LCD_Init();// LCD_ST7735S Һ����ʼ��,���ܳ�ʼ��������ͷǰ��

	}
	EnableInterrupts;//ʹ�����ж�

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
	{ // ä��
		ftm_pwm_duty(FTM0, FTM_CH0, 400);//�ҵ��
		ftm_pwm_duty(FTM0, FTM_CH1, 400); //����
		DELAY_MS(500);//��ʱ500ms
	}
	//int8_t osc_array[6];
	while (1)
	{
		if (*read_datapacket)
		{
			zjgm_getcrotroldata();
			*read_datapacket = 0; // ����
		}

		camera_get_img();//����ʱ13.4ms��ͼ��ɼ�
		img_extract(img, imgbuff);
		//vcan_sendimg(imgbuff,CAMERA_SIZE);//��ͼ���͵���λ��
		SearchCenterBlackline();
		GetBlackEndParam();//��ȡ���߽�ֹ��
		CrossRecognition(&Cross);//ʮ��ʶ��
		CrossConduct();//ʮ�ֲ���
		LoopRecognition(&Loop);//Բ��ʶ��
		FindLoopOptimalExit();//Ѱ��Բ������
		LoopIntoRepair();//Բ������
		ClearLoopControl(); //��Բ����־λ����
		TrackType();

		/*osc_array[0] = StartLineEnd;
		osc_array[1] = StartLineStart + 60;
		osc_array[2] = EndLineFlagCount - 10;
		osc_array[3] = StartLinenNextClear - 5;
		osc_array[4] = GetRightMotorPules;
		osc_array[5] = GetLeftMotorPules;
		vcan_sendware(osc_array, 6); // �����ݷ��͵���λ�� ����ʾ����*/

# if ObstacleOpen  //�������Ҫ���ϰ���������궨����0����
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
			LCDDisplay();//Һ����ʾ
		}
	}

}

void PORTA_IRQHandler()
{
	uint8  n;    //���ź�
	uint32 flag;

	while (!PORTA_ISFR);
	flag = PORTA_ISFR;
	PORTA_ISFR = ~0;                                   //���жϱ�־λ

	n = 29;                                             //���ж�
	if (flag & (1 << n))                                 //PTA29�����ж�
	{
		camera_vsync();
	}
#if ( CAMERA_USE_HREF == 1 )                            //ʹ�����ж�
	n = 28;
	if (flag & (1 << n))                                 //PTA28�����ж�
	{
		camera_href();
	}
#endif


}


void DMA0_IRQHandler()
{
	camera_dma();
}





