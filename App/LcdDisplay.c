#include "common.h"
#include "include.h"

/*LCDҺ����������*/
Site_t site = { 0, 0 };      //��ʾͼ�����Ͻ�λ��
Size_t imgsize = { CAMERA_W, CAMERA_H };     //ͼ���С
Size_t size;                   //��ʾ����ͼ���С   

int aac_tmp = 0;

void LCD_Init()
{
	LCD_init();   //��ʾ����ʼ��  
	LCD_str(site, "Cam init ing", FCOLOUR, BCOLOUR);   //��ʾ����ʾ  �����Ͻ����꣬�ַ�����������ɫ��������ɫ��

	size.H = CAMERA_H;    //��      //����ʾȫ����ʾ��
	size.W = CAMERA_W;    //��

	LCD_str(site, "Cam init OK!", FCOLOUR, BCOLOUR);   //��ʾ����
	LCD_clear(RED);     //���� ��������ɫ��
}



void LCDDisplay(void)
{
	uint32 a, i;
	Site_t site_point = { 0,0 };

	site.y = 126 - CAMERA_H;    //��ʾͼ�����Ͻ�λ�õ�y��
	site.x = 126 - CAMERA_W;
	LCD_Img_Binary_Z(site, size, imgbuff, imgsize);//�ڰ�����ͷ����ʾͼ�����Ͻ�λ�ã���ʾ����ͼ���С������洢����ͼ������飬ͼ���С��

	//�����ߺ�������
	for (i = CAMERA_H - 1; i > 2; i--)
	{
		site_point.y = i + 126 - CAMERA_H;
		//����
		if (RightEdge[i] > 79)RightEdge[i] = 79;
		if (RightEdge[i] < 0)RightEdge[i] = 0;
		site_point.x = RightEdge[i] + 126 - CAMERA_W - 2;      /////LeftEedg   rightEedg
		LCD_point(site_point, BRED);

		//����
		if (LeftEdge[i] > 79)LeftEdge[i] = 79;
		if (LeftEdge[i] < 0)LeftEdge[i] = 0;
		site_point.x = LeftEdge[i] + 126 - CAMERA_W + 2;
		LCD_point(site_point, BLUE);

		//·������
		site_point.x = MiddleLine[i] + 126 - CAMERA_W;
		if (BlackAreaCountRow[i])
		{
			LCD_point(site_point, BLUE);
			site_point.x--;
			LCD_point(site_point, BLUE);
			site_point.x += 2;
			LCD_point(site_point, BLUE);
		}
		else
			LCD_point(site_point, RED);
		//��ͷ����
		//site_point.x=40+126-CAMERA_W;
		//LCD_point(site_point,YELLOW);

	}


	site.x = 0;//������ʾ�е�ַ
	site.y = 0;
	LCD_str(site, "LastLine:", FCOLOUR, BCOLOUR);   //������

	site.x = 48;//������ʾ�е�ַ
	site.y = 0;

	a = (uint32)(LastLine);
	LCD_num(site, abs(a), YELLOW, RED);

	site.x = 0;//������ʾ�е�ַ
	site.y = 16;
	LCD_str(site, " AllLose", FCOLOUR, BCOLOUR);   //������

	site.x = 64;//������ʾ�е�ַ
	site.y = 16;
	LCD_num(site, AllLose, YELLOW, RED);

	site.x = 0;//������ʾ�е�ַ
	site.y = 32;
	LCD_str(site, " Tracktype.LongStraightaway:", FCOLOUR, BCOLOUR);   //������

	site.x = 64;//������ʾ�е�ַ
	site.y = 32;
	LCD_num(site, Tracktype.LongStraightaway, YELLOW, RED);

	site.x = 0;//������ʾ�е�ַ
	site.y = 48;
	LCD_str(site, "RRow:", FCOLOUR, BCOLOUR);   //������

	site.x = 64;//������ʾ�е�ַ
	site.y = 48;
	LCD_num(site, InflectionPointR.InflectionPointRow, YELLOW, RED);


	site.x = 0;//������ʾ�е�ַ
	site.y = 48 + 16;
	LCD_str(site, "End:", FCOLOUR, BCOLOUR);   //������

	site.x = 0;//������ʾ�е�ַ
	site.y = 48 + 32;
	LCD_num(site, EndLineFlag, YELLOW, RED);

	site.x = 32;//������ʾ�е�ַ
	site.y = 48 + 32;
	LCD_num(site, IsStartLine, YELLOW, RED);

	site.x = 0;//������ʾ�е�ַ
	site.y = 48 + 32 + 16;
	LCD_num(site, ObstacleLfetOkFlag, YELLOW, RED);

	site.x = 0;//������ʾ�е�ַ
	site.y = 48 + 32 + 32;
	LCD_num(site, ObstacleRightOkFlag, YELLOW, RED);
}