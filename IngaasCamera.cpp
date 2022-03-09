// InGaAsCamera class

#include "IngaasCamera.h"

IngaasCamera::IngaasCamera()
{
	std::cout << "Loading InGaAs camera communication library via old Pleora eBus SDK..." << std::endl;
}
IngaasCamera::~IngaasCamera()
{
	std::cout << "Ending communication with camera. All seems ok." << std::endl;
}

void IngaasCamera::CreateConfig()
{
	this->lConfig.AddDevice();
}

int IngaasCamera::FindDevices()
{
	// All IP engines available through eBUS Driver
	this->lFinder.Find(
		CY_DEVICE_ACCESS_MODE_EBUS,
		this->lIPEngineList,
		100,
		true);

	// All GigE Vision IP engines available through eBUS Driver
	this->lFinder.Find(
		CY_DEVICE_ACCESS_MODE_GEV_EBUS,
		this->lIPEngineList,
		100,
		true);

	// All IP engines available through High Performance Driver
	this->lFinder.Find(
		CY_DEVICE_ACCESS_MODE_DRV,
		this->lIPEngineList,
		100,
		false);

	// All IP engines available through Network Stack
	this->lFinder.Find(
		CY_DEVICE_ACCESS_MODE_UDP,
		this->lIPEngineList,
		100,
		false);

	int n_devs = this->lIPEngineList.size();

	return n_devs;
}

bool IngaasCamera::SelectDevice()
{
	this->cam_found = false;
	int n_devs = this->lIPEngineList.size();
	if (n_devs > 0)
	{
		switch (this->sel_mode)
		{
		case SELECT_FIRST:
			this->lIPEngine = this->lIPEngineList[0];
			this->cam_found = true;
			break;

		case SELECT_DIALOG:
			if (this->lFinder.SelectDevice(this->lIPEngine) != CY_RESULT_OK)
			{
				return false;
			}
			this->cam_found = true;
			break;

		case SELECT_IP:
			for (int i = 0; i < n_devs; i++)
			{
				if (this->connect_ip == this->lIPEngineList[i].mAddressIP.c_str_ascii())
				{
					this->lIPEngine = this->lIPEngineList[i];
					this->cam_found = true;
					break;
				}
			}
			break;

		case SELECT_MAC:
			for (int i = 0; i < n_devs; i++)
			{
				if (this->mac == this->lIPEngineList[i].mAddressMAC.c_str_ascii())
				{
					this->lIPEngine = this->lIPEngineList[i];
					this->cam_found = true;
				}
			}
			break;

		case SELECT_FORCE_IP:
			for (int i = 0; i < n_devs; i++)
			{
				if (this->mac == this->lIPEngineList[i].mAddressMAC.c_str_ascii())
				{
					this->lIPEngine = this->lIPEngineList[i];
					this->lIPEngine.mAddressIP = this->set_ip;
					this->lIPEngine.mSubnetMask = this->subnet_mask;
					this->cam_found = true;
				}
			}
			break;
		}
	}
	return this->cam_found;
}

bool IngaasCamera::ConfigureDevice(bool show_conf)
{
	this->cam_configured = false;
	if (this->cam_found)
	{
		this->CreateConfig();

		// Configure the CyConfig object's connection parameters
		this->lConfig.SetParameter(CY_CONFIG_PARAM_ACCESS_MODE, this->lIPEngine.mMode);
		this->lConfig.SetParameter(CY_CONFIG_PARAM_ADDRESS_IP, this->lIPEngine.mAddressIP);
		this->lConfig.SetParameter(CY_CONFIG_PARAM_ADDRESS_MAC, this->lIPEngine.mAddressMAC);
		this->lConfig.SetParameter(CY_CONFIG_PARAM_SUBNET_MASK, this->lIPEngine.mSubnetMask);
		this->lConfig.SetParameter(CY_CONFIG_PARAM_ADAPTER_ID, this->lIPEngine.mAdapterID.GetIdentifier());

		// A packet payload size of 1440 bytes is a safe value for all connectionmodes
		this->lConfig.SetParameter(CY_CONFIG_PARAM_PACKET_SIZE, 1440);

		// Set desired timeouts (these are the default values)
		this->lConfig.SetParameter(CY_CONFIG_PARAM_ANSWER_TIMEOUT, 1000);
		this->lConfig.SetParameter(CY_CONFIG_PARAM_FIRST_PACKET_TIMEOUT, 1500);
		this->lConfig.SetParameter(CY_CONFIG_PARAM_PACKET_TIMEOUT, 500);
		this->lConfig.SetParameter(CY_CONFIG_PARAM_REQUEST_TIMEOUT, 5000);

		// Set the connection topology to unicast
		this->lConfig.SetParameter(CY_CONFIG_PARAM_DATA_SENDING_MODE, CY_DEVICE_DSM_UNICAST);
		this->lConfig.SetParameter(CY_CONFIG_PARAM_DATA_SENDING_MODE_MASTER, true);

		// Connect CyGrabber object to the IP Engine.
		if (this->lGrabber.Connect(this->lConfig) != CY_RESULT_OK)
		{
			this->cam_configured = false;
		}
		else
		{
			this->set_ip = this->lIPEngine.mAddressIP.c_str_ascii();
			this->connect_ip = this->lIPEngine.mAddressIP.c_str_ascii();
			this->mac = this->lIPEngine.mAddressMAC.c_str_ascii();
			this->subnet_mask = this->lIPEngine.mSubnetMask.c_str_ascii();
			if (show_conf)
			{
				this->cam_configured = this->OpenCamConfig();
			}
			else
			{
				this->cam_configured = true;
			}
			this->CreateBuffer();
		}
	}
	return this->cam_configured;
}

