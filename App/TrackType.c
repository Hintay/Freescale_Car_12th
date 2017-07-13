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

InflectionPoint  InflectionPointL;//������߹յ�ṹ�����
InflectionPoint  InflectionPointR;//�����ұ߹յ�ṹ�����

#define TestInflectionPointL  1//�˺궨����1���Ϳ��Զ��������յ���в���
#define TestInflectionPointR  1//�˺궨����1���Ϳ��Զ��ұ�����յ���в���

CrossType Cross;//����ʮ�ֽṹ�����

#define TestRightSideling     0 //�˺궨����1���Ϳ��Զ���б����в���
#define TestLeftSideling      0 //�˺궨����1���Ϳ��Զ���б����в���

LoopType Loop;//����Բ���ṹ�����
LoopExitType LoopExit;//����Բ���������ͽṹ�����
LoopScan Scan;

//����Բ�����ߵ�·��
//�����ݲ�ͬ������ͷ��װ��Ҫ���²�
//��ķ������ѳ��������������м䣬����Jlink���ߵ��ԣ���ÿ�е�����-���ߵõ�
unsigned char  LoopWidth[61] = { 2,3,3,3,4,4,5,5,6,6,
							   8,8,10,10,12,13,14,14,16,17,
							   18,18,20,20,22,22,24,24,26,26,
							   28,30,31,32,32,34,36,36,38,39,
							   41,41,43,43,45,45,47,47,49,50,
							   50,51,52,54,55,56,57,58,59,60,61 };

#define  TestStrightIntoLoop      0 //�˺궨����1���Ϳ��Զ�ֱ�뻷�����в���
#define  TestLoopEixt             0 //�˺궨����1���Ϳ��Ի������ڷ�����в���
unsigned char LoopEixtR[60];  //Ѱ�ҳ���ʱ�������
unsigned char LoopEixtL[60];  //Ѱ�ҳ���ʱ�������
TrackTypeFlag  Tracktype;//�������ͽṹ�����
//ʮ�ַ�ʽ���õ��ı���
#if ClossRecognition2
//����ɨ�裬��¼�����������
unsigned char LeftScanCol[80];
unsigned char RightScanCol[80];
unsigned char StrightIntoCrossL;//ֱ��ʮ�֣�������յ�ı�־λ
unsigned char StrightIntoCrossR;//ֱ��ʮ�֣������ҹյ�ı�־
unsigned char StrightIntoCrossLR;//ֱ��ʮ�֣��������ҹյ�ı�־λ
unsigned char StrightIntoCrossNotLR;//ֱ��ʮ�֣��������ҹյ�ı�־λ
#endif

//���ұ��߽��в�����������ڶ���յ�Ѱ������Ĺյ�
//prt:����һ���ṹ������ĵ�ַ
//Start��ɨ�����ʼ��
//End ��ɨ��Ľ�����

