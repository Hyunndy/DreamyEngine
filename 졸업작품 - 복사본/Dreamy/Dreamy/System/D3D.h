#pragma once
//////////////////////////////////////////////////////////////////////////////////
//// Filename: d3dclass.h
//////////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////
//// LINKING //
///////////////
///*
//�� ���̺귯������ 
//DirectX�� ����, 3D�׷����� ������, ���ΰ�ħ ����(FPS) ���, ����ϴ� �׷���ī���� ���� ��� ���� �ϵ������� ����� ���� �ʿ��� ��� Direct3D�� ��ɵ��� ��� �ִ�.
//DX10�� ���̺귯���� �״�� ���̴°� �ش� ���̺귯���� �ٲ� �ʿ䰡 ���� �����̴�.
//*/
//#pragma comment(lib, "dxgi.lib")
//#pragma comment(lib, "d3d11.lib")
//#pragma comment(lib, "d3dx11.lib")
//#pragma comment(lib, "d3dx10.lib")
//
//
////////////////
//// INCLUDES //
////////////////
//#include <string>
//#include <dxgi.h>
//#include <d3dcommon.h>
//#include "stdafx.h"

#include "../FBXAnimation/stdafx_fbx.h"
#include "DepthStencil.h"

struct D3DInfo
{
	wstring appName;
	HINSTANCE instance;
	HWND mainHandle;
	UINT screenWidth;
	UINT screenHeight;
	bool isVsync;
	bool isFullScreen;
	D3DXCOLOR clearColor;
};

class D3D
{
public:
	static D3D* Get();
	static void Delete();

	static ID3D11Device* GetDevice()
	{
		return device;
	}

	static ID3D11DeviceContext* GetDeviceContext()
	{
		return deviceContext;
	}

	static void GetInfo(D3DInfo* info)
	{
		*info = d3dInfo;
	}

	static void SetInfo(D3DInfo info)
	{
		d3dInfo = info;
	}

	IDXGISwapChain* GetSwapChain() { return swapChain; }

	void BeginScene();
	void EndScene();

private:
	D3D();
	~D3D();

	void CreateAdapter();
	void CreateSwapChain();

	static D3D* instance;
	static ID3D11Device* device;
	static ID3D11DeviceContext* deviceContext;
	static D3DInfo d3dInfo;


	D3D_FEATURE_LEVEL version;

	bool isVsync;
	UINT gpuMemorySize;
	wstring gpuDescription;

	UINT numerator;
	UINT denominator;

	IDXGISwapChain* swapChain;
};