
#include "common.h"
#include "include.h"
#include "findtrack.h"


/*********define for SearchCenterBlackline**********/
int   EndLineFlag = 0;//��ֹ�߼���־λ,����ñ�־λ��1��˵����⵽��ֹ��
int   EndLineEnableFlag = 1;//��ֹ��ʹ�ܱ�־λ,����ñ�־λ��1����������ֹ��

int   EdgeRequirementCnt = 0;//��Ե�����������//������ڵ������������ĳֵ����Ϊ��⵽�ڱ�,�������������ڼ���������⵽�ڱߵĸ���
int IsBlackArea = 0; // �Ƿ��ں�ɫ����
int BlackAreaCount = 0; // ��ɫ�������
int   MiddleLine[RowMax + 1];
int   RightEdge[RowMax + 1];
int   LeftEdge[RowMax + 1];
int   Width[RowMax + 1];//={2,3,3,3,4,4,5,5,6,6,
					   //8,8,10,10,12,13,14,14,16,17,
					   //18,18,20,20,22,22,24,24,26,26,
					   //28,30,31,32,32,34,36,36,38,39,
					   //41,41,43,43,45,45,47,47,49,50,
					   //50,51,52,54,55,56,57,58,59,60,61};  // Width[i]  = 20+i*3/4;     //��̬·��
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
int   LeftLoseStart = 0;//��¼��߶��ߵĿ�ʼ��
int   RightLoseStart = 0;//��¼�ұ߱߶��ߵĿ�ʼ��
int   WhiteStart = 0;

//float OverflowOffset = 0.4;//�������ϵ��,�����ǰ�е�ĳһ�����Ѿ�����ɫ��û,�����ɫ��û�̶��������߽���ƫ��

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


//�������ߣ����ߣ����ߵĳ�ʼ��ֵ
//����ÿһ�ж�Ӧ���������
//���ļ��ڲ�����
void SetInitVal()
{
	int i;

	for (i = 0; i < RowMax; i++)//����ֵ             
	{
		RightEdge[i] = ColumnMax;
		LeftEdge[i] = 0;
		MiddleLine[i] = ColumnMax / 2;
		Width[i] = 20 + i * 3 / 4;     //��̬·��
		BlackAreaCountRow[i] = 0;

	}

}


//��ȡ��ָ�������ϵ����ɫ����ľ���

/*int16  GetWhiteHight(int16 H, int16 W)
{
	int16 tmp = H;
	while (tmp > 0 && img[tmp][W] == White_Point)//���tmp��û��������Զ��,��Ŀǰ�������ص㻹�ǰ�ɫ
	{
		tmp--;
	}
	if (tmp > 0)//���tmp>0��˵���ոյļ�鲻����Ϊ���ﻭ����Զ�˶�ֹͣ��
	{
		return H - tmp;//���ؼ�鿪ʼ��H����ǰ��⵽��ɫ�ĸ߶Ȳ�
	}
	return 0;

}*/


