#include "common.h"
#include "include.h"


/*********define for RecognitionObstacle**********/
unsigned char  ObstacleLfetRowStart = 0;
unsigned char  ObstacleLfetColStart = 0;
unsigned char  ObstacleLfetOkFlag = 0;
unsigned char  ObstacleRightRowStart = 0;
unsigned char  ObstacleRightColStart = 0;
unsigned char  ObstacleRightOkFlag = 0;

int IsStartLine = 0;

InflectionPoint  InflectionPointL;//建立左边拐点结构体变量
InflectionPoint  InflectionPointR;//建立右边拐点结构体变量

#define TestInflectionPointL  1//此宏定义置1，就可以对左边最近拐点进行测试
#define TestInflectionPointR  1//此宏定义置1，就可以对右边最近拐点进行测试

CrossType Cross;//建立十字结构体变量

#define TestRightSideling     0 //此宏定义置1，就可以对右斜入进行测试
#define TestLeftSideling      0 //此宏定义置1，就可以对左斜入进行测试

LoopType Loop;//建立圆环结构体变量
LoopExitType LoopExit;//建立圆环出口类型结构体变量
LoopScan Scan;

//用于圆环补线的路宽
//此数据不同的摄像头安装需要重新测
//测的方法：把车子正放在赛道中间，利用Jlink在线调试，用每行的右线-左线得到
unsigned char  LoopWidth[61] = { 2,3,3,3,4,4,5,5,6,6,
							   8,8,10,10,12,13,14,14,16,17,
							   18,18,20,20,22,22,24,24,26,26,
							   28,30,31,32,32,34,36,36,38,39,
							   41,41,43,43,45,45,47,47,49,50,
							   50,51,52,54,55,56,57,58,59,60,61 };

#define  TestStrightIntoLoop      0 //此宏定义置1，就可以对直入环道进行测试
#define  TestLoopEixt             0 //此宏定义置1，就可以环道出口方向进行测试
unsigned char LoopEixtR[60];  //寻找出口时存放数据
unsigned char LoopEixtL[60];  //寻找出口时存放数据
TrackTypeFlag  Tracktype;//赛道类型结构体变量
//十字方式二用到的变量
#if ClossRecognition2
//竖向扫描，记录行坐标的数组
unsigned char LeftScanCol[80];
unsigned char RightScanCol[80];
unsigned char StrightIntoCrossL;//直入十字，存在左拐点的标志位
unsigned char StrightIntoCrossR;//直入十字，存在右拐点的标志
unsigned char StrightIntoCrossLR;//直入十字，存在左右拐点的标志位
unsigned char StrightIntoCrossNotLR;//直入十字，不存在右拐点的标志位
#endif

//对右边线进行操作，如果存在多个拐点寻找最近的拐点
//prt:传入一个结构体变量的地址
//Start：扫描的起始行
//End ：扫描的结束行

void FindInflectionPointR(InflectionPoint *prt, unsigned char Start, unsigned char End)
{
	int i;
	//初始化结构体变量
	prt->ScanInflectionPointStart = Start;//设置扫描的起始行
	prt->ScanInflectionPointEnd = End;//设置扫描的结束行
	prt->InflectionPointRow = 0;
	prt->InflectionPointCol = 0;
	prt->InflectionPointFlag = 0;

	for (i = prt->ScanInflectionPointStart; i >= prt->ScanInflectionPointEnd; i--)//不能扫描太远，否则会误判
	{
		if ((RightEdge[i] != ColumnMax&&RightEdge[i - 1] != ColumnMax&&RightEdge[i + 1] != ColumnMax&&RightEdge[i - 2] != ColumnMax)
			&& (RightEdge[i] > 10 && RightEdge[i - 1] > 10 && RightEdge[i + 1] > 10 && RightEdge[i - 2] > 10)) //连续四行不丢线,并未为了防止跳变，这4行必须大于10
		{
			if ((RightEdge[i] - RightEdge[i + 1] <= 0) && (RightEdge[i - 1] - RightEdge[i]) >= 1 && (RightEdge[i - 2] - RightEdge[i]) >= 2)//找到右边线有拐点
			{
				prt->InflectionPointRow = i;//记录拐点的行
				prt->InflectionPointCol = RightEdge[i];//记录拐点的列
				prt->InflectionPointFlag = 1;//标记找到右拐点
				break;//退出for
			}

		}

		//避免波动，多考虑一种情况:本行丢线，前面连续两行和后面连续两行都不丢线，并且不丢线的都大于10

		else if ((RightEdge[i] == ColumnMax&&RightEdge[i - 1] != ColumnMax&&RightEdge[i + 1] != ColumnMax&&RightEdge[i - 2] != ColumnMax&&RightEdge[i + 2] != ColumnMax)
			&& (RightEdge[i] > 10 && RightEdge[i - 1] > 10 && RightEdge[i + 1] > 10 && RightEdge[i - 2] > 10 && RightEdge[i + 2] > 10))
		{
			prt->InflectionPointRow = i;//记录拐点的行
			prt->InflectionPointCol = RightEdge[i];//记录拐点的列
			prt->InflectionPointFlag = 1;//标记找到右拐点
			break;//退出for

		}
	}

	//对是否找到右拐点进行测试
#if TestInflectionPointR

	if (prt->InflectionPointFlag)
	{
		RightUpLedBright;
	}
	else
	{
		RightUpLedDark;
	}

#endif
}

//对左边线进行操作，如果存在多个拐点寻找最近的拐点
//prt:传入一个结构体变量的地址
//Start：扫描的起始行
//End ：扫描的结束行

