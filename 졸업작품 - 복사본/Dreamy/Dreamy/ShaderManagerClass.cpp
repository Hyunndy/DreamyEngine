#include "ShaderManagerClass.h"


ShaderManagerClass::ShaderManagerClass()
{
	m_TextureShader = 0;
	m_LightShader = 0;
	m_MultiTextureShader = 0;
	m_AlphaMapShader = 0;
	m_BumpMapShader = 0;
	m_SpecMapShader = 0;
	m_FogShader = 0;
	m_TranslateShader = 0;
	m_TransparentShader = 0;
	m_ReflectionShader = 0;
	m_RefractionShader = 0;
	m_WaterShader = 0;
	m_WaterReflectionShader = 0;
	m_ColorShader = 0;
	m_SkydomeShader = 0;
	m_CloudShader = 0;
	m_FireShader = 0;

}

ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{
}

ShaderManagerClass::~ShaderManagerClass()
{
}

bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	//컬러 쉐이더 객체 생성
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader) { return false; }

	bool result = m_ColorShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);	return false;
	}
	// 텍스처 쉐이더 객체를 생성한다.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 초기화한다.
	result = m_TextureShader->Initialize(device, hwnd);
	if (!result)
	{	MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);	return false;	}

	// 라이트 쉐이더 객체를 만듭니다.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{	return false;}

	// 라이트 쉐이더 객체를 초기화합니다.
	result = m_LightShader->Initialize(device, hwnd);
	if (!result)
	{ MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK); return false; }

	//멀티텍스쳐셰이더
	m_MultiTextureShader = new MultiTextureShaderClass;
	if (!m_MultiTextureShader) { return false; }

	result = m_MultiTextureShader->Initialize(device, hwnd);
	if(!result)
	{ MessageBox(hwnd, L"Could not initialize the multitextureshader object.", L"Error", MB_OK); return false; }

	m_AlphaMapShader = new AlphaMapShaderClass;
	if (!m_AlphaMapShader) { return false; }
	
	result = m_AlphaMapShader->Initialize(device, hwnd);
	if(!result)
	{ MessageBox(hwnd, L"Could not initialize the alphamapshader object.", L"Error", MB_OK); return false; }

	m_BumpMapShader = new BumpMapShaderClass;
	if (!m_BumpMapShader) { return false; }

	result = m_BumpMapShader->Initialize(device, hwnd);
	if (!result)
	{	MessageBox(hwnd, L"Could not initialize the bumpmapshader object.", L"Error", MB_OK); return false;}

	m_SpecMapShader = new SpecMapShaderClass;
	if (!m_SpecMapShader) { return false; }

	result = m_SpecMapShader->Initialize(device, hwnd);
	if (!result)
	{MessageBox(hwnd, L"Could not initialize the specmapshader object.", L"Error", MB_OK); return false;}

	m_FogShader = new FogShaderClass;
	if (!m_FogShader) { return false; }

	result = m_FogShader->Initialize(device, hwnd);
	if (!result){	MessageBox(hwnd, L"Could not initialize the fog object.", L"Error", MB_OK); return false;}

	m_TranslateShader = new TranslateShaderClass;
	if (!m_TranslateShader) { return false; }

	result =m_TranslateShader->Initialize(device, hwnd);
	if(!result) { MessageBox(hwnd, L"Could not initialize the fog object.", L"Error", MB_OK); return false; }

	m_TransparentShader = new TransparentShaderClass;
	if (!m_TransparentShader) { return false; }

	result = m_TransparentShader->Initialize(device, hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the fogplanwe object.", L"Error", MB_OK); return false; }

	m_ReflectionShader = new ReflectionShaderClass;
	if (!m_ReflectionShader) { return false; }

	result = m_ReflectionShader->Initialize(device, hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the m_ReflectionShader object.", L"Error", MB_OK); return false; }

	m_RefractionShader = new RefractionShaderClass;
	if (!m_RefractionShader) { return false;}

	result = m_RefractionShader->Initialize(device, hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the m_RefractionShader object.", L"Error", MB_OK); return false; }

	m_WaterShader = new WaterShaderClass;
	if (!m_WaterShader) { return false; }

	result = m_WaterShader->Initialize(device, hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the m_waterShader object.", L"Error", MB_OK); return false; }

	m_WaterReflectionShader = new WaterReflectionShaderClass;
	if (!m_WaterReflectionShader) { return false; }

	result = m_WaterReflectionShader->Initialize(device, hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the m_WaterReflectionShader object.", L"Error", MB_OK); return false; }

	m_SkydomeShader = new SkyDomeShaderClass;
	if (!m_SkydomeShader) { return true; }

	m_SkydomeShader->Initialize(device, hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the Skydomeshader object.", L"Error", MB_OK); return false; }

	m_CloudShader = new SkyPlaneShaderClass;
	if (!m_CloudShader) { return true; }

	m_CloudShader->Initialize(device, hwnd);
	if (!result) { MessageBox(hwnd, L"Could not initialize the Skydomeshader object.", L"Error", MB_OK); return false; }

	m_FireShader = new FireShaderClass;
	if (!m_FireShader) { return true; }

	m_FireShader->Initialize(device, hwnd);
	if(!result){ MessageBox(hwnd, L"Could not initialize the FireShader object.", L"Error", MB_OK); return false; }

	return true;
}


void ShaderManagerClass::Shutdown()
{

	if (m_ColorShader) { m_ColorShader->Shutdown(); delete m_ColorShader; m_ColorShader = 0; }
	if (m_FireShader) { m_FireShader->Shutdown(); delete m_FireShader; m_FireShader = 0; }
	// 라이트 쉐이더 객체를 해제합니다.
	if (m_LightShader) {m_LightShader->Shutdown();delete m_LightShader;m_LightShader = 0;}
	if (m_RefractionShader) { m_RefractionShader->Shutdown(); delete m_RefractionShader; m_RefractionShader = 0; }
	if (m_WaterShader) { m_WaterShader->Shutdown(); delete m_WaterShader; m_WaterShader = 0; }
	if (m_WaterReflectionShader) { m_WaterReflectionShader->Shutdown(); delete m_WaterReflectionShader; m_WaterReflectionShader = 0; }
	// 텍스처 쉐이더 객체를 해제한다.
	if (m_TextureShader) {m_TextureShader->Shutdown();delete m_TextureShader;m_TextureShader = 0;}
	if (m_ReflectionShader) { m_ReflectionShader->Shutdown(); delete m_ReflectionShader; m_ReflectionShader = 0; }
	if (m_MultiTextureShader) { m_MultiTextureShader->Shutdown(); delete m_MultiTextureShader; m_MultiTextureShader = 0; }
	if (m_SkydomeShader) { m_SkydomeShader->Shutdown(); delete m_SkydomeShader; m_SkydomeShader = 0; }
	if (m_CloudShader) { m_CloudShader->Shutdown(); delete m_CloudShader; m_CloudShader = 0; }
	if (m_AlphaMapShader) { m_AlphaMapShader->Shutdown(); delete m_AlphaMapShader; m_AlphaMapShader = 0; }
	if (m_BumpMapShader) { m_BumpMapShader->Shutdown(); delete m_BumpMapShader; m_BumpMapShader = 0; }
	if (m_SpecMapShader) { m_SpecMapShader->Shutdown(); delete m_SpecMapShader; m_SpecMapShader = 0; }
	if (m_FogShader) { m_FogShader->Shutdown(); delete m_FogShader; m_FogShader = 0; }
	if (!m_TranslateShader) { m_TranslateShader->Shutdown(); delete m_TranslateShader; m_TranslateShader = 0; }
	if (m_TransparentShader) { m_TransparentShader->Shutdown(); delete m_TransparentShader; m_TransparentShader = 0; }

}

bool ShaderManagerClass::RenderColorShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix)
{
	return m_ColorShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix);
}

bool ShaderManagerClass::RenderTextureShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	// 텍스처 셰이더를 사용하여 모델을 렌더링합니다.
	return m_TextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}


bool ShaderManagerClass::RenderLightShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambient, D3DXVECTOR4 diffuse,
	D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specular, float specularPower)
{
	// 라이트 쉐이더를 사용하여 모델을 렌더링합니다.
	return m_LightShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambient, diffuse, cameraPosition,
		specular, specularPower);
}

