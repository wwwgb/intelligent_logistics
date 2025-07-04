#ifndef _CHASSIS_FUNCTION_H_
#define _CHASSIS_FUNCTION_H_
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
/* Private macros ------------------------------------------------------------*/
/* Private types -------------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
//���� NULL
#ifndef NULL
#define NULL 0
#endif
//����PI ֵ
#ifndef PI
#define PI 3.14159265358979f
#endif
//���� �Ƕ�(��)ת���� ���ȵı���
#ifndef ANGLE_TO_RAD
#define ANGLE_TO_RAD 0.01745329251994329576923690768489f
#endif
//���� ���� ת���� �Ƕȵı���
#ifndef RAD_TO_ANGLE
#define RAD_TO_ANGLE 57.295779513082320876798154814105f
#endif
#define VAL_LIMIT(val, min, max)    \
    do{                             \
        if( (val) <= (min) ){       \
	        (val) = (min);          \
        }else if( (val) >= (max) ){ \
	        (val) = (max);          \
        }                           \
    } while (0)
/******************************************************************************
 *              ��е��װ����                                                 *
 ******************************************************************************/
#define WHEEL_RADIUS                (38.1f)          //���Ӱ뾶(mm)
#define WHEEL_PERIMETER             (239.3893602035f) //�����ܳ�(mm)
#define WHEELTRACK                  (226.4f)            //�־�(mm)253
#define WHEELBASE                   (190)            //���(mm)205
#define GIMBAL_X_OFFSET             (0)              //��̨��Ե�������X��ƫ��
#define GIMBAL_Y_OFFSET             (0)              //��̨��Ե�������Y��ƫ��
/******************************************************************************
 *              �������                                                     *
 ******************************************************************************/  
#define MOTOR_REDUCTION_RATIO       (1.0f)         //������ٱ�
#define MOTOR_ENCODER_ACCURACY      (420)          //����������ֱ���
/* Exported types ------------------------------------------------------------*/
//���¶�����������
typedef unsigned char bool_t;
typedef float         fp32;
typedef double        fp64;
typedef enum
{
	DEBUG_COMMONWHEEL = 0,             //����ģʽ
//	FOUR_DRIVE_COMMONWHEEL,            //4����ͨ����ģʽ
	FOUR_DRIVE_McNamara,               //�����ķ��
//	FOUR_DRIVE_OMNIDIRECTIONALWHEEL,   //����45�Ȱ�װʽȫ���� 
//	THREE_DRIVE_OMNIDIRECTIONALWHEEL,  //��ʽȫ����   
//	TWO_DRIVE_COMMONWHEEL,             //2����ͨ����ģʽ
//	FOUR_DRIVE_OMNIDIRECTIONALWHEEL_90,//����90�Ȱ�װʽȫ����
}ChassisType_TypeDef;
typedef struct
{
    fp32 wheel_perimeter; /* the perimeter(mm) of wheel */
    fp32 wheeltrack;      /* wheel track distance(mm) */
    fp32 wheelbase;       /* wheelbase distance(mm) */
    fp32 rotate_x_offset; /* rotate offset(mm) relative to the x-axis of the chassis center */
    fp32 rotate_y_offset; /* rotate offset(mm) relative to the y-axis of the chassis center */
	fp32 motor_encoder_accuracy;
	fp32 MAX_Chassis_VX_Speed;
    fp32 MAX_Chassis_VY_Speed;
    fp32 MAX_Chassis_VW_Speed;
    fp32 MAX_Wheel_rpm;
} MechanicalStructure_TypeDef;
typedef struct ChassisHandle_TypeDef
{
	ChassisType_TypeDef ChassisType;
	
	struct{
		int16_t speed_rpm[4];
		int32_t total_ecd[4];
		int32_t position_x_mm;
		int32_t position_y_mm;
		int32_t angle;
		int32_t angle_deg;
		fp32 v_x_mm;
		fp32 v_y_mm;
		fp32 rate_deg;
	}position;
	
	MechanicalStructure_TypeDef   MechanicalStruct_Para;
	
	fp32 rotate_ratio_fr; //��ǰ���ӱ���ֵ
	fp32 rotate_ratio_fl; //��ǰ���ӱ���ֵ
	fp32 rotate_ratio_bl; //������ӱ���ֵ
	fp32 rotate_ratio_br; //�Һ����ӱ���ֵ
	fp32 wheel_rpm_ratio; //����ת�ٱ���ֵ
	fp32 ecd_ratio;       //����������ֵ
	volatile fp32 vy;     //С���趨Y���ٶȣ�ң��ֵ��
	volatile fp32 vx;     //С���趨X���ٶȣ�ң��ֵ��
	volatile fp32 vw;     //С���趨Yaw���ٶȣ�ң��ֵ��
	fp32 sin_yaw; 
	fp32 cos_yaw;
	fp32 gimbal_yaw_ecd_angle;//���������з���н�
	fp32 yaw_gyro_angle;
	fp32 chassis_vy;//С���趨Y���ٶ�
	fp32 chassis_vx;//С���趨X���ٶ� 
	fp32 wheel_rpm[4];//���ִ��ת��
	
	void (*Chassis_CtrlFunc)( struct ChassisHandle_TypeDef* chassis_handle );
	void (*Chassis_Calc_ForwardTransform)( struct ChassisHandle_TypeDef* chassis_handle, 
	                                       int16_t Wheel1Speed,	
								           int16_t Wheel2Speed,
									       int16_t Wheel3Speed,
									       int16_t Wheel4Speed,
									       int32_t Wheel1Distance, 
									       int32_t Wheel2Distance, 
									       int32_t Wheel3Distance,
									       int32_t Wheel4Distance );
} ChassisHandle_TypeDef;
/* Exported constants --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void Chassis_Init( ChassisHandle_TypeDef* chassis_handle,
	               ChassisType_TypeDef ChassisType,
								 fp32 MAX_Chassis_VX_Speed,   //���X���ٶ�
								 fp32 MAX_Chassis_VY_Speed,   //���Y���ٶ�
								 fp32 MAX_Chassis_VW_Speed,   //���������ת���ٶ�
								 fp32 MAX_Wheel_rpm );        //��������ٶ�

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
