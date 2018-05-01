////////////////////////////////////////////////////////////////////////////////
// Filename: RTTTextureClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "RTTTextureClass.h"


RTTTextureClass::RTTTextureClass()
{
	m_renderTargetTexture = 0;
	m_renderTargetView = 0;
	m_shaderResourceView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilView = 0;
}


RTTTextureClass::RTTTextureClass(const RTTTextureClass& other)
{
}


RTTTextureClass::~RTTTextureClass()
{
}


bool RTTTextureClass::Initialize( int textureWidth, int textureHeight, float screenDepth, float screenNear)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT result;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;


	// Initialize the render target texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the render target texture description.
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

	// Create the render target texture.
	result = D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &m_renderTargetTexture);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the render target view.
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the render target view.
	result = D3D::GetDevice()->CreateRenderTargetView(m_renderTargetTexture, &renderTargetViewDesc, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the description of the shader resource view.
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;

	// Create the shader resource view.
	result = D3D::GetDevice()->CreateShaderResourceView(m_renderTargetTexture, &shaderResourceViewDesc, &m_shaderResourceView);
	if (FAILED(result))
	{
		return false;
	}

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = D3D::GetDevice()->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = D3D::GetDevice()->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// Setup the viewport for rendering.
	m_viewport.Width = (float)textureWidth;
	m_viewport.Height = (float)textureHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	// Setup the projection matrix.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, ((float)D3DX_PI / 4.0f), ((float)textureWidth / (float)textureHeight), screenNear, screenDepth);

	// Create an orthographic projection matrix for 2D rendering.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)textureWidth, (float)textureHeight, screenNear, screenDepth);

	return true;
}


void RTTTextureClass::Shutdown()
{
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

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


void RTTTextureClass::SetRenderTarget()
{
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	D3D::GetDeviceContext()->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Set the viewport.
	D3D::GetDeviceContext()->RSSetViewports(1, &m_viewport);

	return;
}


void RTTTextureClass::ClearRenderTarget( float red, float green, float blue, float alpha)
{
	float color[4];


	// Setup the color to clear the buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	// Clear the back buffer.
	D3D::GetDeviceContext()->ClearRenderTargetView(m_renderTargetView, color);

	// Clear the depth buffer.
	D3D::GetDeviceContext()->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


ID3D11ShaderResourceView* RTTTextureClass::GetShaderResourceView()
{
	return m_shaderResourceView;
}


void RTTTextureClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}


void RTTTextureClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
	return;
}

