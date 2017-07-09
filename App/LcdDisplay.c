#include "common.h"
#include "include.h"

/*LCD液晶变量定义*/
Site_t site = { 0, 0 };      //显示图像左上角位置
Size_t imgsize = { CAMERA_W, CAMERA_H };     //图像大小
Size_t size;                   //显示区域图像大小   

uint8 LCD_Page=0;//当前LCD页面

void LCD_Init()
{
	LCD_init();   //显示屏初始化  
	LCD_str(site, "Cam init ing", FCOLOUR, BCOLOUR);   //显示屏显示  （左上角坐标，字符串，字体颜色，背景颜色）

	size.H = CAMERA_H;    //高      //（表示全屏显示）
	size.W = CAMERA_W;    //宽

	LCD_str(site, "Cam init OK!", FCOLOUR, BCOLOUR);   //显示屏显
	LCD_clear(RED);     //清屏 （背景红色）
}



void LCDDisplay(void)
{
	uint32 a, i;
	Site_t site_point = { 0,0 };

   site.y =127-CAMERA_H;    //显示图像左上角位置的y轴
   site.x =127-CAMERA_W;
	LCD_Img_Binary_Z(site, size, imgbuff, imgsize);//黑白摄像头（显示图像左上角位置，显示区域图像大小，定义存储接收图像的数组，图像大小）

	//画边线和中心线
	for (i = CAMERA_H - 1; i > 2; i--)
	{
        site_point.y=i+127-CAMERA_H;
		//右线
		if (RightEdge[i] > 79)RightEdge[i] = 79;
		if (RightEdge[i] < 0)RightEdge[i] = 0;
        site_point.x=RightEdge[i]+127-CAMERA_W-2;      /////LeftEedg   rightEedg
		LCD_point(site_point, BRED);

		//左线
		if (LeftEdge[i] > 79)LeftEdge[i] = 79;
		if (LeftEdge[i] < 0)LeftEdge[i] = 0;
        site_point.x=LeftEdge[i]+127-CAMERA_W+2;
		LCD_point(site_point, BLUE);

		//路径中线
        site_point.x=MiddleLine[i]+127-CAMERA_W;
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
		//镜头中心
		//site_point.x=40+127-CAMERA_W;
		//LCD_point(site_point,YELLOW);

	}

        if(LCD_Page)
	{
	site.x = 0;//变量显示行地址
	site.y = 0;
	LCD_str(site, "LastLine:", FCOLOUR, BCOLOUR);   //变量名

	site.x = 48;//变量显示行地址
	site.y = 0;

	a = (uint32)(LastLine);
	LCD_num(site, abs(a), YELLOW, RED);

	site.x = 0;//变量显示行地址
	site.y = 16;
	LCD_str(site, " AllLose", FCOLOUR, BCOLOUR);   //变量名

	site.x = 64;//变量显示行地址
	site.y = 16;
	LCD_num(site, AllLose, YELLOW, RED);

	site.x = 0;//变量显示行地址
	site.y = 32;
	LCD_str(site, " Tracktype.LongStraightaway:", FCOLOUR, BCOLOUR);   //变量名

	site.x = 64;//变量显示行地址
	site.y = 32;
	LCD_num(site, Tracktype.LongStraightaway, YELLOW, RED);

	site.x = 0;//变量显示行地址
	site.y = 48;
	LCD_str(site, "RRow:", FCOLOUR, BCOLOUR);   //变量名

	site.x = 64;//变量显示行地址
	site.y = 48;
	LCD_num(site, InflectionPointR.InflectionPointRow, YELLOW, RED);
        }
        else
        {
            site.x = 0;//变量显示行地址
            site.y = 0;
            LCD_str(site, "Steer_J*100:", FCOLOUR, BCOLOUR);   //变量名
            
            site.x = 48+48;//变量显示行地址
            site.y = 0;
            LCD_num(site,(uint32)(J*100), YELLOW, RED);
            
            
            site.x = 0;//变量显示行地址
            site.y = 16;
            LCD_str(site, "Steer_JD*100:", FCOLOUR, BCOLOUR);   //变量名
            
            site.x = 48+48+8;//变量显示行地址
            site.y = 16;
            LCD_num(site, (uint32)(JD*100), YELLOW, RED);
            
            site.x = 0;//变量显示行地址
            site.y = 32;
            LCD_str(site, "BasicP*10:", FCOLOUR, BCOLOUR);   //变量名
            
            site.x = 48+32;//变量显示行地址
            site.y = 32;
            LCD_num(site, (uint32)(BasicP*10), YELLOW, RED);
            
            site.x = 0;//变量显示行地址
            site.y = 48;
            LCD_str(site, "BasicD*10:", FCOLOUR, BCOLOUR);   //变量名
            
            site.x = 48+32;//变量显示行地址
            site.y = 48;
            LCD_num(site, (uint32)(BasicD*10), YELLOW, RED);
            

            site.x = 0;//变量显示行地址
            site.y = 16 + 48;
            LCD_str(site, "Speed:", FCOLOUR, BCOLOUR);   //变量名

            site.x = 0;//变量显示行地址
            site.y = 32 + 48;
            LCD_num(site, SpeedSet, YELLOW, RED);
            
            site.x = 0;//变量显示行地址
            site.y = 16+32 + 48;
            LCD_str(site, "End:", FCOLOUR, BCOLOUR);   //变量名
            
            site.x = 0;//变量显示行地址
            site.y = 32 + 48+32;
            LCD_num(site, EndLineFlag, YELLOW, RED);

            site.x = 32;//变量显示行地址
            site.y = 32 + 48+32;
            LCD_num(site, IsStartLine, YELLOW, RED);
        }
}