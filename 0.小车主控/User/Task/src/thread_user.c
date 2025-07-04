/* Includes ------------------------------------------------------------------*/
#include "thread_user.h"
/* �����������ļ� */
#include "data.h"
/* ��������ͷ�ļ� */
#include "thread_comm.h"
#include "thread_rccu.h"
#include "stm32f4xx.h"
#include "SCServo.h"
#include "usart.h"
#include "Servo.h"
/* Private macros ------------------------------------------------------------*/
int16_t wait = 0;
/* Private types -------------------------------------------------------------*/
typedef struct 
{
	uint16_t Color;
	float X;
	float Y;
	float Z;
}_t_MaterialMessage;
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* �����߳̿��ƿ�ָ�� */
rt_thread_t thread_PathWrite = RT_NULL;
/* Private functions ---------------------------------------------------------*/
static void Debug_Await(void)//��ִͣ��
{
	while(KEY_4() == 0)
		My_mDelay(50);
	while(KEY_4() == 1)
		My_mDelay(50);
}
static void Wait_Start(void)//�ȴ���ʼ��
{
	while(Wheel_isReady != 2)
		My_mDelay(50);
}
/*�ַ���ƴ��*/
void Stract(char strDestination[],char strSource[],int num)
{
	int i = 0,j = 0;
	while(strDestination[i]!='\0') i++;
	for(j = 0;j<num;j++)
			strDestination[i++] = strSource[j];
}
void Update_X(float New_X)
{
	char Update_x[8] = "ACTX";
	static union
	{
		float X;
		char data[4];
	}New_set;
	New_set.X = New_X;
	Stract(Update_x,New_set.data,4);
	Bsp_UARTMixed_TxTrigger(&muart2, Update_x, 8);
}
void Update_Y(float New_Y)
{
	char Update_y[8] = "ACTY";
	static union
	{
		float Y;
		char data[4];
	}New_set;
	New_set.Y = New_Y;
	Stract(Update_y,New_set.data,4);
	Bsp_UARTMixed_TxTrigger(&muart2, Update_y, 8);
}
void Update_J(float New_J)
{
	char Update_j[8] = "ACTJ";
	static union
	{
		float J;
		char data[4];
	}New_set;
	New_set.J = New_J;
	Stract(Update_j,New_set.data,4);
	Bsp_UARTMixed_TxTrigger(&muart2, Update_j, 8);
}
static void SetCoordinateXY(float SET_LOCATION_X, float SET_LOCATION_Y)//���ñ���������
{
		ChassisRELAX_set();//�л�����ģʽ
		Update_X(SET_LOCATION_X);
		HAL_Delay(10);
		Update_Y(SET_LOCATION_Y);
		HAL_Delay(100);
		ChassisCoord_Set(SET_LOCATION_X,SET_LOCATION_Y,0);//��������
		ChassisCoord_WaitStop();// �ȴ������ȶ�
		HAL_Delay(50);
}
static void ResetCoordinateXYJ(void)//��������������
{
		ChassisRELAX_set();//�л�����ģʽ
		Bsp_UARTMixed_TxTrigger(&muart2, "ACT0", 4);
		HAL_Delay(50);
		ChassisCoord_Set(0,0,0);//�л�����ģʽ
}
static void Identify_QRCode(void)//ʶ���ά��
{
	uint8_t j,i;
	bool_recognitionflag = 0;
	RecognitionModule_Start(&RecognitionModule_t);
	while( RecognitionModule_t.RecognitionModuleSte != RM_succeed )//�ȴ�����
	{
	}
	RecognitionModule_Stop(&PCModule_t);
	for(j=0; j<2; j++)
	{
		for(i=0; i<3; i++)  
		{//ʶ�𵽵��ַ�����Ӧ��ɫ
			switch(Recognition_Buffer[(4*j)+i])
			{
				case '1':
					ColorSequence[j][i] = RED;
					break;
				case '2':
					ColorSequence[j][i] = GREEN;
					break;
				case '3':
					ColorSequence[j][i] = BLUE;
					break;
			}
		}
	}
	bool_recognitionflag = 0;
	bool_colorsequenceflag = 1;
}
static void IdentifyCircle_Get(float SET_LOCATION_X, float SET_LOCATION_Y)//ʶ����Ĳ�����λ��
{
	double y_err[2] = {0,0};
	double x_err[2] = {0,0};
	double yout,xout;
	
	bool_recognitionflag = 0;
	CircleRecognitionModule_Start(&PCModule_t);
	while(1)
	{
		My_mDelay(10);
		wait++;
		if(PCModule_t.RecognitionModuleSte == RM_error)
		{
			My_mDelay(500);
			bool_recognitionflag = 0;
			CircleRecognitionModule_Start(&PCModule_t);
		}
		else if(PCModule_t.RecognitionModuleSte == RM_succeed)
		{// ����ƫ����㣬����ƶ��ٶ�
			x_err[1] = x_err[0];
			x_err[0] = SET_LOCATION_X - cartesian_loc.x;
			xout = ( 1.5 * x_err[0] ) + ( 1.5f * ( x_err[0] - x_err[1] ) );
			if( xout > 50 ) xout = 50;
		    else if( xout < -50 )xout = -50;
			
			y_err[1] = y_err[0];
			y_err[0] = SET_LOCATION_Y - cartesian_loc.y;
			yout = ( 1.5 * y_err[0] ) + ( 1.5f * ( y_err[0] - y_err[1] ) );
			if( yout > 50 ) yout = 50;
		    else if( yout < -50 )yout = -50;
			
			if( ( ABS(y_err[0]) <= 1 ) && ( ABS(x_err[0]) <= 1 ) )// ��xyƫ��ֵС�ڵ���1ʱ��ֹͣ����
			{
				ChassisSpeed_Set(0,0);// ֹͣ�ƶ�
				wait = 0;
				break;
			}
			else
			{
				ChassisSpeed_Set(xout,-yout);
			}
			
			PCModule_t.RecognitionModuleSte = RM_Identify;
		}
		else if(wait == 2000)//��ʱ�Զ�����
		{
			wait = 0;
			ChassisSpeed_Set(0,0);// ֹͣ�ƶ�
			break;
		}
	}
	RecognitionModule_Stop(&PCModule_t);
}
void Pause_ColorRecognition()//��ͣ��ɫʶ��
{
		RecognitionModule_Stop(&PCModule_t);//ֹͣ�Ӿ�����Ŀǰ����ִ������
		bool_recognitionflag = 0;
		CircleRecognitionModule_Start(&PCModule_t);
}
void Renew_ColorRecognition()//�ָ���ɫʶ��
{
		RecognitionModule_Stop(&PCModule_t);//ֹͣ�Ӿ�����Ŀǰ����ִ������
		bool_recognitionflag = 0;
		ColorRecognitionModule_Start(&PCModule_t);
}
static void IdentifyColor_Get(uint8_t mode)//ʶ����ɫ��ץȡ����һ��mode=0���ڶ���mode=1
{
	uint8_t i = 0;
	
	if(mode > 1)
		mode = 1;
	bool_recognitionflag = 0;
	ColorRecognitionModule_Start(&PCModule_t);
	while(1)
	{
		My_mDelay(10);
		if(PCModule_t.RecognitionModuleSte == RM_error)
		{
			My_mDelay(500);
			bool_recognitionflag = 0;
			ColorRecognitionModule_Start(&PCModule_t);
		}
		else if(PCModule_t.RecognitionModuleSte == RM_succeed)
		{
			if(CurrentMaterialColor == ColorSequence[mode][i])
			{//��ɫʶ��ɹ��ж���ɫ
				//����ץȡ
				Pause_ColorRecognition();//��ͣ��ɫʶ��
				Get_A();
				if(CurrentMaterialColor == RED){
				Put_1();
				}
				else if(CurrentMaterialColor == GREEN){
				Put_2();
				}
				else if(CurrentMaterialColor == BLUE){
				Put_3();
				}
				i++;
				if(i < 3)//�ָ���ʶ��λ��
				{
					IdentifyColor_action();//ʶ��ԭ��������
					HAL_Delay(1200);
					Renew_ColorRecognition();//�ָ���ɫʶ��
				}
			}
			My_mDelay(500);
			PCModule_t.RecognitionModuleSte = RM_Identify;
			bool_recognitionflag = 0;
			My_mDelay(500);
		}
		if(i==3)
			break;
	}
	RecognitionModule_Stop(&PCModule_t);
}

