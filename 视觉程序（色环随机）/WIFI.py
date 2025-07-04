#!/usr/bin/python3

import os
import sys
import time
import json
import re
import subprocess
from maix import camera, display, image
from libs.button import KeyCode, key_pressed
from libs.image_extra import draw_str, draw_button, draw_center_string
import socket

dis_size = (display.width(), display.height())

def is_json(myjson):
    try:
        json_object = json.loads(myjson)
    except ValueError as e:
        return False, None
    return True, json_object

def extract_ssid_password(string):
    # 使用正则表达式提取"S:"和"P:"后的内容
    ssid_pattern = r'S:(.*?);'
    password_pattern = r'P:(.*?);'
    ssid = re.search(ssid_pattern, string).group(1)
    password = re.search(password_pattern, string).group(1)
    return ssid, password

def get_host_ip():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    except Exception as err:
        ip = None 
    finally:
        s.close()

    return ip
def main():
    image_data = image.new(size = (140, 140), mode = "RGB")
    ssid = None
    passwd = None
    ip = get_host_ip()
    last_time = time.time()
    while True:
        img = camera.capture()    #从摄像头中获取一张图像
        if not img:
            time.sleep(0.01)
            continue

        image_data = img.crop(50,50,140,140)
        result = image_data.find_qrcodes()
        if result:
            payload = result[0]['payload']
            ret, json = is_json(payload)
            if ret:
                ssid = json["s"]
                passwd = json["p"]

            else:
                ssid, passwd = extract_ssid_password(payload)
            img.draw_rectangle(50,50,190,190, color=(0,255,0), thickness=2)
        else:
            img.draw_rectangle(50,50,190,190, color=(0,0,255), thickness=2)
        if ssid != None and passwd != None:
            draw_center_string(img, "ssid:"+ssid, dis_size, y=200, fill=(59, 59, 59))
            draw_center_string(img, "passwd:"+passwd, dis_size, y=220,fill=(59, 59, 59))

        if time.time() - last_time > 3:
            last_time = time.time()
            ip =  get_host_ip()
        if ip != None:
            draw_center_string(img, "ip:"+ip, dis_size, y=20, fill=(59, 59, 59))

        key = key_pressed()
        if key == KeyCode.LeftTopButton:
            camera.close()
            sys.exit()
        elif key == KeyCode.RightTopButton and ssid != None and passwd != None:
            #subprocess.call("nmcli dev wifi connect '{0}' password '{1}'".format(ssid, passwd))
            command = "nmcli dev wifi connect '{0}' password '{1}'".format(ssid, passwd)
            os.system(command)

        draw_button(img, dis_size, "退出", "连接")
        display.show(img)




if __name__ == '__main__':
    main()
