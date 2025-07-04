/* Includes ------------------------------------------------------------------*/
#include "thread_comm.h"
#include "thread_rccu.h"
#include "bsp.h"
#include "data.h"
#include "chassis_function.h"
#include "pid.h"
#include "Location_Tracker.h"
#include "thread_user.h"
/* Private macros ------------------------------------------------------------*/
//����Ƶ��_hz
#define CONTROL_FREQ_HZ			  (100)
//YAW PID����
#define De_YAW_KP                 5.0f
#define De_YAW_KI                 0.0f
#define De_YAW_KD                 5.0f
//X PID����
#define De_LOCX_KP                2.8f
#define De_LOCX_KI                0.0f
#define De_LOCX_KD                5.0f
//Y PID����
#define De_LOCY_KP                2.8f
#define De_LOCY_KI                0.0f
#define De_LOCY_KD                5.0f
/* Private types -------------------------------------------------------------*/
typedef enum
{
    CHASSIS_RELAX = 0,          //��ȫģʽ
    CHASSIS_STOP,               //����ֹͣ
    CHASSIS_NORMAL,             //��������ģʽ���������ǣ�
		CHASSIS_COORD,              //��������ģʽ
} ChassisCtrlMode_TypeDef;
typedef struct
{
	//ӲĿ��λ�ú��ٶ�
	float		goal_yaw;
	float		goal_x;
	float		goal_y;
	//��Ŀ��λ�ú��ٶ�
	float		soft_yaw;
	float		soft_x;
	float		soft_y;
	
	Location_Tracker_Typedef Yaw_Tracker_Struct;
	Location_Tracker_Typedef X_Tracker_Struct;
	Location_Tracker_Typedef Y_Tracker_Struct;
}ChassisCoord_CtrlTypeDef;
typedef struct
{
	ChassisCtrlMode_TypeDef mode_order;
	ChassisCtrlMode_TypeDef mode_run;
	float Gyro_YawAngle_zero;
	float Gyro_YawAngle_Calc;
	float Gyro_YawAngle_Chassis;
	float Gyro_YawAngle_Coord;
	ChassisHandle_TypeDef chassis_struct;
	ChassisCoord_CtrlTypeDef ChassisCoord_CtrlStruct;
	pid_t YawAngle_pid;
	pid_t LocationX_pid;
	pid_t LocationY_pid;
	volatile float *qGyro_YawAngle_New;
}RCCUStruct_TypeDef;
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* �����߳̿��ƿ�ָ�� */
rt_thread_t thread_rccu = RT_NULL;
/* ���̿��� */
RCCUStruct_TypeDef rccu_struct;
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  ���ģʽ����
  * @param  _mode
  * @retval NULL
