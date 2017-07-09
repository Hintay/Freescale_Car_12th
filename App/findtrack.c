
#include "common.h"
#include "include.h"
#include "findtrack.h"


/*********define for SearchCenterBlackline**********/
int   EndLineFlag = 0;//终止线检测标志位,如果该标志位置1则说明检测到终止线
int   EndLineEnableFlag = 1;//终止线使能标志位,如果该标志位置1则允许检测终止线

int   EdgeRequirementCnt = 0;//边缘检测条件计数//如果检测黑点持续个数超过某值则认为检测到黑边,本变量就是用于计数持续检测到黑边的个数
int IsBlackArea = 0; // 是否在黑色区域
int BlackAreaCount = 0; // 黑色区域计数
int   MiddleLine[RowMax + 1];
int   RightEdge[RowMax + 1];
int   LeftEdge[RowMax + 1];
int   Width[RowMax + 1];//={2,3,3,3,4,4,5,5,6,6,
					   //8,8,10,10,12,13,14,14,16,17,
					   //18,18,20,20,22,22,24,24,26,26,
					   //28,30,31,32,32,34,36,36,38,39,
					   //41,41,43,43,45,45,47,47,49,50,
					   //50,51,52,54,55,56,57,58,59,60,61};  // Width[i]  = 20+i*3/4;     //动态路宽
int  BlackAreaCountRow[RowMax + 1];
int  EndLineFlagCount = 0;
int  StartLineStart;
int  StartLineEnd;
int8 StartLinenNextClear = 0;

int   MidPri = 40;
int   LastLine = 0;
float AvaliableLines = 0;
int   LeftLose = 0;
int   RightLose = 0;
int   AllLose = 0;
int   LeftLoseStart = 0;//记录左边丢线的开始行
int   RightLoseStart = 0;//记录右边边丢线的开始行
int   WhiteStart = 0;

//float OverflowOffset = 0.4;//溢出补偿系数,如果当前行的某一个边已经被白色淹没,则检测白色淹没程度来对中线进行偏移

/*********define for GetBlackEndParam**********/
int BlackEndMR = 0;
int BlackEndML = 0;
int BlackEndLL = 0;
int BlackEndRR = 0;
int BlackEndL = 0;
int BlackEndM = 0;
int BlackEndR = 0;
int BlackEndMaxMax = 0;
int BlackEndMax = 0;
int DropRow = 0;


//设置中线，左线，右线的初始化值
//设置每一行对应的赛道宽度
//本文件内不调用
void SetInitVal()
{
	int i;

	for (i = 0; i < RowMax; i++)//赋初值             
	{
		RightEdge[i] = ColumnMax;
		LeftEdge[i] = 0;
		MiddleLine[i] = ColumnMax / 2;
		Width[i] = 20 + i * 3 / 4;     //动态路宽
		BlackAreaCountRow[i] = 0;

	}

}


//获取从指定点向上到达黑色区域的距离

/*int16  GetWhiteHight(int16 H, int16 W)
{
	int16 tmp = H;
	while (tmp > 0 && img[tmp][W] == White_Point)//如果tmp还没到画面最远端,且目前检查的像素点还是白色
	{
		tmp--;
	}
	if (tmp > 0)//如果tmp>0就说明刚刚的检查不是因为到达画面最远端而停止的
	{
		return H - tmp;//返回检查开始的H跟当前检测到黑色的高度差
	}
	return 0;

}*/


