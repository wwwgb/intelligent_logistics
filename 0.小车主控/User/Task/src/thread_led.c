/* Includes ------------------------------------------------------------------*/
#include "thread_led.h"
#include "bsp.h"
#include "separate_led.h"
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifndef RT_USING_IDLE_HOOK
/* 定义线程控制块指针 */
rt_thread_t thread_led = RT_NULL;
#endif
LED_T led1_struct;
LED_T led2_struct;
LED_T led3_struct;
/* Private functions ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
#ifndef RT_USING_IDLE_HOOK
void led_task(void *pvParameters)
#else
void vApplicationIdleHook( void )
#endif
{
	while(1)
    {
		LED2_Toggle();
	#ifndef RT_USING_IDLE_HOOK
		My_mDelay( 300 );
	#else
		HAL_xmsDelay( 300 );
	#endif
    }
}
int Task_LED_create(void)
{
#ifndef RT_USING_IDLE_HOOK
    thread_led = rt_thread_create("led",              /* 线程名字 */
								   led_task,          /* 线程入口函数 */
								   RT_NULL,           /* 线程入口函数参数 */
								   128,               /* 线程栈大小 */
								   30,                /* 线程的优先级 */
								   20);               /* 线程时间片 */
	if(thread_led != RT_NULL)
	{
		rt_thread_startup(thread_led);
		rt_kprintf("thread_led startup!\n");
	}
#else
    if( rt_thread_idle_sethook(vApplicationIdleHook) == RT_EOK )
	{
		rt_kprintf("thread_led startup!\n");
	}
#endif
	return 0;
}
INIT_APP_EXPORT(Task_LED_create);
