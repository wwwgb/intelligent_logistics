/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "main.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private macros ------------------------------------------------------------*/
#define SYSCLK (168)
/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t fac_us=0;    //us��ʱ������
#if SYS_SUPPORT_OS
    /* ms��ʱ������,��os��,����ÿ�����ĵ�ms�� */
    static uint16_t fac_ms=0; 
#endif
/* Private functions ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void *rt_heap_begin_get(void)
{
    return (void *)HEAP_BEGIN;
}
void *rt_heap_end_get(void)
{
    return (void *)HEAP_END;
}
static int BSP_Init (void)
{
	/********** Ӳ�����ó�ʼ�� **********/
	MX_GPIO_Init();
    MX_DMA_Init();
    MX_SPI1_Init();
    MX_TIM12_Init();
    MX_USART1_UART_Init();
    MX_USART2_UART_Init();
    MX_USART3_UART_Init();
    MX_UART4_Init();
    MX_UART5_Init();
    MX_CAN1_Init();
    MX_CAN2_Init();
//	Bsp_Cancom_FilterInit();
	
	
	return 0;
}
INIT_BOARD_EXPORT(BSP_Init);
static int StartingMusic(void)
{
	buzzerSound(H5);
	My_mDelay(120);
	buzzerSound(H4);
	My_mDelay(100);
	buzzerSound(H6);
	My_mDelay(120);
	buzzerSound(0);
	
	return 0;
}
//INIT_COMPONENT_EXPORT(StartingMusic);
uint32_t HAL_GetTick(void)
{
    if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        HAL_IncTick();

    return uwTick;
}
void HAL_IncTick(void)
{
    uwTick += fac_ms;
}
/* SysTick configuration */
static void rt_hw_systick_init(void)
{
    HAL_SYSTICK_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    fac_us = SYSCLK;						//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
    NVIC_SetPriority(SysTick_IRQn, 0xFF);

    fac_ms = 1000u / RT_TICK_PER_SECOND;
    if(fac_ms == 0)
        fac_ms = 1;
}
/* re-implement tick interface for STM32 HAL */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    rt_hw_systick_init();

    /* Return function status */
    return HAL_OK;
}
#if 0
void HAL_usDelay(uint32_t nus)
{
    rt_uint32_t ticks;
    rt_uint32_t told, tnow, tcnt = 0;
    rt_uint32_t reload = SysTick->LOAD;

    ticks = us * reload / (1000000 / RT_TICK_PER_SECOND);
    told = SysTick->VAL;
    while (1)
    {
        tnow = SysTick->VAL;
        if (tnow != told)
        {
            if (tnow < told)
            {
                tcnt += told - tnow;
            }
            else
            {
                tcnt += reload - tnow + told;
            }
            told = tnow;
            if (tcnt >= ticks)
            {
                break;
            }
        }
    }
}
#else
/**
 * @brief 	��ʱnus
 * @param  	nus:Ҫ��ʱ��us��.	
 * @retval 	��
 */
void HAL_usDelay(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told, tnow, tcnt = 0;
	uint32_t reload = SysTick->LOAD;			/* LOAD��ֵ 			*/	    	 
	ticks = nus * fac_us; 						/* ��Ҫ�Ľ�����   		*/
	told = SysTick->VAL;        				/* �ս���ʱ�ļ�����ֵ 	*/
	while(1)
	{
		tnow = SysTick->VAL;	
		if(tnow != told)
		{	    
			if(tnow < told)
				tcnt += told - tnow;			/* ����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����. */
			else 
				tcnt += reload - tnow + told;	    
			told = tnow;
			if(tcnt >= ticks)					/* ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�. */
				break;			
		}  
	};									    
}
#endif
void HAL_Delay(__IO uint32_t Delay)
{
	if (Delay < HAL_MAX_DELAY)
    {
        Delay += (uint32_t)(uwTickFreq);
    }
    if (rt_thread_self())
    {
        rt_thread_mdelay(Delay);
    }
    else
    {
        for (rt_uint32_t count = 0; count < Delay; count++)
        {
            HAL_usDelay(1000);
        }
    }
}
/**
 * @brief 	��ʱnms,���������������
 * @param  	nms:Ҫ��ʱ��ms��
 * @retval 	��
 */
void HAL_xmsDelay(uint32_t nms)
{
	uint32_t i;
	for(i = 0; i < nms; i++) 
		HAL_usDelay(1000);
}
uint8_t Tim_CheckTimeOut(uint32_t TimeStart, uint32_t TimeNow, uint32_t TimeOut)
{
	#define ActivateCounter ((uint32_t)(TimeOut+TimeStart))
	
	if (ActivateCounter >= TimeStart)
	{
		if ( (TimeNow >= ActivateCounter) ||
		     (TimeNow < TimeStart))
		{
			return TRUE;
		}
	}
	else if ((TimeNow >= ActivateCounter) &&
	         (TimeNow < TimeStart))
	{
	    return TRUE;
	}
	return FALSE;
}













