#pragma once
#include "stdafx_fbx.h"
#include "D3D.h"

class DepthStencil
{
public:
	static DepthStencil* Get();
	static void Delete();

	void SetOnState();
	void SetOffState();

	void SetDefaultRenderView();

	ID3D11DepthStencilView* GetDepthView()
	{
		return depthView;
	}

	ID3D11RenderTargetView* GetRenderView()
	{
		return renderView;
	}

private:
	void CreateTexture();
	void CreateOnState();
	void CreateOffState();
	void CreateRenderTarget();
	void CreateView();

	static DepthStencil* instance;

	ID3D11Texture2D* texture;
	ID3D11DepthStencilView* depthView;
	ID3D11RenderTargetView* renderView;

	ID3D11DepthStencilState* onState;
	ID3D11DepthStencilState* offState;

	DepthStencil();
	~DepthStencil();
};