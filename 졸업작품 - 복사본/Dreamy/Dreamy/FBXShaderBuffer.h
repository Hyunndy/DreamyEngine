#pragma once

#include "D3D.h"

class ShaderBuffer
{

public:
	void SetVSBuffer(UINT slot)
	{
		D3D::GetDeviceContext()->VSSetConstantBuffers(slot, 1, &buffer);
	}

	void SetPSBuffer(UINT slot)
	{
		D3D::GetDeviceContext()->PSSetConstantBuffers(slot, 1, &buffer);
	}

	virtual void Update() = 0;

protected:
	ShaderBuffer(UINT dataSize)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = dataSize;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer);
		////////////////		assert(SUCCEEDED(hr));
	}

	~ShaderBuffer()
	{
		SAFE_RELEASE(buffer);
	}

	void UpdateBuffer(void* data, UINT dataSize)
	{
		D3D11_MAPPED_SUBRESOURCE subResource;

		HRESULT hr = D3D::GetDeviceContext()->Map
		(
			buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource
		);

		memcpy(subResource.pData, data, dataSize);

		D3D::GetDeviceContext()->Unmap(buffer, 0);
	}

private:
	D3D11_BUFFER_DESC desc;
	ID3D11Buffer* buffer;
};