bool ShaderManagerClass::RenderMultiTextureShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	ID3D11ShaderResourceView** texture)
{
	return m_MultiTextureShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

bool ShaderManagerClass::RenderAlphaMapShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	ID3D11ShaderResourceView** texture)
{
	return m_AlphaMapShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture);
}

bool ShaderManagerClass::RenderBumpMapShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix
	, ID3D11ShaderResourceView** texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffusecolor)
{
	return m_BumpMapShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffusecolor);
}

bool ShaderManagerClass::RenderSpecMapShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor
	, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	return m_SpecMapShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor,diffuseColor, cameraPosition, specularColor, specularPower);
}

bool ShaderManagerClass::RenderFogShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, float FogStart, float FogEnd)
{
	return m_FogShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, FogStart, FogEnd);
}

bool ShaderManagerClass::RenderTranslateShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor
	, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower, float translation)
{
	return m_TranslateShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower, translation);
}


bool ShaderManagerClass::RenderTransparentShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, float blendAmount)
{
	// 텍스처 셰이더를 사용하여 모델을 렌더링합니다.
	return m_TransparentShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, blendAmount);
}

bool ShaderManagerClass::RenderReflectionShader(ID3D11DeviceContext* device, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix)
{
	return m_ReflectionShader->Render(device, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, reflectionTexture, reflectionMatrix);
}

