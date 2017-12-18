
import cv2 as cv
import time
import win32api
#import A3C_atari


class Application:
    
    _cap = None
    _lastUptaetime = 0

    _capedFrame = None
    _capedGrayFrame = None
    _ret = None
    
    _curPos = None
    _model = None

    _eyedetectNetWork = None

    def init(self):
        #创建窗口
        self.Createwindow()
        #初始化摄像机设备
        self.Createdevice()
        #初始化眼睛识别网络
        self.CreateEyeDetectnetwork()    
        pass

    def update(self):

        timeperframe = 1.0/60
        while True:
            CurTime = time.clock()
            DeltaTime = CurTime - self._lastUptaetime
            #print("delta is ", DeltaTime)
            if DeltaTime < timeperframe:
                time.sleep(DeltaTime)
            else :
                self._lastUptaetime = CurTime
                break

        return self.OneFrame()


    def Createwindow(self):
        cv.namedWindow('Camear', cv.WINDOW_NORMAL)

        cv._lastUptaetime = time.clock()
        #print("CurTime is ", cv._lastUptaetime)
        pass

    def Createdevice(self):
        self._cap = cv.VideoCapture(0)
        print("is open ", self._cap.isOpened())
        pass

    def Createnetwork(self):
        # model
        #self._model = A3CAtari()
        pass

    def CreateEyeDetectnetwork(self):
        self._eyedetectNetWork = cv.CascadeClassifier(".\\haarcascades\\haarcascade_eye_tree_eyeglasses.xml")

        if self._eyedetectNetWork.empty():
            print("eyedetect empty")

        if self._eyedetectNetWork != None:
            print("find eyede")
        pass

    def OneFrame(self):

        self.CapOneFrame()
        self.DrawOneframe()

        cv.waitKey(2)
        return 0

    def CapOneFrame(self):
        self._ret , self._capedFrame = self._cap.read()
        self._curPos = win32api.GetCursorPos()
        self._capedGrayFrame = cv.cvtColor(self._capedFrame, cv.COLOR_BGR2GRAY)

        eyerect =  self.DetctEye()
        if len(eyerect) == 2:
            SavedEye = []
            for [x,y,w,h] in eyerect:
                temp = self._capedFrame[y: y + 50, x : x + 50]
                cv.rectangle(self._capedFrame,(x,y),(x+50,y+50),(255,0,0),2)
                SavedEye.append(temp)

            pass
       


        pass
    
    def DrawOneframe(self):
        cv.imshow('Camear', self._capedFrame)

        
        pass


    def DetctEye(self):
        eyeRect = self._eyedetectNetWork.detectMultiScale( self._capedGrayFrame, 1.1, 5)
        return eyeRect

    
    