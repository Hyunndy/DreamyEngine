#include "ImageClass.h"

ImageClass::ImageClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}

ImageClass::ImageClass(const ImageClass&)
{
}

ImageClass::~ImageClass()
{
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
 �̸� : Initialize()
 �뵵 :
 - ȭ�� ũ��� �̹��� ũ�⸦ �����Ѵ�.
 - �������� �� ��Ȯ�� ���� ��ġ�� �ʿ��ϱ� ������(��Ȯ �� �ʿ�� ����.)
 - ũ�� ���x

 - ���� ������ ��ġ ������ ���� �����Ӱ� ��ġ�� ���Ͽ� ��ġ�� ������ �ʾҴٸ� ���� ���� ���۸� �ٲ��� �ʱ� ������ ���� ����� ���� �� �ִ�.
 -------------------------------------------------------------------------------------------------------------------------------------------------------------
*/ 
bool ImageClass::Initialize(ID3D11Device* device, int screenWidth, int screenHegiht, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	//ȭ�� ũ�� ����
	m_screenWidth = screenWidth;
	m_screenHeight = screenHegiht;

	//�̹��� ũ�� ����
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	//���� ��ġ ����
	m_previousPosX = -1;
	m_previousPosY = -1;

	//����&�ε��� ���� �ʱ�ȭ(����)
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//�̹��� �ҷ�����
	result = LoadTexture(device, textureFilename);
	if (!result) { return false; }

	return true;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : Shutdown()
�뵵 :
- ���߿� GraphicsClass���� ȣ��ȴ�.
- 2D�̹����� �������ϱ� ���� �ʿ��ߴ� �����͵��� ��� �����Ѵ�.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void ImageClass::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();

	return;
}
/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : Render()
�뵵 :
- ���߿� GraphicsClass���� ȣ��ȴ�.
- 2D�̹����� ���۸� �׷���ī�忡 �ִ´�.
- �Է����� �̹����� ȭ�鿡 �׷��� ��ġ�� �޴´�.
- UpdateBuffer() : ��ġ�� �����ϴµ� ���Ǿ� ���� �����Ӱ� ��ġ�� �ٲ������ ���� ���� ���۸� ���� �����Ѵ�.
- RenderBuffer() : ���������� ���������ο� ������ ����/�ε��� ���۸� �غ��Ѵ�.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
bool ImageClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	// ���� ���� ���� ������
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result) { return false; }

	// �׷��� ���������ο� ���� ����/�ε��� ���۸� ����
	RenderBuffers(deviceContext);

	return true;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : GetIndexCount()
