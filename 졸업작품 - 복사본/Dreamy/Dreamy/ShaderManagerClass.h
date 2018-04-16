#pragma once

#include "D3D.h"
#include "TextureShaderClass.h"
#include "SkyDomeShaderClass.h"
#include "SkyPlaneShaderClass.h"
#include "InstancingShaderClass.h"
//#include "LightShaderClass.h"
//#include "MultiTextureShaderClass.h"
//#include "AlphaMapShaderClass.h"
//#include "BumpMapShaderClass.h"
//#include "SpecMapShaderClass.h"
//#include "ShaderManagerClass.h"
//#include "FogShaderClass.h"
#include "TranslateShaderClass.h"
//#include "TransparentShaderClass.h"
//#include "ColorShaderClass.h"

//#include "RefractionShaderClass.h"
//#include "ReflectionShaderClass.h"
//#include "WaterShaderClass.h"
//#include "WaterReflectionShaderClass.h"
//#include "FireShaderClass.h"

#include "ParticleShaderClass.h"

class ShaderManagerClass
{
public:
	ShaderManagerClass();
	ShaderManagerClass(const ShaderManagerClass&);
	~ShaderManagerClass();

	//bool Initialize(ID3D11Device*, HWND);
	bool Initialize(HWND);
	void Shutdown();

	//bool RenderColorShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

	//bool RenderTextureShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	bool RenderTextureShader( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);
	//bool RenderLightShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
	//	D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

	//bool RenderMultiTextureShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

	//bool RenderAlphaMapShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

	//bool RenderBumpMapShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4);

	//bool RenderSpecMapShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

	//bool RenderFogShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float, float);

	bool RenderTranslateShader( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float, float);

	//bool RenderTransparentShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float);

	//bool RenderReflectionShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXMATRIX);

	//bool RenderRefractionShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR4);

	//bool RenderWaterShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
	//	/*ID3D11ShaderResourceView*,*/ D3DXVECTOR3, D3DXVECTOR2, float, float, D3DXVECTOR4, D3DXVECTOR3, float);

	//bool RenderWaterReflectionShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXVECTOR4,
	//	D3DXVECTOR3, float, D3DXVECTOR4);

	bool RenderSkydomeShader( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);

	bool RenderCloudShader(int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float, float, float, float);

	//bool RenderFireShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
	//	float, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR2, D3DXVECTOR2, D3DXVECTOR2, float, float);

	bool RenderInstancingShader( int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

	bool RenderParticleShader( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

	TextureShaderClass* m_TextureShader = nullptr;

private:
	//ColorShaderClass* m_ColorShader = nullptr;
	//LightShaderClass* m_LightShader = nullptr;
	//MultiTextureShaderClass* m_MultiTextureShader = nullptr;
	//AlphaMapShaderClass* m_AlphaMapShader = nullptr;
	//BumpMapShaderClass* m_BumpMapShader = nullptr;
	//SpecMapShaderClass* m_SpecMapShader = nullptr;
	//FogShaderClass* m_FogShader = nullptr;
	TranslateShaderClass* m_TranslateShader = nullptr;
	//TransparentShaderClass* m_TransparentShader = nullptr;
	SkyDomeShaderClass* m_SkydomeShader = nullptr;
	SkyPlaneShaderClass* m_CloudShader = nullptr;
	//ReflectionShaderClass* m_ReflectionShader = nullptr;
	//RefractionShaderClass* m_RefractionShader = nullptr;
	//WaterShaderClass* m_WaterShader = nullptr;
	//WaterReflectionShaderClass* m_WaterReflectionShader = nullptr;
	//FireShaderClass* m_FireShader = nullptr;
	InstancingShaderClass* m_InstancingShader = nullptr;
	ParticleShaderClass* m_particleShader = nullptr;


};

