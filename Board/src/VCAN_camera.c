#include "common.h"
#include  "VCAN_camera.h"       //����ͷ��ͷ�ļ�

//ѹ����ֵ��ͼ���ѹ���ռ� �� ʱ�� ��ѹ��
//srclen �Ƕ�ֵ��ͼ���ռ�ÿռ��С
//��ӥ�۽�ѹ��ӥ��ͼ���ѹ��תΪ ��ά���� - ���ܳ������� - ɽ����̳ http://vcan123.com/forum.php?mod=viewthread&tid=17&ctid=6
//��ѹ��ʱ�������и����飬���úڡ��׶�Ӧ��ֵ�Ƕ��١�
#if 1
void img_extract(void *dst, void *src)
{
    uint8_t colour[2] = {255, 0}; //0 �� 1 �ֱ��Ӧ����ɫ
    uint8_t * mdst = dst;  //��ά����
    uint8_t * msrc = src;  //һά����
    uint32_t i,j;
    //ע��ɽ�������ͷ 0 ��ʾ ��ɫ��1��ʾ ��ɫ
/**      ��ѹǰ��0 ��ʾ ��ɫ��1��ʾ ��ɫ��
*        ��ѹ����������������������Ĵ����ǣ�
*        colour[0]=255 ������ɫ
*        colour[1]=0    ������ɫ
*/
    uint8_t tmpsrc;
   
    for(i=0;i<OV7725_EAGLE_H;i++)//��������ѹ
    {
        for(j=0;j<OV7725_EAGLE_W/8;j++)//ÿһ��
        {
            tmpsrc = *(msrc+i*OV7725_EAGLE_W/8+j);//ȡһ��һά�����е�Ԫ��
            *mdst++ = colour[ (tmpsrc >> 7 ) & 0x01 ];  //��ÿһ��Ϊ1��bit������colour[1]���棬��˳��װ����ά������
            *mdst++ = colour[ (tmpsrc >> 6 ) & 0x01 ]; ////��ÿһ��Ϊ0��bit������colour[0]����
            *mdst++ = colour[ (tmpsrc >> 5 ) & 0x01 ];
            *mdst++ = colour[ (tmpsrc >> 4 ) & 0x01 ];
            *mdst++ = colour[ (tmpsrc >> 3 ) & 0x01 ];
            *mdst++ = colour[ (tmpsrc >> 2 ) & 0x01 ];
            *mdst++ = colour[ (tmpsrc >> 1 ) & 0x01 ];
            *mdst++ = colour[ (tmpsrc >> 0 ) & 0x01 ];
        }
    }
}
#endif