void FindInflectionPointL(InflectionPoint *prt, unsigned char Start, unsigned char End)
{

	int i;
	//初始化结构体变量
	prt->ScanInflectionPointStart = Start;//设置扫描的起始行
	prt->ScanInflectionPointEnd = End;//设置扫描的结束行
	prt->InflectionPointRow = 0;
	prt->InflectionPointCol = 0;
	prt->InflectionPointFlag = 0;

	//左拐点
	for (i = prt->ScanInflectionPointStart; i > prt->ScanInflectionPointEnd; i--)
	{
		if (LeftEdge[i] != 0 && LeftEdge[i - 1] != 0 && LeftEdge[i + 1] != 0 && LeftEdge[i - 2] != 0  //连续4行不丢线，并且这4行都小于70
			&& LeftEdge[i] < 70 && LeftEdge[i - 1] < 70 && LeftEdge[i + 1] < 70 && LeftEdge[i - 2] < 70)
		{

			if ((LeftEdge[i] - LeftEdge[i + 1] >= 0) && (LeftEdge[i - 1] - LeftEdge[i] <= -1) && (LeftEdge[i - 2] - LeftEdge[i] <= -2))//找到拐点
			{
				prt->InflectionPointRow = i;//记录该拐点的行
				prt->InflectionPointCol = LeftEdge[i];//记录该拐点的列
				prt->InflectionPointFlag = 1;//标记找到左拐点
				break;//找到退出
			}
		}

		//避免波动，多考虑一种情况:本行丢线，前面连续两行和后面连续两行都不丢线，并且不丢线的都小于70
		else if (LeftEdge[i] == 0 && LeftEdge[i - 1] != 0 && LeftEdge[i + 1] != 0 && LeftEdge[i - 2] != 0 && LeftEdge[i + 2] != 0
			&& LeftEdge[i] < 70 && LeftEdge[i - 1] < 70 && LeftEdge[i + 1] < 70 && LeftEdge[i - 2] < 70 && LeftEdge[i + 2] < 70)
		{

			prt->InflectionPointRow = i;//记录该拐点的行
			prt->InflectionPointCol = LeftEdge[i];//记录该拐点的列
			prt->InflectionPointFlag = 1;//标记找到左拐点
			break;//找到退出

		}

	}

#if  TestInflectionPointL

	if (prt->InflectionPointFlag)
	{
		LeftUpLedBright;
	}
	else
	{
		LeftUpLedDark;
	}

#endif

}



#if ClossRecognition1//方式1，比较粗糙

//对十字进行识别
//prt:传入一个结构体变量的地址

void CrossRecognition(CrossType *prt)
{
	//prt->LastStrightInto=prt->StrightInto;
	//初始化变量
	prt->StrightInto = 0;
	prt->RightSideling = 0;
	prt->LeftSideling = 0;
	prt->CrossFlag = 0;

	//寻找左右拐点
	FindInflectionPointR(&InflectionPointR, 57, 25);
	FindInflectionPointL(&InflectionPointL, 57, 25);

	if (!LoopExit.LeftControlExit && (!LoopExit.RightControlExit))//当遇到环道控制的时候，不再进行十字识别
	{

		//如果左边界和右边界同时丢线大于3行，就认为遇到了十字

		if (AllLose >= 6 && LastLine < 25)
		{
			prt->StrightInto = 1;//直入十字 
		}
		//判断右斜入十字情况

		else if (InflectionPointR.InflectionPointFlag&&InflectionPointL.InflectionPointFlag == 0
			&& InflectionPointR.InflectionPointRow <= 50)//如果只存在右边拐点，可能右斜入十字，利用拐点的行去限制
		{
			//利用倾斜度和视野的关系
			if (InflectionPointR.InflectionPointCol > 40)//车体右倾
			{
				//在车体右倾的情况下，如果是十字，视野的左边赛道会比中间的远
				if (BlackEndL >= BlackEndM)
				{
					//拐点的另一边线，上面两行丢线，下面两行丢线

					if (LeftEdge[InflectionPointR.InflectionPointRow + 1] == 0 && LeftEdge[InflectionPointR.InflectionPointRow + 2] == 0
						&& LeftEdge[InflectionPointR.InflectionPointRow - 2] == 0 && LeftEdge[InflectionPointR.InflectionPointRow - 3] == 0)
					{
						prt->RightSideling = 1;
					}

				}
			}

			else if (InflectionPointR.InflectionPointCol <= 40)//车体左倾
			{
				if (BlackEndLL >= BlackEndL&&BlackEndLL > BlackEndML)
				{
					//拐点的另一边线，上面两行丢线，下面两行丢线

					if (LeftEdge[InflectionPointR.InflectionPointRow + 1] == 0 && LeftEdge[InflectionPointR.InflectionPointRow + 2] == 0
						&& LeftEdge[InflectionPointR.InflectionPointRow - 2] == 0 && LeftEdge[InflectionPointR.InflectionPointRow - 3] == 0)
					{
						prt->RightSideling = 1;

					}

				}
			}
		}

		//判断左斜入十字

		else if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag == 0
			&& InflectionPointL.InflectionPointRow <= 50)//只存在左边拐点，左斜入十字
		{
			//利用倾斜度和视野的关系
			if (InflectionPointL.InflectionPointCol < 40)
			{
				if (BlackEndR > BlackEndM)
				{
					if (RightEdge[InflectionPointL.InflectionPointRow + 1] == 80 && RightEdge[InflectionPointL.InflectionPointRow + 2] == 80
						&& RightEdge[InflectionPointL.InflectionPointRow - 2] == 80 && RightEdge[InflectionPointL.InflectionPointRow - 3] == 80)
					{
						prt->LeftSideling = 1;

					}

				}
			}

			else if (InflectionPointL.InflectionPointCol >= 40)
			{
				if (BlackEndRR >= BlackEndR&&BlackEndRR > BlackEndMR)
				{
					if (RightEdge[InflectionPointL.InflectionPointRow + 1] == 80 && RightEdge[InflectionPointL.InflectionPointRow + 2] == 80
						&& RightEdge[InflectionPointL.InflectionPointRow - 2] == 80 && RightEdge[InflectionPointL.InflectionPointRow - 3] == 80)
					{
						prt->LeftSideling = 1;

					}
				}
			}

		}

		//测试右斜入十字                                                                     
#if TestRightSideling

		if (prt->RightSideling)
		{
			RightUnderLedBright;
		}
		else
		{
			RightUnderLedDark;
		}

#endif   


		//测试左斜入

#if TestLeftSideling

		if (prt->LeftSideling)
		{
			LeftUnderLedBright;
		}
		else
		{
			LeftUnderLedDark;
		}

#endif

	}
}

