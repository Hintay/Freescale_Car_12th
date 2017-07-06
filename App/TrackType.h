#ifndef __TRACKTYPE_H__
#define __TRACKTYPE_H__

#include "common.h"
#include "include.h"

#define ObstacleOpen  1//打开障碍

#define ClossRecognition1   0 //十字识别方式1
#define ClossRecognition2   1//十字识别方式2

#if ClossRecognition2

extern unsigned char StrightIntoCrossL;//直入十字，存在左拐点的标志位
extern unsigned char StrightIntoCrossR;//直入十字，存在右拐点的标志
extern unsigned char StrightIntoCrossLR;//直入十字，存在左右拐点的标志位
extern unsigned char StrightIntoCrossNotLR;//直入十字，不存在右拐点的标志位
extern unsigned char ObstacleLfetOkFlag;
extern unsigned char ObstacleRightOkFlag;
#endif

extern int IsStartLine;


typedef struct 
{
 unsigned char InflectionPointRow;//拐点的行坐标
 unsigned char InflectionPointCol;//拐点的列坐标
 unsigned char InflectionPointFlag;//拐点的标志位
 unsigned char ScanInflectionPointStart;//扫描拐点的起始行
 unsigned char ScanInflectionPointEnd;//扫描拐点结束行 
 unsigned char InflectionPointNumber;//拐点的个数
 
}InflectionPoint;//建立拐点结构体

typedef struct 
{
  unsigned char StrightInto;//直入十字
  unsigned char LeftSideling;//左斜入十字
  unsigned char RightSideling;//右斜入十字
  unsigned char CrossFlag;//直入十字底部丢线标志位
  unsigned char LastStrightInto;//上次直入十字状态
    
}CrossType;//建立十字类型的结构体

typedef struct
{
  unsigned char StrightIntoLoop;//直入圆环
  unsigned char LeftSidelingLoop;//左斜入圆环
  unsigned char RightSidelingLoop;//右斜入圆环

}LoopType;//建立圆环类型结构体

typedef struct
{
  unsigned char StarRow;//起始行
  unsigned char StarCol; //其实列 

}LoopScan;//建立圆环识别时，一些特征变量的结构体

typedef struct
{
  unsigned char RightNearExit ;//右边近出口
  unsigned char RightControlExit  ; //右边出口控制信号
  unsigned char LeftNearExit ;//左边近出口
  unsigned char LeftControlExit  ; //左边出口控制信号
  unsigned char IntoLoopOk;//入环成功标志位
  unsigned char  LoopInFlag;//环内标志
  unsigned char  LoopInDelay;//延时清标志  
}LoopExitType;//建立圆环出口类型结构体

typedef struct
{
  unsigned char LongStraightaway;//长直道
  unsigned char ShortStraightaway;//短直道
  unsigned char LongStraightawayIntoBend;//长直道入弯
  
}TrackTypeFlag;//建立赛道类型标志结构体



extern TrackTypeFlag  Tracktype;

extern CrossType Cross;//外部申明十字结构体变量
extern LoopType Loop;//外部申明圆环结构体变量
extern InflectionPoint  InflectionPointL;//外部申明左边拐点结构体变量
extern InflectionPoint  InflectionPointR;//外部申明右边拐点结构体变量

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