#include "ShaderManagerClass.h"
#include "TextureShaderClass.h"
#include "LightShaderClass.h"
#include "MultiTextureShaderClass.h"
#include "AlphaMapShaderClass.h"
#include "BumpMapShaderClass.h"
#include "SpecMapShaderClass.h"


ShaderManagerClass::ShaderManagerClass()
{
	m_TextureShader = 0;
	m_LightShader = 0;
	m_MultiTextureShader = 0;
	m_AlphaMapShader = 0;
	m_BumpMapShader = 0;
	m_SpecMapShader = 0;
}

ShaderManagerClass::ShaderManagerClass(const ShaderManagerClass& other)
{
}

ShaderManagerClass::~ShaderManagerClass()
{
}

bool ShaderManagerClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	// 텍스처 쉐이더 객체를 생성한다.
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	// 텍스처 쉐이더 객체를 초기화한다.
	bool result = m_TextureShader->Initialize(device, hwnd);
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


	return true;
}


void ShaderManagerClass::Shutdown()
{


	// 라이트 쉐이더 객체를 해제합니다.
	if (m_LightShader) {m_LightShader->Shutdown();delete m_LightShader;m_LightShader = 0;}

	// 텍스처 쉐이더 객체를 해제한다.
	if (m_TextureShader) {m_TextureShader->Shutdown();delete m_TextureShader;m_TextureShader = 0;}

	if (m_MultiTextureShader) { m_MultiTextureShader->Shutdown(); delete m_MultiTextureShader; m_MultiTextureShader = 0; }

	if (m_AlphaMapShader) { m_AlphaMapShader->Shutdown(); delete m_AlphaMapShader; m_AlphaMapShader = 0; }
	if (m_BumpMapShader) { m_BumpMapShader->Shutdown(); delete m_BumpMapShader; m_BumpMapShader = 0; }
	if (m_SpecMapShader) { m_SpecMapShader->Shutdown(); delete m_SpecMapShader; m_SpecMapShader = 0; }

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