void IngaasCamera::CreateBuffer()
{
	if (this->cam_found && this->cam_configured)
	{
		// Retrieve the buffer size
		this->lGrabber.GetParameter(CY_GRABBER_PARAM_SIZE_X, this->lWidth);
		this->lGrabber.GetParameter(CY_GRABBER_PARAM_SIZE_Y, this->lHeight);
		this->lGrabber.GetParameter(CY_GRABBER_PARAM_PIXEL_DEPTH, this->lPixelDepth);
		this->lImageSize = this->lWidth * this->lHeight * (int)((this->lPixelDepth + 7) / 8);

		// Retrieve the buffer's pixel type
		this->lGrabber.GetEffectivePixelType(this->lPixelID, 0);

		// Create the buffer object.
		this->cv_buffer.release();
		this->cv_buffer = cv::Mat(this->cv_size, CV_16UC1);
		this->lBuffer.SetBuffer(this->cv_buffer.data, this->lImageSize);
	}
}

bool IngaasCamera::TestCam()
{
	this->camOK = false;
	if (this->cam_found && this->cam_configured)
	{
		//Grab one image from first channel, can be changed for multi-channel devices
		if (this->lGrabber.Grab(CyChannel(0), this->lBuffer, 0) != CY_RESULT_OK)
		{
			this->camOK = false;
		}
		else
		{
			this->camOK = true;
		}
	}

	return this->camOK;
}

void IngaasCamera::GrabRawFrame()
{
	if (this->camOK)
	{
		//Grab one image from first channel, can be changed for multi-channel devices
		this->lGrabber.Grab(CyChannel(0), this->lBuffer, 0);
	}
}

void IngaasCamera::GrabCorrectedFrame()
{
	this->GrabRawFrame();

	// Subtract darkcurrent and rescale
	if (this->loadedDarkCurrent && this->correctDarkCurrent)
	{
		double avg_light_0 = cv::mean(this->cv_buffer)[0];
		cv::subtract(this->cv_buffer, this->dark_current, this->cv_buffer);
		double avg_light_1 = cv::mean(this->cv_buffer)[0];
		this->cv_buffer = this->cv_buffer * this->contrast_enhancement_factor*(avg_light_0/avg_light_1);
	}

	// Correct bad pixels
	if (this->loadedBadPixels && this->correctBadPixels)
	{
		// Replace bad pixels with blurred data
		cv::Mat cv_blurred_frame(this->cv_size, CV_16UC1);
		cv::GaussianBlur(this->cv_buffer, cv_blurred_frame, cv::Size(this->correction_radius, this->correction_radius), 0.0);
		cv_blurred_frame.copyTo(this->cv_buffer, this->bad_pixels_mask);

		cv_blurred_frame.release();
	}

	// Perform additional smoothing
	if (this->pos_smooth_radius > 2)
	{
		cv::GaussianBlur(this->cv_buffer, this->cv_buffer, cv::Size(this->pos_smooth_radius, this->pos_smooth_radius), 0.0);
	}
}