//ȫ��ɨ��ͱ�Ե�����ȡ����������
//ǰʮ��ȫ��ɨ��
//����50�У�������һ�е�Ѱ�������������ǰ�е�Ѱ�߷�ʽ�����
//�ⲿ����
void SearchCenterBlackline(void)
{

	int16 i = 0;
	int16 j = 0;
	uint8 jj = 0;
	int16 x = 0;
	uint8 WhiteNum = 0;

	LeftLose = 0;//��������
	RightLose = 0;
	AllLose = 0;
	WhiteNum = 0;

	MiddleLine[RowMax] = ColumnMax / 2;//��60��
	LeftEdge[RowMax] = 0;
	RightEdge[RowMax] = ColumnMax;
	Width[RowMax] = 60;

	SetInitVal();

	//ǰʮ�У�������������ұ�ɨ��ķ���
	//==================ǰʮ�д���ʼ====================
	for (i = RowMax - 1; i >= RowMax - 10; i--)//������ǰʮ�У�ȫ��ɨ��
	{
		BlackAreaCount = 0;
		if (i == RowMax - 1)//���о���ͼ��������Ϊɨ�����
		{
			j = MidPri;//40,�ñ���Ĭ��Ϊ40,ÿ����һ��ͼ��,�����¼��ͼ���59��(��ӽ�����)������λ�ø����ñ���
		}
		else
		{
			j = MiddleLine[i + 1];//���������һ���е��λ����Ϊ����ɨ�����
		}
		if (j <= 2)
		{
			j = 2;
		}

		/*=========================�����ǵ�������߽�================*/
		  /*while(j >= 2)//j>=2��Ч��Χ�ڽ�����Ѱ
		  {
			 if(img[i][j]==White_Point&& img[i][j-1]==Black_Point&&img[i][j-2]==Black_Point)//���������ҵ��װ׺�����
			 {
				 LeftEdge[i] =j;//�ҵ���ֵ �Ҳ�������ԭֵ0
				 break;//��������Ѱ��
			  }
				 j--;//���������ƶ�
		   }*/

		EdgeRequirementCnt = 0;//��������
		IsBlackArea = 0;
		for (x = j; x >= 2; x--)
		{
			if (img[i][x] == Black_Point)//�����ǰ���Ǻ�ɫ
			{
				if (!IsBlackArea)
				{
					LeftEdge[i] = x + 1;
					IsBlackArea = 1;
				}
				EdgeRequirementCnt++;//����++
				if (x == 2)//���y==2,˵��ͼ���Ѿ�����Ե��,�������ε��˲�
					break;
			}
			else if (IsBlackArea)
			{
				IsBlackArea = 0;
				BlackAreaCount++;
				EdgeRequirementCnt = 0;//������Ǻ�ɫ����ռ���
			}
			if (EdgeRequirementCnt > EDGE_REQUIREMENT)//����ﵽ����Ҫ��ķ�ֵ
				break;
		}
		/*=========================================================*/

		if (i == RowMax - 1) //�����ұ߽�
		{
			j = MidPri;//������У���ͼ�����Ŀ�ʼ��Ѱ
		}
		else
		{
			j = MiddleLine[i + 1];//�������һ������λ�ÿ�ʼ��Ѱ
		}
		if (j >= ColumnMax - 2)//j >=ColumnMax-2��Ч��Χ����Ѱ����
		{
			j = ColumnMax - 2;
		}

		/*=========================�����ǵ������ұ߽�================*/
		 /*while(j <= ColumnMax-2)
		 {

			if(img[i][j]==White_Point && img[i][j+1]==Black_Point && img[i][j+2]==Black_Point)//���������ҵ��װ׺������
			{
				   RightEdge[i] = j;//�ҵ���ֵ   �Ҳ�������ԭֵ
				   break;//��������Ѱ��
			}
				   j++;//���������ƶ�
		  }*/


		EdgeRequirementCnt = 0; //��������
		IsBlackArea = 0;
		for (x = j; x <= ColumnMax - 2; x++)
		{
			if (img[i][x] == Black_Point)//�����ǰ���Ǻ�ɫ
			{
				if (!IsBlackArea)
				{
					RightEdge[i] = x + 1;
					IsBlackArea = 1;
				}
				EdgeRequirementCnt++;//����++
				if (x == ColumnMax - 2)//���y==2,˵��ͼ���Ѿ�����Ե��,�������ε��˲�
					break;
			}
			else if (IsBlackArea)
			{
				IsBlackArea = 0;
				BlackAreaCount++;
				EdgeRequirementCnt = 0;//������Ǻ�ɫ����ռ���
			}
			if (EdgeRequirementCnt > EDGE_REQUIREMENT)//����ﵽ����Ҫ��ķ�ֵ
				break;
		}

		/*=========================================================*/
		if (LeftEdge[i] != 0 && RightEdge[i] != ColumnMax)//�����жϣ�û�ж���
		{
			MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;//���������ܵ����ұ߽�/2  
		}



		else if (LeftEdge[i] != 0 && RightEdge[i] == ColumnMax)//��������,û�ж�����
		{
			RightLose++;//��¼ֻ�����߶�������

			//������е�

			if ((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 2))//ͻ��,������е��ܵ���>=��һ�е��ܵ���+2
			{
				MiddleLine[i] = MiddleLine[i + 1];//��Ϊͻ�����Ա������ݷ���,��������λ��ʹ����һ�е�����λ��
			}
			else
			{

				//MiddleLine[i] = LeftEdge[i] + Width[i]/2+1+(int)(OverflowOffset*GetWhiteHight(i,0));//����ͨ���ܵ����Ļ����ϼ��ϰ�ɫ���ƫ��
				MiddleLine[i] = LeftEdge[i] + Width[i] / 2 + 3;//�ڶ����ߵ�����û��ͻ��������,�����߽߱�λ��+�ܵ����Ȼ�������ұ�ƫ2��
			}
		}
		else if (LeftEdge[i] == 0 && RightEdge[i] != ColumnMax)//��������
		{

			if ((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 3))//ͻ��,������е��ܵ���>=��һ�е��ܵ���+2      
			{
				MiddleLine[i] = MiddleLine[i + 1]; //��Ϊͻ�����Ա������ݷ���,��������λ��ʹ����һ�е�����λ��
			}
			else
			{
				// MiddleLine[i] = RightEdge[i] - Width[i]/2-1-(int)(OverflowOffset*GetWhiteHight(i,ColumnMax));
				MiddleLine[i] = RightEdge[i] - Width[i] / 2 - 3;//�ڶ����ߵ�����û��ͻ��������,�����߽߱�λ��+�ܵ����Ȼ���������ƫ2��
			}
		}
		else if (LeftEdge[i] == 0 && RightEdge[i] == ColumnMax)//���߶����˵Ļ�  
		{
			AllLose++;

			if (i == RowMax - 1)//��������о���ͼ��������Ϊ�е�
			{
				MiddleLine[i] = MidPri;
			}
			else
			{
				MiddleLine[i] = MiddleLine[i + 1];//����������о�����һ�е�������Ϊ�����е�
			}
		}



		if (MiddleLine[RowMax - 1] >= 70)//����λ���޷�
		{
			MidPri = 70;
		}
		else if (MiddleLine[RowMax - 1] <= 10)//����λ���޷�
		{
			MidPri = 10;
		}
		else
		{
			MidPri = MiddleLine[RowMax - 1];//��¼��֡ͼ���59�е�����ֵ����Ϊ��һ��ͼ���59��ɨ����ʼ��
		}

		BlackAreaCountRow[i] = BlackAreaCount;
	}
	//==================ǰʮ�д������====================
	for (i = 49; i > 0; i--)//����ʣ����
	{
		BlackAreaCount = 0;
		if (LeftEdge[i + 1] != 0 && RightEdge[i + 1] != ColumnMax) //��һ�����߶��ҵ� ���ñ���ɨ��     
		{
			j = ((LeftEdge[i + 1] + 15) >= ColumnMax - 2) ? ColumnMax - 2 : (LeftEdge[i + 1] + 15);//������߽�    
			jj = ((LeftEdge[i + 1] - 10) <= 1) ? 1 : (LeftEdge[i + 1] - 10);
			IsBlackArea = 0;
			EdgeRequirementCnt = 0;//��������
			while (j >= jj)//���������(û�й��ڽӽ�ĳһͼ���Ե),�ʹ���һ����߽���ұ�10����λ���ҵ����5����λ��֮��Ѱ��    
			{
				/*if (img[i][j] == White_Point && img[i][j - 1] == Black_Point&& img[i][j - 2] == Black_Point)//�ж���û�кڰ�����
				{
					LeftEdge[i] = j;//�ҵ��˾ͼ�¼��߽��λ��
					break;
				}*/

				if (img[i][j] == Black_Point)//�����ǰ��Ϊ��ɫ
				{
					if (j == jj)//����Ѿ��������յ�
					{
						break; //���������˲�
					}
					EdgeRequirementCnt++;//����++
					if (!IsBlackArea)
					{
						IsBlackArea = 1;
						LeftEdge[i] = j + 1; // ȷ���ڰ�ɫ������
					}
				}
				else if (IsBlackArea)
				{
					BlackAreaCount++;
					IsBlackArea = 0;
					EdgeRequirementCnt = 0;//������Ǻ�ɫ����ռ���
				}

				if (EdgeRequirementCnt >= EDGE_REQUIREMENT)//����ﵽ����Ҫ��ķ�ֵ
					break;

				j--;
			}
			j = ((RightEdge[i + 1] - 15) <= 1) ? 1 : (RightEdge[i + 1] - 15); //�����ұ߽�   
			jj = ((RightEdge[i + 1] + 10) >= ColumnMax - 2) ? ColumnMax - 2 : (RightEdge[i + 1] + 10);
			EdgeRequirementCnt = 0;//��������
			while (j <= jj)
			{
				/*if (img[i][j] == White_Point&& img[i][j + 1] == Black_Point&& img[i][j + 2] == Black_Point)
				{
					RightEdge[i] = j;
					break;
				}*/

				if (img[i][j] == Black_Point)//�����ǰ��Ϊ��ɫ
				{
					if (j == jj)//����Ѿ��������յ�
					{
						break;//���������˲�
					}
					EdgeRequirementCnt++;//����++
					RightEdge[i] = j - EdgeRequirementCnt - 1;
					if (!IsBlackArea)
					{
						IsBlackArea = 1;
						RightEdge[i] = j - 1; //ȷ���ڰ�ɫ������
					}

				}
				else if (IsBlackArea)
				{
					BlackAreaCount++;
					IsBlackArea = 0;
					EdgeRequirementCnt = 0;//������Ǻ�ɫ����ռ���
				}
				if (EdgeRequirementCnt > EDGE_REQUIREMENT)//����ﵽ����Ҫ��ķ�ֵ
					break;
				j++;
			}
		}
		else if (LeftEdge[i + 1] != 0 && RightEdge[i + 1] == ColumnMax)//��һ��ֻ�ҵ���߽�   
		{
			j = ((LeftEdge[i + 1] + 10) >= ColumnMax - 2) ? ColumnMax - 2 : (LeftEdge[i + 1] + 10);//��߽��ñ���ɨ��   
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
			j = MiddleLine[i + 1];//��һ�ж����ұ߽���ȫ��ɨ�� 
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
		else if (LeftEdge[i + 1] == 0 && RightEdge[i + 1] != ColumnMax) //��һ��ֻ�ҵ��ұ߽�      
		{
			j = ((RightEdge[i + 1] - 10) <= 1) ? 1 : (RightEdge[i + 1] - 10);//��Ե׷�����ұ߽� 
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
			j = MiddleLine[i + 1]; //ȫ��ɨ������߽�
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
		else if (LeftEdge[i + 1] == 0 && RightEdge[i + 1] == ColumnMax)  //��һ��û�ҵ��߽磬������ʮ�ֻ��߻���  
		{

			j = MiddleLine[i + 1];   //��ȫ������߽�
			while (j >= 1)
			{
				if (img[i][j] == White_Point && img[i][j - 1] == Black_Point&& img[i][j - 2] == Black_Point)
				{
					LeftEdge[i] = j;
					break;
				}
				j--;
			}
			j = MiddleLine[i + 1];   //ȫ�����ұ߽�   
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
		if ((RightEdge[i] - LeftEdge[i]) >= (RightEdge[i + 1] - LeftEdge[i + 1] + 3))//��������� 
		{
			MiddleLine[i] = MiddleLine[i + 1];//����һ��
		}
		else
		{
			if (LeftEdge[i] != 0 && RightEdge[i] != ColumnMax)
			{
				MiddleLine[i] = (LeftEdge[i] + RightEdge[i]) / 2;
				//��б��ʮ�ֽ��о���

				if (MiddleLine[i] - MiddleLine[i + 1] > 8 && ((ABS(LeftEdge[i] - LeftEdge[i + 1] > 3) || ABS(RightEdge[i] - RightEdge[i + 1] > 3))) && i >= 30)//��������ͻ��
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
			else if (LeftEdge[i] == 0 && RightEdge[i] == ColumnMax)//���߶���    
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
		if ((LeftEdge[i] != 0 && LeftEdge[i] >= 65) || (RightEdge[i] != ColumnMax && RightEdge[i] < 15) || (i >= 1) && (img[i - 1][m] == Black_Point) && BlackAreaCount < BLACK_AREA_STARTLINE) //���һ��              
		{
			LastLine = i;//���һ�У���̬ǰհ
			AvaliableLines = 60 - i;//��Ч����
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
		else if (EndLineFlagCount && !StartLinenNextClear) // ����
			StartLinenNextClear = 1;
		else if (StartLinenNextClear)
		{
			EndLineFlagCount = 0;
			StartLinenNextClear = 0;
		}
	}
}
#

// ��������������
void ConvertStartLine()
{
	// ��ʼ��
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
			if (!BlackAreaRowFlag) // ��������������������������
			{
				if (BlackAreaRowCount >= 3)
				{ // ����ʱ���� 3 ����������������������Ϊ������������
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

//��ȡͼ�������
//
//�ڲ�����
void GetBlackEndParam()//��ȡ���߽�ֹ��
{

	unsigned char LEndFlag = 0;//��־λ
	unsigned char MEndFlag = 0;
	unsigned char REndFlag = 0;
	unsigned char MREndFlag = 0;
	unsigned char MLEndFlag = 0;
	unsigned char LLEndFlag = 0;
	unsigned char RREndFlag = 0;

	int i = 0;

	BlackEndMR = 0;//����
	BlackEndML = 0;
	BlackEndLL = 0;
	BlackEndRR = 0;
	BlackEndL = 0;
	BlackEndM = 0;
	BlackEndR = 0;

	for (i = RowMax - 1; i >= 3; i--)
	{
		if (img[i][ColumnMax / 2] == White_Point && !MEndFlag)//!MEndFlag=1 //40//��������м���Ǹ������ǰ�ɫ,����MEndFlag��־λû����λ(����ע�Ͱ�������С�±�Ϊ1������)
		{
			BlackEndM++;//�к��߽�����
		}
		else if (i > 1 && img[i - 1][ColumnMax / 2] == Black_Point && img[i - 2][ColumnMax / 2] == Black_Point)//���������Ǻ�ɫ//�����һ�и�����һ�е��м䶼�Ǻ�ɫ        
		{
			MEndFlag = 1;//MEndFlag��λ
		}
		if (img[i][ColumnMax / 4] == White_Point && !LEndFlag)//20//������д�������20������Ϊ��ɫ,��LEndFlagû����λ
		{
			BlackEndL++;//����߽�����
		}
		else if (i > 1 && img[i - 1][ColumnMax / 4] == Black_Point && img[i - 2][ColumnMax / 4] == Black_Point)//�����һ�к�����һ�еĴ�������20�����ض��Ǻ�ɫ
		{
			LEndFlag = 1;//LEndFlag��λ
		}
		if (img[i][ColumnMax * 3 / 4] == White_Point && !REndFlag)//60//������д�������20������Ϊ��ɫ,��REndFlagû����λ
		{
			BlackEndR++;//�Һ��߽�����
		}
		else if (i > 1 && img[i - 1][ColumnMax * 3 / 4] == Black_Point && img[i - 2][ColumnMax * 3 / 4] == Black_Point)//�����һ�к�����һ�еĴ�������20�����ض��Ǻ�ɫ
		{
			REndFlag = 1;//REndFlag��λ
		}
		if (img[i][30] == White_Point && !MLEndFlag)//������д�����������30�������ǰ�ɫ����MLEndFlagû����λ
		{
			BlackEndML++;
		}
		else if (i > 1 && img[i - 1][30] == Black_Point && img[i - 2][30] == Black_Point)//�����һ�к�����һ�д�����������30�����ض��ǰ�ɫ
		{
			MLEndFlag = 1;//MLEndFlag��λ
		}
		if (img[i][50] == White_Point && !MREndFlag)//������д�����������30�������ǰ�ɫ����MREndFlagû����λ
		{
			BlackEndMR++;
		}
		else if (i > 1 && img[i - 1][50] == Black_Point && img[i - 2][50] == Black_Point)//�����һ�к�����һ�д�����������30�����ض��ǰ�ɫ
		{
			MREndFlag = 1;//MREndFlag��λ
		}
		if (img[i][10] == White_Point && !LLEndFlag)//������д�����������10�������ǰ�ɫ����LLEndFlagû����λ
		{
			BlackEndLL++;
		}
		else if (i > 1 && img[i - 1][10] == Black_Point && img[i - 2][10] == Black_Point)//�����һ�к�����һ�д�����������10�����ض��ǰ�ɫ
		{
			LLEndFlag = 1;//LLEndFlag��λ
		}
		if (img[i][70] == White_Point && !RREndFlag)//������д�����������10�������ǰ�ɫ����RREndFlagû����λ
		{
			BlackEndRR++;
		}
		else if (i > 1 && img[i - 1][70] == Black_Point && img[i - 2][70] == Black_Point)//�����һ�к�����һ�д�����������10�����ض��ǰ�ɫ
		{
			RREndFlag = 1;//RREndFlag��λ
		}
	}


	BlackEndMax = MAX(BlackEndL, BlackEndM);//ȡ��ֵ
	BlackEndMax = MAX(BlackEndMax, BlackEndR);
	BlackEndMaxMax = MAX(BlackEndMax, BlackEndMR);
	BlackEndMaxMax = MAX(BlackEndMax, BlackEndML);
	BlackEndMaxMax = MAX(BlackEndMax, BlackEndLL);
	BlackEndMaxMax = MAX(BlackEndMax, BlackEndRR);
	if (BlackEndMaxMax >= 60)
	{
		BlackEndMaxMax = 58;
	}
	DropRow = 60 - BlackEndMaxMax;//�ⶥ������      
}








