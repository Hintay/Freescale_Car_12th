/**
 * Copyright (c) 2017 ZJITC.
 * All rights reserved.
 *
 * @file       ZJGM_Crotrol.c
 * @brief      ʵʱ���ݿ���ʵ��
 * @author     Hintay <hintay@me.com>
 * @version    v1.0
 * @date       2017-07-10
 */

#include "include.h"

#define ZJGM_CROTROL 5

union converter_size2 size2_converter;
union converter_size4 size4_converter;
union converter_size8 size8_converter;

char read_tmp; // ��ʱ����
char uart_read_buff[256]; //�������ݻ�����
char read_datapacket[256]; //���յ������ݰ�
char reading_data = 0; //���յ����ݰ���־λ
char reading_data_pos = 0; // ��ǰ���ݰ�λ��
char head_checking = 0;
char *read_buff_p;

/**
 * ��ȡ������ַ
 * ���ݱ�־��������Ӧ�ı�����ַ��
 * ��ת��Ϊ char * ���Ա��ں�������
 *
 * @since   1.0
 * @date    2017-07-10
 * @author  Hintay <hintay@me.com>
 *
 * @param char input  ���ݱ�־����
 * @return char *     ��Ӧ���ݵĵ�ַ��
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
 * ���ݰ�������Ӧ��
 * �����յ����ݰ��ֿ���н���������Ӧ�õ���Ӧ�ı����С�
 *
 * ���ݿ�ṹ��
 *		[���ݱ�־](1�ֽ�)[���ݳ���](1�ֽ�) [����...]
 *
 * @since   1.0
 * @date    2017-07-09
 * @author  Hintay <hintay@me.com>
 */
void zjgm_getcrotroldata()
{
	char *data_end = read_datapacket + *read_datapacket; // ���ݰ������޶�����ֹԽ��
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
 * �����жϷ�����
 * �Ӵ��ڻ�ȡ���ݰ����顣
 *
 * ���ݰ��ṹ��[0x05](ͷ)[0xFA](~0x05 ������)  [���ݳ���](1�ֽ�)[����...]
 *
 * @since   1.0
 * @date    2017-07-08
 * @author  Hintay <hintay@me.com>
 */
void uart_handler(void)
{
	if (UART_S1_REG(UARTN[VCAN_PORT]) & UART_S1_RDRF_MASK)
	{
		uart_querychar(VCAN_PORT, &read_tmp); //ȡ�����յ�����
		if (!reading_data)
		{	// ���ݰ�ͷ�����
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
			if (!(*uart_read_buff)) // ��һ���ֽ����������һ�ν��յ�����
			{
				*uart_read_buff = read_tmp;
				read_buff_p = uart_read_buff + 1; // ָ�����һλ
				reading_data_pos = 0;
			}
			else
			{
				if (reading_data_pos++ < *uart_read_buff-1)
					*read_buff_p++ = read_tmp;
				else
				{ // ���һ���ֽ�ʱ�������鲢���ò���
					*read_buff_p = read_tmp;
					for (unsigned char i = 0; i <= *uart_read_buff + 1; i++)
						read_datapacket[i] = uart_read_buff[i];
					*uart_read_buff = 0;//���
					reading_data = 0;
				}
			}
		}
	}
}