//全行扫描和边缘结合提取赛道的中线
//前十行全行扫描
//后面50行，根据上一行的寻线情况来决定当前行的寻线方式和起点
//外部调用
void SearchCenterBlackline(void)
{

	int16 i = 0;
	int16 j = 0;
	uint8 jj = 0;
	int16 x = 0;
	uint8 WhiteNum = 0;

	LeftLose = 0;//变量清零
	RightLose = 0;
	AllLose = 0;
	WhiteNum = 0;

	MiddleLine[RowMax] = ColumnMax / 2;//第60行
	LeftEdge[RowMax] = 0;
	RightEdge[RowMax] = ColumnMax;
	Width[RowMax] = 60;

	SetInitVal();

	//前十行，采用右左边往右边扫描的方法
	//==================前十行处理开始====================
	for (i = RowMax - 1; i >= RowMax - 10; i--)//首先找前十行，全行扫描
	{
		BlackAreaCount = 0;
		if (i == RowMax - 1)//首行就以图像中心作为扫描起点
		{
			j = MidPri;//40,该变量默认为40,每处理一张图像,都会记录下图像的59行(最接近车的)的中线位置赋给该变量
		}
		else
		{
			j = MiddleLine[i + 1];//否则就以上一行中点的位置作为本行扫描起点
		}
		if (j <= 2)
		{
			j = 2;
		}

		/*=========================以下是单行找左边界================*/
		  /*while(j >= 2)//j>=2有效范围内进行搜寻
		  {
			 if(img[i][j]==White_Point&& img[i][j-1]==Black_Point&&img[i][j-2]==Black_Point)//从右向左找到白白黑跳变
			 {
				 LeftEdge[i] =j;//找到则赋值 找不到保持原值0
				 break;//跳出本行寻线
			  }
				 j--;//列数往左移动
		   }*/

		EdgeRequirementCnt = 0;//计数清零
		IsBlackArea = 0;
		for (x = j; x >= 2; x--)
		{
			if (img[i][x] == Black_Point)//如果当前点是黑色
			{
				if (!IsBlackArea)
				{
					LeftEdge[i] = x + 1;
					IsBlackArea = 1;
				}
				EdgeRequirementCnt++;//计数++
				if (x == 2)//如果y==2,说明图像已经到边缘了,放弃本次的滤波
					break;
			}
			else if (IsBlackArea)
			{
				IsBlackArea = 0;
				BlackAreaCount++;
				EdgeRequirementCnt = 0;//如果不是黑色就清空计数
			}
			if (EdgeRequirementCnt > EDGE_REQUIREMENT)//如果达到连续要求的阀值
				break;
		}
		/*=========================================================*/

		if (i == RowMax - 1) //再找右边界
		{
			j = MidPri;//如果首行，从图像中心开始搜寻
		}
		else
		{
			j = MiddleLine[i + 1];//否则从上一行中心位置开始搜寻
		}
		if (j >= ColumnMax - 2)//j >=ColumnMax-2有效范围内搜寻右线
		{
			j = ColumnMax - 2;
		}

		/*=========================以下是单行找右边界================*/
		 /*while(j <= ColumnMax-2)
		 {

			if(img[i][j]==White_Point && img[i][j+1]==Black_Point && img[i][j+2]==Black_Point)//从左向右找到白白黑跳变点
			{
				   RightEdge[i] = j;//找到则赋值   找不到保持原值
				   break;//跳出本行寻线
			}
				   j++;//列数往右移动
		  }*/


		EdgeRequirementCnt = 0; //计数清零
		IsBlackArea = 0;
		for (x = j; x <= ColumnMax - 2; x++)
		{
			if (img[i][x] == Black_Point)//如果当前点是黑色
			{
				if (!IsBlackArea)
				{
					RightEdge[i] = x + 1;
					IsBlackArea = 1;
				}
				EdgeRequirementCnt++;//计数++
				if (x == ColumnMax - 2)//如果y==2,说明图像已经到边缘了,放弃本次的滤波
					break;
			}
			else if (IsBlackArea)
			{
				IsBlackArea = 0;
				BlackAreaCount++;
				EdgeRequirementCnt = 0;//如果不是黑色就清空计数
			}
			if (EdgeRequirementCnt > EDGE_REQUIREMENT)//如果达到连续要求的阀值
				break;
		}

		/*=========================================================*/
		if (LeftEdge[i] != 0 && RightEdge[i] != ColumnMax)//中线判断，没有丢线
		{
			MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;//则中线是跑道左右边界/2  
		}



		else if (LeftEdge[i] != 0 && RightEdge[i] == ColumnMax)//丢了右线,没有丢左线
		{
			RightLose++;//记录只有右线丢的数量

			//如果当行的

			if ((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 2))//突变,如果本行的跑道宽>=上一行的跑道宽+2
			{
				MiddleLine[i] = MiddleLine[i + 1];//因为突变所以本行数据放弃,本行中线位置使用上一行的中线位置
			}
			else
			{

				//MiddleLine[i] = LeftEdge[i] + Width[i]/2+1+(int)(OverflowOffset*GetWhiteHight(i,0));//在普通的跑道半宽的基础上加上白色溢出偏差
				MiddleLine[i] = LeftEdge[i] + Width[i] / 2 + 3;//在丢右线但是又没有突变的情况下,拿左线边界位置+跑道半宽然后再往右边偏2个
			}
		}
		else if (LeftEdge[i] == 0 && RightEdge[i] != ColumnMax)//丢了左线
		{

			if ((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 3))//突变,如果本行的跑道宽>=上一行的跑道宽+2      
			{
				MiddleLine[i] = MiddleLine[i + 1]; //因为突变所以本行数据放弃,本行中线位置使用上一行的中线位置
			}
			else
			{
				// MiddleLine[i] = RightEdge[i] - Width[i]/2-1-(int)(OverflowOffset*GetWhiteHight(i,ColumnMax));
				MiddleLine[i] = RightEdge[i] - Width[i] / 2 - 3;//在丢左线但是又没有突变的情况下,拿右线边界位置+跑道半宽然后再往左边偏2个
			}
		}
		else if (LeftEdge[i] == 0 && RightEdge[i] == ColumnMax)//两边都丢了的话  
		{
			AllLose++;

			if (i == RowMax - 1)//如果是首行就以图像中心作为中点
			{
				MiddleLine[i] = MidPri;
			}
			else
			{
				MiddleLine[i] = MiddleLine[i + 1];//如果不是首行就用上一行的中线作为本行中点
			}
		}



		if (MiddleLine[RowMax - 1] >= 70)//中线位置限幅
		{
			MidPri = 70;
		}
		else if (MiddleLine[RowMax - 1] <= 10)//中线位置限幅
		{
			MidPri = 10;
		}
		else
		{
			MidPri = MiddleLine[RowMax - 1];//记录本帧图像第59行的中线值，作为下一幅图像的59行扫描起始点
		}

		BlackAreaCountRow[i] = BlackAreaCount;
	}
	//==================前十行处理结束====================
	for (i = 49; i > 0; i--)//查找剩余行
	{
		BlackAreaCount = 0;
		if (LeftEdge[i + 1] != 0 && RightEdge[i + 1] != ColumnMax) //上一行两边都找到 启用边沿扫描     
		{
			j = ((LeftEdge[i + 1] + 15) >= ColumnMax - 2) ? ColumnMax - 2 : (LeftEdge[i + 1] + 15);//先找左边界    
			jj = ((LeftEdge[i + 1] - 10) <= 1) ? 1 : (LeftEdge[i + 1] - 10);
			IsBlackArea = 0;
			EdgeRequirementCnt = 0;//计数清零
			while (j >= jj)//正常情况下(没有过于接近某一图像边缘),就从上一行左边界的右边10个的位置找到左边5个的位置之间寻找    
			{
				/*if (img[i][j] == White_Point && img[i][j - 1] == Black_Point&& img[i][j - 2] == Black_Point)//判断有没有黑白跳变
				{
					LeftEdge[i] = j;//找到了就记录左边界的位置
					break;
				}*/

				if (img[i][j] == Black_Point)//如果当前点为黑色
				{
					if (j == jj)//如果已经到搜索终点
					{
						break; //放弃本次滤波
					}
					EdgeRequirementCnt++;//计数++
					if (!IsBlackArea)
					{
						IsBlackArea = 1;
						LeftEdge[i] = j + 1; // 确保在白色区域内
					}
				}
				else if (IsBlackArea)
				{
					BlackAreaCount++;
					IsBlackArea = 0;
					EdgeRequirementCnt = 0;//如果不是黑色就清空计数
				}

				if (EdgeRequirementCnt >= EDGE_REQUIREMENT)//如果达到连续要求的阀值
					break;

				j--;
			}
			j = ((RightEdge[i + 1] - 15) <= 1) ? 1 : (RightEdge[i + 1] - 15); //在找右边界   
			jj = ((RightEdge[i + 1] + 10) >= ColumnMax - 2) ? ColumnMax - 2 : (RightEdge[i + 1] + 10);
			EdgeRequirementCnt = 0;//计数清零
			while (j <= jj)
			{
				/*if (img[i][j] == White_Point&& img[i][j + 1] == Black_Point&& img[i][j + 2] == Black_Point)
				{
					RightEdge[i] = j;
					break;
				}*/

				if (img[i][j] == Black_Point)//如果当前行为黑色
				{
					if (j == jj)//如果已经到搜索终点
					{
						break;//放弃本次滤波
					}
					EdgeRequirementCnt++;//计数++
					RightEdge[i] = j - EdgeRequirementCnt - 1;
					if (!IsBlackArea)
					{
						IsBlackArea = 1;
						RightEdge[i] = j - 1; //确保在白色区域内
					}

				}
				else if (IsBlackArea)
				{
					BlackAreaCount++;
					IsBlackArea = 0;
					EdgeRequirementCnt = 0;//如果不是黑色就清空计数
				}
				if (EdgeRequirementCnt > EDGE_REQUIREMENT)//如果达到连续要求的阀值
					break;
				j++;
			}
		}
		else if (LeftEdge[i + 1] != 0 && RightEdge[i + 1] == ColumnMax)//上一行只找到左边界   
		{
			j = ((LeftEdge[i + 1] + 10) >= ColumnMax - 2) ? ColumnMax - 2 : (LeftEdge[i + 1] + 10);//左边界用边沿扫描   
			jj = ((LeftEdge[i + 1] - 5) <= 1) ? 1 : (LeftEdge[i + 1] - 5);
			while (j >= jj)
			{
				if (img[i][j] == White_Point && img[i][j - 1] == Black_Point && img[i][j - 2] == Black_Point)
				{
					LeftEdge[i] = j;
					break;
				}
				j--;
			}
			j = MiddleLine[i + 1];//上一行丢了右边界用全行扫描 
			if (j >= 78)
			{
				j = 78;
			}
			while (j <= ColumnMax - 2)
			{
				if (img[i][j] == White_Point && img[i][j + 1] == Black_Point&&img[i][j + 2] == Black_Point)
				{
					RightEdge[i] = j;
					break;
				}
				j++;
			}

		}
		else if (LeftEdge[i + 1] == 0 && RightEdge[i + 1] != ColumnMax) //上一行只找到右边界      
		{
			j = ((RightEdge[i + 1] - 10) <= 1) ? 1 : (RightEdge[i + 1] - 10);//边缘追踪找右边界 
			jj = ((RightEdge[i + 1] + 5) >= ColumnMax - 2) ? ColumnMax - 2 : (RightEdge[i + 1] + 5);
			while (j <= jj)
			{
				if (img[i][j] == White_Point&&img[i][j + 1] == Black_Point&&img[i][j + 2] == Black_Point)
				{
					RightEdge[i] = j;
					break;
				}
				j++;
			}
			j = MiddleLine[i + 1]; //全行扫描找左边界
			if (j < 2)
			{
				j = 2;
			}
			while (j >= 1)
			{
				if (img[i][j] == White_Point && img[i][j - 1] == Black_Point&& img[i][j - 2] == Black_Point)
				{
					LeftEdge[i] = j;
					break;
				}
				j--;
			}
		}
		else if (LeftEdge[i + 1] == 0 && RightEdge[i + 1] == ColumnMax)  //上一行没找到边界，可能是十字或者环形  
		{

			j = MiddleLine[i + 1];   //找全行找左边界
			while (j >= 1)
			{
				if (img[i][j] == White_Point && img[i][j - 1] == Black_Point&& img[i][j - 2] == Black_Point)
				{
					LeftEdge[i] = j;
					break;
				}
				j--;
			}
			j = MiddleLine[i + 1];   //全行找右边界   
			while (j <= ColumnMax - 2)
			{
				if (img[i][j] == White_Point&&img[i][j + 1] == Black_Point)
				{
					RightEdge[i] = j;
					break;
				}
				j++;

			}
		}
		if ((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 3))//不满足畸变 
		{
			MiddleLine[i] = MiddleLine[i + 1];//用上一行
		}
		else
		{
			if (LeftEdge[i] != 0 && RightEdge[i] != ColumnMax)
			{
				MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;
				//对斜出十字进行纠正

				if (MiddleLine[i] - MiddleLine[i + 1] > 8 && ((ABS(LeftEdge[i] - LeftEdge[i + 1] > 3) || ABS(RightEdge[i] - RightEdge[i + 1] > 3))) && i >= 30)//中线向右突变
				{
					uint8 ii = i;

					while (1)
					{
						MiddleLine[ii + 1] = MiddleLine[ii] - 1;
						ii++;

						if (ii >= 50 || (MiddleLine[ii] - MiddleLine[ii + 1] <= 1))
						{
							break;
						}
					}
				}
				if ((MiddleLine[i + 1] - MiddleLine[i] > 8) && ((ABS(LeftEdge[i] - LeftEdge[i + 1] > 3) || ABS(RightEdge[i] - RightEdge[i + 1] > 3))) && i >= 30)
				{
					uint8 ii = i;

					while (1)
					{
						MiddleLine[ii + 1] = MiddleLine[ii] + 1;
						ii++;

						if (ii >= 50 || (MiddleLine[ii + 1] - MiddleLine[ii] <= 1))
						{
							break;
						}
					}
				}
			}
			else if (LeftEdge[i] != 0 && RightEdge[i] == ColumnMax)//find left
			{
				RightLose++;


				if (LeftEdge[i + 1] != 0)
				{
					MiddleLine[i] = MiddleLine[i + 1] + LeftEdge[i] - LeftEdge[i + 1];
				}
				else
				{
					MiddleLine[i] = LeftEdge[i] + Width[i] / 2;
				}
			}

			else if (LeftEdge[i] == 0 && RightEdge[i] != ColumnMax)//find right
			{
				LeftLose++;


				if (RightEdge[i + 1] != ColumnMax)
				{

					MiddleLine[i] = MiddleLine[i + 1] + RightEdge[i] - RightEdge[i + 1];
				}
				else
				{

					MiddleLine[i] = RightEdge[i] - Width[i] / 2;
				}
			}
			else if (LeftEdge[i] == 0 && RightEdge[i] == ColumnMax)//两边丢线    
			{
				WhiteNum++;
				AllLose++;

				if (WhiteNum == 1)
				{
					WhiteStart = i;
				}
				MiddleLine[i] = MiddleLine[i + 1];
			}

		}

		BlackAreaCountRow[i] = BlackAreaCount;
		if (i == 0)
		{
			AvaliableLines = 60;
			LastLine = 0;
			break;
		}
		uint16 m = MiddleLine[i];
		if (m < 5)
		{
			m = 5;
		}
		if (m > 75)
		{
			m = 75;
		}
		if ((LeftEdge[i] != 0 && LeftEdge[i] >= 65) || (RightEdge[i] != ColumnMax && RightEdge[i] < 15) || (i >= 1) && (img[i - 1][m] == Black_Point) && BlackAreaCount < BLACK_AREA_STARTLINE) //最后一行              
		{
			LastLine = i;//最后一行，动态前瞻
			AvaliableLines = 60 - i;//有效行数
			break;
		}

	}

	ConvertStartLine();

	if (EndLineEnableFlag && !EndLineFlag)
	{
		if (EndLineFlagCount >= 3)
			EndLineFlag = 1;

		if (StartLineStart && StartLineStart > LastLine)
		{
			EndLineFlagCount++;
			StartLinenNextClear = 0;
		}
		else if (EndLineFlagCount && !StartLinenNextClear) // 消抖
			StartLinenNextClear = 1;
		else if (StartLinenNextClear)
		{
			EndLineFlagCount = 0;
			StartLinenNextClear = 0;
		}
	}
}
#

