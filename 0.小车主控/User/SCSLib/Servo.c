#include "data.h"
#include "stm32f4xx.h"
#include "SCServo.h"
#include "usart.h"
#include "Servo.h"
#include "thread_rccu.h"
typedef struct{
	int16_t Num1;//舵机ID
	int16_t Num2;
	int16_t Num3;
	int16_t Num4;
}Servo;
/* ------------------------------------------------------------*/
Servo bowl_1 		= {//1号碗
								784,
								2142,
								2070,//>>>>>2050
								};
Servo bowl_2 		= {//2号碗
								1020,
								2139,
								2083,//>>>>>2050
								};
Servo bowl_3 		= {//3号碗
								1268,
								2141,
								2072,//>>>>>2050
								};
/* ------------------------------------------------------------*/
Servo Red 		= {//红一层
								3580,
								2446,
								1466
								};
Servo Green 	= {//绿一层
								3039,
								2264,
								1317
								};
Servo Blue 		= {//蓝一层
								2544,
								2470,
								1498
								};
/* ------------------------------------------------------------*/
Servo Red_Stacking 		= {//红码垛
								3585,
								2237,
								1791	
								};
Servo Green_Stacking 	= {//绿码垛
								3028,
								1937,
								1773
								};
Servo Blue_Stacking 	= {//蓝码垛
								2546,
								2263,
								1808
								};