//对十字进行处理，得出舵机的控制中线
//经测试，这种方法还不够好，后期可能要进行斜率补线

void CrossConduct()
{

	int i;
	//直入十字可以分为两种情况 ：1.底部的线还存在，2.底部丢线
	if (Cross.StrightInto)
	{

		if ((LeftEdge[58] != 0 && LeftEdge[57] != 0 && LeftEdge[56] != 0 && LeftEdge[55] != 0
			)

			|| (RightEdge[58] != 80 && RightEdge[57] != 80 && RightEdge[56] != 80 && RightEdge[55] != 80
				))//底部4行一边不丢线
		{
			//对中线进行平滑
			for (i = 58; i > LastLine; i--)
			{
				if (ABS(MiddleLine[i] - MiddleLine[i + 1]) >= 2)
				{
					MiddleLine[i] = MiddleLine[i + 1];
				}

			}
		}

		else
		{
			Cross.CrossFlag = 1;//这种情况直接取后面的

		}
	}
	else if (Cross.LeftSideling)//左斜入十字
	{
		//拐点行上面的中线不要
		for (i = InflectionPointL.InflectionPointRow; i > LastLine; i--)
		{
			MiddleLine[i] = 40;//只是为了显示
		}
	}

	else if (Cross.RightSideling)//右斜入十字
	{
		//拐点行上面的中线不要
		for (i = InflectionPointR.InflectionPointRow; i > LastLine; i--)
		{
			MiddleLine[i] = 40;//只是为了显示
		}
	}
}

#endif



#if ClossRecognition2//方式2

void CrossRecognition(CrossType *prt)
{
	int i, j, ScanEndCol;

	//初始化变量
	prt->StrightInto = 0;
	prt->RightSideling = 0;
	prt->LeftSideling = 0;
	prt->CrossFlag = 0;

	//寻找左右拐点
	FindInflectionPointR(&InflectionPointR, 57, 25);
	FindInflectionPointL(&InflectionPointL, 57, 25);

	if ((!LoopExit.LeftControlExit) && (!LoopExit.RightControlExit))//当遇到环道控制的时候，不再进行十字识别
	{
		//如果左边界和右边界同时丢线大于5行，就认为遇到了十字
		if (AllLose >= 7 && LastLine < 25)
		{
			prt->StrightInto = 1;//直入十字 
		}
		//判断右斜入十字情况

		else if (InflectionPointR.InflectionPointFlag //存在右边拐点
			&&InflectionPointL.InflectionPointFlag == 0//并且不存在左边拐点
			&& InflectionPointR.InflectionPointRow <= 52
			&& ((BlackEndLL >= BlackEndM) || BlackEndML >= BlackEndM))//并且右拐点的离车体较远
		{

			for (i = 0; i < 80; i++)
			{
				RightScanCol[i] = 0;//初始化数组
			}
			//选择最远的那一列
			if (BlackEndM >= BlackEndML&&BlackEndM >= BlackEndLL)//中间的视野最远
			{
				ScanEndCol = 40;
			}
			else if (BlackEndML > BlackEndM&&BlackEndML > BlackEndLL)//中间偏左视野最远
			{
				ScanEndCol = 30;
			}

			else if (BlackEndLL > BlackEndM&&BlackEndLL > BlackEndML)//最左视野最远
			{
				ScanEndCol = 20;
			}

			//以右拐点的列作为基准，视野最远的一列最为结束，从右往左竖向扫描
		   //寻找十字上面的直角

			for (j = InflectionPointR.InflectionPointCol; j > ScanEndCol; j--)
			{
				//以拐点的行为起始基准，最远列的行作为结束行
				for (i = InflectionPointR.InflectionPointRow; i > DropRow; i--)
				{
					if (img[i][j] == White_Point&&img[i - 1][j] == Black_Point&&img[i - 2][j] == Black_Point)
					{
						RightScanCol[j] = i - 1;
						break;
					}
				}
			}
			for (j = InflectionPointR.InflectionPointCol; j > ScanEndCol; j--)
			{
				if (RightScanCol[j] != 0 && RightScanCol[j - 1] != 0 && RightScanCol[j + 1] != 0)
					if (RightScanCol[j] - RightScanCol[j + 1] >= 0 && RightScanCol[j] - RightScanCol[j - 1] < 0)
					{
						prt->RightSideling = 1;
					}
			}

		}
		//判断左斜入十字情况
		else if (InflectionPointR.InflectionPointFlag == 0 //存在左边拐点
			&& InflectionPointL.InflectionPointFlag//并且不存在右边拐点
			&&InflectionPointL.InflectionPointRow <= 52
			&& ((BlackEndRR >= BlackEndM) || BlackEndMR >= BlackEndM))//并且左拐点的离车体较远
		{
			for (i = 0; i < 80; i++)
			{
				LeftScanCol[i] = 0;//初始化数组
			}

			//选择最远的那一列
			if (BlackEndM >= BlackEndMR&&BlackEndM >= BlackEndRR)
			{
				ScanEndCol = 40;
			}
			else if (BlackEndMR > BlackEndM&&BlackEndMR > BlackEndRR)
			{
				ScanEndCol = 50;
			}

			else if (BlackEndRR > BlackEndM&&BlackEndRR > BlackEndMR)
			{
				ScanEndCol = 60;
			}

			for (j = InflectionPointL.InflectionPointCol; j < ScanEndCol; j++)
			{
				//以拐点的行为起始基准，最远列的行作为结束行
				for (i = InflectionPointL.InflectionPointRow; i > DropRow; i--)
				{
					if (img[i][j] == White_Point&&img[i - 1][j] == Black_Point&&img[i - 2][j] == Black_Point)
					{
						LeftScanCol[j] = i - 1;
						break;
					}
				}
			}
			for (j = InflectionPointL.InflectionPointCol; j < ScanEndCol; j++)
			{
				if (LeftScanCol[j] != 0 && LeftScanCol[j - 1] != 0 && LeftScanCol[j + 1] != 0)
					if (LeftScanCol[j] - LeftScanCol[j + 1] <= 0 && LeftScanCol[j] - LeftScanCol[j - 1] > 0)
					{
						prt->LeftSideling = 1;//标志左斜入十字
					}
			}
		}



		//测试右斜入十字                                                                     
#if TestRightSideling 
//K60核心板右下角的led灯亮，代表此时检测到右斜入十字
		RightUnderLedTest(prt->RightSideling);
#endif   


		//测试左斜入十字

#if TestLeftSideling
//K60核心板右下角的led灯亮，代表此时检测到右斜入十字
		LeftUnderLedTest(prt->LeftSideling);
#endif

	}

}

