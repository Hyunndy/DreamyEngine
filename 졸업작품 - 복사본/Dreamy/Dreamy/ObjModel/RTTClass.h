//#pragma once
//#ifndef _RTTCLASS_H_
//#define _RTTCLASS_H_
//
//#include <d3d11.h>
///*
//	RTTClass
//	- DirectX11의 RTT기능을 캡슐화한 클래스.
//	- RTT란?
//	  백버퍼가 아닌 텍스처에도 장면을 그릴 수 있게 하는 방법을 제공하는 기능
//	  거울, TV화면 등 장면을 그릴 수 있다.
//
//	- 백버퍼 대신 텍스처를 렌더 타겟으로 설정할 수 있게 한다.
//	- 또한 렌더링된 데이터를 ID3D11ShaderResourceView 형식으로 가져올 수 있다.
//*/
//
//
////
////class RTTClass
////{
////public:
////	RTTClass();
////	RTTClass(const RTTClass&);
////	~RTTClass();
//
//	bool Initialize(ID3D11Device*, int, int);
//	void Shutdown();
//	
//
//	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
//	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);
//
//	ID3D11ShaderResourceView* GetShaderResourceView();
//
//private: 
//	ID3D11Texture2D* m_renderTargetTexture;
//	ID3D11RenderTargetView* m_renderTargetView;
//	ID3D11ShaderResourceView* m_shaderResourceView;
//
//
//};
//
//#endif
//
//