**/
static void rccu_setmode( RCCUStruct_TypeDef *rccu_handle, ChassisCtrlMode_TypeDef _mode )
{
	rccu_handle->mode_order = _mode;
}
static float Read_RealYawAngle( RCCUStruct_TypeDef* rccu_handle )
{
	return (*rccu_handle->qGyro_YawAngle_New);
}
static float Read_GyroYawAngleCalc( RCCUStruct_TypeDef* rccu_handle )
{//��ȡ������ֵ
	#ifdef EncodingWheel
		return zangle;
  #else
		float YawAngleCalc;
		YawAngleCalc = Read_RealYawAngle(rccu_handle) - rccu_handle->Gyro_YawAngle_zero;
		if(YawAngleCalc < 0)
			YawAngleCalc += 360;
		if(YawAngleCalc > 360)
			YawAngleCalc -= 360;
		return YawAngleCalc;
	#endif
}
static void rccu_chassisctrl( RCCUStruct_TypeDef* rccu_handle )
{//С�����̿���
	int16_t speed1buff;
    int16_t speed2buff;
    int16_t speed3buff;
    int16_t speed4buff;
	
	int32_t distance11buff;
	int32_t distance12buff;
	int32_t distance13buff;
	int32_t distance14buff;
	
	if(rccu_struct.chassis_struct.Chassis_CtrlFunc != 0)
		rccu_struct.chassis_struct.Chassis_CtrlFunc(&rccu_struct.chassis_struct);
    /**************************�����ٶ�����������*******************************/	
	speed1buff = rccu_handle->chassis_struct.wheel_rpm[0];
	speed2buff = rccu_handle->chassis_struct.wheel_rpm[1];
	speed3buff = rccu_handle->chassis_struct.wheel_rpm[2];
	speed4buff = rccu_handle->chassis_struct.wheel_rpm[3];
#if (DCMOTOR1_DIR == REV)
	 speed1buff = -speed1buff;
#endif
#if (DCMOTOR2_DIR == REV)
	 speed2buff = -speed2buff;
#endif
#if (DCMOTOR3_DIR == REV)
	 speed3buff = -speed3buff;
#endif
#if (DCMOTOR4_DIR == REV)
	 speed4buff = -speed4buff;
#endif
#if 1
	SLAVE_DCMotorMiniwatt_SpeedSet(&DCMotorMiniwatt1_S,speed1buff);
	SLAVE_DCMotorMiniwatt_SpeedSet(&DCMotorMiniwatt2_S,speed2buff);
	SLAVE_DCMotorMiniwatt_SpeedSet(&DCMotorMiniwatt3_S,speed3buff);
	SLAVE_DCMotorMiniwatt_SpeedSet(&DCMotorMiniwatt4_S,speed4buff);
#endif
	/**************************������ݼ��㵽������*******************************/
	rccu_handle->chassis_struct.yaw_gyro_angle = rccu_handle->Gyro_YawAngle_Calc;
	speed1buff = SLAVE_DCMotorMiniwatt_SpeedRead(&DCMotorMiniwatt1_S);
	speed2buff = SLAVE_DCMotorMiniwatt_SpeedRead(&DCMotorMiniwatt2_S);
	speed3buff = SLAVE_DCMotorMiniwatt_SpeedRead(&DCMotorMiniwatt3_S);
	speed4buff = SLAVE_DCMotorMiniwatt_SpeedRead(&DCMotorMiniwatt4_S);
	distance11buff = SLAVE_DCMotorMiniwatt_PlaceRead(&DCMotorMiniwatt1_S);
	distance12buff = SLAVE_DCMotorMiniwatt_PlaceRead(&DCMotorMiniwatt2_S);
	distance13buff = SLAVE_DCMotorMiniwatt_PlaceRead(&DCMotorMiniwatt3_S);
	distance14buff = SLAVE_DCMotorMiniwatt_PlaceRead(&DCMotorMiniwatt4_S);
	#if (DCMOTOR1_DIR == REV)
	 speed1buff = -speed1buff;
	 distance11buff = -distance11buff;
	#endif
	#if (DCMOTOR2_DIR == REV)
		 speed2buff = -speed2buff;
		 distance12buff = -distance12buff;
	#endif
	#if (DCMOTOR3_DIR == REV)
		 speed3buff = -speed3buff;
		 distance13buff = -distance13buff;
	#endif
	#if (DCMOTOR4_DIR == REV)
		 speed4buff = -speed4buff;
		 distance14buff = -distance14buff;
	#endif
	rccu_handle->chassis_struct.Chassis_Calc_ForwardTransform( &rccu_handle->chassis_struct,
															   speed1buff,
															   speed2buff,
															   speed3buff,
															   speed4buff,
															   distance11buff,	
															   distance12buff,
															   distance13buff,
															   distance14buff );
}
static void rccu_chassisctrl_normalmode( RCCUStruct_TypeDef* rccu_handle )
{//�������ǵ�����ģʽ(�޼������)
	float YawAngle_Diff;

	YawAngle_Diff = rccu_handle->Gyro_YawAngle_Calc - rccu_handle->ChassisCoord_CtrlStruct.soft_yaw;
	while(YawAngle_Diff < -180)
		YawAngle_Diff += 360;
	while(YawAngle_Diff > 180)
		YawAngle_Diff -= 360;
	
	rccu_handle->chassis_struct.vw = rccu_handle->YawAngle_pid.f_pid_calc( &rccu_handle->YawAngle_pid,YawAngle_Diff,0 );
	rccu_handle->chassis_struct.gimbal_yaw_ecd_angle = 0;
	rccu_handle->chassis_struct.vx = sRemoteCtrl_Info.sPosition.vx;
	rccu_handle->chassis_struct.vy = sRemoteCtrl_Info.sPosition.vy;
}
static void rccu_chassisctrl_coordmode( RCCUStruct_TypeDef* rccu_handle )
{//�������ǵ�����ģʽ(�������)
	float YawAngle_Diff;

	YawAngle_Diff = rccu_handle->Gyro_YawAngle_Calc - rccu_handle->ChassisCoord_CtrlStruct.soft_yaw;
	while(YawAngle_Diff < -180)
		YawAngle_Diff += 360;
	while(YawAngle_Diff > 180)
		YawAngle_Diff -= 360;
	
	rccu_handle->chassis_struct.vw = rccu_handle->YawAngle_pid.f_pid_calc( &rccu_handle->YawAngle_pid,YawAngle_Diff,0 );
	rccu_handle->chassis_struct.vx = rccu_handle->LocationX_pid.f_pid_calc( &rccu_handle->LocationX_pid, \
	                                                                        rccu_handle->chassis_struct.position.position_x_mm, \
	                                                                        rccu_handle->ChassisCoord_CtrlStruct.soft_x );
	
	rccu_handle->chassis_struct.vy = rccu_handle->LocationY_pid.f_pid_calc( &rccu_handle->LocationY_pid, \
	                                                                        rccu_handle->chassis_struct.position.position_y_mm, \
	                                                                        rccu_handle->ChassisCoord_CtrlStruct.soft_y );
	
	/************************************�������********************************************/
	YawAngle_Diff = rccu_handle->Gyro_YawAngle_Coord - rccu_handle->Gyro_YawAngle_Calc;  
	if(YawAngle_Diff < 0)
		YawAngle_Diff += 360;
	if(YawAngle_Diff > 360)
		YawAngle_Diff -= 360;
	rccu_handle->chassis_struct.gimbal_yaw_ecd_angle = YawAngle_Diff;
}
/**
  * @brief  ������Ƴ�ʼ��
  * @param  NULL
  * @retval NULL
**/
static void rccu_init( RCCUStruct_TypeDef* rccu_handle )
{
	memset(rccu_handle, 0, sizeof(RCCUStruct_TypeDef));
	rccu_setmode( rccu_handle, CHASSIS_RELAX );
//#######################################################################################################
	#ifdef EncodingWheel
		rccu_handle->qGyro_YawAngle_New = &zangle;//ʵʱ��ȡ�����ǽǶ�
    #else
		rccu_handle->qGyro_YawAngle_New = &GyroData_Struct.Yaw;
	#endif
//#######################################################################################################
	Chassis_Init( &rccu_handle->chassis_struct,
	              FOUR_DRIVE_McNamara,
								RC_CHASSIS_MAX_SPEED_X,   //���X���ٶ�
								RC_CHASSIS_MAX_SPEED_Y,   //���Y���ٶ�
								RC_CHASSIS_MAX_SPEED_R,   //���������ת���ٶ�
								MAX_WHEEL_RPM );          //��������ٶ�
	//YAW PID
	PID_struct_init(&rccu_handle->YawAngle_pid, POSITION_PID, RC_CHASSIS_MAX_SPEED_R, 50.0f,De_YAW_KP, De_YAW_KI, De_YAW_KD);
	rccu_handle->YawAngle_pid.output_deadband = 0;
	//X PID
	PID_struct_init(&rccu_handle->LocationX_pid, POSITION_PID, RC_CHASSIS_MAX_SPEED_X, 50.0f,De_LOCX_KP, De_LOCX_KI, De_LOCX_KD);
	rccu_handle->LocationX_pid.output_deadband = 0;
	//Y PID
	PID_struct_init(&rccu_handle->LocationY_pid, POSITION_PID, RC_CHASSIS_MAX_SPEED_Y, 50.0f,De_LOCY_KP, De_LOCY_KI, De_LOCY_KD);
	rccu_handle->LocationY_pid.output_deadband = 0;
	//YAW ������
	Location_Tracker_Init( &rccu_handle->ChassisCoord_CtrlStruct.Yaw_Tracker_Struct,
													CONTROL_FREQ_HZ,//CtrlFreqHZ
													RC_CHASSIS_MAX_SPEED_R,//max_speed
													RC_CHASSIS_MAX_SPEED_R*2,//up_acc	Ĭ��*2/3
													RC_CHASSIS_MAX_SPEED_R*2,//down_acc Ĭ��*2/3
													RC_CHASSIS_MAX_SPEED_R );//speed_locking_stop
	//X ������
	Location_Tracker_Init( &rccu_handle->ChassisCoord_CtrlStruct.X_Tracker_Struct,
													CONTROL_FREQ_HZ,//CtrlFreqHZ
													RC_CHASSIS_MAX_SPEED_X,//max_speed
													RC_CHASSIS_MAX_SPEED_X/1,//up_acc	Ĭ��/4
													RC_CHASSIS_MAX_SPEED_X/1,//down_acc Ĭ��/4
													RC_CHASSIS_MAX_SPEED_X );//speed_locking_stop
	//Y ������
	Location_Tracker_Init( &rccu_handle->ChassisCoord_CtrlStruct.Y_Tracker_Struct,
													CONTROL_FREQ_HZ,
													RC_CHASSIS_MAX_SPEED_Y,//max_speed
													RC_CHASSIS_MAX_SPEED_Y/1,//up_acc	Ĭ��/4
													RC_CHASSIS_MAX_SPEED_Y/1,//down_acc Ĭ��/4
													RC_CHASSIS_MAX_SPEED_Y );//speed_locking_stop
}
/* Exported macros -----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void ChassisModle_Set(int model)
{
		switch(model)
	{
		case 1:
		rccu_struct.mode_order = CHASSIS_RELAX;//������
		break;
		case 2:
		rccu_struct.mode_order = CHASSIS_COORD;//x��y��yaw���Խ���
		break;
		case 3:
		rccu_struct.mode_order = CHASSIS_NORMAL;//yaw���Խ���
		break;
	}
}
void ChassisCoord_Set(float _x_diff, float _y_diff, float _yaw_diff)
{
	if( rccu_struct.mode_order != CHASSIS_COORD )
	{
		rccu_struct.mode_order = CHASSIS_COORD;
		My_mDelay(100);
	}
	#ifdef EncodingWheel//X Y����λ������	,Z���λ������
			rccu_struct.ChassisCoord_CtrlStruct.goal_y = _y_diff;
			rccu_struct.ChassisCoord_CtrlStruct.goal_x = _x_diff;
			rccu_struct.ChassisCoord_CtrlStruct.goal_yaw += _yaw_diff;
  #else
			rccu_struct.ChassisCoord_CtrlStruct.goal_y += _y_diff;
			rccu_struct.ChassisCoord_CtrlStruct.goal_x += _x_diff;
			rccu_struct.ChassisCoord_CtrlStruct.goal_yaw += _yaw_diff;
	#endif
#if 0	
	while( ( rccu_struct.ChassisCoord_CtrlStruct.goal_yaw < 0 ) || \
		   ( rccu_struct.ChassisCoord_CtrlStruct.goal_yaw > 360 ) )
	{
		if(rccu_struct.ChassisCoord_CtrlStruct.goal_yaw < 0)
			rccu_struct.ChassisCoord_CtrlStruct.goal_yaw += 360;
		if(rccu_struct.ChassisCoord_CtrlStruct.goal_yaw >= 360)
			rccu_struct.ChassisCoord_CtrlStruct.goal_yaw -= 360;
	}
#endif
}
void ChassisCoord_WaitStop(void)
{
	while( ( rccu_struct.ChassisCoord_CtrlStruct.soft_y != rccu_struct.ChassisCoord_CtrlStruct.goal_y ) || \
		   ( ABS(rccu_struct.LocationY_pid.err[NOW]) > 20 ) || \
		   ( rccu_struct.ChassisCoord_CtrlStruct.soft_x != rccu_struct.ChassisCoord_CtrlStruct.goal_x ) || \
		   ( ABS(rccu_struct.LocationX_pid.err[NOW]) > 20 ) || \
		   ( ABS(rccu_struct.ChassisCoord_CtrlStruct.soft_yaw - rccu_struct.ChassisCoord_CtrlStruct.goal_yaw) > 0.5f) || \
		   ( ABS(rccu_struct.YawAngle_pid.err[NOW]) > 0.2f ) )
	{
		My_mDelay(10);
	}
}
void ChassisSpeed_Set(float _x_spd, float _y_spd)
{
	if( rccu_struct.mode_order != CHASSIS_NORMAL )
	{
		rccu_struct.mode_order = CHASSIS_NORMAL;
		My_mDelay(100);
	}
	sRemoteCtrl_Info.sPosition.vx = _x_spd;
	sRemoteCtrl_Info.sPosition.vy = _y_spd;
}
void ChassisRELAX_set(void)
{
	if( rccu_struct.mode_order != CHASSIS_RELAX )
	{
		rccu_struct.mode_order = CHASSIS_RELAX;
		My_mDelay(100);
	}
}
int32_t Read_Position_x_mm(void)
{
	return rccu_struct.chassis_struct.position.position_x_mm;
}
int32_t Read_Position_y_mm(void)
{
	return rccu_struct.chassis_struct.position.position_y_mm;
}
void rccu_task(void *pvParameters)
{
	//��ʱ�ȴ������ȶ�
	My_mDelay(1000);
	//��ȡ��ǰ����
	rccu_struct.Gyro_YawAngle_zero = Read_RealYawAngle(&rccu_struct);
	rccu_struct.Gyro_YawAngle_Calc = Read_GyroYawAngleCalc(&rccu_struct);
	rccu_struct.Gyro_YawAngle_Chassis = rccu_struct.Gyro_YawAngle_Calc;
	rccu_struct.Gyro_YawAngle_Coord = rccu_struct.Gyro_YawAngle_Calc;
	
	rccu_setmode( &rccu_struct, CHASSIS_RELAX );//��ʼģʽ
	Task_User_create();
	while(1)
    {
		/************************ ���ݲɼ� ************************************/
		/************************ ���ݲɼ� ************************************/
		rccu_struct.Gyro_YawAngle_Calc = Read_GyroYawAngleCalc(&rccu_struct);
		/************************ �˶����� ************************************/
		/************************ �˶����� ************************************/
		switch ( rccu_struct.mode_run )
		{
			case CHASSIS_RELAX://����ģʽ
			case CHASSIS_STOP:
			{//ֹͣ
				rccu_struct.chassis_struct.vx = 0;
				rccu_struct.chassis_struct.vy = 0;
				rccu_struct.chassis_struct.vw = 0;
			}break;
			case CHASSIS_NORMAL:
			{
				rccu_chassisctrl_normalmode(&rccu_struct);
			}break;
			case CHASSIS_COORD:
			{//����ģʽ
				rccu_chassisctrl_coordmode(&rccu_struct);
			}break;
			default:
				break;
		}
		rccu_chassisctrl( &rccu_struct );
		/************************ ģʽ��� ************************************/
		/************************ ģʽ��� ************************************/
		if(rccu_struct.mode_run != rccu_struct.mode_order)
	    {//���
			rccu_struct.mode_run = rccu_struct.mode_order;
			switch (rccu_struct.mode_run)
			{
				case CHASSIS_RELAX:
				case CHASSIS_STOP:
				case CHASSIS_NORMAL:  //ң��
					rccu_struct.Gyro_YawAngle_Chassis = rccu_struct.Gyro_YawAngle_Calc;
					break;
				case CHASSIS_COORD:   //����ģʽ
						rccu_struct.ChassisCoord_CtrlStruct.soft_yaw = rccu_struct.Gyro_YawAngle_Calc;
				    rccu_struct.ChassisCoord_CtrlStruct.goal_yaw = rccu_struct.ChassisCoord_CtrlStruct.soft_yaw;
				    Location_Tracker_NewTask( &rccu_struct.ChassisCoord_CtrlStruct.Yaw_Tracker_Struct, \
	                                          rccu_struct.ChassisCoord_CtrlStruct.soft_yaw, \
                                              0 );
				    rccu_struct.ChassisCoord_CtrlStruct.soft_x = rccu_struct.chassis_struct.position.position_x_mm;
				    rccu_struct.ChassisCoord_CtrlStruct.goal_x = rccu_struct.ChassisCoord_CtrlStruct.soft_x;
				    Location_Tracker_NewTask( &rccu_struct.ChassisCoord_CtrlStruct.X_Tracker_Struct, \
	                                          rccu_struct.ChassisCoord_CtrlStruct.soft_x, \
                                              0 );
				    rccu_struct.ChassisCoord_CtrlStruct.soft_y = rccu_struct.chassis_struct.position.position_y_mm;
				    rccu_struct.ChassisCoord_CtrlStruct.goal_y = rccu_struct.ChassisCoord_CtrlStruct.soft_y;
				    Location_Tracker_NewTask( &rccu_struct.ChassisCoord_CtrlStruct.Y_Tracker_Struct, \
	                                          rccu_struct.ChassisCoord_CtrlStruct.soft_y, \
                                              0 );
					break;
				default:
					break;
			}
		}
		/************************ ��Ŀ����ȡ **********************************/
		/************************ ��Ŀ����ȡ **********************************/
		switch (rccu_struct.mode_run)
		{
			case CHASSIS_COORD://����ģʽ
				Location_Tracker_Capture_Goal( &rccu_struct.ChassisCoord_CtrlStruct.Yaw_Tracker_Struct, \
			                                   rccu_struct.ChassisCoord_CtrlStruct.goal_yaw );
	            if(ABS(rccu_struct.ChassisCoord_CtrlStruct.soft_yaw-rccu_struct.ChassisCoord_CtrlStruct.goal_yaw) < 1.0f)
					rccu_struct.ChassisCoord_CtrlStruct.soft_yaw = rccu_struct.ChassisCoord_CtrlStruct.goal_yaw;
				else
			        rccu_struct.ChassisCoord_CtrlStruct.soft_yaw = rccu_struct.ChassisCoord_CtrlStruct.Yaw_Tracker_Struct.go_location;
			    Location_Tracker_Capture_Goal( &rccu_struct.ChassisCoord_CtrlStruct.X_Tracker_Struct, \
			                                   rccu_struct.ChassisCoord_CtrlStruct.goal_x );
	            rccu_struct.ChassisCoord_CtrlStruct.soft_x = rccu_struct.ChassisCoord_CtrlStruct.X_Tracker_Struct.go_location;
			
			    Location_Tracker_Capture_Goal( &rccu_struct.ChassisCoord_CtrlStruct.Y_Tracker_Struct, \
			                                   rccu_struct.ChassisCoord_CtrlStruct.goal_y );
	            rccu_struct.ChassisCoord_CtrlStruct.soft_y = rccu_struct.ChassisCoord_CtrlStruct.Y_Tracker_Struct.go_location;
			    break;
			default:
				break;
		}
		My_mDelay( 10 );
    }
}
int Task_RCCU_create(void)
{
	rccu_init( &rccu_struct );
	
    thread_rccu = rt_thread_create("rccu",            /* �߳����� */
								   rccu_task,         /* �߳���ں��� */
								   RT_NULL,           /* �߳���ں������� */
								   1024,              /* �߳�ջ��С */
								   2,                 /* �̵߳����ȼ� */
								   20);               /* �߳�ʱ��Ƭ */
	if(thread_rccu != RT_NULL)
	{
		rt_thread_startup(thread_rccu);
		rt_kprintf("thread_rccu startup!\n");
	}
	return 0;
}
INIT_APP_EXPORT(Task_RCCU_create);
