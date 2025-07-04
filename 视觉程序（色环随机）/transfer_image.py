#!/usr/bin/python3

import os
import sys
import time
from maix import camera, mjpg, utils, display
from libs.button import KeyCode, key_pressed
from libs.image_extra import draw_button



def main():
    queue = mjpg.Queue(maxsize=8)
    mjpg.MjpgServerThread("0.0.0.0", 18811, mjpg.BytesImageHandlerFactory(q=queue)).start()

    while True:
        img = camera.capture()
        jpg = utils.rgb2jpg(img.convert("RGB").tobytes(), img.width, img.height)
        queue.put(mjpg.BytesImage(jpg))
        display.show(img)
        draw_button(img, "退出", None)
        key = key_pressed()
        if key == KeyCode.LeftTopButton:
            camera.close()
            sys.exit()

if __name__ == '__main__':
    main()
