#include "RTTClass.h"



RTTClass::RTTClass()
{
	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView = 0;
}

RTTClass::RTTClass(const RTTClass& other)
{
}

RTTClass::~RTTClass()
{
}


/*----------------------------------------------------------------------------------------
�̸�: Initialize()
�뵵:
- RTT�� ������ �ʺ�� ���̸� ���ڷ� �޴´�
*********���� ȭ���� ������ �ؽ�ó�� �׸��ٸ� ��׷����� �����ϱ� ���� �ݵ�� RTT�� ���μ��� ������ ȭ�� ������ ���� �Ѵ�.

- 1. �ؽ�ó�� DESC�� �ۼ��ϰ� �����ϴ� ������� Ÿ���� �Ǵ� �ؽ�ó�� �����.
- 2. ������� �ؽ�ó�� ���� Ÿ�� ��� �����Ͽ� �������� �ؽ�ó�� �Ͼ���� �Ѵ�.
  ���⼭ ���� Ÿ�� �並 �����ϴ� CreateRenderTargetView()�Լ��� D3DŬ�������� ù��°���ڰ� ����������Ϳ���.
  ������ ���� ������� �ؽ�ó�� ����.

- 3. �� �ؽ�ó�� ���� ID3D11ShaderResourceView�� ����� �������� �����Ϳ� �����Ѵ�.
  ���̴� ���ҽ� ��� ó�� ���°�????
----------------------------------------------------------------------------------------*/
bool RTTClass::Initialize(ID3D11Device* device, int textureWidth, int textureHeight)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;


	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;


	//RTT DESC�� �ʱ�ȭ�Ѵ�.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// RTT DESC�� �����Ѵ�.
	// BindFlgs�� ��� vertexbuffer, indexbuffer, constantbuffer �� ����.
	textureDesc.Width = textureWidth;
	textureDesc.Height = textureHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// RTT�� �����Ѵ�. (�ؽ�ó DESC�� �ۼ��� �� Ÿ���� �Ǵ� �ؽ�ó�� �����Ѵ�.)
	result = device->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// ���� Ÿ�� �� DESC�� �����Ѵ�.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// ���� Ÿ�� �並 �����Ѵ�. 
	//���⼭ ���� Ÿ�� �並 �����ϴ� CreateRenderTargetView()�Լ��� D3DŬ�������� ù��°���ڰ� ����������Ϳ���.
	//������ ���� ������� �ؽ�ó�� ����.
	result = device->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// ���̴� ���ҽ� �信 ���� DESC�� �����Ѵ�.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// ���̴� ���ҽ� �並 �����Ѵ�.
	result = device->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}


void RTTClass::Shutdown()
{
	if (m_shaderResourceView)
	{
		m_shaderResourceView->Release();
		m_shaderResourceView = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_renderTargetTexture)
	{
		m_renderTargetTexture->Release();
		m_renderTargetTexture = 0;
	}

	return;
}

/*----------------------------------------------------------------------------------------
�̸�: SetRenderTarget()
�뵵: m_renderTargetView�� ���� Ÿ������ �����ؼ� ��� �������� �� �ؽ�ó�� ����ǰ� �Ѵ�.
	  D3DŬ�������� �ִ�.
----------------------------------------------------------------------------------------*/
void RTTClass::SetRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView)
{
	// ���� Ÿ�� ��� ���� ���ٽ� ���۸� ��� ���������ο� ���ε��Ѵ�.
	deviceContext->OMSetRenderTargets(1, &m_renderTargetView, depthStencilView);

	return;
}

/*----------------------------------------------------------------------------------------
�̸�: ClearRenderTarget()
�뵵: 
- D3DClass::BeginScene()�� ����.
- �������� ����۰� �ƴ� m_renderTargetView�� Ŭ��� �̷������.
- �� ������ RTT�� �Ͼ������ ȣ���ؾ� �Ѵ�.
----------------------------------------------------------------------------------------*/
void RTTClass::ClearRenderTarget(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView,
	float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

/*----------------------------------------------------------------------------------------
�̸�: GetShaderResourceView()
�뵵:
- �ؽ�ó �����͸� ���̴� ���ҽ� ��� �����Ѵ�.
- RTT�� ����� �ؽ�ó�� �̿��ϴ� ���� �ٸ� ���̴��鿡�� �ؽ�ó �����Ϳ� ������ �� �ְ� �Ѵ�.
- RTT �ؽ�ó�� ������ �Ϲ������� ���̴��� �ؽ�ó�� ���� ���� ���ִ� �κп� �� �Լ��� ȣ���ϴ� �κ��� �ִ´�.
  ( graphicsŬ�������� renderShader()���� GetTextureArray() �ϴ��� ���ε�..?)
----------------------------------------------------------------------------------------*/
ID3D11ShaderResourceView* RTTClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}