/**
* Copyright (c) 2017 ZJITC.
* All rights reserved.
*
* @file       ZJGM_Crotrol.h
* @brief      实时数据控制实现
* @author     Hintay <hintay@me.com>
* @version    v1.0
* @date       2017-07-10
*/

#ifndef _ZJGM_CROTROL_H_
#define _ZJGM_CROTROL_H_

/*
* 包含头文件
*/
#include "common.h"
#include "MK60_uart.h"

#define  DEBUG_MODE 0

extern void zjgm_getcrotroldata();
extern void uart_handler(void);

extern char read_datapacket[];

/// 变量转换器 ///
union converter_size2
{
	unsigned char _char[2];
	short _short;
	unsigned short _ushort;
};

union converter_size4
{
	unsigned char _uchar[4];
	char _char[4];
	int _int;
	unsigned int _uint;
	float _float;
};

union converter_size8
{
	unsigned char _char[8];
	long long _long;
	unsigned long long _ulong;
	double _double;
};

extern union converter_size2 size2_converter;
extern union converter_size4 size4_converter;
extern union converter_size8 size8_converter;

#endif  //_ZJGM_CROTROL_H_