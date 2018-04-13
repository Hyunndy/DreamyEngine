#include "DepthStencil.h"

DepthStencil* DepthStencil::instance = NULL;

DepthStencil::DepthStencil()
{
	CreateTexture();
	CreateOnState();
	CreateOffState();
	CreateRenderTarget();
	CreateView();
}

DepthStencil::~DepthStencil()
{
	SAFE_RELEASE(onState);
	SAFE_RELEASE(offState);
	SAFE_RELEASE(texture);
	SAFE_RELEASE(depthView);
	SAFE_RELEASE(renderView);
}

DepthStencil * DepthStencil::Get()
{
	if (instance == NULL)
		instance = new DepthStencil();

	return instance;
}

void DepthStencil::Delete()
{
	SAFE_DELETE(instance);
}

void DepthStencil::SetOnState()
{
	D3D::GetDeviceContext()->OMSetDepthStencilState(onState, 1);
}

void DepthStencil::SetOffState()
{
	D3D::GetDeviceContext()->OMSetDepthStencilState(offState, 1);
}

void DepthStencil::SetDefaultRenderView()
{
	D3D::GetDeviceContext()->OMSetRenderTargets(1, &renderView, depthView);
}

void DepthStencil::CreateTexture()
{
	D3DInfo info;
	D3D::GetInfo(&info);

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = info.screenWidth;
	desc.Height = info.screenHeight;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	//	//assert(SUCCEEDED(hr));
}

void DepthStencil::CreateOnState()
{
	D3D11_DEPTH_STENCIL_DESC desc = { 0 };
	desc.DepthEnable = true;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;

	desc.StencilEnable = true;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;

	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateDepthStencilState(&desc, &onState);
	////assert(SUCCEEDED(hr));

	D3D::GetDeviceContext()->OMSetDepthStencilState(onState, 1);
}

void DepthStencil::CreateOffState()
{
	D3D11_DEPTH_STENCIL_DESC desc = { 0 };
	desc.DepthEnable = false;
	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	desc.DepthFunc = D3D11_COMPARISON_LESS;
	desc.StencilEnable = true;
	desc.StencilReadMask = 0xFF;
	desc.StencilWriteMask = 0xFF;
	desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	HRESULT hr;
	hr = D3D::GetDevice()->CreateDepthStencilState(&desc, &offState);
	//assert(SUCCEEDED(hr));
}

void DepthStencil::CreateRenderTarget()
{
	ID3D11Texture2D* backbufferPointer;
	HRESULT hr = D3D::Get()->GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backbufferPointer);
	//assert(SUCCEEDED(hr));

	hr = D3D::GetDevice()->CreateRenderTargetView(backbufferPointer, NULL, &renderView);
	//assert(SUCCEEDED(hr));
	SAFE_RELEASE(backbufferPointer);
}

void DepthStencil::CreateView()
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	HRESULT hr = D3D::GetDevice()->CreateDepthStencilView(texture, &desc, &depthView);
	//assert(SUCCEEDED(hr));

	D3D::GetDeviceContext()->OMSetRenderTargets(1, &renderView, depthView);
}