// 计算起跑线区域
void ConvertStartLine()
{
	// 初始化
	int8 BlackAreaRowFlag = 0;
	int StartLineStartRow = 0;
	int BlackAreaRowCount = 0;
	int8 StartLineLock = 0;
	StartLineStart = 0;
	StartLineEnd = 0;

	for (int i = 0; i < RowMax - 1; i++)
	{
		if (BlackAreaCountRow[i] >= BLACK_AREA_STARTLINE)
		{
			BlackAreaCountRow[i] = 0;
			if (!BlackAreaRowFlag && !BlackAreaRowCount)
				StartLineStartRow = i;
			BlackAreaRowFlag = 1;
			BlackAreaRowCount++;
		}
		else
		{
			BlackAreaCountRow[i] = 0;
			if (!BlackAreaRowFlag) // 连续两行无起跑线特征则重置
			{
				if (BlackAreaRowCount >= 3)
				{ // 重置时连续 3 行以上有起跑线特征则被认为是起跑线区域
					if (!StartLineLock)
					{
						StartLineStart = StartLineStartRow;
						StartLineEnd = i - 1;
						StartLineLock = 1;
					}
					for (int j = StartLineStartRow; j <= i - 1; j++)
						BlackAreaCountRow[j] = 1;
				}
				BlackAreaRowCount = 0;
			}
			else
			{
				BlackAreaRowFlag = 0;
			}
		}
	}
}