//对十字进行处理，得出舵机的控制中线
//经测试，此方法可以达到十字控制要求，基本不存在抖动情况

void CrossConduct()
{
	int i;
	//初始化变量
	StrightIntoCrossL = 0;
	StrightIntoCrossR = 0;
	StrightIntoCrossLR = 0;
	StrightIntoCrossNotLR = 0;
	//直入十字可以分为两种情况 ：1.底部的线还存在，2.底部丢线
	if (Cross.StrightInto)
	{
		//底部4行有一边不丢线
		if ((LeftEdge[58] != 0 && LeftEdge[57] != 0 && LeftEdge[56] != 0 && LeftEdge[55] != 0)
			|| (RightEdge[58] != 80 && RightEdge[57] != 80 && RightEdge[56] != 80 && RightEdge[55] != 80))
		{

			if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag == 0)
			{
				StrightIntoCrossL = 1;

			}
			//存在左拐点，不存右拐点
			else if (InflectionPointL.InflectionPointFlag == 0 && InflectionPointR.InflectionPointFlag)
			{
				StrightIntoCrossR = 1;
			}
			//存在左拐点，存在右拐点
			else if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag)
			{
				StrightIntoCrossLR = 1;
			}
			else
			{
				//平滑中线
				for (i = 57; i > LastLine; i--)
				{
					if (ABS(MiddleLine[i] - MiddleLine[i - 1]) > 2)
					{
						MiddleLine[i] = MiddleLine[i + 1];
					}

				}
			}

		}

		else
		{
			Cross.CrossFlag = 1;//这种情况直接取后面的

		}
	}

}



#endif


//对圆环进行识别
//利用左右拐点和中间黑圆的跳变
//因为环道前面为直道，一般只考虑直入

void LoopRecognition(LoopType *prt)
{

	int i;
	static unsigned char StrightIntoLoopNum = 0;//记录直入圆环的次数，滤掉偶然情况

	//初始化结构体变量
   // prt->StrightIntoLoop=0;//锁住直入圆环的标志位
	//这两个斜入标志，暂时还没用到
	prt->LeftSidelingLoop = 0;
	prt->RightSidelingLoop = 0;

	//直入圆环
	//左右拐点同时找到，左右同时丢线就不考虑了
	if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag&&AllLose < 4 && Loop.StrightIntoLoop == 0)
	{

		if (ABS(InflectionPointL.InflectionPointRow - InflectionPointR.InflectionPointRow) < 6//左右拐点的行数小于某个值
		   //
			&& ABS(LeftEdge[InflectionPointL.InflectionPointRow - 1] - RightEdge[InflectionPointR.InflectionPointRow - 1]) >=
			ABS(LeftEdge[InflectionPointL.InflectionPointRow] - RightEdge[InflectionPointR.InflectionPointRow]))
		{
			StrightIntoLoopNum++;
			if (StrightIntoLoopNum >= 2)//连续两次满足条件，判定为直入圆环
			{
				prt->StrightIntoLoop = 1;
				StrightIntoLoopNum = 0;
			}
		}
		else
		{
			StrightIntoLoopNum = 0;//清掉
		}
	}

	//左斜入环路