void FindInflectionPointR(InflectionPoint *prt, unsigned char Start, unsigned char End)
{
	int i;
	//��ʼ���ṹ�����
	prt->ScanInflectionPointStart = Start;//����ɨ�����ʼ��
	prt->ScanInflectionPointEnd = End;//����ɨ��Ľ�����
	prt->InflectionPointRow = 0;
	prt->InflectionPointCol = 0;
	prt->InflectionPointFlag = 0;

	for (i = prt->ScanInflectionPointStart; i >= prt->ScanInflectionPointEnd; i--)//����ɨ��̫Զ�����������
	{
		if ((RightEdge[i] != ColumnMax&&RightEdge[i - 1] != ColumnMax&&RightEdge[i + 1] != ColumnMax&&RightEdge[i - 2] != ColumnMax)
			&& (RightEdge[i] > 10 && RightEdge[i - 1] > 10 && RightEdge[i + 1] > 10 && RightEdge[i - 2] > 10)) //�������в�����,��δΪ�˷�ֹ���䣬��4�б������10
		{
			if ((RightEdge[i] - RightEdge[i + 1] <= 0) && (RightEdge[i - 1] - RightEdge[i]) >= 1 && (RightEdge[i - 2] - RightEdge[i]) >= 2)//�ҵ��ұ����йյ�
			{
				prt->InflectionPointRow = i;//��¼�յ����
				prt->InflectionPointCol = RightEdge[i];//��¼�յ����
				prt->InflectionPointFlag = 1;//����ҵ��ҹյ�
				break;//�˳�for
			}

		}

		//���Ⲩ�����࿼��һ�����:���ж��ߣ�ǰ���������кͺ����������ж������ߣ����Ҳ����ߵĶ�����10

		else if ((RightEdge[i] == ColumnMax&&RightEdge[i - 1] != ColumnMax&&RightEdge[i + 1] != ColumnMax&&RightEdge[i - 2] != ColumnMax&&RightEdge[i + 2] != ColumnMax)
			&& (RightEdge[i] > 10 && RightEdge[i - 1] > 10 && RightEdge[i + 1] > 10 && RightEdge[i - 2] > 10 && RightEdge[i + 2] > 10))
		{
			prt->InflectionPointRow = i;//��¼�յ����
			prt->InflectionPointCol = RightEdge[i];//��¼�յ����
			prt->InflectionPointFlag = 1;//����ҵ��ҹյ�
			break;//�˳�for

		}
	}

	//���Ƿ��ҵ��ҹյ���в���
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

//������߽��в�����������ڶ���յ�Ѱ������Ĺյ�
//prt:����һ���ṹ������ĵ�ַ
//Start��ɨ�����ʼ��
//End ��ɨ��Ľ�����

void FindInflectionPointL(InflectionPoint *prt, unsigned char Start, unsigned char End)
{

	int i;
	//��ʼ���ṹ�����
	prt->ScanInflectionPointStart = Start;//����ɨ�����ʼ��
	prt->ScanInflectionPointEnd = End;//����ɨ��Ľ�����
	prt->InflectionPointRow = 0;
	prt->InflectionPointCol = 0;
	prt->InflectionPointFlag = 0;

	//��յ�
	for (i = prt->ScanInflectionPointStart; i > prt->ScanInflectionPointEnd; i--)
	{
		if (LeftEdge[i] != 0 && LeftEdge[i - 1] != 0 && LeftEdge[i + 1] != 0 && LeftEdge[i - 2] != 0  //����4�в����ߣ�������4�ж�С��70
			&& LeftEdge[i] < 70 && LeftEdge[i - 1] < 70 && LeftEdge[i + 1] < 70 && LeftEdge[i - 2] < 70)
		{

			if ((LeftEdge[i] - LeftEdge[i + 1] >= 0) && (LeftEdge[i - 1] - LeftEdge[i] <= -1) && (LeftEdge[i - 2] - LeftEdge[i] <= -2))//�ҵ��յ�
			{
				prt->InflectionPointRow = i;//��¼�ùյ����
				prt->InflectionPointCol = LeftEdge[i];//��¼�ùյ����
				prt->InflectionPointFlag = 1;//����ҵ���յ�
				break;//�ҵ��˳�
			}
		}

		//���Ⲩ�����࿼��һ�����:���ж��ߣ�ǰ���������кͺ����������ж������ߣ����Ҳ����ߵĶ�С��70
		else if (LeftEdge[i] == 0 && LeftEdge[i - 1] != 0 && LeftEdge[i + 1] != 0 && LeftEdge[i - 2] != 0 && LeftEdge[i + 2] != 0
			&& LeftEdge[i] < 70 && LeftEdge[i - 1] < 70 && LeftEdge[i + 1] < 70 && LeftEdge[i - 2] < 70 && LeftEdge[i + 2] < 70)
		{

			prt->InflectionPointRow = i;//��¼�ùյ����
			prt->InflectionPointCol = LeftEdge[i];//��¼�ùյ����
			prt->InflectionPointFlag = 1;//����ҵ���յ�
			break;//�ҵ��˳�

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



#if ClossRecognition1//��ʽ1���Ƚϴֲ�

//��ʮ�ֽ���ʶ��
//prt:����һ���ṹ������ĵ�ַ

void CrossRecognition(CrossType *prt)
{
	//prt->LastStrightInto=prt->StrightInto;
	//��ʼ������
	prt->StrightInto = 0;
	prt->RightSideling = 0;
	prt->LeftSideling = 0;
	prt->CrossFlag = 0;

	//Ѱ�����ҹյ�
	FindInflectionPointR(&InflectionPointR, 57, 25);
	FindInflectionPointL(&InflectionPointL, 57, 25);

	if (!LoopExit.LeftControlExit && (!LoopExit.RightControlExit))//�������������Ƶ�ʱ�򣬲��ٽ���ʮ��ʶ��
	{

		//�����߽���ұ߽�ͬʱ���ߴ���3�У�����Ϊ������ʮ��

		if (AllLose >= 6 && LastLine < 25)
		{
			prt->StrightInto = 1;//ֱ��ʮ�� 
		}
		//�ж���б��ʮ�����

		else if (InflectionPointR.InflectionPointFlag&&InflectionPointL.InflectionPointFlag == 0
			&& InflectionPointR.InflectionPointRow <= 50)//���ֻ�����ұ߹յ㣬������б��ʮ�֣����ùյ����ȥ����
		{
			//������б�Ⱥ���Ұ�Ĺ�ϵ
			if (InflectionPointR.InflectionPointCol > 40)//��������
			{
				//�ڳ������������£������ʮ�֣���Ұ�������������м��Զ
				if (BlackEndL >= BlackEndM)
				{
					//�յ����һ���ߣ��������ж��ߣ��������ж���

					if (LeftEdge[InflectionPointR.InflectionPointRow + 1] == 0 && LeftEdge[InflectionPointR.InflectionPointRow + 2] == 0
						&& LeftEdge[InflectionPointR.InflectionPointRow - 2] == 0 && LeftEdge[InflectionPointR.InflectionPointRow - 3] == 0)
					{
						prt->RightSideling = 1;
					}

				}
			}

			else if (InflectionPointR.InflectionPointCol <= 40)//��������
			{
				if (BlackEndLL >= BlackEndL&&BlackEndLL > BlackEndML)
				{
					//�յ����һ���ߣ��������ж��ߣ��������ж���

					if (LeftEdge[InflectionPointR.InflectionPointRow + 1] == 0 && LeftEdge[InflectionPointR.InflectionPointRow + 2] == 0
						&& LeftEdge[InflectionPointR.InflectionPointRow - 2] == 0 && LeftEdge[InflectionPointR.InflectionPointRow - 3] == 0)
					{
						prt->RightSideling = 1;

					}

				}
			}
		}

		//�ж���б��ʮ��

		else if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag == 0
			&& InflectionPointL.InflectionPointRow <= 50)//ֻ������߹յ㣬��б��ʮ��
		{
			//������б�Ⱥ���Ұ�Ĺ�ϵ
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

		//������б��ʮ��                                                                     
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


		//������б��

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

//��ʮ�ֽ��д����ó�����Ŀ�������
//�����ԣ����ַ����������ã����ڿ���Ҫ����б�ʲ���

void CrossConduct()
{

	int i;
	//ֱ��ʮ�ֿ��Է�Ϊ������� ��1.�ײ����߻����ڣ�2.�ײ�����
	if (Cross.StrightInto)
	{

		if ((LeftEdge[58] != 0 && LeftEdge[57] != 0 && LeftEdge[56] != 0 && LeftEdge[55] != 0
			)

			|| (RightEdge[58] != 80 && RightEdge[57] != 80 && RightEdge[56] != 80 && RightEdge[55] != 80
				))//�ײ�4��һ�߲�����
		{
			//�����߽���ƽ��
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
			Cross.CrossFlag = 1;//�������ֱ��ȡ�����

		}
	}
	else if (Cross.LeftSideling)//��б��ʮ��
	{
		//�յ�����������߲�Ҫ
		for (i = InflectionPointL.InflectionPointRow; i > LastLine; i--)
		{
			MiddleLine[i] = 40;//ֻ��Ϊ����ʾ
		}
	}

	else if (Cross.RightSideling)//��б��ʮ��
	{
		//�յ�����������߲�Ҫ
		for (i = InflectionPointR.InflectionPointRow; i > LastLine; i--)
		{
			MiddleLine[i] = 40;//ֻ��Ϊ����ʾ
		}
	}
}

#endif



#if ClossRecognition2//��ʽ2

void CrossRecognition(CrossType *prt)
{
	int i, j, ScanEndCol;

	//��ʼ������
	prt->StrightInto = 0;
	prt->RightSideling = 0;
	prt->LeftSideling = 0;
	prt->CrossFlag = 0;

	//Ѱ�����ҹյ�
	FindInflectionPointR(&InflectionPointR, 57, 25);
	FindInflectionPointL(&InflectionPointL, 57, 25);

	if ((!LoopExit.LeftControlExit) && (!LoopExit.RightControlExit))//�������������Ƶ�ʱ�򣬲��ٽ���ʮ��ʶ��
	{
		//�����߽���ұ߽�ͬʱ���ߴ���5�У�����Ϊ������ʮ��
		if (AllLose >= 7 && LastLine < 25)
		{
			prt->StrightInto = 1;//ֱ��ʮ�� 
		}
		//�ж���б��ʮ�����

		else if (InflectionPointR.InflectionPointFlag //�����ұ߹յ�
			&&InflectionPointL.InflectionPointFlag == 0//���Ҳ�������߹յ�
			&& InflectionPointR.InflectionPointRow <= 52
			&& ((BlackEndLL >= BlackEndM) || BlackEndML >= BlackEndM))//�����ҹյ���복���Զ
		{

			for (i = 0; i < 80; i++)
			{
				RightScanCol[i] = 0;//��ʼ������
			}
			//ѡ����Զ����һ��
			if (BlackEndM >= BlackEndML&&BlackEndM >= BlackEndLL)//�м����Ұ��Զ
			{
				ScanEndCol = 40;
			}
			else if (BlackEndML > BlackEndM&&BlackEndML > BlackEndLL)//�м�ƫ����Ұ��Զ
			{
				ScanEndCol = 30;
			}

			else if (BlackEndLL > BlackEndM&&BlackEndLL > BlackEndML)//������Ұ��Զ
			{
				ScanEndCol = 20;
			}

			//���ҹյ������Ϊ��׼����Ұ��Զ��һ����Ϊ������������������ɨ��
		   //Ѱ��ʮ�������ֱ��

			for (j = InflectionPointR.InflectionPointCol; j > ScanEndCol; j--)
			{
				//�Թյ����Ϊ��ʼ��׼����Զ�е�����Ϊ������
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
		//�ж���б��ʮ�����
		else if (InflectionPointR.InflectionPointFlag == 0 //������߹յ�
			&& InflectionPointL.InflectionPointFlag//���Ҳ������ұ߹յ�
			&&InflectionPointL.InflectionPointRow <= 52
			&& ((BlackEndRR >= BlackEndM) || BlackEndMR >= BlackEndM))//������յ���복���Զ
		{
			for (i = 0; i < 80; i++)
			{
				LeftScanCol[i] = 0;//��ʼ������
			}

			//ѡ����Զ����һ��
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
				//�Թյ����Ϊ��ʼ��׼����Զ�е�����Ϊ������
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
						prt->LeftSideling = 1;//��־��б��ʮ��
					}
			}
		}



		//������б��ʮ��                                                                     
#if TestRightSideling 
//K60���İ����½ǵ�led�����������ʱ��⵽��б��ʮ��
		RightUnderLedTest(prt->RightSideling);
#endif   


		//������б��ʮ��

#if TestLeftSideling
//K60���İ����½ǵ�led�����������ʱ��⵽��б��ʮ��
		LeftUnderLedTest(prt->LeftSideling);
#endif

	}

}

//��ʮ�ֽ��д����ó�����Ŀ�������
//�����ԣ��˷������Դﵽʮ�ֿ���Ҫ�󣬻��������ڶ������

void CrossConduct()
{
	int i;
	//��ʼ������
	StrightIntoCrossL = 0;
	StrightIntoCrossR = 0;
	StrightIntoCrossLR = 0;
	StrightIntoCrossNotLR = 0;
	//ֱ��ʮ�ֿ��Է�Ϊ������� ��1.�ײ����߻����ڣ�2.�ײ�����
	if (Cross.StrightInto)
	{
		//�ײ�4����һ�߲�����
		if ((LeftEdge[58] != 0 && LeftEdge[57] != 0 && LeftEdge[56] != 0 && LeftEdge[55] != 0)
			|| (RightEdge[58] != 80 && RightEdge[57] != 80 && RightEdge[56] != 80 && RightEdge[55] != 80))
		{

			if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag == 0)
			{
				StrightIntoCrossL = 1;

			}
			//������յ㣬�����ҹյ�
			else if (InflectionPointL.InflectionPointFlag == 0 && InflectionPointR.InflectionPointFlag)
			{
				StrightIntoCrossR = 1;
			}
			//������յ㣬�����ҹյ�
			else if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag)
			{
				StrightIntoCrossLR = 1;
			}
			else
			{
				//ƽ������
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
			Cross.CrossFlag = 1;//�������ֱ��ȡ�����

		}
	}

}



#endif


//��Բ������ʶ��
//�������ҹյ���м��Բ������
//��Ϊ����ǰ��Ϊֱ����һ��ֻ����ֱ��

void LoopRecognition(LoopType *prt)
{

	int i;
	static unsigned char StrightIntoLoopNum = 0;//��¼ֱ��Բ���Ĵ������˵�żȻ���

	//��ʼ���ṹ�����
   // prt->StrightIntoLoop=0;//��סֱ��Բ���ı�־λ
	//������б���־����ʱ��û�õ�
	prt->LeftSidelingLoop = 0;
	prt->RightSidelingLoop = 0;

	//ֱ��Բ��
	//���ҹյ�ͬʱ�ҵ�������ͬʱ���߾Ͳ�������
	if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag&&AllLose < 4 && Loop.StrightIntoLoop == 0)
	{

		if (ABS(InflectionPointL.InflectionPointRow - InflectionPointR.InflectionPointRow) < 6//���ҹյ������С��ĳ��ֵ
		   //
			&& ABS(LeftEdge[InflectionPointL.InflectionPointRow - 1] - RightEdge[InflectionPointR.InflectionPointRow - 1]) >=
			ABS(LeftEdge[InflectionPointL.InflectionPointRow] - RightEdge[InflectionPointR.InflectionPointRow]))
		{
			StrightIntoLoopNum++;
			if (StrightIntoLoopNum >= 2)//�������������������ж�Ϊֱ��Բ��
			{
				prt->StrightIntoLoop = 1;
				StrightIntoLoopNum = 0;
			}
		}
		else
		{
			StrightIntoLoopNum = 0;//���
		}
	}

	//��б�뻷·
//ֻ��һ���յ㣬�ӹյ����ϲ���������һ���յ�
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
	//��б�뻷·
	//ֻ��һ���յ㣬�ӹյ����ϲ���������һ���յ�
	//��ߵײ�������
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

//Ѱ��Բ�������ų���
//�����̫�󣬿����޷�ʶ�����ų��ڣ�����ȷ����һ�߿�����

void FindLoopOptimalExit()
{
	int i, j, start;

	unsigned char LoopNumberL = 0;
	unsigned char LoopNumberR = 0;
	//��ʼ������
	//�л�����־λ�����һ�û�ҵ�����
	if (Loop.StrightIntoLoop && (!LoopExit.RightControlExit) && (!LoopExit.LeftControlExit))
	{
		//��ʼ������
		for (i = 0; i < 60; i++)
		{
			LoopEixtR[i] = 80;
		}
		//�ӷⶥ��ǰ���п�ʼѰ��,�����ҳ������������
		for (i = LastLine; i > 12; i--)//�����ұ�
		{
			//���ݲ�ͬ������ж�ɨ����ʼ��
			if (i == LastLine)
			{
				start = RightEdge[LastLine + 2] - 3;
			}

			else
			{
				if (LoopEixtR[i + 1] != 80)
				{
					start = LoopEixtR[i + 1] - 3;//��Ե׷�ٵ�˼��
				}

				else
				{
					start = RightEdge[LastLine + 2] - 3;
				}
			}

			for (j = start; j < 78; j++)
			{
				if (img[i][j] == White_Point&&img[i][j + 1] == Black_Point)//��->������
				{
					LoopEixtR[i] = j;
					break;//�ҵ������˳�
				}
			}

		}
		//�������
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
				if (img[i][j] == White_Point&&img[i][j - 1] == Black_Point)//����
				{
					LoopEixtL[i] = j;
					break;//�ҵ������˳�
				}
			}

		}

		for (i = LastLine; i > 12; i--)
		{
			if (LoopEixtL[i] != 0)
			{
				LoopNumberL++;//��¼����ҵ��˶�����

			}

			if (LoopEixtR[i] != 80)
			{
				LoopNumberR++;//��¼����ҵ��˶�����
			}

		}

		if (InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag)//����
		{
			Scan.StarCol = 0;
			Scan.StarCol = (InflectionPointL.InflectionPointCol + InflectionPointR.InflectionPointCol) / 2;
			if (Scan.StarCol >= 37 && Scan.StarCol <= 41)
			{

				if (((LastLine - 12) - LoopNumberR <= 3) && ((LastLine - 12) - LoopNumberL > 3))
				{

					LoopExit.LeftNearExit = 1;//����ҵ���߽���
				}

				else if (((LastLine - 12) - LoopNumberR > 3) && ((LastLine - 12) - LoopNumberL <= 3))
				{
					LoopExit.RightNearExit = 1;//����ҵ���߽���
				}
			}

			else if (Scan.StarCol > 41)//ƫ��
			{
				if ((LastLine - 12) - LoopNumberL > 2)
				{
					LoopExit.LeftNearExit = 1;//����ҵ���߽���
				}
				else
				{
					LoopExit.RightNearExit = 1;//����ҵ��ұ߽���
				}

			}

			else if (Scan.StarCol < 37)//ƫ��
			{
				if ((LastLine - 12) - LoopNumberR > 2)
				{
					LoopExit.RightNearExit = 1;//����ҵ��ұ߽���
				}
				else
				{
					LoopExit.LeftNearExit = 1;//����ҵ���߽���
				}

			}

		}

		else if (!InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag)//ƫ��
		{
			LoopExit.RightNearExit = 1;//����ҵ��ұ߽���
		}

		else if (!InflectionPointL.InflectionPointFlag&&InflectionPointR.InflectionPointFlag)//ƫ��
		{
			LoopExit.LeftNearExit = 1;//����ҵ���߽���
		}

		else  //���ǰ�涼û��һ����ȷ�ķ��򣬾�Ĭ�����
		{
			LoopExit.LeftNearExit = 1;//����ҵ���߽���
		}

		//��ȡ���յĳ��ڷ�������źţ�ֻ�ɹ���ȡ1�μ��ɣ�Ȼ��ֱ�����Ʊ�־λ�����
		if (LoopExit.LeftNearExit && (!LoopExit.RightControlExit) && (!LoopExit.LeftControlExit))//��߳���
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

//�����ȡ�껷�����ڷ������¶Ի���������ڽ��в���
//���ߵ�˼·�����������İ��

void LoopIntoRepair()
{
	int i;

	if (LoopExit.RightControlExit)//�������˳������ұߵĿ����ź�
	{
		if (LoopExit.LoopInDelay)
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = RightEdge[i] - LoopWidth[i] / 3;//�ұ���-�������
			}
		}
		else if (LoopExit.IntoLoopOk || LoopExit.LoopInFlag)
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = RightEdge[i] - LoopWidth[i] / 2;//�ұ���-�������
			}

		}
		else
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = RightEdge[i] - LoopWidth[i] / 2;//�ұ���-�������
			}
		}
	}


	else if (LoopExit.LeftControlExit)//�������˳�������ߵĿ����ź�
	{
		if (LoopExit.LoopInDelay)
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = LeftEdge[i] + LoopWidth[i] / 3;//�ұ���-�������
			}
		}
		else if (LoopExit.IntoLoopOk || LoopExit.LoopInFlag)
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = LeftEdge[i] + LoopWidth[i] / 2;//�����+�������

			}
		}
		else
		{
			for (i = 58; i > LastLine; i--)
			{
				MiddleLine[i] = LeftEdge[i] + LoopWidth[i] / 2;//�����+�������
			}
		}
	}
}

