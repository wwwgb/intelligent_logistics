#ifndef __DATA_H
#define __DATA_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* Includes ------------------------------------------------------------------*/
#include "mytype.h"
#include "bsp.h"
#include "RecognitionModule.h"
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
#ifdef __SLAVE_DCMotorMiniwatt_H__
#define DCMotorMiniwatt1_S DCMotorMiniwatt_S[0]
#define DCMotorMiniwatt2_S DCMotorMiniwatt_S[1]
#define DCMotorMiniwatt3_S DCMotorMiniwatt_S[2]
#define DCMotorMiniwatt4_S DCMotorMiniwatt_S[3]
#endif

#define MAX_WHEEL_RPM               (200.0f)
#define RC_CHASSIS_MAX_SPEED_X      (1300.0f)     //X轴方向最大速度(mm/s)
#define RC_CHASSIS_MAX_SPEED_Y      (1300.0f)     //Y轴方向最大速度(mm/s)
#define RC_CHASSIS_MAX_SPEED_R      (300.0f)      //旋转最大速度(deg/s)
#define RC_GIMBAL_MOVE_RATIO_YAW    (0.12f)       //yaw移动比例
/******************************************************************************
 *              电机旋转方向设置                                             *
 ******************************************************************************/
#define FWD                         (0) //正转
#define REV                         (1) //反转
#define DCMOTOR1_DIR                REV
#define DCMOTOR2_DIR                FWD
#define DCMOTOR3_DIR                FWD
#define DCMOTOR4_DIR                REV

#define Recognition_RX_LEN  	20
//#define Location_RX_LEN  	    10
/* Exported types ------------------------------------------------------------*/
typedef struct 
{
	int32_t x;
	int32_t y;
}RM_cartesian;
typedef struct 
{
	uint8_t  mark;
	int16_t stcAcc[3];  //加速度
	int16_t stcGyro[3]; //角速度
	int16_t stcAngle[3];//角度
	int16_t  stcTemp;
	float temp;
	float ax;
	float ay;
	float az;
	float wx;
	float wy;
	float wz;
	float Roll;
	float Pitch;
	float Yaw;
}GyroData_Typedef;
typedef struct
{
	__IO struct
    {
        float vx;
        float vy;
        float vyaw;
		float vpitch;
    } sPosition;
}_s_RemoteCtrl_Info;
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/*******直流电机驱动模块数据声明*******/
#ifdef __SLAVE_DCMotorMiniwatt_H__
extern DCMotorMiniwattDef_t DCMotorMiniwatt_S[4];
#endif
/*******陀螺仪数据声明*******/
extern GyroData_Typedef GyroData_Struct;
/*******二维码颜色模块数据声明*******/
extern RecognitionModule_s RecognitionModule_t;
/*******二维码颜色、圆位置模块数据声明*******/
extern RecognitionModule_s PCModule_t;
extern RM_cartesian cartesian_loc;
/*******颜色缓存数据声明*******/
extern uint8_t bool_recognitionflag; 
extern uint8_t Recognition_Buffer[Recognition_RX_LEN];
extern uint16_t ColorSequence[2][3];
extern uint8_t bool_colorsequenceflag;
extern uint16_t CurrentMaterialColor;
/*******遥控数据数据声明*******/
extern _s_RemoteCtrl_Info sRemoteCtrl_Info;
/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
