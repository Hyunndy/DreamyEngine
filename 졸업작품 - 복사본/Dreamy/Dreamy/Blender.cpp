
#include "Blender.h"

Blender* Blender::instance = NULL;

Blender::Blender()
{
	HRESULT hr;

	D3D11_BLEND_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));

	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = 0x0F;

	hr = D3D::GetDevice()->CreateBlendState(&desc, &linearState);
	//assert(SUCCEEDED(hr));


	desc.RenderTarget[0].BlendEnable = FALSE;
	hr = D3D::GetDevice()->CreateBlendState(&desc, &offState);
	//assert(SUCCEEDED(hr));

	desc.RenderTarget[0].BlendEnable = TRUE;
	desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	desc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	hr = D3D::GetDevice()->CreateBlendState(&desc, &AddState);
}

Blender::~Blender()
{
	SAFE_RELEASE(offState);
	SAFE_RELEASE(linearState);
}

Blender * Blender::Get()
{
	if (instance == NULL)
		instance = new Blender();

	return instance;
}

void Blender::Delete()
{
	SAFE_DELETE(instance);
}

void Blender::SetLinear()
{
	D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 0);

	D3D::GetDeviceContext()->OMSetBlendState
	(
		linearState, (float *)&color, 0xFFFFFFFF
	);
}

void Blender::SetOff()
{
	D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 0);

	D3D::GetDeviceContext()->OMSetBlendState
	(
		offState, (float *)&color, 0xFFFFFFFF
	);
}

void Blender::SetAdd()
{
	D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 0);

	D3D::GetDeviceContext()->OMSetBlendState
	(
		AddState, (float *)&color, 0xFFFFFFFF
	);
}