//���˻�����������Ʊ�־λ,��Ҫ���ж�ɶʱ���˳���
//Ϊ�˱���״̬���л��������У���ʹ��һЩ��־λ���ж���ʱ
void ClearLoopControl()
{
	//��ʱ�Լӱ���
	static int N = 0;
	static int M = 0;
	static int CloseThis = 0;
	if (LoopExit.LeftControlExit&&LoopExit.LoopInFlag == 0 && CloseThis == 0)//��߳��ڵ�ʱ��,���ұ߽������
	{
		//�����ж��뻷�ɹ�
	   //��߹յ��
		if (InflectionPointL.InflectionPointRow > 55
			|| (InflectionPointL.InflectionPointFlag == 0
				&& InflectionPointR.InflectionPointFlag == 0))//ͬ���ڷ���Ĺյ����ĳ��ֵ���������յ�ͬʱ��ʧ
		{
			LoopExit.IntoLoopOk = 1;//��־�뻷�ɹ�
			CloseThis = 1;
		}


	}
	else if (LoopExit.RightControlExit&&LoopExit.LoopInFlag == 0 && CloseThis == 0)
	{
		if (InflectionPointR.InflectionPointRow > 55
			|| (InflectionPointR.InflectionPointFlag == 0
				&& InflectionPointL.InflectionPointFlag == 0))
		{

			LoopExit.IntoLoopOk = 1;//��־�뻷�ɹ�
			CloseThis = 1;

		}
	}

	//����뻷�ɹ�

	if (LoopExit.IntoLoopOk)
	{
		//�˴�Ӧ���и�С��ʱ���ȶ�һ��
		M++;
		if (M > 2)
		{
			M = 0;
			if (LoopExit.RightControlExit)//���һ�ȡ���ĳ������ұ�
			{  //���ڻ��ڣ���������йյ�
				if (InflectionPointL.InflectionPointFlag)
				{
					LoopExit.LoopInFlag = 1; //��ǽ�������
					LoopExit.IntoLoopOk = 0;//��������ı�־
				}
			}
			else if (LoopExit.LeftControlExit)//���һ�ȡ���ĳ��������
			{
				//���ڻ��ڣ������ұ��йյ�
				if (InflectionPointR.InflectionPointFlag)
				{
					LoopExit.LoopInFlag = 1; //��ǽ�������
					LoopExit.IntoLoopOk = 0; //��������ı�־
				}
			}
		}
	}

	//��������£������Ѿ����뵽�����ˣ�����ʱ��Ȼ���ڻ�������λ����
	//������������취Ѱ�ҳ��ڵ���������������Ŀ���λ
	//��������������־����������Ҫ������ʱ���־

	if (LoopExit.LoopInFlag&&LoopExit.RightControlExit&& LoopExit.LoopInDelay == 0)//���ڣ��������ұ�
	{
		//�ұ߹յ��д���ĳ��ֵ�����һ�û�п����־��ʱ��־λ

		if (InflectionPointR.InflectionPointRow >= 35)//����������
		{
			LoopExit.LoopInFlag = 0;//������ڵı�־λ
			LoopExit.LoopInDelay = 1;//����ʱ��־λ����ʼ��ʱ�廷�����Ʊ�־
		}
	}

	else if (LoopExit.LoopInFlag&&LoopExit.LeftControlExit&& LoopExit.LoopInDelay == 0)//���ڣ����������
	{
		if (InflectionPointL.InflectionPointRow >= 35)
		{
			LoopExit.LoopInFlag = 0;//������ڵı�־λ
			LoopExit.LoopInDelay = 1;//����ʱ��־λ����ʼ��ʱ�廷�����Ʊ�
		}
	}

	if (LoopExit.LoopInDelay)//��ʼ��ʱ�廷�����Ʊ�
	{
		N++;
		if (N >= 12)
		{
			N = 0;
			LoopExit.LoopInDelay = 0;//�����ʱ��־λ
			LoopExit.LeftControlExit = 0;//�����������ڿ���λ
			LoopExit.RightControlExit = 0;//��������ҳ��ڿ���λ
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
  ·��ʶ����Ҫ��Ҫ���ֳ�ֱ�����ҹ����䣬��ֱ�����ҹ�����
  �õ���Щ��־λ����Ҫ���ں�����ٶȸ���
  ��Ҫͨ���յ㣬�ⶥ��ƫ���������
*/
void TrackType()
{
	static unsigned char  LongStraightawayCount = 0;//��¼������ֱ���Ĵ���
	Tracktype.LongStraightaway = 0;
	Tracktype.ShortStraightaway = 0;
	//��ֱ������
	if (InflectionPointL.InflectionPointFlag == 0
		&& InflectionPointR.InflectionPointFlag == 0
		&& ABS(Error) <= 6
		&& LastLine <= 18)//�����յ㶼û��,ƫ���С,��ҰԶ
	{
		Tracktype.LongStraightaway = 1;

		if (Tracktype.LongStraightaway == 1 && Tracktype.LongStraightawayIntoBend == 0)//��ǳ�ֱ��
		{
			LongStraightawayCount++;
			if (LongStraightawayCount >= 40)//������ʮ��
			{
				Tracktype.LongStraightawayIntoBend = 1;
				LongStraightawayCount = 0;
			}
		}
	}

	else
	{

		LongStraightawayCount = 0;//����

		if ((InflectionPointL.InflectionPointFlag == 0
			&& InflectionPointR.InflectionPointFlag
			&&ABS(Error) < 15)
			|| (InflectionPointL.InflectionPointFlag
				&&InflectionPointR.InflectionPointFlag == 0
				&& ABS(Error) < 15))//����һ�߹յ㣬ƫ���
		{
			Tracktype.ShortStraightaway = 1;
		}
	}



}












#if ObstacleOpen  //����������ϰ�ʶ��

//�ϰ�ʶ��

void RecognitionObstacle(void)
{
	int i, j;
	static unsigned char ObstacleLfetRowNum = 0;
	static unsigned char ObstacleLfetNum = 0;
	unsigned char  ObstacleLeftFlag = 0;
	unsigned char ObstacleLeftLFlag = 0;
	static unsigned char ScanColStartLeft = 0;//ɨ�����ʼ��
	static unsigned char ScanColEndLeft = 0;//ɨ�����ֹ��

	static unsigned char ObstacleRightRowNum = 0;
	static unsigned char ObstacleRightNum = 0;
	unsigned char   ObstacleRightFlag = 0;
	unsigned char  ObstacleRightRFlag = 0;
	static unsigned char ScanColStartRight = 0;//ɨ�����ʼ��
	static unsigned char ScanColEndRight = 0;//ɨ�����ֹ��

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
			/// �ж������� ///
			if (BlackAreaCountRow[i] || IsStartLine)
			{
				IsStartLine = 1;
				continue;
			}
			/*else if(IsStartLine)
			{
				// �������߽���ʱ�����ϰ����־
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

			if (LeftEdge[i] != 0)//������е�����ߴ���
			{
				if (ABS(LeftEdge[i + 1] - LeftEdge[i]) >= 3)//��߽�����
				{
					if (LeftEdge[i + 1] != 0)
					{
						ScanColStartLeft = LeftEdge[i + 1];//��ǰһ����Ϊɨ�����ʼ��
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

			else//�����������߲�����
			{
				ScanColStartLeft = LeftEdge[i + 1];//��ǰһ����Ϊɨ�����ʼ��
			}


			ScanColEndLeft = MiddleLine[i];

			for (j = ScanColStartLeft; j < ScanColEndLeft; j++)//�����Ѱ���ϰ���
			{
				if (img[i][j] == White_Point&&img[i][j + 1] == Black_Point)//�ҵ�������
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
					if (img[i][jj] == Black_Point&&img[i][jj + 1] == White_Point)//�ҵ���һ������
					{
						ObstacleLeftLFlag = 1;

						break;//�˳�while
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

		if (ObstacleLfetRowNum >= 4 && ObstacleLeftLFlag && ObstacleLeftFlag)//����4�������������϶�����������ϰ�
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

		//�ұ��ϰ�
		if (!ObstacleLfetOkFlag)  //���û���ҵ�����ϰ����Ѱ���ұ���û���ϰ���
		{
			for (i = 58; i > 20; i--)
			{
				/// �ж������� ///
				if (BlackAreaCountRow[i] || IsStartLine)
				{
					IsStartLine = 1;
					continue;
				}
				/*else if (IsStartLine)
				{
					// �������߽���ʱ�����ϰ����־
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

				if (RightEdge[i] != 80)//������е��ұ��ߴ���
				{
					if (ABS(RightEdge[i + 1] - RightEdge[i]) >= 3)//�ұ߽�����
					{
						if (RightEdge[i + 1] != 80)
						{
							ScanColStartRight = RightEdge[i + 1];//��ǰһ����Ϊɨ�����ʼ��
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
				else//�����������߲�����
				{
					ScanColStartRight = RightEdge[i + 1];//��ǰһ����Ϊɨ�����ʼ��
				}

				ScanColEndRight = MiddleLine[i];

				for (j = ScanColStartRight; j > ScanColEndRight; j--)//�����Ѱ���ϰ���
				{
					if (img[i][j] == White_Point&&img[i][j - 1] == Black_Point)//�ҵ�������
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
						if (img[i][jj] == Black_Point&&img[i][jj - 1] == White_Point)//�ҵ���һ������
						{
							ObstacleRightRFlag = 1;
							break;//�˳�while
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

			if (ObstacleRightRowNum >= 4 && ObstacleRightFlag && ObstacleRightRFlag)//����4�������������϶�����������ϰ�
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



// ���ϰ���ʱ���ߴ���
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
