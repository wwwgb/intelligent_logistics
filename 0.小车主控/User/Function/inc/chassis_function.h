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
//定义 NULL
#ifndef NULL
#define NULL 0
#endif
//定义PI 值
#ifndef PI
#define PI 3.14159265358979f
#endif
//定义 角度(度)转换到 弧度的比例
#ifndef ANGLE_TO_RAD
#define ANGLE_TO_RAD 0.01745329251994329576923690768489f
#endif
//定义 弧度 转换到 角度的比例
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
 *              机械安装参数                                                 *
 ******************************************************************************/
#define WHEEL_RADIUS                (38.1f)          //轮子半径(mm)
#define WHEEL_PERIMETER             (239.3893602035f) //轮子周长(mm)
#define WHEELTRACK                  (226.4f)            //轮距(mm)253
#define WHEELBASE                   (190)            //轴距(mm)205
#define GIMBAL_X_OFFSET             (0)              //云台相对底盘中心X轴偏移
#define GIMBAL_Y_OFFSET             (0)              //云台相对底盘中心Y轴偏移
/******************************************************************************
 *              电机参数                                                     *
 ******************************************************************************/  
#define MOTOR_REDUCTION_RATIO       (1.0f)         //电机减速比
#define MOTOR_ENCODER_ACCURACY      (420)          //电机编码器分辨率
/* Exported types ------------------------------------------------------------*/
//重新定义数据类型
typedef unsigned char bool_t;
typedef float         fp32;
typedef double        fp64;
typedef enum
{
	DEBUG_COMMONWHEEL = 0,             //调试模式
//	FOUR_DRIVE_COMMONWHEEL,            //4个普通胶轮模式
	FOUR_DRIVE_McNamara,               //麦克纳姆轮
//	FOUR_DRIVE_OMNIDIRECTIONALWHEEL,   //四轮45度安装式全向轮 
//	THREE_DRIVE_OMNIDIRECTIONALWHEEL,  //三式全向轮   
//	TWO_DRIVE_COMMONWHEEL,             //2个普通胶轮模式
//	FOUR_DRIVE_OMNIDIRECTIONALWHEEL_90,//四轮90度安装式全向轮
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
	
	fp32 rotate_ratio_fr; //右前轮子比例值
	fp32 rotate_ratio_fl; //左前轮子比例值
	fp32 rotate_ratio_bl; //左后轮子比例值
	fp32 rotate_ratio_br; //右后轮子比例值
	fp32 wheel_rpm_ratio; //轮子转速比例值
	fp32 ecd_ratio;       //编码器比例值
	volatile fp32 vy;     //小车设定Y轴速度（遥控值）
	volatile fp32 vx;     //小车设定X轴速度（遥控值）
	volatile fp32 vw;     //小车设定Yaw轴速度（遥控值）
	fp32 sin_yaw; 
	fp32 cos_yaw;
	fp32 gimbal_yaw_ecd_angle;//底盘与运行方向夹角
	fp32 yaw_gyro_angle;
	fp32 chassis_vy;//小车设定Y轴速度
	fp32 chassis_vx;//小车设定X轴速度 
	fp32 wheel_rpm[4];//电机执行转速
	
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
								 fp32 MAX_Chassis_VX_Speed,   //最大X轴速度
								 fp32 MAX_Chassis_VY_Speed,   //最大Y轴速度
								 fp32 MAX_Chassis_VW_Speed,   //最大中心旋转轴速度
								 fp32 MAX_Wheel_rpm );        //轮子最大速度

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
