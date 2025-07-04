/* Includes ------------------------------------------------------------------*/
#include "thread_user.h"
/* 开发板数据文件 */
#include "data.h"
/* 其他任务头文件 */
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
/* 定义线程控制块指针 */
rt_thread_t thread_PathWrite = RT_NULL;
/* Private functions ---------------------------------------------------------*/
static void Debug_Await(void)//暂停执行
{
	while(KEY_4() == 0)
		My_mDelay(50);
	while(KEY_4() == 1)
		My_mDelay(50);
}
static void Wait_Start(void)//等待初始化
{
	while(Wheel_isReady != 2)
		My_mDelay(50);
}
/*字符串拼接*/
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
static void SetCoordinateXY(float SET_LOCATION_X, float SET_LOCATION_Y)//设置编码轮坐标
{
		ChassisRELAX_set();//切换自由模式
		Update_X(SET_LOCATION_X);
		HAL_Delay(10);
		Update_Y(SET_LOCATION_Y);
		HAL_Delay(100);
		ChassisCoord_Set(SET_LOCATION_X,SET_LOCATION_Y,0);//设置坐标
		ChassisCoord_WaitStop();// 等待车身稳定
		HAL_Delay(50);
}
static void ResetCoordinateXYJ(void)//编码轮坐标清零
{
		ChassisRELAX_set();//切换自由模式
		Bsp_UARTMixed_TxTrigger(&muart2, "ACT0", 4);
		HAL_Delay(50);
		ChassisCoord_Set(0,0,0);//切换坐标模式
}
static void Identify_QRCode(void)//识别二维码
{
	uint8_t j,i;
	bool_recognitionflag = 0;
	RecognitionModule_Start(&RecognitionModule_t);
	while( RecognitionModule_t.RecognitionModuleSte != RM_succeed )//等待数据
	{
	}
	RecognitionModule_Stop(&PCModule_t);
	for(j=0; j<2; j++)
	{
		for(i=0; i<3; i++)  
		{//识别到的字符串对应颜色
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
static void IdentifyCircle_Get(float SET_LOCATION_X, float SET_LOCATION_Y)//识别靶心并调整位置
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
		{// 积分偏差计算，算出移动速度
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
			
			if( ( ABS(y_err[0]) <= 1 ) && ( ABS(x_err[0]) <= 1 ) )// 当xy偏差值小于等于1时，停止调整
			{
				ChassisSpeed_Set(0,0);// 停止移动
				wait = 0;
				break;
			}
			else
			{
				ChassisSpeed_Set(xout,-yout);
			}
			
			PCModule_t.RecognitionModuleSte = RM_Identify;
		}
		else if(wait == 2000)//超时自动跳出
		{
			wait = 0;
			ChassisSpeed_Set(0,0);// 停止移动
			break;
		}
	}
	RecognitionModule_Stop(&PCModule_t);
}
void Pause_ColorRecognition()//暂停颜色识别
{
		RecognitionModule_Stop(&PCModule_t);//停止视觉主机目前所有执行任务
		bool_recognitionflag = 0;
		CircleRecognitionModule_Start(&PCModule_t);
}
void Renew_ColorRecognition()//恢复颜色识别
{
		RecognitionModule_Stop(&PCModule_t);//停止视觉主机目前所有执行任务
		bool_recognitionflag = 0;
		ColorRecognitionModule_Start(&PCModule_t);
}
static void IdentifyColor_Get(uint8_t mode)//识别颜色并抓取；第一趟mode=0，第二趟mode=1
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
			{//颜色识别成功判断颜色
				//进行抓取
				Pause_ColorRecognition();//暂停颜色识别
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
				if(i < 3)//恢复到识别位置
				{
					IdentifyColor_action();//识别原料区动作
					HAL_Delay(1200);
					Renew_ColorRecognition();//恢复颜色识别
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

void RoughingArea_PutGet(uint8_t ColorSequenceNum)//粗加工区放置物料
{
	uint8_t i;
		for(i=0; i<3; i++) //提取要取颜色索引号
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
		for(i=0; i<3; i++) //提取要取颜色索引号
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
void FashioningArea_Put(uint8_t ColorSequenceNum)//精加工区（暂存区）放置物料
{
	uint8_t i;
	if(ColorSequenceNum == 0){
		for(i=0; i<3; i++) //放下层物料
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
		for(i=0; i<3; i++) //码垛物料
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
void PathWrite_task(void *pvParameters)/////////////////////////////////////////主线程路劲规划任务
{
		Pause_ColorRecognition();
		Rest_action_S2();//舵机初始抬起

	while(1)
    {
	#if 0
		//调试模式
		Debug_Await();// 等待按键被按下
		Wait_Start();// 等待编码轮初始化完成
		//================出发扫码区================
		ChassisCoord_Set(150,200, 0); 
		ChassisCoord_WaitStop();
		Debug_Await();// 等待按键被按下
		//==============前进到原料区==============
		ChassisCoord_Set(1480, 190, 0);
		ChassisCoord_WaitStop();
		IdentifyColor_action();//识别物料动作
		Debug_Await();// 等待按键被按下
		Rest_action_S2();
		//===============一号转角区===============
		ChassisCoord_Set(1900, 200, 90);
		ChassisCoord_WaitStop();
		//=============移动到粗加工区=============→→→→→→→→→→→→→→→→→→→→→→→→→→→粗
		ChassisCoord_Set(1905,1069, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//识别圆环机械臂动作
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//视觉识别圆心
		Debug_Await();// 等待按键被按下
		Rest_action_S2();
		//===============二号转角区===============
		ChassisCoord_Set(1850, 1750, 90);
		ChassisCoord_WaitStop();
		//=============移动到精加工区===============→→→→→→→→→→→→→→→→→→→→→→→→→精
		ChassisCoord_Set(1051,1781, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//识别圆环机械臂动作
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//视觉识别圆心
		Debug_Await();// 等待按键被按下
    #else
//****************************************************************************第一趟********************************
		Debug_Await();// 等待按键被按下
		Wait_Start();// 等待编码轮初始化完成
		//================出发扫码区================
		ChassisCoord_Set(150,200, 0); // 移动到坐标值
		ChassisCoord_WaitStop();// 等待车身稳定
		//===============识别二维码===============
		Identify_QRCode();
		//==============前进到原料区==============
		ChassisCoord_Set(1470, 190, 0);
		ChassisCoord_WaitStop();
		IdentifyColor_action();//识别物料动作
		//=============识别颜色并取料=============
		HAL_Delay(1300);
		IdentifyColor_Get(0);
		Rest_action_S2();
		HAL_Delay(2000);
		//===============一号转角区===============
		ChassisCoord_Set(1900, 200, 90);
		ChassisCoord_WaitStop();
		//=============移动到粗加工区=============→→→→→→→→→→→→→→→→→→→→→→→→→→→粗
		ChassisCoord_Set(1925,1068, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//识别圆环机械臂动作
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//视觉识别圆心
		SetCoordinateXY(1925,1068);//位置重标点
		//==========粗加工区进行放料取料==========
		RoughingArea_PutGet(0);
		Rest_action_S2();
		//===============二号转角区===============
		ChassisCoord_Set(1850, 1750, 90);
		ChassisCoord_WaitStop();
		//=============移动到精加工区===============→→→→→→→→→→→→→→→→→→→→→→→→→精
		ChassisCoord_Set(1067,1815, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//识别圆环机械臂动作
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//视觉识别圆心
		SetCoordinateXY(1067,1815);//位置重标点
		//==========精加工区进行下层放料==========
		FashioningArea_Put(0);
		HAL_Delay(500);
		Rest_action_S2();
		HAL_Delay(2000);
		//===============二号转角区===============
		ChassisCoord_Set(1850, 1750, 180);
		ChassisCoord_WaitStop();
		//===============一号转角区===============
		ChassisCoord_Set(1900, 200, 0);
		ChassisCoord_WaitStop();
		//==============前进到原料区==============
		ChassisCoord_Set(1480, 190, 0);
		ChassisCoord_WaitStop();
		IdentifyColor_action();//识别物料动作
//****************************************************************************第二趟********************************
		//=============识别颜色并取料=============
		HAL_Delay(1800);
		IdentifyColor_Get(1);
		Rest_action_S2();
		//===============一号转角区===============
		ChassisCoord_Set(1900, 200, 90);
		ChassisCoord_WaitStop();
		//=============移动到粗加工区=============→→→→→→→→→→→→→→→→→→→→→→→→→→粗
		ChassisCoord_Set(1925,1068, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//识别圆环机械臂动作
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//视觉识别圆心
		SetCoordinateXY(1925,1068);//位置重标点
		//==========粗加工区进行放料取料==========
		RoughingArea_PutGet(1);
		Rest_action_S2();
		//===============二号转角区===============
		ChassisCoord_Set(1850, 1750, 90);
		ChassisCoord_WaitStop();
		//=============移动到精加工区===============→→→→→→→→→→→→→→→→→→→→→→→→精
		ChassisCoord_Set(1067,1815, 0);
		ChassisCoord_WaitStop();
		IdentifyCircle_action();//识别圆环机械臂动作
		HAL_Delay(2500);
		IdentifyCircle_Get(156,110);//视觉识别圆心
		SetCoordinateXY(1067,1815);//位置重标点
		//==========精加工区进行物料码垛==========
		FashioningArea_Put(1);
		HAL_Delay(500);
		Rest_action_S2();
		//===============三号转角区===============
		ChassisCoord_Set(150, 1800, 0);
		ChassisCoord_WaitStop();
		//===============返回区=============
		ChassisCoord_Set(200, 100, 0);
		ChassisCoord_WaitStop();
		ChassisSpeed_Set(100,50);//速度模式0
		HAL_Delay(1000);
		ChassisSpeed_Set(0,50);//速度模式0
		HAL_Delay(1300);
		ChassisSpeed_Set(0,0);// 停止

		#endif	
		
		//*/
		Debug_Await();// 等待下一次任务
	}
}
int Task_User_create(void)
{
	thread_PathWrite = rt_thread_create( "PathWrite",             /* 线程名字 */
								         PathWrite_task,          /* 线程入口函数 */
								         RT_NULL,                 /* 线程入口函数参数 */
								         1024,          		      /* 线程栈大小 */
								         10,                      /* 线程的优先级 */
								         20);                     /* 线程时间片 */
	if(thread_PathWrite != RT_NULL)
	{
		rt_thread_startup(thread_PathWrite);
		rt_kprintf("thread_PathWrite startup!\n");
	}
	
	return 0;
}
