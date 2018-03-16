#pragma once
#include "ShaderClass.h"

/*
이름: RefractionShaderClass(굴절 셰이더)
용도:
- 굴절 셰이더는 라이트 셰이더(정반사광(=Specular light)이 없는)에 클리핑 평면이 추가된 형태이다.
- 굴절이 정상적으로 씬을 렌더링 하지만 물 아래있는 것들만 그려내야 하기 때문에 클리핑 평면을 사용하여 해결해야 한다.
*/


class RefractionShaderClass : public ShaderClass
{
private:
	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

	struct ClipPlaneBufferType
	{
		D3DXVECTOR4 clipPlane;
	};

public:
	RefractionShaderClass();
	RefractionShaderClass(const RefractionShaderClass&);
	~RefractionShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void ShutDown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4,D3DXVECTOR4, D3DXVECTOR4);

private:
	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3,
		D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR4);

private:
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_clipPlaneBuffer;

};

