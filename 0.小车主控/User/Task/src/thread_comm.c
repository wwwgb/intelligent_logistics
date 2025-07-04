/* Includes ------------------------------------------------------------------*/
#include "thread_comm.h"
#include "bsp.h"
#include "data.h"
#include "CheckCalc.h"

/* Private macros ------------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* �����߳̿��ƿ�ָ�� */
rt_thread_t thread_comm = RT_NULL;
/* Private functions ---------------------------------------------------------*/
/**
 * @brief �������������������ļ����ã���Ҫ�ⲿ����
 */
float Wheel_isReady=0;
float pos_x=0;
float pos_y=0;
float zangle=0;
float xangle=0;
float yangle=0;
float w_z=0;

/**
 * @brief ���ݽ�������  �����MCUƽ̨���������⣬ֻ�轫���ڽ��յ���ֵ����ú������ɽ���
 * @param  rec ���ڽ��յ����ֽ�����
 */
void Data_Analyse(uint8_t rec)//���������ݽ���
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
		{//֡ͷ
			if( checksum((uint8_t*)&pdata[i*11],10) == pdata[i*11+10] )
			{//��У��
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
		{//֡ͷ
			if( checksum((uint8_t*)&pdata[i*11],10) == pdata[i*11+10] )
			{//��У��
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
static void RecognitionModule_Transmit(uint8_t *pData, uint16_t Size)//��ά��ģ�鴮�ڷ���
{
	Bsp_UARTMixed_TxTrigger(&muart4, (char*)pData, Size);
}
static void PCModule_Transmit(uint8_t *pData, uint16_t Size)//�Ӿ��������ڷ���
{
	Bsp_UARTMixed_TxTrigger(&muart3, (char*)pData, Size);
}
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/***********************************************************************************************

                                       Ӧ���¼����жϣ�����

************************************************************************************************/
/**
  * @brief  ���ڽ�������¼�
  * @param  None
  * @retval None
  */
void XferExternalUart2Rx_Handler(char *pdata, uint16_t len)//�����̡���RS232ͨ��
{//������ɻص�����ָ��
		uint8_t i;
	#ifdef EncodingWheel
//  ����ACTION���������ݽ���
		for(i=0; i<len; i++)
		{
			Data_Analyse(pdata[i]);
		}
  #else
//  ����ά�����������ݽ���
		if(len == 33)
				JY61_DataAnalysis( pdata, len, &GyroData_Struct.mark, GyroData_Struct.stcAcc, GyroData_Struct.stcGyro, GyroData_Struct.stcAngle, &GyroData_Struct.stcTemp );
		else if(len == 22)
			HWT101_DataAnalysis( pdata, len, &GyroData_Struct.mark, &GyroData_Struct.stcGyro[2], &GyroData_Struct.stcAngle[2] );
		
		if( ( GyroData_Struct.mark & 0x01 ) == 0x01 )
		{
			GyroData_Struct.temp = GyroData_Struct.stcTemp/340.0f+36.53f;
			GyroData_Struct.ax = (float)GyroData_Struct.stcAcc[0]*16.0f/32768.0f;//(g Ϊ�������ٶȣ���ȡ 9.8m/s 2 )
			GyroData_Struct.ay = (float)GyroData_Struct.stcAcc[1]*16.0f/32768.0f;//(g Ϊ�������ٶȣ���ȡ 9.8m/s 2 )
			GyroData_Struct.az = (float)GyroData_Struct.stcAcc[2]*16.0f/32768.0f;//(g Ϊ�������ٶȣ���ȡ 9.8m/s 2 )
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
			GyroData_Struct.Roll  = GyroData_Struct.stcAngle[0]*180/32768.0f;//��ת�ǣ�x �ᣩ
			GyroData_Struct.Pitch = GyroData_Struct.stcAngle[1]*180/32768.0f;//�����ǣ�y �ᣩ
			GyroData_Struct.Yaw   = GyroData_Struct.stcAngle[2]*180/32768.0f;//ƫ���ǣ�z �ᣩ
			GyroData_Struct.mark &= (0xfb);
		}
	#endif
}
void XferExternalUart3Rx_Handler(char *pdata, uint16_t len)//�Ӿ���������TTLͨ��
{//������ɻص�����ָ��
	uint8_t i;
	
	RecognitionModule_ReceivingProcess(&PCModule_t,(uint8_t*)pdata,len);
	if( PCModule_t.RecognitionModuleSte == RM_succeed )
	{//���ճɹ�
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
void XferExternalUart4Rx_Handler(char *pdata, uint16_t len)//��ά��ģ�顪��TTLͨ��
{//������ɻص�����ָ��
	uint8_t i;
	
	RecognitionModule_ReceivingProcess(&RecognitionModule_t,(uint8_t*)pdata,len);
	if( RecognitionModule_t.RecognitionModuleSte == RM_succeed )
	{//���ճɹ�
		for(i=0; i<Recognition_RX_LEN; i++)
			Recognition_Buffer[i] = pdata[i];
		bool_recognitionflag = 1;
	}
}
void XferExternalUart5Rx_Handler(char *pdata, uint16_t len)//������ư塪��TTLͨ��
{//������ɻص�����ָ��

}
/***********************************************************************************************

                                       Ӧ��������

************************************************************************************************/
void comm_task(void *pvParameters)
{
	while(1)
    {
		//=========ʶ��ģ��ͨѶЭ��============================
		RecognitionModule_Scan1Ms(&RecognitionModule_t);
		RecognitionModule_Scan1Ms(&PCModule_t);
		//=========CANͨѶЭ��============================
		Tim_GetCurrentTimeAdd_Scan1MS();
		CANCommunication_Scan();            //���Ҳ1ms����ɨ��
		My_mDelay( 1 );
		}
}
/***********************************************************************************************

                                       Ӧ��������

************************************************************************************************/
int Task_COMM_create(void)
{
	/*********************ֱ�������ʼ��***************************/
#ifdef __SLAVE_DCMotorMiniwatt_H__
	SLAVE_DCMotorMiniwatt_Init(&DCMotorMiniwatt1_S,1);//ID��1
	SLAVE_DCMotorMiniwatt_Init(&DCMotorMiniwatt2_S,2);//ID��2
	SLAVE_DCMotorMiniwatt_Init(&DCMotorMiniwatt3_S,3);//ID��3
	SLAVE_DCMotorMiniwatt_Init(&DCMotorMiniwatt4_S,4);//ID��4
#endif
	/**********************CAN��ʼ��***************************/
	/* CAN1 1000kbps * CAN2 500kbps */
	CANCommunication_Init();
	/**********************��ά����ɫ��ȡ��ʼ��**************************/  					
	RecognitionModule_Init(&RecognitionModule_t,RecognitionModule_Transmit);
	RecognitionModule_Init(&PCModule_t,PCModule_Transmit);
	/**********************���ڳ�ʼ��***************************/
	Bsp_UartMixed_Init( &muart2, XferExternalUart2Rx_Handler, 0 );
	Bsp_UartMixed_Init( &muart3, XferExternalUart3Rx_Handler, 0 );
	Bsp_UartMixed_Init( &muart4, XferExternalUart4Rx_Handler, 0 );
	Bsp_UartMixed_Init( &muart5, XferExternalUart5Rx_Handler, 0 );
	/**********************��������****************************/
    thread_comm = rt_thread_create("comm",           /* �߳����� */
								   comm_task,        /* �߳���ں��� */
								   RT_NULL,          /* �߳���ں������� */
								   1024,              /* �߳�ջ��С */
								   1,                /* �̵߳����ȼ� */
								   20);              /* �߳�ʱ��Ƭ */
	if(thread_comm != RT_NULL)
	{
		rt_thread_startup(thread_comm);
		rt_kprintf("thread_comm startup!\n");
	}
	
	return 0;
}
INIT_APP_EXPORT(Task_COMM_create);



