//只有一个拐点，从拐点往上操作会有另一个拐点
	else if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag == 0 && AllLose < 2 && LeftLose < 3
		&& RightLose >= 5
		&& RightEdge[56] != 80
		&& RightEdge[55] != 80
		&& RightEdge[54] != 80
		&& RightEdge[53] != 80)
	{
		for (i = InflectionPointL.InflectionPointRow - 3; i > LastLine; i--)
		{
			if (LeftEdge[i] != 0 && LeftEdge[i - 1] != 0 && LeftEdge[i + 1] != 0 && LeftEdge[i - 2] != 0
				&& LeftEdge[i] < InflectionPointL.InflectionPointCol
				&&LeftEdge[i - 1] < InflectionPointL.InflectionPointCol
				&&LeftEdge[i + 1] < InflectionPointL.InflectionPointCol
				&&LeftEdge[i - 2] < InflectionPointL.InflectionPointCol)
			{
				if (LeftEdge[i] - LeftEdge[i + 1] <= 0 && LeftEdge[i - 1] - LeftEdge[i] > 0 && LeftEdge[i - 2] - LeftEdge[i] > 1)
				{
					prt->LeftSidelingLoop = 1;
					break;
				}
			}
		}

	}
	//右斜入环路
	//只有一个拐点，从拐点往上操作会有另一个拐点
	//左边底部不丢线
	else if (InflectionPointL.InflectionPointFlag == 0 && InflectionPointR.InflectionPointFlag&&AllLose < 2 && RightLose < 3
		&& LeftLose >= 5
		&& LeftEdge[56] != 0
		&& LeftEdge[55] != 0
		&& LeftEdge[54] != 0
		&& LeftEdge[53] != 0)
	{
		for (i = InflectionPointR.InflectionPointRow - 3; i > LastLine; i--)
		{
			if (RightEdge[i] != 80 && RightEdge[i - 1] != 80 && RightEdge[i + 1] != 80 && RightEdge[i - 2] != 80
				&& RightEdge[i] > InflectionPointR.InflectionPointCol
				&&RightEdge[i - 1] > InflectionPointR.InflectionPointCol
				&&RightEdge[i - 2] > InflectionPointR.InflectionPointCol
				&&RightEdge[i + 1] > InflectionPointR.InflectionPointCol)
			{
				if (RightEdge[i] - RightEdge[i + 1] >= 0 && RightEdge[i - 1] - RightEdge[i] < 0 && RightEdge[i - 2] - RightEdge[i] < 1)
				{
					prt->RightSidelingLoop = 1;
					break;
				}
			}
		}

	}
#if TestStrightIntoLoop

	BuzzerTest(Loop.StrightIntoLoop);
	//BuzzerTest(LoopExit.LoopInFlag);

#endif

}

//寻找圆环的最优出口
//如果环太大，可能无法识别到最优出口，但是确保有一边可以走

void FindLoopOptimalExit()
{
	int i, j, start;

	unsigned char LoopNumberL = 0;
	unsigned char LoopNumberR = 0;
	//初始化变量
	//有环道标志位，并且还没找到出口
	if (Loop.StrightIntoLoop && (!LoopExit.RightControlExit) && (!LoopExit.LeftControlExit))
	{
		//初始化数组
		for (i = 0; i < 60; i++)
		{
			LoopEixtR[i] = 80;
		}
		//从封顶的前两行开始寻找,重新找出更多的左右线
		for (i = LastLine; i > 12; i--)//先找右边
		{
			//根据不同的情况判断扫描起始行
			if (i == LastLine)
			{
				start = RightEdge[LastLine + 2] - 3;
			}

			else
			{
				if (LoopEixtR[i + 1] != 80)
				{
					start = LoopEixtR[i + 1] - 3;//边缘追踪的思想
				}

				else
				{
					start = RightEdge[LastLine + 2] - 3;
				}
			}

			for (j = start; j < 78; j++)
			{
				if (img[i][j] == White_Point&&img[i][j + 1] == Black_Point)//白->黑跳变
				{
					LoopEixtR[i] = j;
					break;//找到跳变退出
				}
			}

		}
		//再找左边
		for (i = 0; i < 60; i++)
		{
			LoopEixtL[i] = 0;
		}

		for (i = LastLine; i > 12; i--)
		{
			if (i == LastLine)
			{
				start = LeftEdge[LastLine + 2] + 3;
			}

			else
			{
				if (LoopEixtL[i + 1] != 0)
				{
					start = LoopEixtL[i + 1] + 3;
				}

				else
				{
					start = LeftEdge[LastLine + 2] + 3;
				}
			}
			for (j = start; j > 1; j--)
			{
				if (img[i][j] == White_Point&&img[i][j - 1] == Black_Point)//跳变
				{
					LoopEixtL[i] = j;
					break;//找到跳变退出
				}
			}

		}

		for (i = LastLine; i > 12; i--)
		{
			if (LoopEixtL[i] != 0)
			{
				LoopNumberL++;//记录左边找到了多少行

			}

			if (LoopEixtR[i] != 80)
			{
				LoopNumberR++;//记录左边找到了多少行
			}

		}

		if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag)//正的
		{
			Scan.StarCol = 0;
			Scan.StarCol = (InflectionPointL.InflectionPointCol + InflectionPointR.InflectionPointCol) / 2;
			if (Scan.StarCol >= 37 && Scan.StarCol <= 41)
			{

				if (((LastLine - 12) - LoopNumberR <= 3) && ((LastLine - 12) - LoopNumberL > 3))
				{

					LoopExit.LeftNearExit = 1;//标记找到左边近处
				}

				else if (((LastLine - 12) - LoopNumberR > 3) && ((LastLine - 12) - LoopNumberL <= 3))
				{
					LoopExit.RightNearExit = 1;//标记找到左边近处
				}
			}

			else if (Scan.StarCol > 41)//偏左
			{
				if ((LastLine - 12) - LoopNumberL > 2)
				{
					LoopExit.LeftNearExit = 1;//标记找到左边近处
				}
				else
				{
					LoopExit.RightNearExit = 1;//标记找到右边近处
				}

			}

			else if (Scan.StarCol < 37)//偏右
			{
				if ((LastLine - 12) - LoopNumberR > 2)
				{
					LoopExit.RightNearExit = 1;//标记找到右边近处
				}
				else
				{
					LoopExit.LeftNearExit = 1;//标记找到左边近处
				}

			}

		}

		else if (!InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag)//偏右
		{
			LoopExit.RightNearExit = 1;//标记找到右边近处
		}

		else if (!InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag)//偏左
		{
			LoopExit.LeftNearExit = 1;//标记找到左边近处
		}

		else  //如果前面都没有一个明确的方向，就默认左边
		{
			LoopExit.LeftNearExit = 1;//标记找到左边近处
		}

		//获取最终的出口方向控制信号，只成功获取1次即可，然后直到控制标志位被清掉
		if (LoopExit.LeftNearExit && (!LoopExit.RightControlExit) && (!LoopExit.LeftControlExit))//左边出口
		{
			LoopExit.LeftControlExit = 1;
			LoopExit.LeftNearExit = 0;
			LoopExit.RightNearExit = 0;
			//Loop.StrightIntoLoop=0;
		}

		else if (LoopExit.RightNearExit && (!LoopExit.RightControlExit) && (!LoopExit.LeftControlExit))
		{
			LoopExit.RightControlExit = 1;
			LoopExit.LeftNearExit = 0;
			LoopExit.RightNearExit = 0;
			//Loop.StrightIntoLoop=0;
		}

	}

}

