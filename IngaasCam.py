#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Jan 25 2022

@author: pfjarschel
"""

from IngaasCamConfCl import IngaasCamConfCl
import cv2
import ctypes
import numpy as np
from PyQt5.QtGui import QImage

class IngaasCam:
    fps = 0
    maxW = 640
    maxH = 512
    frameW = maxW
    frameH = maxH
    exposure = 10 #ms
    gain = 0  # 0 or 1
    cam = None
    ifaceOK = False
    camOK = False
    ccdSize = [16000,12800]
    
    def c_str(pstring):
        return ctypes.create_string_buffer(pstring.encode(encoding="utf-8"))
    
    def __init__(self, exposure=10, hgain=0, correct=True, show_conf_menu=False):  
        if hgain == 1:
            self.cam_conf = IngaasCamConfCl(1)
        else:
            self.cam_conf = IngaasCamConfCl(0)
        self.gain = hgain
        self.exposure = exposure
        self.cam_conf.SetExposure(self.exposure)
        
        self.camlib = ctypes.cdll.LoadLibrary("./IngaasCameraDriver.dll")

        CamLibHandle = ctypes.POINTER(ctypes.c_char_p)

        self.camlib.SetCorrections.argtypes = [CamLibHandle, ctypes.c_bool, ctypes.c_bool, ctypes.c_double, ctypes.c_int]
        self.camlib.InitLib.restype = CamLibHandle
        self.camlib.GetCaptureDimensions.restype = ctypes.c_int
        self.camlib.GetConnectionIP.restype = ctypes.c_char_p
        self.camlib.GetCameraIP.restype = ctypes.c_char_p
        self.camlib.GetConnectionMAC.restype = ctypes.c_char_p
        self.camlib.GetCameraSubnet.restype = ctypes.c_char_p
        self.camlib.CaptureRawFrame.restype = ctypes.POINTER(ctypes.c_ushort)
        self.camlib.CaptureCorrectedFrame.restype = ctypes.POINTER(ctypes.c_ushort)
        self.camlib.CaptureAveragedFrame.restype = ctypes.POINTER(ctypes.c_ushort)

        self.cam = self.camlib.InitLib()
        self.camlib.SetSelectMode(self.cam, 0)
        self.camOK = self.camlib.InitializeCam(self.cam, show_conf_menu)
        self.ifaceOK = self.camOK
        self.correct = correct
        print(f"Camera OK: {self.camok}")

        badpixels_file = self.c_str("bad_pixels.bin")
        # badpixels_file = c_str("darkcurrent.flf")
        darkcurrent_file = self.c_str("darkcurr_HG_50ms.bin")

        self.camlib.SetCorrections(self.cam, correct, correct, 1.0, 1)
        self.camlib.SetCorrectionRadius(self.cam, 15)
        self.camlib.SetMaskThresholds(self.cam, 64, 96)
        self.camlib.LoadBadPixelCorrectionFile(self.cam, badpixels_file)
        self.camlib.LoadDarkCurrentCorrectionFile(self.cam, darkcurrent_file)                   
        
        self.GetExposure()
        self.GetGain()
        
    def __del__(self):
        self.Close()
    
    def Close(self):
        if self.ifaceOK:
            self.camlib.CloseCamera(self.cam)
            del self.cam
            del self.camlib

        self.cam_conf.Close()
        
    def GetFrameMatrix(self, scale=1):
        frame = None
        if self.camOK:
            frame = np.ctypeslib.as_array(self.camlib.CaptureRawFrame(self.cam), shape=(512, 640))
        else:
            frame = np.zeros([int(np.round(self.frameH*scale)), int(np.round(self.frameW*scale)), 3])
        return frame

    def GetQImage(self):
        frame = self.GetFrameMatrix()
        frame = cv2.convertScaleAbs(frame, alpha=(255.0/65535.0))
        w = len(frame[0])
        h = len(frame)
        img = QImage(frame.data, w, h, QImage.Format_Grayscale8)
        return img
    
    def SetCaptureDimensions(self, w, h):
        self.frameH = 512
        self.frameW = 640
        
    def SetExposure(self, expms):
        if expms < 1:
            expms = 1
        self.cam_conf.SetExposure(expms)
        self.exposure = expms
        
    def SetGain(self, hgain):
        self.gain = self.gain
        
    def GetExposure(self):
        return self.exposure
    
    def GetGain(self):
        return self.gain
    
    def GetCaptureDimensions(self):
        return [640, 512]
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        