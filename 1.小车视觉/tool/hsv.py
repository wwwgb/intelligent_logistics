import cv2
import numpy as np

Pic_Capture = cv2.VideoCapture(0)
Pic_Capture.set(cv2.CAP_PROP_FRAME_WIDTH, 320)
Pic_Capture.set(cv2.CAP_PROP_FRAME_HEIGHT, 240)
Pic_Capture.set(cv2.CAP_PROP_BUFFERSIZE, 1)

hsv_low = np.array([0, 0, 0])
hsv_high = np.array([0, 0, 0])


def h_low(value):
    hsv_low[0] = value

def h_high(value):
    hsv_high[0] = value

def s_low(value):
    hsv_low[1] = value

def s_high(value):
    hsv_high[1] = value

def v_low(value):
    hsv_low[2] = value

def v_high(value):
    hsv_high[2] = value


cv2.namedWindow('Set',cv2.WINDOW_FREERATIO)

cv2.createTrackbar('H low', 'Set', 0, 180, h_low) 
cv2.createTrackbar('S low', 'Set', 0, 255, s_low)
cv2.createTrackbar('V low', 'Set', 0, 255, v_low)

cv2.createTrackbar('H high', 'Set', 0, 180, h_high)
cv2.createTrackbar('S high', 'Set', 0, 255, s_high)
cv2.createTrackbar('V high', 'Set', 0, 255, v_high)

while Pic_Capture.isOpened():
    ret, image = Pic_Capture.read()
    if ret:
        if image is not None:
           dst = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
           dst = cv2.GaussianBlur(dst, (5, 5), 0)
           dst = cv2.inRange(dst, hsv_low, hsv_high)
           #cv2.namedWindow("Img",0)
           #cv2.resizeWindow("Img", 800, 480)
           #cv2.imshow('Img', image)
           cv2.imshow('dst', dst)
           cv2.waitKey(1)
Pic_Capture.release()
cv2.destroyAllWindows()