/* ------------------------------------------------------------*/
		void Rest_action()//归位动作
		{
			RegWritePosEx(2, 1802, 2250, 30);
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
		}
		void Rest_action_S2()//初始动作
		{
			RegWritePosEx(1, 1024, 2250, 30);
			RegWritePosEx(2, 2150, 2250, 30);
			RegWritePosEx(3, 2274, 2250, 30);
			RegWritePosEx(4, 2047, 2250, 30);
			RegWriteAction();
		}
		void IdentifyColor_action()//识别原料颜色动作
		{
			RegWritePosEx(1, 3071, 2250, 30);
			RegWritePosEx(2, 2047, 2250, 30);
			RegWritePosEx(3, 2047, 2250, 30);
			RegWritePosEx(4, 2047, 2250, 30);
			RegWriteAction();
		}
		void IdentifyCircle_action()//粗加工区识别圆环动作
		{
			RegWritePosEx(1, 3071, 2250, 30);
			RegWritePosEx(2, 1915, 2250, 30);
			RegWritePosEx(3, 2200, 2250, 30);//2154
			RegWritePosEx(4, 2047, 2250, 30);
			RegWriteAction();
		}
		void Get_A()//抓取原料区物料
		{
			RegWritePosEx(1, 3071, 4250, 60);
			RegWritePosEx(2, 2094, 4250, 60);
			RegWritePosEx(3, 1800, 4250, 60);
			RegWriteAction();
			HAL_Delay(100);
			WritePosEx(4, 1500, 4096, 100);//抓
			HAL_Delay(500);
			Rest_action();//归位
			HAL_Delay(500);
		}
		void Put_1()//放到1号碗（红）
		{
			WritePosEx(1, bowl_1.Num1, 2250, 30);//转到1号碗方向
			HAL_Delay(1800);
			RegWritePosEx(2, bowl_1.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_1.Num3, 1897, 30);//向下伸
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 2047, 4095, 100);//放
			HAL_Delay(500);
		}
		void Put_2()//放到2号碗（绿）
		{
			WritePosEx(1, bowl_2.Num1, 2250, 30);//转到2号碗方向
			HAL_Delay(1400);
			RegWritePosEx(2, bowl_2.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_2.Num3, 1897, 30);//向下伸
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 2047, 4096, 100);//放
			HAL_Delay(500);
		}
		void Put_3()//放到3号碗（蓝）
		{
			WritePosEx(1, bowl_3.Num1, 2250, 30);//转到3号碗方向
			HAL_Delay(800);
			RegWritePosEx(2, bowl_3.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_3.Num3, 1897, 30);//向下伸
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 2047, 4096, 100);//放
			HAL_Delay(1000);
		}
		void Get_1()//抓1号碗物料（红）
		{
			WritePosEx(4, 2047, 4096, 100);//放
			WritePosEx(1, bowl_1.Num1, 2250, 30);//转到1号碗方向
			HAL_Delay(500);
			RegWritePosEx(2, bowl_1.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(500);
			RegWritePosEx(3, bowl_1.Num3, 1897, 30);//向下伸
			RegWriteAction();
			HAL_Delay(400);
			WritePosEx(4, 1500, 4096, 100);//抓
			HAL_Delay(200);
			RegWritePosEx(3, 2274, 1897, 30);//向上伸
			RegWriteAction();
			HAL_Delay(300);
			Rest_action();//归位
			HAL_Delay(500);
		}
		void Get_2()//抓2号碗物料（绿）
		{
			WritePosEx(4, 2047, 4096, 100);//放
			WritePosEx(1, bowl_2.Num1, 2250, 30);//转到2号碗方向
			HAL_Delay(800);
			RegWritePosEx(2, bowl_2.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_2.Num3, 1897, 30);//向下伸
			RegWriteAction();
			HAL_Delay(300);
			WritePosEx(4, 1500, 4096, 100);//抓
			HAL_Delay(200);
			RegWritePosEx(3, 2274, 1897, 30);//向上伸
			RegWriteAction();
			HAL_Delay(300);
			Rest_action();//归位
			HAL_Delay(500);
		}
		void Get_3()//抓3号碗物料（蓝）
		{
			WritePosEx(4, 2047, 4096, 100);//放
			WritePosEx(1, bowl_3.Num1, 2250, 30);//转到3号碗方向
			HAL_Delay(800);
			RegWritePosEx(2, bowl_3.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, 2274, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			RegWritePosEx(3, bowl_3.Num3, 1897, 30);//向下伸
			RegWriteAction();
			HAL_Delay(300);
			WritePosEx(4, 1500, 4096, 100);//抓
			HAL_Delay(200);
			RegWritePosEx(3, 2274, 1897, 30);//向上伸
			RegWriteAction();
			HAL_Delay(300);
			Rest_action();//归位
			HAL_Delay(500);
		}
		void Put_Red()//放红环——第一层
		{
			WritePosEx(1, Red.Num1, 2250, 30);//转到红环方向
			HAL_Delay(1800);
			RegWritePosEx(2, Red.Num2, 2250, 30);//向前伸2445
			RegWritePosEx(3, Red.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(1500);
			WritePosEx(4, 2047, 4095, 50);//爪子松开
			HAL_Delay(800);//300
			Rest_action();
		}
		void Put_Green()//放绿环——第一层
		{
			WritePosEx(1, Green.Num1, 2250, 30);//转到绿环方向
			HAL_Delay(1400);
			WritePosEx(3, Green.Num3, 4250, 60);
			WritePosEx(2, Green.Num2, 2250, 30);//向前伸
			HAL_Delay(1500);
			RegWritePosEx(4, 2047, 0, 0);//爪子松开
			HAL_Delay(300);
			Rest_action();
		}
		void Put_Blue()//放蓝环——第一层
		{
			WritePosEx(1, Blue.Num1, 2250, 30);//转到蓝环方向
			HAL_Delay(800);
			RegWritePosEx(2, Blue.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, Blue.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(1500);
			RegWritePosEx(4, 2047, 0, 0);//爪子松开
			HAL_Delay(300);
			Rest_action();
		}
		void Put_Red_Stacking()//放红环——码垛
		{
			WritePosEx(1, Red_Stacking.Num1, 2250, 30);//转到红环方向
			HAL_Delay(1800);
			RegWritePosEx(2, Red_Stacking.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, Red_Stacking.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(1500);
			WritePosEx(4, 2047, 4095, 50);//爪子松开
			HAL_Delay(800);//300
			Rest_action();
		}
		void Put_Green_Stacking()//放绿环——码垛
		{
			WritePosEx(1, Green_Stacking.Num1, 2250, 30);//转到绿环方向
			HAL_Delay(1400);
			WritePosEx(3, Green_Stacking.Num3, 4250, 60);
			WritePosEx(2, Green_Stacking.Num2, 2250, 30);//向前伸
			HAL_Delay(1500);
			RegWritePosEx(4, 2047, 0, 0);//爪子松开
			HAL_Delay(300);
			Rest_action();
		}
		void Put_Blue_Stacking()//放蓝环——码垛
		{
			WritePosEx(1, Blue_Stacking.Num1, 2250, 30);//转到蓝环方向
			HAL_Delay(800);
			RegWritePosEx(2, Blue_Stacking.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, Blue_Stacking.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(1500);
			RegWritePosEx(4, 2047, 0, 0);//爪子松开
			HAL_Delay(300);
			Rest_action();
		}
		void Get_Red()//抓红环物料
		{
			WritePosEx(4, 2047, 2250, 30);//爪子松开
			HAL_Delay(100);
			WritePosEx(1, Red.Num1, 2250, 30);//转到红环方向
			HAL_Delay(1500);
			WritePosEx(2, Red.Num2, 2250, 30);//向前伸
			WritePosEx(3, Red.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 1500, 0, 0);//爪子抓紧
			HAL_Delay(500);
			Rest_action();
			HAL_Delay(500);
		}
		void Get_Green()//抓绿环物料
		{
			WritePosEx(4, 2047, 2250, 30);//爪子松开
			HAL_Delay(100);
			WritePosEx(1, Green.Num1, 2250, 30);//转到绿环方向
			HAL_Delay(1500);
			WritePosEx(3, Green.Num3, 2250, 30);
			WritePosEx(2, Green.Num2, 2250, 30);//向前伸
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 1500, 0, 0);//爪子抓紧
			HAL_Delay(500);
			Rest_action();
			HAL_Delay(500);
		}
		void Get_Blue()//抓蓝环物料
		{
			WritePosEx(4, 2047, 2250, 30);//爪子松开
			HAL_Delay(100);
			WritePosEx(1, Blue.Num1, 2250, 30);//转到蓝环方向
			HAL_Delay(1500);
			RegWritePosEx(2, Blue.Num2, 2250, 30);//向前伸
			RegWritePosEx(3, Blue.Num3, 2250, 30);
			RegWriteAction();
			HAL_Delay(800);
			WritePosEx(4, 1500, 0, 0);//爪子抓紧
			HAL_Delay(500);
			Rest_action();
			HAL_Delay(500);
		}