//上面获取完环道出口方向，以下对环道进行入口进行补线
//补线的思路是利用赛道的半宽

void LoopIntoRepair()
{
	int i;

	if (LoopExit.RightControlExit)//如果获得了出口在右边的控制信号
	{
		if (LoopExit.LoopInDelay)
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = RightEdge[i] - LoopWidth[i] / 3;//右边线-赛道半宽
			}
		}
		else if (LoopExit.IntoLoopOk || LoopExit.LoopInFlag)
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = RightEdge[i] - LoopWidth[i] / 2;//右边线-赛道半宽
			}

		}
		else
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = RightEdge[i] - LoopWidth[i] / 2;//右边线-赛道半宽
			}
		}
	}


	else if (LoopExit.LeftControlExit)//如果获得了出口在左边的控制信号
	{
		if (LoopExit.LoopInDelay)
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = LeftEdge[i] + LoopWidth[i] / 3;//右边线-赛道半宽
			}
		}
		else if (LoopExit.IntoLoopOk || LoopExit.LoopInFlag)
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = LeftEdge[i] + LoopWidth[i] / 2;//左边线+赛道半宽

			}
		}
		else
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = LeftEdge[i] + LoopWidth[i] / 2;//左边线+赛道半宽
			}
		}
	}
}

//出了环道口再清控制标志位,主要是判断啥时候到了出口
//为了避免状态的切换遇到误判，会使用一些标志位进行短延时
void ClearLoopControl()
{
	//延时自加变量
	static int N = 0;
	static int M = 0;
	static int CloseThis = 0;
	if (LoopExit.LeftControlExit&&LoopExit.LoopInFlag == 0 && CloseThis == 0)//左边出口的时候,看右边界的特征
	{
		//首先判断入环成功
	   //左边拐点的
		if (InflectionPointL.InflectionPointRow > 55
			|| (InflectionPointL.InflectionPointFlag == 0
				&& InflectionPointR.InflectionPointFlag == 0))//同出口方向的拐点大于某个值或者两个拐点同时消失
		{
			LoopExit.IntoLoopOk = 1;//标志入环成功
			CloseThis = 1;
		}


	}
	else if (LoopExit.RightControlExit&&LoopExit.LoopInFlag == 0 && CloseThis == 0)
	{
		if (InflectionPointR.InflectionPointRow > 55
			|| (InflectionPointR.InflectionPointFlag == 0
				&& InflectionPointL.InflectionPointFlag == 0))
		{

			LoopExit.IntoLoopOk = 1;//标志入环成功
			CloseThis = 1;

		}
	}

	//如果入环成功

	if (LoopExit.IntoLoopOk)
	{
		//此处应该有个小延时，稳定一下
		M++;
		if (M > 2)
		{
			M = 0;
			if (LoopExit.RightControlExit)//并且获取到的出口是右边
			{  //不在环内，并且左边有拐点
				if (InflectionPointL.InflectionPointFlag)
				{
					LoopExit.LoopInFlag = 1; //标记进环内了
					LoopExit.IntoLoopOk = 0;//清掉进环的标志
				}
			}
			else if (LoopExit.LeftControlExit)//并且获取到的出口是左边
			{
				//不在环内，并且右边有拐点
				if (InflectionPointR.InflectionPointFlag)
				{
					LoopExit.LoopInFlag = 1; //标记进环内了
					LoopExit.IntoLoopOk = 0; //清掉进环的标志
				}
			}
		}
	}

	//正常情况下，车子已经进入到环内了，但此时仍然属于环道控制位作用
	//接下来就是想办法寻找出口的特征，清掉环道的控制位
	//如果不能正常清标志，则这里需要保险延时清标志

	if (LoopExit.LoopInFlag&&LoopExit.RightControlExit&& LoopExit.LoopInDelay == 0)//环内，出口在右边
	{
		//右边拐点行大于某个值，并且还没有开清标志延时标志位

		if (InflectionPointR.InflectionPointRow >= 35)//这里有问题
		{
			LoopExit.LoopInFlag = 0;//清掉环内的标志位
			LoopExit.LoopInDelay = 1;//开延时标志位，开始延时清环道控制标志
		}
	}

	else if (LoopExit.LoopInFlag&&LoopExit.LeftControlExit&& LoopExit.LoopInDelay == 0)//环内，出口在左边
	{
		if (InflectionPointL.InflectionPointRow >= 35)
		{
			LoopExit.LoopInFlag = 0;//清掉环内的标志位
			LoopExit.LoopInDelay = 1;//开延时标志位，开始延时清环道控制标
		}
	}

	if (LoopExit.LoopInDelay)//开始延时清环道控制标
	{
		N++;
		if (N >= 12)
		{
			N = 0;
			LoopExit.LoopInDelay = 0;//清掉延时标志位
			LoopExit.LeftControlExit = 0;//清掉环道左出口控制位
			LoopExit.RightControlExit = 0;//清掉环道右出口控制位
			Loop.StrightIntoLoop = 0;
			Loop.LeftSidelingLoop = 0;
			Loop.RightSidelingLoop = 0;
			CloseThis = 0;

		}
	}

#if TestLoopEixt

	BuzzerTest(LoopExit.LeftControlExit || LoopExit.RightControlExit);

	if (LoopExit.LeftControlExit)
	{
		LeftUnderLedBright;
	}
	else
	{
		LeftUnderLedDark;
	}


	if (LoopExit.RightControlExit)
	{
		RightUnderLedBright;
	}
	else
	{
		RightUnderLedDark;
	}

#endif 

}

