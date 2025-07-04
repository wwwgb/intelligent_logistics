import sys
import serial
import cv2 as cv
import numpy as np
import time
from threading import Thread
from maix import event
from select import select
from libs.button import KeyCode, key_pressed
from detect_color import Color_detect
from find_circle import Find_circle
#   -------		串口通讯的规则为：	-------      
#   波特率115200		                        
#   下位机：帧头(1字节)0x1B	 数据(1字节)0x30     
#   上位机：数据(字符)       帧尾(1字节)0x0D     
#   ---------------------------------------	   
#   当接收到下位机数据后会发送'0x06'，表示命令已接收
# 0x30(停止) 0x31(二维码) 0x32(颜色)  0x33(圆) 0x34(定位) 0x35(标定)

# /dev/ttyS1
ser = serial.Serial("/dev/ttyS1", 115200)    # 连接串口
# print('serial test start ...')
response = None
Flag = True

def check_key():
    import os
    tmp = "/dev/input/by-path/"
    if os.path.exists(tmp):
        for i in os.listdir(tmp):
            if i.find("kbd") != -1:
                return tmp + i
    return "/dev/input/event0"


dev = event.InputDevice(check_key())


def rece():
    global response, Flag
    #res = None

    while Flag:
        res = None
        time.sleep(0.3)
        while Flag:
            # print("thread_1:{}".format(time.time()))
            if ser.inWaiting():
                read_bytes = ser.read(1)
                # print(read_bytes)
                if res is None:
                    if read_bytes == b'\x1B':
                        res = read_bytes
                else:
                    res += read_bytes
                    if (len(res) == 2):
                        ser.write(b'\x06')
                        break

        if res != None:
            if res == b'\x1B\x30':
                response = None
            else:
                response = res


def remove(string):
    return string.replace(" ", "")

def main():
    Cdet = Color_detect()
    Fdet = Find_circle()
    t = Thread(target=rece)
    #t.daemon  = True
    t.start()

    camera = cv.VideoCapture(0)
    camera.set(cv.CAP_PROP_FRAME_WIDTH, 320)
    camera.set(cv.CAP_PROP_FRAME_HEIGHT, 240)
    camera.set(cv.CAP_PROP_BUFFERSIZE, 1)
    #camera.set(cv.CAP_PROP_AUTO_WB, 0)

    # gain = camera.get(cv.CAP_PROP_GAIN)

    # response_ = ''
    run_text = ''
    res_text = ''
    frame_tail = b'\x0D'
    color_start = b'\x1B\x32'
    cir_start = b'\x1B\x33'
    f = open('/dev/fb0', 'wb')

    oi_x = 10
    oi_y = 0#10
    oi_w = 150
    oi_h = 150

    fps = ""
    target = [color_start, cir_start]
    count = 0
    #response =cir_start##################################################################################################

    #thread_num = len(threading.enumerate())
    #print("主线程：线程数量是%d" % thread_num)
    # 输出所有线程名字
    #print(str(threading.enumerate()))
     
    while True:
    
        #print("thread_main:{}".format(time.time()))
        # print(response)
        st = time.time()
        # 读取当前帧
        ret, frame = camera.read()
        #print(frame.shape)
        #frame = cv.resize(frame, (160, 120))
        #frame = cv.rotate(frame, cv.ROTATE_180)

        if not ret:
            #Flag = False
            print("frame is empty.")
            camera = cv.VideoCapture(-1)
            camera.set(cv.CAP_PROP_FRAME_WIDTH, 160)
            camera.set(cv.CAP_PROP_FRAME_HEIGHT, 120)
            
            continue

        if response == color_start:
            # roi = img[y:y+h, x:x+w]
            C_text = Cdet.run(frame[oi_y:oi_y+oi_h, oi_x:oi_x+oi_w])
            run_text = "COLOR"
            if C_text:
                res_text = C_text
                # response = None
                ser.write(res_text.encode()+frame_tail)
            else:
                res_text = ""
            cv.rectangle(frame, (oi_x, oi_y),
                         (oi_x + oi_w, oi_y + oi_h), (0, 255, 0), 2)
        elif response == cir_start:

            # roi = img[y:y+h, x:x+w]
            F_text = Fdet.find(frame, True)
            run_text = "CIRCLE"
            if F_text:
                res_text = remove(str(F_text))
                # response = None
                ser.write(res_text.encode()+frame_tail)
                # ser.flush()
            else:
                res_text = ""

        elif response is None:
            run_text = ""
            res_text = ""

        run_t = round(time.time() - st, 2)

        cv.putText(frame, "run:{0}" .format(run_text), (0, 30),
                   cv.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 255), 2)
        cv.putText(frame, "res:{0}" .format(res_text), (0, 60),
                   cv.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 255), 2)
        cv.putText(frame, "run_t:{0}" .format(run_t), (0, 90),
                   cv.FONT_HERSHEY_SIMPLEX, 1, (255, 0, 255), 2)

        imagex1 = cv.resize(frame, (240, 240))
        dst = cv.cvtColor(imagex1, cv.COLOR_BGR2BGR565)
        f.seek(0)
        f.write(bytearray(dst))

        key = key_pressed()
        if key == KeyCode.LeftTopButton:
            camera.release()
            sys.exit()

    camera.release()
    cv.destroyAllWindows()

if __name__ == '__main__':
    main()