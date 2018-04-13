
#include "Sampler.h"
#include <assert.h>

Sampler* Sampler::instance = NULL;

Sampler::Sampler()
{
	HRESULT hr;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = D3D::GetDevice()->CreateSamplerState(&samplerDesc, &defaultState);
	assert(SUCCEEDED(hr));
}

Sampler::~Sampler()
{

}

Sampler * Sampler::Get()
{
	if (instance == NULL)
		instance = new Sampler();

	return instance;
}

void Sampler::Delete()
{
	SAFE_DELETE(instance);
}

void Sampler::SetDefault()
{
	D3D::GetDeviceContext()->PSSetSamplers(0, 1, &defaultState);
}

