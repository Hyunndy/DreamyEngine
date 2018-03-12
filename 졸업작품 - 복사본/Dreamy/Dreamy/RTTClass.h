#pragma once
#ifndef _RTTCLASS_H_
#define _RTTCLASS_H_

#include <d3d11.h>
/*
	RTTClass
	- DirectX11�� RTT����� ĸ��ȭ�� Ŭ����.
	- RTT��?
	  ����۰� �ƴ� �ؽ�ó���� ����� �׸� �� �ְ� �ϴ� ����� �����ϴ� ���
	  �ſ�, TVȭ�� �� ����� �׸� �� �ִ�.

	- ����� ��� �ؽ�ó�� ���� Ÿ������ ������ �� �ְ� �Ѵ�.
	- ���� �������� �����͸� ID3D11ShaderResourceView �������� ������ �� �ִ�.
*/



class RTTClass
{
public:
	RTTClass();
	RTTClass(const RTTClass&);
	~RTTClass();

	bool Initialize(ID3D11Device*, int, int);
	void Shutdown();
	

	void SetRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*);
	void ClearRenderTarget(ID3D11DeviceContext*, ID3D11DepthStencilView*, float, float, float, float);

	ID3D11ShaderResourceView* GetShaderResourceView();

private: 
	ID3D11Texture2D* m_renderTargetTexture;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_shaderResourceView;


};

#endif

