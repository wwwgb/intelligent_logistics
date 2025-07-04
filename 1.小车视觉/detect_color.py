import cv2 as cv
import numpy as np
import configparser


class Color_detect():
    def __init__(self):
        config = configparser.ConfigParser()  # 类实例化
        # 定义文件路径
        path = '/root/config.ini'
        config.read(path)

        red_threshold_1 = list(
            map(int, config.get('using', 'red_hsv_1')[1:-1].split(',')))  # 红色阈值
        red_threshold_2 = list(
            map(int, config.get('using', 'red_hsv_1')[1:-1].split(',')))

        green_threshold = list(
            map(int, config.get('using', 'green_hsv')[1:-1].split(',')))  # 绿色阈值
        blue_threshold = list(
            map(int, config.get('using', 'blue_hsv')[1:-1].split(',')))  # 蓝色阈值
        # print(red_threshold[3])
        self.threshold_r = [red_threshold_1,
                            red_threshold_2, green_threshold, blue_threshold]
        self.color = [(0, 0, 255), (0, 0, 255), (0, 255, 0), (255, 0, 0)]

    def detect(self, hsv_image, threshold_value):

        up_ = np.array(threshold_value[-3:])
        lower_ = np.array(threshold_value[:3])
        mask = cv.inRange(hsv_image, lower_, up_)
        kernel = np.ones((3, 3), np.uint8)
        dilation = cv.dilate(mask, kernel, iterations=1)
        cnts = cv.findContours(
            dilation, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_NONE)
        area = 1500
        blob = None
        #max_area = 99999
        for c in cnts[0]:
            area_img = abs(cv.contourArea(c, True))
            #cv.drawContours(hsv_image, [c], 0, (255, 0, 0), 2)
            # print(area_img)
            if area_img > area:
                # print(area_img)
                blob = c
                area = area_img
        # cv.imshow("dilation", dilation)
        # cv.imshow("hsv_image", hsv_image)
        if blob is not None:
            # Draw a rect around the blob.
            rect = cv.minAreaRect(blob)
            # cv.boxPoints(rect) for OpenCV 3.x 获取最小外接矩形的4个顶点
            box = cv.boxPoints(rect)
            box = np.int0(box)
            #print("四个顶点坐标为;", box)
            return True, area
        return False, None

    def run(self, img):
        # roi =  img[y:y+h, x:x+w]
        box = [0, 0, 0, 0, 10]
        hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
        for i in range(len(self.threshold_r)):
            _ret, _box = self.detect(hsv, self.threshold_r[i])
            if _ret:
                # cv.drawContours(img, _box, -1, self.color[i], 3)
                box[i] = _box

                # if i < 2:
                #     return str(1)
                # else:
                #     return str(i)
        num = box.index(max(box))
        if num == 4:
            return None

        if num < 2:
            return str(1)
        else:
            return str(num)


if __name__ == '__main__':
    C_d = Color_detect()

    maix = True
    camera = cv.VideoCapture(0)
    camera.set(cv.CAP_PROP_FRAME_WIDTH, 160)
    camera.set(cv.CAP_PROP_FRAME_HEIGHT, 120)
    camera.set(cv.CAP_PROP_BUFFERSIZE, 1)
    camera.set(cv.CAP_PROP_AUTO_WB, 0)
    if maix:
        f = open('/dev/fb0', 'wb')
    while True:
        # 读取当前帧
        ret, frame = camera.read()

        # print(ret.shape)
        if not ret:
            print("frame is empty.")
            continue
        # frame1 = cv.imread("./qr_test.png")
        hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
        for i in range(len(C_d.threshold_r)):
            _ret, _box = C_d.detect(hsv, C_d.threshold_r[i])
            # if _ret:
            #     cv.drawContours(frame, _box, -1, C_d.color[i], 3)

        if maix:
            frame = cv.resize(frame, (240, 240))
            dst = cv.cvtColor(frame, cv.COLOR_BGR2BGR565)
            # cv.imshow('frame', frame)

            f.seek(0)
            f.write(bytearray(dst))
        else:
            cv.imshow("camera", frame)

        if(cv.waitKey(5) == 27):
            break
    camera.release()
    cv.destroyAllWindows()
