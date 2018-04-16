#pragma once
#include "FBXShaderBuffer.h"

class FBXSunBuffer : public ShaderBuffer
{
public:
	FBXSunBuffer()
		: ShaderBuffer(sizeof(Data))
	{
		data.ambientColor= D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		data.diffuseColor = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		data.direction = D3DXVECTOR3(0, -1, -0.5);
		data.padding = 0;
	}

	void SetDirection(D3DXVECTOR3& direction)
	{
		data.direction = direction;
	}

	void Update()
	{
		UpdateBuffer(&data, sizeof(Data));
	}

	struct Data
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 direction;
		float padding;
	};

private:
	Data data;
};
