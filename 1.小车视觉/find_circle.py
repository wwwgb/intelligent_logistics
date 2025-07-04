import cv2 as cv
import numpy as np

hsv_min_green = np.array([44, 23, 63])
hsv_max_green = np.array([73, 255, 255])

class Find_circle():
    def __init__(self):
        self.PI = np.pi

    def find(self, img, debug=False, minR=25, maxR=150):
        hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
        hsv = cv.GaussianBlur(hsv, (5, 5), 0)
        dst = cv.inRange(hsv, hsv_min_green, hsv_max_green)
        contours, _ = cv.findContours(dst, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
        max_area = 0
        max_contour = None
        circle_center = None

        for cnt in contours:
            if len(cnt) < 5:
                continue

            area = cv.contourArea(cnt)
            if abs(area) > 7000 and area > max_area:
                max_area = area
                max_contour = cnt
        if max_contour is not None:
            (x, y), _ = cv.minEnclosingCircle(max_contour)
            circle_center = (int(x), int(y))

        if debug and circle_center:
            cv.circle(img, circle_center, 2, (0, 255, 0), 2, 8, 0)

        return circle_center