void RoughingArea_PutGet(uint8_t ColorSequenceNum)//�ּӹ�����������
{
	uint8_t i;
		for(i=0; i<3; i++) //��ȡҪȡ��ɫ������
		{
			if(ColorSequence[ColorSequenceNum][i] == RED){
			Get_1();
			Put_Red();
			}
			else if(ColorSequence[ColorSequenceNum][i] == GREEN){
			Get_2();
			Put_Green();
			}
			else if(ColorSequence[ColorSequenceNum][i] == BLUE){
			Get_3();
			Put_Blue();
			}
		}
		//Debug_Await();
		for(i=0; i<3; i++) //��ȡҪȡ��ɫ������
		{
			if(ColorSequence[ColorSequenceNum][i] == RED){
			Get_Red();
			Put_1();
			}
			else if(ColorSequence[ColorSequenceNum][i] == GREEN){
			Get_Green();
			Put_2();
			}
			else if(ColorSequence[ColorSequenceNum][i] == BLUE){
			Get_Blue();
			Put_3();
			}
		}
}
void FashioningArea_Put(uint8_t ColorSequenceNum)//���ӹ������ݴ�������������
{
	uint8_t i;
	if(ColorSequenceNum == 0){
		for(i=0; i<3; i++) //���²�����
		{
			if(ColorSequence[ColorSequenceNum][i] == RED){
			Get_1();
			Put_Red();
			}
			else if(ColorSequence[ColorSequenceNum][i] == GREEN){
			Get_2();
			Put_Green();
			}
			else if(ColorSequence[ColorSequenceNum][i] == BLUE){
			Get_3();
			Put_Blue();
			}
		}
	}
	if(ColorSequenceNum == 1){
		for(i=0; i<3; i++) //�������
		{
			if(ColorSequence[ColorSequenceNum][i] == RED){
			Get_1();
			Put_Red_Stacking();
			}
			else if(ColorSequence[ColorSequenceNum][i] == GREEN){
			Get_2();
			Put_Green_Stacking();
			}
			else if(ColorSequence[ColorSequenceNum][i] == BLUE){
			Get_3();
			Put_Blue_Stacking();
			}
		}
}
}
/* ---------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/
/* ---------------------------------------------------------------------------*/
void PathWrite_task(void *pvParameters)/////////////////////////////////////////���߳�·���滮����
{
		Pause_ColorRecognition();
		Rest_action_S2();//�����ʼ̧��

	while(1)
    {
	#if 0
		//����ģʽ
		Debug_Await();// �ȴ�����������
		Wait_Start();// �ȴ������ֳ�ʼ�����
		//================����ɨ����================
		ChassisCoord_Set(150,200, 0); 
		ChassisCoord_WaitStop();
		Debug_Await();// �ȴ�����������
		//==============ǰ����ԭ����==============
		ChassisCoord_Set(1480, 190, 0);
		ChassisCoord_WaitStop();
		IdentifyColor_action();//ʶ�����϶���
		Debug_Await();// �ȴ�����������
		Rest_action_S2();
		//===============һ��ת����===============
		ChassisCoord_Set(1900, 200, 90);
		ChassisCoord_WaitStop();
		//=============�ƶ����ּӹ���=============��������������������������������������������������������
		ChassisCoord_Set(1905,1069, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//ʶ��Բ����е�۶���
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//�Ӿ�ʶ��Բ��
		Debug_Await();// �ȴ�����������
		Rest_action_S2();
		//===============����ת����===============
		ChassisCoord_Set(1850, 1750, 90);
		ChassisCoord_WaitStop();
		//=============�ƶ������ӹ���===============����������������������������������������������������
		ChassisCoord_Set(1051,1781, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//ʶ��Բ����е�۶���
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//�Ӿ�ʶ��Բ��
		Debug_Await();// �ȴ�����������
    #else
//****************************************************************************��һ��********************************
		Debug_Await();// �ȴ�����������
		Wait_Start();// �ȴ������ֳ�ʼ�����
		//================����ɨ����================
		ChassisCoord_Set(150,200, 0); // �ƶ�������ֵ
		ChassisCoord_WaitStop();// �ȴ������ȶ�
		//===============ʶ���ά��===============
		Identify_QRCode();
		//==============ǰ����ԭ����==============
		ChassisCoord_Set(1470, 190, 0);
		ChassisCoord_WaitStop();
		IdentifyColor_action();//ʶ�����϶���
		//=============ʶ����ɫ��ȡ��=============
		HAL_Delay(1300);
		IdentifyColor_Get(0);
		Rest_action_S2();
		HAL_Delay(2000);
		//===============һ��ת����===============
		ChassisCoord_Set(1900, 200, 90);
		ChassisCoord_WaitStop();
		//=============�ƶ����ּӹ���=============��������������������������������������������������������
		ChassisCoord_Set(1925,1068, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//ʶ��Բ����е�۶���
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//�Ӿ�ʶ��Բ��
		SetCoordinateXY(1925,1068);//λ���ر��
		//==========�ּӹ������з���ȡ��==========
		RoughingArea_PutGet(0);
		Rest_action_S2();
		//===============����ת����===============
		ChassisCoord_Set(1850, 1750, 90);
		ChassisCoord_WaitStop();
		//=============�ƶ������ӹ���===============����������������������������������������������������
		ChassisCoord_Set(1067,1815, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//ʶ��Բ����е�۶���
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//�Ӿ�ʶ��Բ��
		SetCoordinateXY(1067,1815);//λ���ر��
		//==========���ӹ��������²����==========
		FashioningArea_Put(0);
		HAL_Delay(500);
		Rest_action_S2();
		HAL_Delay(2000);
		//===============����ת����===============
		ChassisCoord_Set(1850, 1750, 180);
		ChassisCoord_WaitStop();
		//===============һ��ת����===============
		ChassisCoord_Set(1900, 200, 0);
		ChassisCoord_WaitStop();
		//==============ǰ����ԭ����==============
		ChassisCoord_Set(1480, 190, 0);
		ChassisCoord_WaitStop();
		IdentifyColor_action();//ʶ�����϶���
//****************************************************************************�ڶ���********************************
		//=============ʶ����ɫ��ȡ��=============
		HAL_Delay(1800);
		IdentifyColor_Get(1);
		Rest_action_S2();
		//===============һ��ת����===============
		ChassisCoord_Set(1900, 200, 90);
		ChassisCoord_WaitStop();
		//=============�ƶ����ּӹ���=============������������������������������������������������������
		ChassisCoord_Set(1925,1068, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//ʶ��Բ����е�۶���
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//�Ӿ�ʶ��Բ��
		SetCoordinateXY(1925,1068);//λ���ر��
		//==========�ּӹ������з���ȡ��==========
		RoughingArea_PutGet(1);
		Rest_action_S2();
		//===============����ת����===============
		ChassisCoord_Set(1850, 1750, 90);
		ChassisCoord_WaitStop();
		//=============�ƶ������ӹ���===============��������������������������������������������������
		ChassisCoord_Set(1067,1815, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//ʶ��Բ����е�۶���
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//�Ӿ�ʶ��Բ��
		SetCoordinateXY(1067,1815);//λ���ر��
		//==========���ӹ��������������==========
		FashioningArea_Put(1);
		HAL_Delay(500);
		Rest_action_S2();
		//===============����ת����===============
		ChassisCoord_Set(150, 1800, 0);
		ChassisCoord_WaitStop();
		//===============������=============
		ChassisCoord_Set(200, 100, 0);
		ChassisCoord_WaitStop();
		ChassisSpeed_Set(100,50);//�ٶ�ģʽ0
		HAL_Delay(1000);
		ChassisSpeed_Set(0,50);//�ٶ�ģʽ0
		HAL_Delay(1300);
		ChassisSpeed_Set(0,0);// ֹͣ

		#endif	
		
		//*/
		Debug_Await();// �ȴ���һ������
	}
}
int Task_User_create(void)
{
	thread_PathWrite = rt_thread_create( "PathWrite",             /* �߳����� */
								         PathWrite_task,          /* �߳���ں��� */
								         RT_NULL,                 /* �߳���ں������� */
								         1024,          		      /* �߳�ջ��С */
								         10,                      /* �̵߳����ȼ� */
								         20);                     /* �߳�ʱ��Ƭ */
	if(thread_PathWrite != RT_NULL)
	{
		rt_thread_startup(thread_PathWrite);
		rt_kprintf("thread_PathWrite startup!\n");
	}
	
	return 0;
}
