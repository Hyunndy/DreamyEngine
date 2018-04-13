#pragma once

#include "D3D.h"
#include "FBXCameraBuffer.h"
#include "FBXSunBuffer.h"
#include "FBXWorldBuffer.h"

class FBXShader
{

public:
	static void CreateBuffer();
	static void DeleteBuffer();

protected:
	FBXShader(wstring ShaderFile);
	~FBXShader();

	void CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* desc, UINT count);

	virtual void Update();
	virtual void Render();

	ID3D10Blob* vertexBlob;
	ID3D10Blob* pixelBlob;

	FBXWorldBuffer* worldBuffer;
	FBXCameraBuffer* cameraBuffer;

private:
	static FBXSunBuffer* sunBuffer;

	void CreateVertexShader();
	void CreatePixelShader();
	void CheckShaderError(HRESULT hr, ID3D10Blob* error);

	wstring ShaderFile;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;

	ID3D11InputLayout* layout;
};