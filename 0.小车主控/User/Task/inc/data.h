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
#define RC_CHASSIS_MAX_SPEED_X      (1300.0f)     //X�᷽������ٶ�(mm/s)
#define RC_CHASSIS_MAX_SPEED_Y      (1300.0f)     //Y�᷽������ٶ�(mm/s)
#define RC_CHASSIS_MAX_SPEED_R      (300.0f)      //��ת����ٶ�(deg/s)
#define RC_GIMBAL_MOVE_RATIO_YAW    (0.12f)       //yaw�ƶ�����
/******************************************************************************
 *              �����ת��������                                             *
 ******************************************************************************/
#define FWD                         (0) //��ת
#define REV                         (1) //��ת
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
	int16_t stcAcc[3];  //���ٶ�
	int16_t stcGyro[3]; //���ٶ�
	int16_t stcAngle[3];//�Ƕ�
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
/*******ֱ���������ģ����������*******/
#ifdef __SLAVE_DCMotorMiniwatt_H__
extern DCMotorMiniwattDef_t DCMotorMiniwatt_S[4];
#endif
/*******��������������*******/
extern GyroData_Typedef GyroData_Struct;
/*******��ά����ɫģ����������*******/
extern RecognitionModule_s RecognitionModule_t;
/*******��ά����ɫ��Բλ��ģ����������*******/
extern RecognitionModule_s PCModule_t;
extern RM_cartesian cartesian_loc;
/*******��ɫ������������*******/
extern uint8_t bool_recognitionflag; 
extern uint8_t Recognition_Buffer[Recognition_RX_LEN];
extern uint16_t ColorSequence[2][3];
extern uint8_t bool_colorsequenceflag;
extern uint16_t CurrentMaterialColor;
/*******ң��������������*******/
extern _s_RemoteCtrl_Info sRemoteCtrl_Info;
/* Exported functions --------------------------------------------------------*/


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
