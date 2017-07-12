/**
 * Copyright (c) 2017 ZJITC.
 * All rights reserved.
 *
 * @file       ZJGM_Crotrol.c
 * @brief      实时数据控制实现
 * @author     Hintay <hintay@me.com>
 * @version    v1.0
 * @date       2017-07-10
 */

#include "include.h"

#define ZJGM_CROTROL 5

union converter_size2 size2_converter;
union converter_size4 size4_converter;
union converter_size8 size8_converter;

char read_tmp; // 临时数据
char uart_read_buff[256]; //接收数据缓冲区
char read_datapacket[256]; //接收到的数据包
char reading_data = 0; //接收到数据包标志位
char reading_data_pos = 0; // 当前数据包位置
char head_checking = 0;
char *read_buff_p;

/**
 * 获取变量地址
 * 根据标志符返回相应的变量地址，
 * 并转换为 char * 型以便于后续处理。
 *
 * @since   1.0
 * @date    2017-07-10
 * @author  Hintay <hintay@me.com>
 *
 * @param char input  数据标志符。
 * @return char *     相应数据的地址。
 */
char *zjgm_getdataaddr(char input)
{
	switch (input)
	{
		case 0x01:
			return (char *)&J;
		case 0x02:
			return (char *)&JD;
		case 0x03:
			return (char *)&BasicP;
		case 0x04:
			return (char *)&BasicD;
		default:
			return 0;
	}
}

/**
 * 数据包解析并应用
 * 将接收的数据包分块进行解析并将其应用到相应的变量中。
 *
 * 数据块结构：
 *		[数据标志](1字节)[数据长度](1字节) [数据...]
 *
 * @since   1.0
 * @date    2017-07-09
 * @author  Hintay <hintay@me.com>
 */
void zjgm_getcrotroldata()
{
	char *data_end = read_datapacket + *read_datapacket; // 数据包长度限定，防止越界
	char *data_pos = read_datapacket +1;
	char block_size = 0;
	char *block_end;
	char *ver_addr;

	while (data_pos <= data_end)
	{
		ver_addr = zjgm_getdataaddr(*data_pos++);
		block_size = *data_pos;
		block_end = data_pos + block_size;
		while(data_pos++ < block_end)
			*ver_addr++ = *data_pos;
	}
}


/**
 * 串口中断服务函数
 * 从串口获取数据包数组。
 *
 * 数据包结构：[0x05](头)[0xFA](~0x05 检验码)  [数据长度](1字节)[数据...]
 *
 * @since   1.0
 * @date    2017-07-08
 * @author  Hintay <hintay@me.com>
 */
void uart_handler(void)
{
	if (UART_S1_REG(UARTN[VCAN_PORT]) & UART_S1_RDRF_MASK)
	{
		uart_querychar(VCAN_PORT, &read_tmp); //取出接收的数据
		if (!reading_data)
		{	// 数据包头部检测
			if (!head_checking)
			{
				if (read_tmp == ZJGM_CROTROL)
					head_checking = 1;
			}
			else if (read_tmp == (unsigned char)~ZJGM_CROTROL)
			{
				reading_data = 1;
				head_checking = 0;
			}
		}
		else
		{
			if (!(*uart_read_buff)) // 第一个字节无数据则第一次接收到数组
			{
				*uart_read_buff = read_tmp;
				read_buff_p = uart_read_buff + 1; // 指针后移一位
				reading_data_pos = 0;
			}
			else
			{
				if (reading_data_pos++ < *uart_read_buff-1)
					*read_buff_p++ = read_tmp;
				else
				{ // 最后一个字节时复制数组并重置参数
					*read_buff_p = read_tmp;
					for (unsigned char i = 0; i <= *uart_read_buff + 1; i++)
						read_datapacket[i] = uart_read_buff[i];
					*uart_read_buff = 0;//清空
					reading_data = 0;
				}
			}
		}
	}
}