void IngaasCamera::SaveRawImageToFile(cv::Mat data, const char* filename)
{
	FILE* pFile = NULL;
	errno_t err;
	err = fopen_s(&pFile, filename, "wb");
	if (!err && fopen_s != NULL)
	{
		fwrite(data.data, 2, this->lImageSize / sizeof(unsigned short), pFile);
		fclose(pFile);
	}
}

cv::Mat IngaasCamera::LoadCorrectionFrame(const char* filename)
{
	cv::Mat frame(this->cv_size, CV_16UC1);

	FILE* pFile = NULL;
	errno_t err;
	err = fopen_s(&pFile, filename, "rb");
	if (!err && fopen_s != NULL)
	{
		fread_s(frame.data, this->lImageSize, 1, this->lImageSize, pFile);
		fclose(pFile);
	}

	return frame;
}

void IngaasCamera::DisplayFrame(cv::Mat frame)
{
	if (this->camOK)
	{
		CyUserBuffer buffer(frame.data, this->lImageSize);
		this->lDisplayEx.Open(
			NULL,
			CyDisplayEx::DEFAULT_POSITION,
			CyDisplayEx::DEFAULT_POSITION,
			(unsigned short)this->lWidth,
			(unsigned short)this->lHeight);

		this->lDisplayEx.Display(
			buffer.GetBuffer(),
			buffer.GetBufferSize(),
			(unsigned short)this->lWidth,
			(unsigned short)this->lHeight,
			this->lPixelID);

		// Wait until the user closes the display window
		CyDisplayEx::PerformGUIMessagePump();

		this->lDisplayEx.Close();
	}
}

void IngaasCamera::SetSelectMode(int new_mode)
{
	this->sel_mode = new_mode;
}
int IngaasCamera::GetSelectMode()
{
	return this->sel_mode;
}

void IngaasCamera::SetConnectionIP(std::string new_ip)
{
	new_ip = "[" + new_ip + "]";
	this->connect_ip = new_ip;
}
std::string IngaasCamera::GetConnectionIP()
{
	std::string new_ip = this->connect_ip.substr(1, this->connect_ip.size() - 2);
	return new_ip;
}

void IngaasCamera::SetCameraIP(std::string new_ip)
{
	new_ip = "[" + new_ip + "]";
	this->set_ip = new_ip;
}
std::string IngaasCamera::GetCameraIP()
{
	std::string new_ip = this->set_ip.substr(1, this->set_ip.size() - 2);
	return new_ip;
}

void IngaasCamera::SetConnectionMAC(std::string new_mac)
{
	this->mac = new_mac;
}
std::string IngaasCamera::GetConnectionMAC()
{
	return this->mac;
}

void IngaasCamera::SetCameraSubnet(std::string new_subn)
{
	this->subnet_mask = new_subn;
}
std::string IngaasCamera::GetCameraSubnet()
{
	return this->subnet_mask;
}

bool IngaasCamera::CheckIfFoundDevices()
{
	return this->cam_found;
}

bool IngaasCamera::CheckIfCamConfigured()
{
	return this->cam_configured;
}

bool IngaasCamera::CheckIfCamOK()
{
	return this->camOK;
}

cv::Size IngaasCamera::GetCaptureDimensions()
{
	return this->cv_size;
}

unsigned long IngaasCamera::GetBitDepth()
{
	return this->lPixelDepth;
}

bool IngaasCamera::OpenCamConfig()
{
	this->lGrabber.ShowDialog(NULL);
	if (this->lGrabber.SaveConfig() != CY_RESULT_OK)
	{
		this->cam_configured = false;
	}
	else
	{
		this->cam_configured = true;
	}

	return this->cam_configured;
}

void IngaasCamera::DisplayLastFrame()
{
	if (this->camOK)
	{
		this->lDisplayEx.Open(
			NULL,
			CyDisplayEx::DEFAULT_POSITION,
			CyDisplayEx::DEFAULT_POSITION,
			(unsigned short)this->lWidth,
			(unsigned short)this->lHeight);

		this->lDisplayEx.Display(
			this->lBuffer.GetBuffer(),
			this->lBuffer.GetBufferSize(),
			(unsigned short)this->lWidth,
			(unsigned short)this->lHeight,
			this->lPixelID);

		// Wait until the user closes the display window
		CyDisplayEx::PerformGUIMessagePump();

		this->lDisplayEx.Close();
	}
}