//void RTTTextureClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
//{
//	projectionMatrix = m_projectionMatrix;
//	return;
//}
//
//
//void RTTTextureClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
//{
//	orthoMatrix = m_orthoMatrix;
//	return;
//}
//
//RTTTextureClass::RTTTextureClass()
//{
//	m_vertexBuffer = 0;
//	m_indexBuffer = 0;
//
//}
//
//RTTTextureClass::RTTTextureClass(const RTTTextureClass&)
//{
//}
//
//RTTTextureClass::~RTTTextureClass()
//{
//}
//
///*
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//�̸� : Initialize()
//�뵵 :
//- ȭ�� ũ��� �̹��� ũ�⸦ �����Ѵ�.
//- �������� �� ��Ȯ�� ���� ��ġ�� �ʿ��ϱ� ������(��Ȯ �� �ʿ�� ����.)
//- ũ�� ���x
//
//- ���� ������ ��ġ ������ ���� �����Ӱ� ��ġ�� ���Ͽ� ��ġ�� ������ �ʾҴٸ� ���� ���� ���۸� �ٲ��� �ʱ� ������ ���� ����� ���� �� �ִ�.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//*/
//bool RTTTextureClass::Initialize(ID3D11Device* device, int screenWidth, int screenHegiht, int bitmapWidth, int bitmapHeight)
//{
//	bool result;
//
//	//ȭ�� ũ�� ����
//	m_screenWidth = screenWidth;
//	m_screenHeight = screenHegiht;
//
//	//�̹��� ũ�� ����
//	m_bitmapWidth = bitmapWidth;
//	m_bitmapHeight = bitmapHeight;
//
//	//���� ��ġ ����
//	m_previousPosX = -1;
//	m_previousPosY = -1;
//
//	//����&�ε��� ���� �ʱ�ȭ(����)
//	result = InitializeBuffers(device);
//	if (!result) { return false; }
//
//
//	return true;
//}
//
///*
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//�̸� : Shutdown()
//�뵵 :
//- ���߿� GraphicsClass���� ȣ��ȴ�.
//- 2D�̹����� �������ϱ� ���� �ʿ��ߴ� �����͵��� ��� �����Ѵ�.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//*/
//void RTTTextureClass::Shutdown()
//{
//
//	ShutdownBuffers();
//
//	return;
//}
///*
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//�̸� : Render()
//�뵵 :
//- ���߿� GraphicsClass���� ȣ��ȴ�.
//- 2D�̹����� ���۸� �׷���ī�忡 �ִ´�.
//- �Է����� �̹����� ȭ�鿡 �׷��� ��ġ�� �޴´�.
//- UpdateBuffer() : ��ġ�� �����ϴµ� ���Ǿ� ���� �����Ӱ� ��ġ�� �ٲ������ ���� ���� ���۸� ���� �����Ѵ�.
//- RenderBuffer() : ���������� ���������ο� ������ ����/�ε��� ���۸� �غ��Ѵ�.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//*/
//bool RTTTextureClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
//{
//	bool result;
//
//	// ���� ���� ���� ������
//	result = UpdateBuffers(deviceContext, positionX, positionY);
//	if (!result) { return false; }
//
//	// �׷��� ���������ο� ���� ����/�ε��� ���۸� ����
//	RenderBuffers(deviceContext);
//
//	return true;
//}
//
///*
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//�̸� : GetIndexCount()
//�뵵 :
//- 2D �̹����� ���� ���� �����Ѵ�.
//- 2D�� ������ �׻� 6�� ��ȯ�Ѵ�!
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//*/
//int RTTTextureClass::GetIndexCount()
//{
//	return m_indexCount;
//}
//
//
///*
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//�̸� : InitializeBuffers()
//�뵵 :
//- 2D �̹����� �׸��µ� ����ϴ� ����/�ε��� ���۸� ����� �� ���ȴ�.
//- D3D11_BUFFER_DESC ����ü�� VertexType�� �ִ� �������� ������ ������ ���������ο� �˷��ش�.
//- D3D11_SUBRESOURCE_DATA ����ü�� ���긮�ҽ��̴�.
//
//- ���� ���ҽ���? Direct3D�� ���ҽ� ��ü �Ǵ� ���ҽ��� �Ϻθ� ������ �� �ִµ�, �̴� ���ҽ��� �Ϻκ��� ��Ÿ���� ���� ���ҽ��� �κ������̴�.
//- �ؽ�ó�� ���� ���긮�ҽ��� �ؽ�ó�� ���� �� �ִ� �Ӹ� ������ �ؽ�ó �迭 ������ ���� �����ϴ�.
//- �ϳ��� ���۴� �⺻������ �ϳ��� ���� ���ҽ��� ���ǵȴ�.(����=���긮�ҽ�)
//
//- RTTTextureClass�� ModelClass�� �����Ѱ��ε�, ���� ū ���̴� RTTTextureClass���� **������������**�� ����Ѵ�.
//- ������ �ε����� 2D�̹����̰� �����̱� ������ �׻� 6�̴�. ���� �ε��� ���۴� �������۷� �����.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//*/
//bool RTTTextureClass::InitializeBuffers(ID3D11Device* device)
//{
//	VertexType* vertices;
//	unsigned long* indices;
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData, indexData;
//	HRESULT result;
//	int i;
//
//
//	// 2���� �ﰢ���� ������ �ϹǷ� ���� ���� 6.
//	m_vertexCount = 6;
//
//	// Set the number of indices in the index array.
//	m_indexCount = m_vertexCount;
//
//	// Create the vertex array.
//	vertices = new VertexType[m_vertexCount];
//	if (!vertices)
//	{
//		return false;
//	}
//
//	// Create the index array.
//	indices = new unsigned long[m_indexCount];
//	if (!indices)
//	{
//		return false;
//	}
//
//	// Initialize vertex array to zeros at first.
//	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));
//
//	// Load the index array with data.
//	for (i = 0; i<m_indexCount; i++)
//	{
//		indices[i] = i;
//	}
//
//	// ���� ���� ���� ����
//	// D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE
//	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the vertex data.
//	vertexData.pSysMem = vertices;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	// Now create the vertex buffer.
//	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	//�ε��� ���۴� ���� ���۷� �����.
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the index data.
//	indexData.pSysMem = indices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	// Create the index buffer.
//	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// Release the arrays now that the vertex and index buffers have been created and loaded.
//	delete[] vertices;
//	vertices = 0;
//
//	delete[] indices;
//	indices = 0;
//
//	return true;
//}
//
///*
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//�̸� : ShutdwonBuffers()
//�뵵 : ����&�ε��� ���۸� �����Ѵ�.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//*/
//void RTTTextureClass::ShutdownBuffers()
//{
//	// Release the index buffer.
//	if (m_indexBuffer)
//	{
//		m_indexBuffer->Release();
//		m_indexBuffer = 0;
//	}
//
//	// Release the vertex buffer.
//	if (m_vertexBuffer)
//	{
//		m_vertexBuffer->Release();
//		m_vertexBuffer = 0;
//	}
//
//	return;
//}
//
///*
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//�̸� : UpdateBuffers()
//�뵵 :
//- �������� �ʴ� ModelClass�� �ٸ���!
//
//- InitializeBuffer()���� ���� ���� ���۸� ������ �������� UpdateBuffer()�� �� ������ ���� ���ư��� Render()�� �������ν�
//�� ������ ���� �ҷ� �ʿ��� ��� ���� ���� ������ ������ �� ��ġ�� �����ϰ� �Ѵ�.
//- �ʿ��� ��� : ��ǥ���� �ٲ�� ���
//
//- 1. ��ġ�� �ٲ������ �˻��Ѵ�.
//- 2. �ٲ���ٸ� �ӽ÷� ���� �迭�� �����ϰ�, �� �ȿ� �ٲ� �����͸� �ִ´�.
//- 3. InitializeBuffer()���� ���� ���� ���۷� ����� �������� Map�Լ��� ���� ������ ������ �� �ְ� �Ѵ�.
//- 4. ���� ���� �����Ϳ� ������ �� �ִ� ������(verticesPtr)�� ����� �����Ϳ� memcpy�� 2���� �������� ���� �迭�� �����͸� �����Ѵ�.
//- 5. ���۸� ������Ų��.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//*/
//bool RTTTextureClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
//{
//	float left, right, top, bottom;
//	VertexType* vertices;
//	D3D11_MAPPED_SUBRESOURCE mappedResource;
//	VertexType* verticesPtr;
//	HRESULT result;
//
//
//	// �̹����� ��ġ�� ������ ���Ͽ� �޶����� �ʾҴٸ� return true;�� �׳� ���� ������.
//	// �� Ȯ���� ���� ������ ���ҽ�Ų��.
//	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
//	{
//		return true;
//	}
//
//	// �ٲ��ٸ� �˻��غ���!
//	m_previousPosX = positionX;
//	m_previousPosY = positionY;
//
//	// �̹����� �� ���� ��ġ�� ����Ѵ�.
//	// Direct3D���� ��ũ�� �߾��� (0.0)�̴�.
//	// M_SCREENHEIGHT�̷��ɷ� �˾Ƽ� ����� �ߵǰ� �س��Ҵ� �̰�.
//	// TOP,BOTTOM��!!! 2���� �ؽ�ó ��ǥ���� ���������� (0,0)�̴�. �ű⼭���� �׸� �׷����� ������ POSITIONY�� -�� ����� ���� �Ʒ��� �̹����� ��Ÿ����.
//	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
//	right = left + (float)m_bitmapWidth;
//
//	top = (float)(m_screenHeight / 2) - (float)positionY;
//	bottom = top - (float)m_bitmapHeight;
//
//	// �ӽ� ���� ���� �迭
//	vertices = new VertexType[m_vertexCount];
//	if (!vertices)
//	{
//		return false;
//	}
//
//	// Load the vertex array with data.
//	// First triangle.
//	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
//	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);
//
//	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
//	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);
//
//	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
//	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);
//
//	// Second triangle.
//	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
//	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);
//
//	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
//	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);
//
//	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
//	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);
//
//	//InitializeBuffer()���� ������ ���� ���۸� Map�Լ��� ���� ���� ���� ������ �� �� �ְ� �Ѵ�. (lock�Լ��� ����� ������)
//	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//	if (FAILED(result))
//	{
//		return false;
//	}
//
//	// ���ؽ� ���ۿ� �ִ� �����͸� ���� �����͸� ���´�.
//	verticesPtr = (VertexType*)mappedResource.pData;
//
//	// ���� ���� �����Ϳ� ������ �ӽ� ������ vertices ���� �迭�� �ִ� ������ �����Ѵ�.
//	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));
//
//	// �����͸� ���ͼ� ���� �ٲ�� ������ ���� �ٲ��.
//	// ���� �ٲ� �� ���ؽ� ���۸� �ٽ� unlock���ش�.
//	deviceContext->Unmap(m_vertexBuffer, 0);
//
//	// ������ �����Ѵ�.
//	delete[] vertices;
//	vertices = 0;
//
//	return true;
//}
//
///*
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//�̸� : RenderBuffers()
//�뵵 :
//- ���̴��� GPU���� ����/�ε��� ���۸� ����� �� �ֵ��� ������ ���������ο� ������.
//- ������ ������ ���� 1�ܰ� �Է�������(I/A)�� ������.
//- I/A������ ���� ���̴� �ܰ迡 ���Եȴ�!
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//*/
//void RTTTextureClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
//{
//	unsigned int stride;
//	unsigned int offset;
//
//
//	stride = sizeof(VertexType);
//	offset = 0;
//
//	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
//
//	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	return;
//}
//
//
//
//