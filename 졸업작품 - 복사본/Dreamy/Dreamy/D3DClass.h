//////////////////////////////////////////////////////////////////////////////////
//// Filename: d3dclass.h
//////////////////////////////////////////////////////////////////////////////////
//#ifndef _D3DCLASS_H_
//#define _D3DCLASS_H_
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
//#include <dxgi.h>
//#include <d3dcommon.h>
//#include "stdafx.h"
//
//
///*--------------------------------------------------------------------------------------------------------------------------------------
//수정사항
//- 18.01.23 2D이미지 렌더링을 위해 Z버퍼를 켜고 끌 수 있도록 수정됨.
//- 18.01.23 Text렌더링을 위해 블렌딩 상태를 통합하는 함수가 추가됨
//		   블렌딩(blending) : 글자가 3D물체들을 배경으로 조화롭게 그려지게 한다.
//- 18.01.30 RTT기능을 사용할 수 있는 함수들이 추가됨.
//- 추가적인 블렌딩할 수 있게 함
//--------------------------------------------------------------------------------------------------------------------------------------*/
//
//////////////////////////////////////////////////////////////////////////////////
//// Class name: D3DClass
//////////////////////////////////////////////////////////////////////////////////
//
//class D3DClass
//{
//public:
//	D3DClass();
//	D3DClass(const D3DClass&);
//	~D3DClass();
//
//	bool Initialize(int, int, bool, HWND, bool, float, float);
//	void Shutdown();
//
//	void BeginScene(float, float, float, float);
//	void EndScene();
//
//	ID3D11Device* GetDevice();
//	ID3D11DeviceContext* GetDeviceContext();
//
//	void GetProjectionMatrix(D3DXMATRIX&);
//	void GetWorldMatrix(D3DXMATRIX&);
//	void GetOrthoMatrix(D3DXMATRIX&);
//
//	void GetVideoCardInfo(char*, int&);
//
//	// Z버퍼를 켜고 끄는 함수
//	void TurnZBufferOn();
//	void TurnZBufferOff();
//
//	// 알파블렌딩을 켜고 끄는 함수
//	void TurnOnAlphaBlending();
//	void TurnOffAlphaBlending();
//
//	// 후면 컬링을 켜고 끄는 함수
//	void TurnOnCulling();
//	void TurnOffCulling();
//
//	//additive blending
//	void EnableSecondBlendState();
//
//	// RTT기능을 쓸 수 있게 해주는 함수.
//	ID3D11DepthStencilView* GetDepthStencilView();
//	void SetBackBufferRenderTarget();
//	void ResetViewport();
//
//	
//private:
//
//	bool m_vsync_enabled;
//	int m_videoCardMemory;
//	char m_videoCardDescription[128];
//
//	IDXGISwapChain* m_swapChain;
//
//	ID3D11Device* m_device;
//	ID3D11DeviceContext* m_deviceContext;
//
//	ID3D11RenderTargetView* m_renderTargetView;
//	ID3D11Texture2D* m_depthStencilBuffer;
//
//	//z버퍼를 켜고 끄기 위한 깊이-스텐실
//	ID3D11DepthStencilState* m_depthStencilState;
//	ID3D11DepthStencilState* m_depthDisabledStencilState;
//
//	ID3D11DepthStencilView* m_depthStencilView;
//
//
//	D3DXMATRIX m_projectionMatrix;
//	D3D11_VIEWPORT m_viewport;
//	D3DXMATRIX m_worldMatrix;
//	D3DXMATRIX m_orthoMatrix;
//
//	//블렌딩 상태를 표현하는 변수
//	ID3D11BlendState* m_alphaEnableBlendingState;
//	ID3D11BlendState* m_alphaDisableBlendingState;
//
//
//	//additive blending for cloud
//	ID3D11BlendState * m_alphaBlendState2;
//
//	//래스터라이즈 상태 후면 컬링 켜고 끄는것.
//	ID3D11RasterizerState* m_rasterState;	
//	ID3D11RasterizerState* m_rasterStateNoCulling;
//};
//
//#endif