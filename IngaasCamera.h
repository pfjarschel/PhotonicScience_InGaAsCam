#pragma once
#ifndef INGAASCAMERA // include guard
#define INGAASCAMERA

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <CyConfig.h>
#include <CyGrabber.h>
#include <CyImageBuffer.h>
#include <CyDeviceFinder.h>
#include <opencv2/opencv.hpp>

#ifdef WIN32
#include <CyDisplayEx.h>
#endif // WIN32

#ifdef _UNIX_
#include <CyDisplay.h>
#endif // _UNIX_

#define SELECT_FIRST 0
#define SELECT_IP 1
#define SELECT_MAC 2
#define SELECT_DIALOG 3
#define SELECT_FORCE_IP 4

class IngaasCamera
{
protected:
	int sel_mode = SELECT_DIALOG;
	std::string connect_ip = "[143.106.153.64]";
	std::string set_ip = "[143.106.153.64]";
	std::string mac = "00-11-1C-01-BE-40";
	std::string subnet_mask = "[255.255.0.0]";
	bool cam_found = false;
	bool cam_configured = false;
	bool camOK = false;
	bool loadedBadPixels = false;
	bool loadedDarkCurrent = false;
	bool correctBadPixels = false;
	bool correctDarkCurrent = false;
	int correction_radius = 15;
	int pos_smooth_radius = 3;
	int mask_thr_low = 64;
	int mask_thr_high = 96;
	double contrast_enhancement_factor = 1.0;
	unsigned long lWidth = 640;
	unsigned long lHeight = 512;
	unsigned long lPixelDepth = 16;
	unsigned long lImageSize = 655360;

	cv::Size cv_size = cv::Size(lWidth, lHeight);
	cv::Mat cv_buffer = cv::Mat(cv_size, CV_16UC1);
	cv::Mat dark_current = cv::Mat(cv_size, CV_16UC1);
	cv::Mat bad_pixels = cv::Mat(cv_size, CV_16UC1);
	cv::Mat bad_pixels_mask = cv::Mat(cv_size, CV_8UC1);

	CyDeviceFinder lFinder;
	CyDeviceFinder::DeviceList lIPEngineList;
	CyDeviceFinder::DeviceEntry lIPEngine;
	CyConfig lConfig;
	CyGrabber lGrabber;
	CyPixelTypeID lPixelID = 0;
	CyUserBuffer lBuffer = CyUserBuffer(cv_buffer.data, lImageSize);
	CyDisplayEx lDisplayEx;


	void CreateConfig();
	int FindDevices();
	bool SelectDevice();
	bool ConfigureDevice(bool show_conf = false);
	void CreateBuffer();
	bool TestCam();
	void GrabRawFrame();
	void GrabCorrectedFrame();
	void SaveRawImageToFile(cv::Mat data, const char* filename);
	cv::Mat LoadCorrectionFrame(const char* filename);
	void DisplayFrame(cv::Mat frame);

public:
	IngaasCamera();
	~IngaasCamera();
	void SetSelectMode(int new_mode = 3);
	int GetSelectMode();
	void SetConnectionIP(std::string new_ip);
	std::string GetConnectionIP();
	void SetCameraIP(std::string new_ip);
	std::string GetCameraIP();
	void SetConnectionMAC(std::string new_mac);
	std::string GetConnectionMAC();
	void SetCameraSubnet(std::string new_subn);
	std::string GetCameraSubnet();
	bool CheckIfFoundDevices();
	bool CheckIfCamConfigured();
	bool CheckIfCamOK();
	cv::Size GetCaptureDimensions();
	unsigned long GetBitDepth();
	bool OpenCamConfig();
	void DisplayLastFrame();
	bool InitializeCam(bool show_conf = false);
	void AcquireCorrectionData(std::string filename);
	void ShowCalFrame(std::string filename);
	void LoadDarkCurrentCorrectionFile(std::string filename);
	void LoadBadPixelCorrectionFile(std::string filename);
	void SetCorrections(bool dark_current, bool bad_pixels, double contrast_enhancement = 1.0, int pos_smooth = 1);
	void SetCorrectionRadius(int radius);
	void SetMaskThresholds(int low, int high);
	void DisplayRawFrame();
	void DisplayCorrectedFrame();
	cv::Mat* CaptureRawFrame();
	cv::Mat* CaptureCorrectedFrame();
	cv::Mat* CaptureAveragedFrame(int n, unsigned int interval_ms = 0);
};

#endif // INGAASCAMERA