bool IngaasCamera::InitializeCam(bool show_conf)
{
	std::cout << "Initializing camera..." << std::endl;

	int n_devs = this->FindDevices();
	if (!(bool)n_devs) { std::cout << "No valid devices found!" << std::endl; }
	else { std::cout << std::to_string(n_devs) + " devices found..." << std::endl; }

	bool selected = this->SelectDevice();
	if (!selected) { std::cout << "Failed to select camera!" << std::endl; }
	else { std::cout << "Camera selected..." << std::endl; }

	bool configured = this->ConfigureDevice(show_conf);
	if (!configured) { std::cout << "Failed to configure camera!" << std::endl; }
	else { std::cout << "Camera configured..." << std::endl; }

	bool camOK = this->TestCam();
	if (!camOK) { std::cout << "Camera failed acquisition test!" << std::endl; }
	else { std::cout << "Acquisition test OK! Camera is ready to use!" << std::endl; }

	return camOK;
}

void IngaasCamera::AcquireCorrectionData(std::string filename)
{
	const char* filename_c = filename.c_str();
	cv::Mat* data = this->CaptureAveragedFrame(100, 10);
	this->SaveRawImageToFile(*data, filename_c);
}

void IngaasCamera::ShowCalFrame(std::string filename)
{
	const char* filename_c = filename.c_str();
	cv::Mat frame = this->LoadCorrectionFrame(filename_c);
	this->DisplayFrame(frame);
}

void IngaasCamera::LoadDarkCurrentCorrectionFile(std::string filename)
{
	this->loadedDarkCurrent = false;
	const char* filename_c = filename.c_str();
	this->dark_current.release();
	this->dark_current = this->LoadCorrectionFrame(filename_c);
	this->loadedDarkCurrent = true;
}

void IngaasCamera::LoadBadPixelCorrectionFile(std::string filename)
{
	this->loadedBadPixels = false;
	const char* filename_c = filename.c_str();
	this->bad_pixels.release();
	this->bad_pixels = this->LoadCorrectionFrame(filename_c);

	// Create mask from bad pixels data
	this->bad_pixels_mask.release();
	this->bad_pixels.convertTo(this->bad_pixels_mask, CV_8U, 0.00390625);
	cv::inRange(this->bad_pixels_mask, this->mask_thr_low, this->mask_thr_high, this->bad_pixels_mask);
	cv::bitwise_not(this->bad_pixels_mask, this->bad_pixels_mask);

	this->loadedBadPixels = true;
}

void IngaasCamera::SetCorrections(bool dark_current, bool bad_pixels, double contrast_enhancement, int pos_smooth)
{
	this->correctDarkCurrent = dark_current;
	this->correctBadPixels = bad_pixels;
	this->contrast_enhancement_factor = contrast_enhancement_factor;
	this->pos_smooth_radius = (int)(2 * floor((float)pos_smooth / 2.0f) + 1);
}

void IngaasCamera::SetCorrectionRadius(int radius)
{
	if (radius > 2)
	{
		this->correction_radius = (int)(2*floor((float)radius/2.0f) + 1);
	}
	else
	{
		this->correction_radius = 3;
	}
	
}

void IngaasCamera::SetMaskThresholds(int low, int high)
{
	this->mask_thr_low = low;
	this->mask_thr_high = high;
}

void IngaasCamera::DisplayRawFrame()
{
	this->GrabRawFrame();
	this->DisplayFrame(this->cv_buffer);
}

void IngaasCamera::DisplayCorrectedFrame()
{
	this->GrabCorrectedFrame();
	this->DisplayFrame(this->cv_buffer);
}

cv::Mat* IngaasCamera::CaptureRawFrame()
{
	this->GrabRawFrame();
	return &this->cv_buffer;
}

cv::Mat* IngaasCamera::CaptureCorrectedFrame()
{
	this->GrabCorrectedFrame();
	return &this->cv_buffer;
}

cv::Mat* IngaasCamera::CaptureAveragedFrame(int n, unsigned int interval_ms)
{
	cv::Mat accumulator(this->cv_size, CV_64FC1);

	for (int i = 0; i < n; i++)
	{
		this->GrabRawFrame();
		cv::accumulate(this->cv_buffer, accumulator);

		Sleep(interval_ms);
	}

	accumulator.convertTo(this->cv_buffer, CV_16UC1, (double)1.0 / (double)n);

	return &this->cv_buffer;
}