#include <stdint.h>
#include "ws2812.h"
#include "main.h"

/*��Ҫ�û�����������������޸�*/
extern TIM_HandleTypeDef htim1;
extern DMA_HandleTypeDef hdma_tim1_ch1;
#define WS2812_TIM 					&htim1
#define WS2812_TIM_CHANNEL 	TIM_CHANNEL_1
#define WS2812_DELAY 				HAL_Delay				//��λms



uint32_t WS2812_SendBuf0[25] = {0};   //���ͻ�����0
uint32_t WS2812_SendBuf1[25] = {0};   //���ͻ�����1
const uint32_t WS2812_Rst[240] = {0}; //��λ�뻺����
uint32_t WS2812_En = 0;               //����ʹ��

uint32_t WS2812_Data[WS2812_Num] = {0};



/**
 * @brief ��uint32תΪ���͵�����
 * @param Data:��ɫ����	//0xff 00 00
 * @param Ret:����������(PWMռ�ձ�)
 * @return
 * @author HZ12138
 * @date 2022-10-03 18:03:17
 */
void WS2812_uint32ToData(uint32_t Data, uint32_t *Ret)
{
    uint32_t zj = Data;
    uint8_t *p = (uint8_t *)&zj;
    uint8_t R = 0, G = 0, B = 0;
    B = *(p);     // B	00
    G = *(p + 1); // G	00
    R = *(p + 2); // R  ff
    zj = (G << 16) | (R << 8) | B;
    for (int i = 0; i < 24; i++)	
    {
        if (zj & (1 << 23))
            Ret[i] = WS2812_Code_1;	//71/105
        else
            Ret[i] = WS2812_Code_0;	//32/105
        zj <<= 1;
    }
    Ret[24] = 0;
}
/**
 * @brief ��ʼ������ɫ����
 * @param ��
 * @return ��
 * @author HZ12138
 * @date 2022-10-03 18:05:13
 */
void WS2812_Start(void)
{
    HAL_TIM_PWM_Start_DMA(WS2812_TIM, WS2812_TIM_CHANNEL, (uint32_t *)WS2812_Rst, 240);	//��WS2812����һ��RESET�ź�
    WS2812_uint32ToData(WS2812_Data[0], WS2812_SendBuf0);		//RGB->TIM���������ض�PWM������ 0xff 00 00 -> 72 72 
    WS2812_En = 1;
}
/**
 * @brief ���͸�λ��
 * @param ��
 * @return ��
 * @author HZ12138
 * @date 2022-10-03 18:05:33
 */
void WS2812_Code_Reast(void)
{
    HAL_TIM_PWM_Start_DMA(WS2812_TIM, WS2812_TIM_CHANNEL, (uint32_t *)WS2812_Rst, 240);
    WS2812_En = 0;
}


/**
 * @brief ���ͺ���(DMA�жϵ���)
 * @param ��
 * @return ��
 * @author HZ12138
 * @date 2022-10-03 18:04:50
 */
void WS2812_Send(void)
{
    static uint32_t j = 0;
    static uint32_t ins = 0;
    if (WS2812_En == 1)
    {
        if (j == WS2812_Num)	
        {
            j = 0;
            HAL_TIM_PWM_Stop_DMA(WS2812_TIM, WS2812_TIM_CHANNEL);
            WS2812_En = 0;
            return;
        }
        j++;	
        if (ins == 0)
        {
            HAL_TIM_PWM_Start_DMA(WS2812_TIM, WS2812_TIM_CHANNEL, WS2812_SendBuf0, 25);
            WS2812_uint32ToData(WS2812_Data[j], WS2812_SendBuf1);	//71 71 32 32 
            ins = 1;
        }
        else
        {
            HAL_TIM_PWM_Start_DMA(WS2812_TIM, WS2812_TIM_CHANNEL, WS2812_SendBuf1, 25);
            WS2812_uint32ToData(WS2812_Data[j], WS2812_SendBuf0);
            ins = 0;
        }
    }
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
  WS2812_Send();
}

int i,flag;
void light_mode_breath(void)
{
    static uint32_t color = 0;	

	
        
       
    switch(color)
    {
        case 0x00ff00:flag = 1;break;
        case 0x000000:flag = 0;break;
        default:
            break;
    }
    while(flag)
    {
        color -= 0x000500;    
        if(color == 0x000000)
            flag = 0;
        for (int i = 0; i < 60; i++)
        {
            WS2812_Data[i] = color;
        }
        WS2812_Start();		
        WS2812_DELAY(20);	        
    }

    color += 0x000500;
    for (int i = 0; i < 60; i++)
    {
        WS2812_Data[i] = color;
    }
    WS2812_Start();		
    WS2812_DELAY(20);	        
        
        
}
	
void light_mode_marquee(void)
{
	static uint32_t first = 0, second = 0, third = 0, flag = 0;	
	if (3 == flag )
	{
		flag = 0;
	}
	
	if (0 == flag)
	{
		first 	= 0xff0000;
		second 	= 0x00ff00;
		third 	= 0x0000ff;
	}
	else if (1 == flag)
	{
		first 	= 0x00ff00;
		second 	= 0x0000ff;
		third 	= 0xff0000;	
	}
	else
	{
		first 	= 0x0000ff;
		second 	= 0xff0000;
		third 	= 0x00ff00;		
	}
	flag ++;	
		
  for (int i = 0; i < 60; i+=3)
	{
    WS2812_Data[i+0] = first;	//R
		WS2812_Data[i+1] = second;	//G
		WS2812_Data[i+2] = third;	//B		
	}	

		WS2812_Start();
    WS2812_DELAY(200);	

}