�뵵 :
- 2D �̹����� ���� ���� �����Ѵ�.
- 2D�� ������ �׻� 6�� ��ȯ�Ѵ�!
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
int ImageClass::GetIndexCount()
{
	return m_indexCount;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : GetIndexCount()
�뵵 :
- 2D �̹����� ���� ���� �����Ѵ�.
- 2D�� ������ �׻� 6�� ��ȯ�Ѵ�!
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
ID3D11ShaderResourceView* ImageClass::GetTexture()
{ 
	return m_Texture->GetTexture();
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : InitializeBuffers()
�뵵 :
- 2D �̹����� �׸��µ� ����ϴ� ����/�ε��� ���۸� ����� �� ���ȴ�.
- D3D11_BUFFER_DESC ����ü�� VertexType�� �ִ� �������� ������ ������ ���������ο� �˷��ش�.
- D3D11_SUBRESOURCE_DATA ����ü�� ���긮�ҽ��̴�.

- ���� ���ҽ���? Direct3D�� ���ҽ� ��ü �Ǵ� ���ҽ��� �Ϻθ� ������ �� �ִµ�, �̴� ���ҽ��� �Ϻκ��� ��Ÿ���� ���� ���ҽ��� �κ������̴�.
- �ؽ�ó�� ���� ���긮�ҽ��� �ؽ�ó�� ���� �� �ִ� �Ӹ� ������ �ؽ�ó �迭 ������ ���� �����ϴ�.
- �ϳ��� ���۴� �⺻������ �ϳ��� ���� ���ҽ��� ���ǵȴ�.(����=���긮�ҽ�)

- ImageClass�� ModelClass�� �����Ѱ��ε�, ���� ū ���̴� ImageClass���� **������������**�� ����Ѵ�.
- ������ �ε����� 2D�̹����̰� �����̱� ������ �׻� 6�̴�. ���� �ε��� ���۴� �������۷� �����.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
bool ImageClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// 2���� �ﰢ���� ������ �ϹǷ� ���� ���� 6.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Load the index array with data.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// ���� ���� ���� ����
	// D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//�ε��� ���۴� ���� ���۷� �����.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : ShutdwonBuffers()
�뵵 : ����&�ε��� ���۸� �����Ѵ�.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void ImageClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : UpdateBuffers()
�뵵 : 
- �������� �ʴ� ModelClass�� �ٸ���!

- InitializeBuffer()���� ���� ���� ���۸� ������ �������� UpdateBuffer()�� �� ������ ���� ���ư��� Render()�� �������ν�
  �� ������ ���� �ҷ� �ʿ��� ��� ���� ���� ������ ������ �� ��ġ�� �����ϰ� �Ѵ�.
- �ʿ��� ��� : ��ǥ���� �ٲ�� ���

- 1. ��ġ�� �ٲ������ �˻��Ѵ�.
- 2. �ٲ���ٸ� �ӽ÷� ���� �迭�� �����ϰ�, �� �ȿ� �ٲ� �����͸� �ִ´�.
- 3. InitializeBuffer()���� ���� ���� ���۷� ����� �������� Map�Լ��� ���� ������ ������ �� �ְ� �Ѵ�.
- 4. ���� ���� �����Ϳ� ������ �� �ִ� ������(verticesPtr)�� ����� �����Ϳ� memcpy�� 2���� �������� ���� �迭�� �����͸� �����Ѵ�.
- 5. ���۸� ������Ų��.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
bool ImageClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;


	// �̹����� ��ġ�� ������ ���Ͽ� �޶����� �ʾҴٸ� return true;�� �׳� ���� ������.
	// �� Ȯ���� ���� ������ ���ҽ�Ų��.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// �ٲ��ٸ� �˻��غ���!
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// �̹����� �� ���� ��ġ�� ����Ѵ�.
	// Direct3D���� ��ũ�� �߾��� (0.0)�̴�.
	// M_SCREENHEIGHT�̷��ɷ� �˾Ƽ� ����� �ߵǰ� �س��Ҵ� �̰�.
	// TOP,BOTTOM��!!! 2���� �ؽ�ó ��ǥ���� ���������� (0,0)�̴�. �ű⼭���� �׸� �׷����� ������ POSITIONY�� -�� ����� ���� �Ʒ��� �̹����� ��Ÿ����.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
	right = left + (float)m_bitmapWidth;

	top = (float)(m_screenHeight / 2) - (float)positionY;
	bottom = top - (float)m_bitmapHeight;

	// �ӽ� ���� ���� �迭
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	//InitializeBuffer()���� ������ ���� ���۸� Map�Լ��� ���� ���� ���� ������ �� �� �ְ� �Ѵ�. (lock�Լ��� ����� ������)
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// ���ؽ� ���ۿ� �ִ� �����͸� ���� �����͸� ���´�.
	verticesPtr = (VertexType*)mappedResource.pData;

	// ���� ���� �����Ϳ� ������ �ӽ� ������ vertices ���� �迭�� �ִ� ������ �����Ѵ�.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// �����͸� ���ͼ� ���� �ٲ�� ������ ���� �ٲ��.
	// ���� �ٲ� �� ���ؽ� ���۸� �ٽ� unlock���ش�.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// ������ �����Ѵ�.
	delete[] vertices;
	vertices = 0;

	return true;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : RenderBuffers()
�뵵 : 
- ���̴��� GPU���� ����/�ε��� ���۸� ����� �� �ֵ��� ������ ���������ο� ������.
- ������ ������ ���� 1�ܰ� �Է�������(I/A)�� ������.
- I/A������ ���� ���̴� �ܰ迡 ���Եȴ�!
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void ImageClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : LoadTexture()
�뵵 : 2D �̹����� �׸� �ؽ��ĸ� �ε��Ѵ�. (�ؽ�ó Ŭ������ ����)
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
bool ImageClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture)	{	return false;	}

	result = m_Texture->Initialize(device, filename);
	if (!result){	return false;	}

	return true;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
�̸� : ReleaseTexuture()
�뵵 : �ؽ�ó�� �����Ѵ�.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void ImageClass::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}