bool ShaderManagerClass::RenderSkydomeShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, D3DXVECTOR4 apexColor, D3DXVECTOR4 centerColor)
{
	return m_SkydomeShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, apexColor, centerColor);
}

bool ShaderManagerClass::RenderCloudShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2,
	float firstTranslationX, float firstTranslationZ, float secondTranslationX, float secondTranslationZ, float brightness)
{
	return m_CloudShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, texture2, firstTranslationX, firstTranslationZ, secondTranslationX, secondTranslationZ, brightness);
}

bool ShaderManagerClass::RenderRefractionShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture,
	D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor,
	D3DXVECTOR4 clipPlane)
{
	return m_RefractionShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, ambientColor, diffuseColor, clipPlane);

}

bool ShaderManagerClass::RenderWaterShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, D3DXMATRIX reflectionMatrix, ID3D11ShaderResourceView* refractionTexture,
	ID3D11ShaderResourceView* reflectionTexture, ID3D11ShaderResourceView* normalTexture, D3DXVECTOR3 cameraPosition,
	D3DXVECTOR2 normalMapTiling, float waterTranslation, float reflectRefractScale, D3DXVECTOR4 refractionTint,
	D3DXVECTOR3 lightDirection, float specularShininess)
{
	return m_WaterShader->Render(deviceContext,indexCount, worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, refractionTexture, reflectionTexture,
		normalTexture, cameraPosition, normalMapTiling, waterTranslation, reflectRefractScale, refractionTint, lightDirection,
		specularShininess);
}

bool ShaderManagerClass::RenderWaterReflectionShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* colorTexture, ID3D11ShaderResourceView* normalTexture,
	D3DXVECTOR4 lightDiffuseColor, D3DXVECTOR3 lightDirection, float colorTextureBrightness, D3DXVECTOR4 clipPlane)
{
	return m_WaterReflectionShader->Render(deviceContext, indexCount, worldMatrix, viewMatrix, projectionMatrix, colorTexture, normalTexture, lightDiffuseColor, lightDirection, colorTextureBrightness, clipPlane);
}

bool ShaderManagerClass::RenderFireShader(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* fireTexture, ID3D11ShaderResourceView* noiseTexture,
	ID3D11ShaderResourceView* alphaTexture, float frameTime, D3DXVECTOR3 scrollSpeeds, D3DXVECTOR3 scales, D3DXVECTOR2 distortion1,
	D3DXVECTOR2 distortion2, D3DXVECTOR2 distortion3, float distortionScale, float distortionBias)
{
	return m_FireShader->Render(deviceContext,indexCount, worldMatrix, viewMatrix, projectionMatrix, fireTexture, noiseTexture, alphaTexture, frameTime, scrollSpeeds, scales, distortion1, distortion2, distortion3, distortionScale, distortionBias);
}