//提取图像的特征
//
//内部调用
void GetBlackEndParam()//获取黑线截止行
{

	unsigned char LEndFlag = 0;//标志位
	unsigned char MEndFlag = 0;
	unsigned char REndFlag = 0;
	unsigned char MREndFlag = 0;
	unsigned char MLEndFlag = 0;
	unsigned char LLEndFlag = 0;
	unsigned char RREndFlag = 0;

	int i = 0;

	BlackEndMR = 0;//清零
	BlackEndML = 0;
	BlackEndLL = 0;
	BlackEndRR = 0;
	BlackEndL = 0;
	BlackEndM = 0;
	BlackEndR = 0;

	for (i = RowMax - 1; i >= 3; i--)
	{
		if (img[i][ColumnMax / 2] == White_Point && !MEndFlag)//!MEndFlag=1 //40//如果该行中间的那个像素是白色,而且MEndFlag标志位没有置位(以下注释按数组最小下标为1来计算)
		{
			BlackEndM++;//中黑线截至行
		}
		else if (i > 1 && img[i - 1][ColumnMax / 2] == Black_Point && img[i - 2][ColumnMax / 2] == Black_Point)//连续两行是黑色//如果下一行跟下下一行的中间都是黑色        
		{
			MEndFlag = 1;//MEndFlag置位
		}
		if (img[i][ColumnMax / 4] == White_Point && !LEndFlag)//20//如果该行从左数第20个像素为白色,且LEndFlag没有置位
		{
			BlackEndL++;//左黑线截至行
		}
		else if (i > 1 && img[i - 1][ColumnMax / 4] == Black_Point && img[i - 2][ColumnMax / 4] == Black_Point)//如果下一行和下下一行的从左数第20个像素都是黑色
		{
			LEndFlag = 1;//LEndFlag置位
		}
		if (img[i][ColumnMax * 3 / 4] == White_Point && !REndFlag)//60//如果该行从右数第20个像素为白色,且REndFlag没有置位
		{
			BlackEndR++;//右黑线截至行
		}
		else if (i > 1 && img[i - 1][ColumnMax * 3 / 4] == Black_Point && img[i - 2][ColumnMax * 3 / 4] == Black_Point)//如果下一行和下下一行的从右数第20个像素都是黑色
		{
			REndFlag = 1;//REndFlag置位
		}
		if (img[i][30] == White_Point && !MLEndFlag)//如果该行从左往右数第30个像素是白色而且MLEndFlag没有置位
		{
			BlackEndML++;
		}
		else if (i > 1 && img[i - 1][30] == Black_Point && img[i - 2][30] == Black_Point)//如果下一行和下下一行从左往右数第30个像素都是白色
		{
			MLEndFlag = 1;//MLEndFlag置位
		}
		if (img[i][50] == White_Point && !MREndFlag)//如果该行从右往左数第30个像素是白色而且MREndFlag没有置位
		{
			BlackEndMR++;
		}
		else if (i > 1 && img[i - 1][50] == Black_Point && img[i - 2][50] == Black_Point)//如果下一行和下下一行从右往左数第30个像素都是白色
		{
			MREndFlag = 1;//MREndFlag置位
		}
		if (img[i][10] == White_Point && !LLEndFlag)//如果该行从左往右数第10个像素是白色而且LLEndFlag没有置位
		{
			BlackEndLL++;
		}
		else if (i > 1 && img[i - 1][10] == Black_Point && img[i - 2][10] == Black_Point)//如果下一行和下下一行从左往右数第10个像素都是白色
		{
			LLEndFlag = 1;//LLEndFlag置位
		}
		if (img[i][70] == White_Point && !RREndFlag)//如果该行从右往左数第10个像素是白色而且RREndFlag没有置位
		{
			BlackEndRR++;
		}
		else if (i > 1 && img[i - 1][70] == Black_Point && img[i - 2][70] == Black_Point)//如果下一行和下下一行从右往左数第10个像素都是白色
		{
			RREndFlag = 1;//RREndFlag置位
		}
	}


	BlackEndMax = MAX(BlackEndL, BlackEndM);//取大值
	BlackEndMax = MAX(BlackEndMax, BlackEndR);
	BlackEndMaxMax = MAX(BlackEndMax, BlackEndMR);
	BlackEndMaxMax = MAX(BlackEndMax, BlackEndML);
	BlackEndMaxMax = MAX(BlackEndMax, BlackEndLL);
	BlackEndMaxMax = MAX(BlackEndMax, BlackEndRR);
	if (BlackEndMaxMax >= 60)
	{
		BlackEndMaxMax = 58;
	}
	DropRow = 60 - BlackEndMaxMax;//封顶的行数      
}