/*
  路径识别，主要需要区分长直道左右拐入弯，短直道左右拐入弯
  得到这些标志位，主要用于后面的速度给定
  主要通过拐点，封顶，偏差进行区分
*/
void TrackType()
{
	static unsigned char  LongStraightawayCount = 0;//记录连续长直道的次数
	Tracktype.LongStraightaway = 0;
	Tracktype.ShortStraightaway = 0;
	//长直道入弯
	if (InflectionPointL.InflectionPointFlag == 0
		&& InflectionPointR.InflectionPointFlag == 0
		&& ABS(Error) <= 6
		&& LastLine <= 18)//两个拐点都没有,偏差很小,视野远
	{
		Tracktype.LongStraightaway = 1;

		if (Tracktype.LongStraightaway == 1 && Tracktype.LongStraightawayIntoBend == 0)//标记长直道
		{
			LongStraightawayCount++;
			if (LongStraightawayCount >= 40)//连续三十次
			{
				Tracktype.LongStraightawayIntoBend = 1;
				LongStraightawayCount = 0;
			}
		}
	}

	else
	{

		LongStraightawayCount = 0;//清零

		if ((InflectionPointL.InflectionPointFlag == 0
			&& InflectionPointR.InflectionPointFlag
			&&ABS(Error) < 15)
			|| (InflectionPointL.InflectionPointFlag
				&&InflectionPointR.InflectionPointFlag == 0
				&& ABS(Error) < 15))//存在一边拐点，偏差不大
		{
			Tracktype.ShortStraightaway = 1;
		}
	}



}












#if ObstacleOpen  //如果开启了障碍识别

//障碍识别

