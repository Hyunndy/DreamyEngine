#pragma once
#include "FBXCamera.h"
#include "FBXShaderBuffer.h"

class FBXCameraBuffer : public ShaderBuffer
{
public:
	FBXCameraBuffer()
		: ShaderBuffer(sizeof(Data))
		, useCameraMatrix(true)
	{

	}
	~FBXCameraBuffer()
	{
	}

	void Update()
	{
		if (useCameraMatrix == true)
		{
			Camera::Get()->GetView(&data.view);
			Camera::Get()->GetProjection(&data.projection);
		}

		Data temp;
		D3DXMatrixTranspose(&temp.view, &data.view);
		D3DXMatrixTranspose(&temp.projection, &data.projection);

		UpdateBuffer(&temp, sizeof(Data));
	}

	void SetUserMatrix()
	{
		useCameraMatrix = false;
	}

	void SetMatrix(D3DXMATRIX& view, D3DXMATRIX& projection)
	{
		memcpy(&data.view, &view, sizeof(D3DXMATRIX));
		memcpy(&data.projection, &projection, sizeof(D3DXMATRIX));
	}

	struct Data
	{
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

private:
	Data data;

	bool useCameraMatrix;
};
