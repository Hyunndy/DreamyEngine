#pragma once

#include"stdafx.h"
#include <d3dx11async.h>
#include <fstream>
using namespace std;

#include "ShaderManagerClass.h"
#include "TextureShaderClass.h"
#include "LightShaderClass.h"
#include "MultiTextureShaderClass.h"
#include "AlphaMapShaderClass.h"
#include "BumpMapShaderClass.h"
#include "SpecMapShaderClass.h"
#include "ShaderManagerClass.h"
#include "FogShaderClass.h"
#include "TranslateShaderClass.h"
#include "TransparentShaderClass.h"
#include "ReflectionShaderClass.h"
#include "ColorShaderClass.h"
#include "SkyDomeShaderClass.h"
#include "SkyPlaneShaderClass.h"


class ShaderManagerClass
{
public:
	ShaderManagerClass();
	ShaderManagerClass(const ShaderManagerClass&);
	~ShaderManagerClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();

	bool RenderColorShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX);

	bool RenderTextureShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*);

	bool RenderLightShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
		D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

	bool RenderMultiTextureShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

	bool RenderAlphaMapShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**);

	bool RenderBumpMapShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4);

	bool RenderSpecMapShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float);

	bool RenderFogShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float, float);

	bool RenderTranslateShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView**, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR4, float, float);

	bool RenderTransparentShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, float);

	bool RenderReflectionShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, D3DXMATRIX);

	bool RenderSkydomeShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);

	bool RenderCloudShader(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float, float,
		float, float);

private:
	ColorShaderClass* m_ColorShader = nullptr;
	TextureShaderClass* m_TextureShader = nullptr;
	LightShaderClass* m_LightShader = nullptr;
	MultiTextureShaderClass* m_MultiTextureShader = nullptr;
	AlphaMapShaderClass* m_AlphaMapShader = nullptr;
	BumpMapShaderClass* m_BumpMapShader = nullptr;
	SpecMapShaderClass* m_SpecMapShader = nullptr;
	FogShaderClass* m_FogShader = nullptr;
	TranslateShaderClass* m_TranslateShader = nullptr;
	TransparentShaderClass* m_TransparentShader = nullptr;
	ReflectionShaderClass* m_ReflectionShader = nullptr;
	SkyDomeShaderClass* m_SkydomeShader = nullptr;
	SkyPlaneShaderClass* m_CloudShader = nullptr;

};

