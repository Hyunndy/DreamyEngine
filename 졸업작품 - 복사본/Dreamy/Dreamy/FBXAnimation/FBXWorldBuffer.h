#pragma once
#include "FBXShaderBuffer.h"

class FBXWorldBuffer : public ShaderBuffer
{
public:
	FBXWorldBuffer()
		: ShaderBuffer(sizeof(Data))
	{
		D3DXMatrixIdentity(&data.world);
	}


	void SetWorld(D3DXMATRIX& world)
	{
		data.world = world;
	}

	void Update()
	{
		Data temp;
		D3DXMatrixTranspose(&temp.world, &data.world);

		UpdateBuffer(&temp, sizeof(Data));
	}

	struct Data
	{
		D3DXMATRIX world;
	};

private:
	Data data;
};