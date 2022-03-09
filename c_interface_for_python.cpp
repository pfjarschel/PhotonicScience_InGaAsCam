#include "IngaasCamera.h"

extern "C"
{
	__declspec(dllexport) IngaasCamera* InitLib()
	{
		return new IngaasCamera();
	}
	__declspec(dllexport) void CloseCamera(IngaasCamera* cam)
	{
		delete cam;
	}

	__declspec(dllexport) void SetSelectMode(IngaasCamera* cam, int mode = 3)
	{
		cam->SetSelectMode(mode);
	}
	__declspec(dllexport) int GetSelectMode(IngaasCamera* cam)
	{
		return cam->GetSelectMode();
	}

	__declspec(dllexport) void SetConnectionIP(IngaasCamera* cam, char* ip)
	{
		cam->SetConnectionIP(ip);
	}
	__declspec(dllexport) char* GetConnectionIP(IngaasCamera* cam)
	{
		std::string ip_str = cam->GetConnectionIP();
		const std::string::size_type size = ip_str.size();
		char* ip = new char[size + 1];
		memcpy(ip, ip_str.c_str(), size + 1);
		return ip;
	}

	__declspec(dllexport) void SetCameraIP(IngaasCamera* cam, char* ip)
	{
		cam->SetCameraIP(ip);
	}
	__declspec(dllexport) char* GetCameraIP(IngaasCamera* cam)
	{
		std::string ip_str = cam->GetCameraIP();
		const std::string::size_type size = ip_str.size();
		char* ip = new char[size + 1];
		memcpy(ip, ip_str.c_str(), size + 1);
		return ip;
	}

	__declspec(dllexport) void SetConnectionMAC(IngaasCamera* cam, char* mac)
	{
		cam->SetConnectionMAC(mac);
	}
	__declspec(dllexport) char* GetConnectionMAC(IngaasCamera* cam)
	{
		std::string mac_str = cam->GetConnectionMAC();
		const std::string::size_type size = mac_str.size();
		char* mac = new char[size + 1];
		memcpy(mac, mac_str.c_str(), size + 1);
		return mac;
	}

	__declspec(dllexport) void SetCameraSubnet(IngaasCamera* cam, char* subnet)
	{
		cam->SetCameraSubnet(subnet);
	}
	__declspec(dllexport) char* GetCameraSubnet(IngaasCamera* cam)
	{
		std::string subn_str = cam->GetCameraSubnet();
		const std::string::size_type size = subn_str.size();
		char* subn = new char[size + 1];
		memcpy(subn, subn_str.c_str(), size + 1);
		return subn;
	}

	__declspec(dllexport) bool CheckIfFoundDevices(IngaasCamera* cam)
	{
		return cam->CheckIfFoundDevices();
	}
	__declspec(dllexport) bool CheckIfCamConfigured(IngaasCamera* cam)
	{
		return cam->CheckIfCamConfigured();
	}
	__declspec(dllexport) bool CheckIfCamOK(IngaasCamera* cam)
	{
		return cam->CheckIfCamOK();
	}

	__declspec(dllexport) int* GetCaptureDimensions(IngaasCamera* cam)
	{
		int size[2] = { cam->GetCaptureDimensions().width, cam->GetCaptureDimensions().height};
		return size;
	}

	__declspec(dllexport) int GetBitDepth(IngaasCamera* cam)
	{
		return (int)cam->GetBitDepth();
	}

	__declspec(dllexport) bool OpenCamConfig(IngaasCamera* cam)
	{
		return cam->OpenCamConfig();
	}

	__declspec(dllexport) void DisplayLastFrame(IngaasCamera* cam)
	{
		cam->DisplayLastFrame();
	}

	__declspec(dllexport) void InitializeCam(IngaasCamera* cam, bool show_conf = false)
	{
		cam->InitializeCam(show_conf);
	}

	__declspec(dllexport) void AcquireCorrectionData(IngaasCamera* cam, char* filename)
	{
		cam->AcquireCorrectionData(filename);
	}

	__declspec(dllexport) void ShowCalFrame(IngaasCamera* cam, char* filename)
	{
		cam->ShowCalFrame(filename);
	}
	
	__declspec(dllexport) void LoadDarkCurrentCorrectionFile(IngaasCamera* cam, char* filename)
	{
		cam->LoadDarkCurrentCorrectionFile(filename);
	}

	__declspec(dllexport) void LoadBadPixelCorrectionFile(IngaasCamera* cam, char* filename)
	{
		cam->LoadBadPixelCorrectionFile(filename);
	}

	__declspec(dllexport) void SetCorrections(IngaasCamera* cam, bool dark_current, bool bad_pixels, double contrast_enhancement = 1.0, int pos_smooth = 1)
	{
		cam->SetCorrections(dark_current, bad_pixels, contrast_enhancement, pos_smooth);
	}

	__declspec(dllexport) void SetCorrectionRadius(IngaasCamera* cam, int radius)
	{
		cam->SetCorrectionRadius(radius);
	}

	__declspec(dllexport) void SetMaskThresholds(IngaasCamera* cam, int low, int high)
	{
		cam->SetMaskThresholds(low, high);
	}
	
	__declspec(dllexport) void DisplayRawFrame(IngaasCamera* cam)
	{
		cam->DisplayRawFrame();
	}

	__declspec(dllexport) void DisplayCorrectedFrame(IngaasCamera* cam)
	{
		cam->DisplayCorrectedFrame();
	}

	__declspec(dllexport) unsigned short* CaptureRawFrame(IngaasCamera* cam)
	{
		return (unsigned short*)((void*)cam->CaptureRawFrame()->data);
	}

	__declspec(dllexport) unsigned short* CaptureCorrectedFrame(IngaasCamera* cam)
	{
		return (unsigned short*)((void*)cam->CaptureCorrectedFrame()->data);
	}

	__declspec(dllexport) unsigned short* CaptureAveragedFrame(IngaasCamera* cam, int n, unsigned int interval_ms = 0)
	{
		return (unsigned short*)((void*)cam->CaptureAveragedFrame(n, interval_ms)->data);
	}
}; 