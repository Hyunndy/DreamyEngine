#pragma once

#include <d3d11.h>
#include <D3DX10math.h>

#include "TextureClass.h"
/*
이름: WaterClass
용도: 호수의 실제 물리 면적을 나타내는 클래스.


*/

class WaterClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};
public:
	WaterClass();
	WaterClass(const WaterClass&);
	~WaterClass();


	bool Initialize(ID3D11Device*, WCHAR*, float, float);
	//bool Initialize(ID3D11Device*, WCHAR*, float);
	void Shutdown();
	void Frame(int);
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	float GetWaterHeight();
	D3DXVECTOR2 GetNormalMapTiling();
	float GetWaterTranslation();
	float GetReflectRefractScale();
	D3DXVECTOR4 GetRefractionTint();
	float GetSpecularShininess();

private:
	bool InitializeBuffers(ID3D11Device*, float);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	float m_waterHeight;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	D3DXVECTOR2 m_normalMapTiling;
	float m_waterTranslation;
	float m_reflectRefractScale;
	D3DXVECTOR4 m_refractionTint;
	float m_specularShininess;
};

