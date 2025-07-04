/* Includes ------------------------------------------------------------------*/
#include "thread_comm.h"
#include "bsp.h"
#include "data.h"
#include "CheckCalc.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* 定义线程控制块指针 */
rt_thread_t thread_comm = RT_NULL;
/* Private functions ---------------------------------------------------------*/
/**
 * @brief 解析结果变量，如需跨文件调用，需要外部声明
 */
float Wheel_isReady=0;
float pos_x=0;
float pos_y=0;
float zangle=0;
float xangle=0;
float yangle=0;
float w_z=0;

/**
 * @brief 数据解析函数  如更换MCU平台或更换软件库，只需将串口接收到的值传入该函数即可解析
 * @param  rec 串口接收到的字节数据
 */
void Data_Analyse(uint8_t rec)//编码轮数据解析
{
	static uint8_t ch;
	static union
	{
		uint8_t date[24];
		float ActVal[6];
	}posture;
	static uint8_t count=0;
	static uint8_t i=0;

	ch=rec;
	switch(count)
	{
		case 0:
			if(ch==0x0d)
				count++;
			else
				count=0;
			break;
		case 1:
			if(ch==0x0a)
			{
				i=0;
				count++;
			}
			else if(ch==0x0d);
			else
				count=0;
			break;
		case 2:
			posture.date[i]=ch;
			i++;
			if(i>=24)
			{
				i=0;
				count++;
			}
			break;
		case 3:
			if(ch==0x0a)
				count++;
			else
				count=0;
			break;
		case 4:
			if(ch==0x0d)
			{
				zangle=posture.ActVal[0];
				xangle=posture.ActVal[1];
				yangle=posture.ActVal[2];
				pos_x=posture.ActVal[3];
				pos_y=posture.ActVal[4];
				w_z=posture.ActVal[5];
				if(Wheel_isReady == 0)
					{
						Wheel_isReady = 1;
					}
			}
			count=0;
			break;
		default:
			count=0;
		break;
	}
}
static void JY61_DataAnalysis(char *pdata, uint16_t len, uint8_t* pmark, int16_t* pstcAcc, int16_t* pstcGyro, int16_t* pstcAngle, int16_t* pstcTemp )
{
	uint8_t i;
	
	for(i=0; i<3; i++)
	{
		if( 0x55 == pdata[i*11] )
		{//帧头
			if( checksum((uint8_t*)&pdata[i*11],10) == pdata[i*11+10] )
			{//和校验
				switch(pdata[i*11+1])
				{
					case 0x51:
						pstcAcc[0] = pdata[i*11+2] | (pdata[i*11+3]<<8);
						pstcAcc[1] = pdata[i*11+4] | (pdata[i*11+5]<<8);
						pstcAcc[2] = pdata[i*11+6] | (pdata[i*11+7]<<8);
						*pstcTemp = pdata[i*11+8] | (pdata[i*11+9]<<8);
						*pmark |= 0x01;
						break;
					case 0x52:
						pstcGyro[0] = pdata[i*11+2] | (pdata[i*11+3]<<8);
						pstcGyro[1] = pdata[i*11+4] | (pdata[i*11+5]<<8);
						pstcGyro[2] = pdata[i*11+6] | (pdata[i*11+7]<<8);
						*pmark |= 0x02;
						break;
					case 0x53:
						pstcAngle[0] = pdata[i*11+2] | (pdata[i*11+3]<<8);
						pstcAngle[1] = pdata[i*11+4] | (pdata[i*11+5]<<8);
						pstcAngle[2] = pdata[i*11+6] | (pdata[i*11+7]<<8);
						*pmark |= 0x04;
						break;
				}
			}
		}
	}
}
static void HWT101_DataAnalysis(char *pdata, uint16_t len, uint8_t* pmark, int16_t* pstcGyro, int16_t* pstcAngle )
{
	uint8_t i;
	
	for(i=0; i<2; i++)
	{
		if( 0x55 == pdata[i*11] )
		{//帧头
			if( checksum((uint8_t*)&pdata[i*11],10) == pdata[i*11+10] )
			{//和校验
				switch(pdata[i*11+1])
				{
					case 0x52:
						*pstcGyro = pdata[i*11+6] | (pdata[i*11+7]<<8);
						*pmark |= 0x02;
						break;
					case 0x53:
						*pstcAngle = pdata[i*11+6] | (pdata[i*11+7]<<8);
						*pmark |= 0x04;
						break;
				}
			}
		}
	}
}
static void RecognitionModule_Transmit(uint8_t *pData, uint16_t Size)//二维码模块串口发送
{
	Bsp_UARTMixed_TxTrigger(&muart4, (char*)pData, Size);
}
static void PCModule_Transmit(uint8_t *pData, uint16_t Size)//视觉主机串口发送
{
	Bsp_UARTMixed_TxTrigger(&muart3, (char*)pData, Size);
}
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/***********************************************************************************************

                                       应用事件（中断）函数

************************************************************************************************/
/**
  * @brief  串口接收完成事件
  * @param  None
  * @retval None
  */
