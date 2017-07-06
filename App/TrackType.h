#ifndef __TRACKTYPE_H__
#define __TRACKTYPE_H__

#include "common.h"
#include "include.h"

#define ObstacleOpen  1//���ϰ�

#define ClossRecognition1   0 //ʮ��ʶ��ʽ1
#define ClossRecognition2   1//ʮ��ʶ��ʽ2

#if ClossRecognition2

extern unsigned char StrightIntoCrossL;//ֱ��ʮ�֣�������յ�ı�־λ
extern unsigned char StrightIntoCrossR;//ֱ��ʮ�֣������ҹյ�ı�־
extern unsigned char StrightIntoCrossLR;//ֱ��ʮ�֣��������ҹյ�ı�־λ
extern unsigned char StrightIntoCrossNotLR;//ֱ��ʮ�֣��������ҹյ�ı�־λ
extern unsigned char ObstacleLfetOkFlag;
extern unsigned char ObstacleRightOkFlag;
#endif

extern int IsStartLine;


typedef struct 
{
 unsigned char InflectionPointRow;//�յ��������
 unsigned char InflectionPointCol;//�յ��������
 unsigned char InflectionPointFlag;//�յ�ı�־λ
 unsigned char ScanInflectionPointStart;//ɨ��յ����ʼ��
 unsigned char ScanInflectionPointEnd;//ɨ��յ������ 
 unsigned char InflectionPointNumber;//�յ�ĸ���
 
}InflectionPoint;//�����յ�ṹ��

typedef struct 
{
  unsigned char StrightInto;//ֱ��ʮ��
  unsigned char LeftSideling;//��б��ʮ��
  unsigned char RightSideling;//��б��ʮ��
  unsigned char CrossFlag;//ֱ��ʮ�ֵײ����߱�־λ
  unsigned char LastStrightInto;//�ϴ�ֱ��ʮ��״̬
    
}CrossType;//����ʮ�����͵Ľṹ��

typedef struct
{
  unsigned char StrightIntoLoop;//ֱ��Բ��
  unsigned char LeftSidelingLoop;//��б��Բ��
  unsigned char RightSidelingLoop;//��б��Բ��

}LoopType;//����Բ�����ͽṹ��

typedef struct
{
  unsigned char StarRow;//��ʼ��
  unsigned char StarCol; //��ʵ�� 

}LoopScan;//����Բ��ʶ��ʱ��һЩ���������Ľṹ��

typedef struct
{
  unsigned char RightNearExit ;//�ұ߽�����
  unsigned char RightControlExit  ; //�ұ߳��ڿ����ź�
  unsigned char LeftNearExit ;//��߽�����
  unsigned char LeftControlExit  ; //��߳��ڿ����ź�
  unsigned char IntoLoopOk;//�뻷�ɹ���־λ
  unsigned char  LoopInFlag;//���ڱ�־
  unsigned char  LoopInDelay;//��ʱ���־  
}LoopExitType;//����Բ���������ͽṹ��

typedef struct
{
  unsigned char LongStraightaway;//��ֱ��
  unsigned char ShortStraightaway;//��ֱ��
  unsigned char LongStraightawayIntoBend;//��ֱ������
  
}TrackTypeFlag;//�����������ͱ�־�ṹ��



extern TrackTypeFlag  Tracktype;

extern CrossType Cross;//�ⲿ����ʮ�ֽṹ�����
extern LoopType Loop;//�ⲿ����Բ���ṹ�����
extern InflectionPoint  InflectionPointL;//�ⲿ������߹յ�ṹ�����
extern InflectionPoint  InflectionPointR;//�ⲿ�����ұ߹յ�ṹ�����

void TrackType();
extern void ClearLoopControl();
extern void LoopIntoRepair();
extern void FindLoopOptimalExit();
extern void LoopRecognition(LoopType *prt);
extern void CrossConduct();
extern void CrossRecognition(CrossType *prt);
void FindInflectionPointR(InflectionPoint *prt ,unsigned char Start ,unsigned char End );
void FindInflectionPointL(InflectionPoint *prt ,unsigned char Start ,unsigned char End );

extern void RecognitionObstacle(void);
extern void ObstacleRepair();

#endif