void RecognitionObstacle(void)
{
	int i, j;
	static unsigned char ObstacleLfetRowNum = 0;
	static unsigned char ObstacleLfetNum = 0;
	unsigned char  ObstacleLeftFlag = 0;
	unsigned char ObstacleLeftLFlag = 0;
	static unsigned char ScanColStartLeft = 0;//扫描的起始列
	static unsigned char ScanColEndLeft = 0;//扫描的终止列

	static unsigned char ObstacleRightRowNum = 0;
	static unsigned char ObstacleRightNum = 0;
	unsigned char   ObstacleRightFlag = 0;
	unsigned char  ObstacleRightRFlag = 0;
	static unsigned char ScanColStartRight = 0;//扫描的起始列
	static unsigned char ScanColEndRight = 0;//扫描的终止列

	ScanColStartLeft = 0;
	ScanColEndLeft = 0;
	ObstacleLeftFlag = 0;
	ObstacleLeftLFlag = 0;
	ObstacleLfetRowStart = 0;
	ObstacleLfetColStart = 0;
	ObstacleLfetRowNum = 0;

	ScanColStartRight = 0;
	ScanColEndRight = 0;
	ObstacleRightFlag = 0;
	ObstacleRightRFlag = 0;
	ObstacleRightRowStart = 0;
	ObstacleRightColStart = 0;
	ObstacleRightRowNum = 0;
	//ObstacleLfetOkFlag=0;

	if (IsStartLine)
		return;

	if (ABS(Error) <= 6 && LeftLose < 4 && RightLose <= 4 && AllLose < 2 && !LoopExit.LeftControlExit && (!LoopExit.RightControlExit) && (!Cross.RightSideling) && (!Cross.LeftSideling))
	{
		IsStartLine = 0;
		for (i = 58; i > 20; i--)
		{
			/// 判断起跑线 ///
			if (BlackAreaCountRow[i] || IsStartLine)
			{
				IsStartLine = 1;
				continue;
			}
			/*else if(IsStartLine)
			{
				// 在起跑线结束时重置障碍物标志
				IsStartLine = 0;

				ScanColStartLeft = 0;
				ScanColEndLeft = 0;
				ObstacleLeftFlag = 0;
				ObstacleLeftLFlag = 0;
				ObstacleLfetRowStart = 0;
				ObstacleLfetColStart = 0;
				ObstacleLfetRowNum = 0;
				ObstacleLfetNum = 0;
			}*/

			if (LeftEdge[i] != 0)//如果当行的左边线存在
			{
				if (ABS(LeftEdge[i + 1] - LeftEdge[i]) >= 3)//左边界跳变
				{
					if (LeftEdge[i + 1] != 0)
					{
						ScanColStartLeft = LeftEdge[i + 1];//用前一行作为扫描的起始列
					}
					else
					{
						ScanColStartLeft = LeftEdge[56];
					}
				}

				else
				{
					ScanColStartLeft = LeftEdge[i];
				}

			}

			else//如果当行左边线不存在
			{
				ScanColStartLeft = LeftEdge[i + 1];//用前一行作为扫描的起始列
			}


			ScanColEndLeft = MiddleLine[i];

			for (j = ScanColStartLeft; j < ScanColEndLeft; j++)//往左边寻找障碍物
			{
				if (img[i][j] == White_Point&&img[i][j + 1] == Black_Point)//找到了跳变
				{
					ObstacleLeftFlag = 1;

					break;

				}

			}

			unsigned char jj = j + 1;

			if (ObstacleLeftFlag)
			{

				while (jj < ScanColEndLeft)
				{
					if (img[i][jj] == Black_Point&&img[i][jj + 1] == White_Point)//找到又一个跳变
					{
						ObstacleLeftLFlag = 1;

						break;//退出while
					}

					jj++;

				}


			}

			if (ObstacleLeftLFlag&&ObstacleLeftFlag)
			{
				ObstacleLfetRowNum++;
				if (ObstacleLfetRowNum == 1)
				{
					ObstacleLfetRowStart = i;
					ObstacleLfetColStart = j + 1;
					//ObstacleLfetRowNum=0;
				}
			}


		}

		if (ObstacleLfetRowNum >= 4 && ObstacleLeftLFlag && ObstacleLeftFlag)//大于4行满足条件，认定这是左边有障碍
		{
			ObstacleLfetNum++;
			if (ObstacleLfetNum >= 3)
			{
				ObstacleLfetOkFlag = 1;
				ObstacleLfetNum = 0;
				ObstacleLfetRowNum = 0;
			}
		}
		else
		{
			// ObstacleLfetOkFlag=0;
			ObstacleLfetNum = 0;
		}

		//右边障碍
		if (!ObstacleLfetOkFlag)  //如果没有找到左边障碍物，再寻找右边有没有障碍物
		{
			for (i = 58; i > 20; i--)
			{
				/// 判断起跑线 ///
				if (BlackAreaCountRow[i] || IsStartLine)
				{
					IsStartLine = 1;
					continue;
				}
				/*else if (IsStartLine)
				{
					// 在起跑线结束时重置障碍物标志
					IsStartLine = 0;

					ScanColStartRight = 0;
					ScanColEndRight = 0;
					ObstacleRightFlag = 0;
					ObstacleRightRFlag = 0;
					ObstacleRightRowStart = 0;
					ObstacleRightColStart = 0;
					ObstacleRightRowNum = 0;
					ObstacleRightNum = 0;
				}*/

				if (RightEdge[i] != 80)//如果当行的右边线存在
				{
					if (ABS(RightEdge[i + 1] - RightEdge[i]) >= 3)//右边界跳变
					{
						if (RightEdge[i + 1] != 80)
						{
							ScanColStartRight = RightEdge[i + 1];//用前一行作为扫描的起始列
						}
						else
						{
							ScanColStartRight = RightEdge[56];
						}
					}
					else
					{
						ScanColStartRight = RightEdge[i];
					}
				}
				else//如果当行左边线不存在
				{
					ScanColStartRight = RightEdge[i + 1];//用前一行作为扫描的起始列
				}

				ScanColEndRight = MiddleLine[i];

				for (j = ScanColStartRight; j > ScanColEndRight; j--)//往左边寻找障碍物
				{
					if (img[i][j] == White_Point&&img[i][j - 1] == Black_Point)//找到了跳变
					{
						ObstacleRightFlag = 1;
						break;

					}

				}

				unsigned char jj = j - 1;

				if (ObstacleRightFlag)
				{
					while (jj > ScanColEndRight)
					{
						if (img[i][jj] == Black_Point&&img[i][jj - 1] == White_Point)//找到又一个跳变
						{
							ObstacleRightRFlag = 1;
							break;//退出while
						}
						jj--;
					}
				}

				if (ObstacleRightFlag&&ObstacleRightRFlag)
				{
					ObstacleRightRowNum++;
					if (ObstacleRightRowNum == 1)
					{
						ObstacleLfetRowStart = i;
						ObstacleLfetColStart = j + 1;
						//ObstacleLfetRowNum=0;
					}
				}
			}

			if (ObstacleRightRowNum >= 4 && ObstacleRightFlag && ObstacleRightRFlag)//大于4行满足条件，认定这是左边有障碍
			{
				ObstacleRightNum++;
				if (ObstacleRightNum >= 3)
				{
					ObstacleRightOkFlag = 1;
					ObstacleRightNum = 0;
					ObstacleRightRowNum = 0;
				}
			}
			else
			{
				// ObstacleLfetOkFlag=0;
				ObstacleRightNum = 0;
			}
		}
	}

	ObstacleRepair();
}



// 有障碍物时中线处理
void ObstacleRepair()
{
	int i;
	static unsigned char ObstacleLfetControlDelay = 0;

	if (ObstacleLfetOkFlag && !ObstacleRightOkFlag)
	{
		for (i = 59; i >= LastLine; i--)
		{
			MiddleLine[i] = (int)(RightEdge[i] - Width[i] * 0.2);
		}
		ObstacleLfetControlDelay++;
		if (ObstacleLfetControlDelay >= 20)
		{
			ObstacleLfetControlDelay = 0;
			ObstacleLfetOkFlag = 0;
		}
	}

	if (ObstacleRightOkFlag && !ObstacleLfetOkFlag)
	{
		for (i = 59; i >= LastLine; i--)
		{
			MiddleLine[i] = (int)(LeftEdge[i] + Width[i] * 0.2);
		}
		ObstacleLfetControlDelay++;
		if (ObstacleLfetControlDelay >= 20)
		{
			ObstacleLfetControlDelay = 0;
			ObstacleRightOkFlag = 0;
		}
	}
}

#endif
