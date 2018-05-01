#pragma once

#include "../Dreamy/System/D3D.h"
#include "../Dreamy/FBXAnimation/FBXCameraBuffer.h"
#include "../Dreamy/FBXAnimation/FBXSunBuffer.h"
#include "../Dreamy/FBXAnimation/FBXWorldBuffer.h"

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