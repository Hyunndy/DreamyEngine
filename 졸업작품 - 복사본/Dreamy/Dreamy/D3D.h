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
//이 라이브러리들은 
//DirectX의 시작, 3D그래픽의 렌더링, 새로고침 비율(FPS) 얻기, 사용하는 그래픽카드의 정보 등과 같은 하드웨어로의 통신을 위해 필요한 모든 Direct3D의 기능등을 담고 있다.
//DX10의 라이브러리가 그대로 쓰이는건 해당 라이브러리는 바뀔 필요가 없기 때문이다.
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

#include "stdafx_fbx.h"
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