void XferExternalUart2Rx_Handler(char *pdata, uint16_t len)//编码盘——RS232通信
{//接收完成回调函数指针
		uint8_t i;
	#ifdef EncodingWheel
//  以下ACTION编码轮数据解析
		for(i=0; i<len; i++)
		{
			Data_Analyse(pdata[i]);
		}
  #else
//  以下维特陀螺仪数据解析
		if(len == 33)
				JY61_DataAnalysis( pdata, len, &GyroData_Struct.mark, GyroData_Struct.stcAcc, GyroData_Struct.stcGyro, GyroData_Struct.stcAngle, &GyroData_Struct.stcTemp );
		else if(len == 22)
			HWT101_DataAnalysis( pdata, len, &GyroData_Struct.mark, &GyroData_Struct.stcGyro[2], &GyroData_Struct.stcAngle[2] );
		
		if( ( GyroData_Struct.mark & 0x01 ) == 0x01 )
		{
			GyroData_Struct.temp = GyroData_Struct.stcTemp/340.0f+36.53f;
			GyroData_Struct.ax = (float)GyroData_Struct.stcAcc[0]*16.0f/32768.0f;//(g 为重力加速度，可取 9.8m/s 2 )
			GyroData_Struct.ay = (float)GyroData_Struct.stcAcc[1]*16.0f/32768.0f;//(g 为重力加速度，可取 9.8m/s 2 )
			GyroData_Struct.az = (float)GyroData_Struct.stcAcc[2]*16.0f/32768.0f;//(g 为重力加速度，可取 9.8m/s 2 )
			GyroData_Struct.mark &= (0xfe);
		}
		if( ( GyroData_Struct.mark & 0x02 ) == 0x02 )
		{
			GyroData_Struct.wx = (float)GyroData_Struct.stcGyro[0]*2000/32768.0f;
			GyroData_Struct.wy = (float)GyroData_Struct.stcGyro[1]*2000/32768.0f;
			GyroData_Struct.wz = (float)GyroData_Struct.stcGyro[2]*2000/32768.0f;
			GyroData_Struct.mark &= (0xfd);
		}
		if( ( GyroData_Struct.mark & 0x04 ) == 0x04 )
		{
			GyroData_Struct.Roll  = GyroData_Struct.stcAngle[0]*180/32768.0f;//滚转角（x 轴）
			GyroData_Struct.Pitch = GyroData_Struct.stcAngle[1]*180/32768.0f;//俯仰角（y 轴）
			GyroData_Struct.Yaw   = GyroData_Struct.stcAngle[2]*180/32768.0f;//偏航角（z 轴）
			GyroData_Struct.mark &= (0xfb);
		}
	#endif
}
void XferExternalUart3Rx_Handler(char *pdata, uint16_t len)//视觉主机——TTL通信
{//接收完成回调函数指针
	uint8_t i;
	
	RecognitionModule_ReceivingProcess(&PCModule_t,(uint8_t*)pdata,len);
	if( PCModule_t.RecognitionModuleSte == RM_succeed )
	{//接收成功
		for(i=0; i<Recognition_RX_LEN; i++)
			Recognition_Buffer[i] = pdata[i];
		if(PCModule_t.state == RM_Colorstart)
		{
			if(pdata[0] == '1')
			    CurrentMaterialColor = RED;
			else if(pdata[0] == '2')
			    CurrentMaterialColor = GREEN;
			else if(pdata[0] == '3')
			    CurrentMaterialColor = BLUE;
		}
		if(PCModule_t.state == RM_Location)
		{
			if( ( pdata[0] == '(' ) && ( pdata[len-2] == ')' ) )
			{
				cartesian_loc.x = atol(pdata+1);
				while( ( *pdata != ',' ) && ( *pdata != '\0' ) && ( *pdata != ')' ) )
					pdata++;
				cartesian_loc.y = atol(pdata+1);
			}
		}
		if(PCModule_t.state == RM_Circle)
		{
			if( ( pdata[0] == '(' ) && ( pdata[len-2] == ')' ) )
			{
				cartesian_loc.x = atol(pdata+1);
				while( ( *pdata != ',' ) && ( *pdata != '\0' ) && ( *pdata != ')' ) )
					pdata++;
				cartesian_loc.y = atol(pdata+1);
			}
		}
		bool_recognitionflag = 1;
	}
}
void XferExternalUart4Rx_Handler(char *pdata, uint16_t len)//二维码模块——TTL通信
{//接收完成回调函数指针
	uint8_t i;
	
	RecognitionModule_ReceivingProcess(&RecognitionModule_t,(uint8_t*)pdata,len);
	if( RecognitionModule_t.RecognitionModuleSte == RM_succeed )
	{//接收成功
		for(i=0; i<Recognition_RX_LEN; i++)
			Recognition_Buffer[i] = pdata[i];
		bool_recognitionflag = 1;
	}
}
void XferExternalUart5Rx_Handler(char *pdata, uint16_t len)//舵机控制板——TTL通信
{//接收完成回调函数指针

}
/***********************************************************************************************

                                       应用任务函数

************************************************************************************************/
void comm_task(void *pvParameters)
{
	while(1)
    {
		//=========识别模块通讯协议============================
		RecognitionModule_Scan1Ms(&RecognitionModule_t);
		RecognitionModule_Scan1Ms(&PCModule_t);
		//=========CAN通讯协议============================
		Tim_GetCurrentTimeAdd_Scan1MS();
		CANCommunication_Scan();            //最好也1ms进行扫描
		My_mDelay( 1 );
		}
}
/***********************************************************************************************

                                       应用主函数

************************************************************************************************/
int Task_COMM_create(void)
{
	/*********************直流电机初始化***************************/
#ifdef __SLAVE_DCMotorMiniwatt_H__
	SLAVE_DCMotorMiniwatt_Init(&DCMotorMiniwatt1_S,1);//ID号1
	SLAVE_DCMotorMiniwatt_Init(&DCMotorMiniwatt2_S,2);//ID号2
	SLAVE_DCMotorMiniwatt_Init(&DCMotorMiniwatt3_S,3);//ID号3
	SLAVE_DCMotorMiniwatt_Init(&DCMotorMiniwatt4_S,4);//ID号4
#endif
	/**********************CAN初始化***************************/
	/* CAN1 1000kbps * CAN2 500kbps */
	CANCommunication_Init();
	/**********************二维码颜色读取初始化**************************/  					
	RecognitionModule_Init(&RecognitionModule_t,RecognitionModule_Transmit);
	RecognitionModule_Init(&PCModule_t,PCModule_Transmit);
	/**********************串口初始化***************************/
	Bsp_UartMixed_Init( &muart2, XferExternalUart2Rx_Handler, 0 );
	Bsp_UartMixed_Init( &muart3, XferExternalUart3Rx_Handler, 0 );
	Bsp_UartMixed_Init( &muart4, XferExternalUart4Rx_Handler, 0 );
	Bsp_UartMixed_Init( &muart5, XferExternalUart5Rx_Handler, 0 );
	/**********************创建任务****************************/
    thread_comm = rt_thread_create("comm",           /* 线程名字 */
								   comm_task,        /* 线程入口函数 */
								   RT_NULL,          /* 线程入口函数参数 */
								   1024,              /* 线程栈大小 */
								   1,                /* 线程的优先级 */
								   20);              /* 线程时间片 */
	if(thread_comm != RT_NULL)
	{
		rt_thread_startup(thread_comm);
		rt_kprintf("thread_comm startup!\n");
	}
	
	return 0;
}
INIT_APP_EXPORT(Task_COMM_create);



















