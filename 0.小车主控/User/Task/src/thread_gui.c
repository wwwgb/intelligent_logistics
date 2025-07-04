/* Includes ------------------------------------------------------------------*/
#include "thread_comm.h"
#include "thread_gui.h"
#include "bsp.h"
#include "data.h"
#include "separate_button.h"

#include "bsp_hal_time_CMPx_BEEP.h"
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
KEY_T key1_struct;
KEY_T key2_struct;
KEY_T key3_struct;
KEY_T key4_struct;
KEY_T key5_struct;
KEY_T key6_struct;
KEY_T key7_struct;
uint8_t key_flag = 0;
/* 定义线程控制块指针 */
rt_thread_t thread_gui = RT_NULL;
/* Private functions ---------------------------------------------------------*/
static void Display_123(uint16_t (*qdata)[3])
{
	uint8_t i,j;
	
	for(j=0;j<2;j++)
	{
		for(i=0;i<3;i++)
		{
			if(qdata[j][i] == RED)
				lcd_show_num(i*48, j*68, 64, 1, 1, 1);
			else if(qdata[j][i] == GREEN)
				lcd_show_num(i*48, j*68, 64, 2, 1, 1);
			else if(qdata[j][i] == BLUE)
				lcd_show_num(i*48, j*68, 64, 3, 1, 1);
		}
	}
	lcd_show_string(58, 54, 24, "+");
}
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/***********************************************************************************************

                                       应用事件（中断）函数

************************************************************************************************/
void XferExternalKey1_Handler( void* btn )
{
	switch(((KEY_T*)btn)->event_flg)
    {
        case SIGNAL_PRESS_DOWN:{
//			RecognitionModule_Start(&RecognitionModule_t);
			key_flag |= 0x01;
		}break;
		case SIGNAL_PRESS_UP:{
			key_flag &= 0xfe;
		}break;
	}
}
void XferExternalKey2_Handler( void* btn )
{
	switch(((KEY_T*)btn)->event_flg)
    {
        case SIGNAL_PRESS_DOWN:{
			key_flag |= 0x02;
		}break;
		case SIGNAL_PRESS_UP:{
			key_flag &= 0xfd;
		}break;
	}
}
void XferExternalKey3_Handler( void* btn )
{
	switch(((KEY_T*)btn)->event_flg)
    {
        case SIGNAL_PRESS_DOWN:{
			key_flag |= 0x04;
		}break;
		case SIGNAL_PRESS_UP:{
			key_flag &= 0xfb;
		}break;
	}
}
void XferExternalKey4_Handler( void* btn )
{
	switch(((KEY_T*)btn)->event_flg)
    {
        case SIGNAL_PRESS_DOWN:{
			key_flag |= 0x08;
		}break;
		case SIGNAL_PRESS_UP:{
			key_flag &= 0xf7;
		}break;
	}
}
void XferExternalKey5_Handler( void* btn )
{
	switch(((KEY_T*)btn)->event_flg)
    {
        case SIGNAL_PRESS_DOWN:{
			key_flag |= 0x10;
		}break;
		case SIGNAL_PRESS_UP:{
			key_flag &= 0xef;
		}break;
	}
}
void XferExternalKey6_Handler( void* btn )
{
	switch(((KEY_T*)btn)->event_flg)
    {
        case SIGNAL_PRESS_DOWN:{
			key_flag |= 0x20;
		}break;
		case SIGNAL_PRESS_UP:{
			key_flag &= 0xdf;
		}break;
	}
}
void XferExternalKey7_Handler( void* btn )
{
	switch(((KEY_T*)btn)->event_flg)
    {
        case SIGNAL_PRESS_DOWN:{
			key_flag |= 0x40;
		}break;
		case SIGNAL_PRESS_UP:{
			key_flag &= 0xbf;
		}break;
	}
}
/***********************************************************************************************

                                       应用任务函数

************************************************************************************************/
uint8_t gui_time = 0;
void gui_task(void *pvParameters)
{
	mybtn_init( &key1_struct, KEY_1, 0, 300, 1200 );
	mybtn_init( &key2_struct, KEY_2, 0, 300, 1200 );
	mybtn_init( &key3_struct, KEY_3, 0, 300, 1200 );
	mybtn_init( &key4_struct, KEY_4, 0, 300, 1200 );
	mybtn_init( &key5_struct, KEY_5, 0, 300, 1200 );
	mybtn_init( &key6_struct, KEY_6, 0, 300, 1200 );
	mybtn_init( &key7_struct, KEY_7, 0, 300, 1200 );
	
	mybtn_attach( &key1_struct, PRESS_DOWN, XferExternalKey1_Handler);
	mybtn_attach( &key1_struct, PRESS_UP, XferExternalKey1_Handler);
	mybtn_attach( &key2_struct, PRESS_DOWN, XferExternalKey2_Handler);
	mybtn_attach( &key2_struct, PRESS_UP, XferExternalKey2_Handler);
	mybtn_attach( &key3_struct, PRESS_DOWN, XferExternalKey3_Handler);
	mybtn_attach( &key3_struct, PRESS_UP, XferExternalKey3_Handler);
	mybtn_attach( &key4_struct, PRESS_DOWN, XferExternalKey4_Handler);
	mybtn_attach( &key4_struct, PRESS_UP, XferExternalKey4_Handler);
	mybtn_attach( &key5_struct, PRESS_DOWN, XferExternalKey5_Handler);
	mybtn_attach( &key5_struct, PRESS_UP, XferExternalKey5_Handler);
	mybtn_attach( &key6_struct, PRESS_DOWN, XferExternalKey6_Handler);
	mybtn_attach( &key6_struct, PRESS_UP, XferExternalKey6_Handler);
	mybtn_attach( &key7_struct, PRESS_DOWN, XferExternalKey7_Handler);
	mybtn_attach( &key7_struct, PRESS_UP, XferExternalKey7_Handler);
	
	mybtn_start( &key1_struct );
	mybtn_start( &key2_struct );
	mybtn_start( &key3_struct );
	mybtn_start( &key4_struct );
	mybtn_start( &key5_struct );
	mybtn_start( &key6_struct );
	mybtn_start( &key7_struct );
	st7735s_init();
	while(1)
    {
		mybtn_ticks();
		My_mDelay( 5 );
		if( bool_colorsequenceflag == 1 )
		{
			bool_colorsequenceflag = 0;
			lcd_clear(BACK_COLOR);
			Display_123(ColorSequence);
//			lcd_show_num(70, 54,24,result1.millimeter,4,0);
		
		}
	#ifdef EncodingWheel
		if(Wheel_isReady == 1)
		{
			buzzerSound(H6);
			My_mDelay(120);
			buzzerSound(H5);
			My_mDelay(100);
			buzzerSound(H4);
			My_mDelay(120);
			buzzerSound(0);
			Wheel_isReady = 2;
		}
		if(Wheel_isReady == 2)
		{
			lcd_show_string(58, 54, 24, "+");
		}

	#endif
    }
}
/***********************************************************************************************

                                       应用主函数

************************************************************************************************/
int Task_GUI_create(void)
{
    thread_gui = rt_thread_create("gui",           /* 线程名字 */
								   gui_task,       /* 线程入口函数 */
								   RT_NULL,        /* 线程入口函数参数 */
								   512,            /* 线程栈大小 */
								   RT_THREAD_PRIORITY_MAX/2,/* 线程的优先级 */
								   20);            /* 线程时间片 */
	if(thread_gui != RT_NULL)
	{
		rt_thread_startup(thread_gui);
		rt_kprintf("thread_gui startup!\n");
	}
	
	return 0;
}
INIT_APP_EXPORT(Task_GUI_create);



















