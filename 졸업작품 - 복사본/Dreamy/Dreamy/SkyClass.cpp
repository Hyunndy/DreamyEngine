#include "SkyClass.h"


/*---------------------------------------------------------------------------------------------
�̸�:
�뵵:
-----------------------------------------------------------------------------------------------*/
SkyClass::SkyClass()
{
	m_model = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_skyPlane = 0;
	m_SkyPlaneindexBuffer = 0;
	m_SkyPlanevertexBuffer = 0;
	m_CloudTexture1 = 0;
	m_CloudTexture2 = 0;

}

SkyClass::SkyClass(const SkyClass& other)
{
}

SkyClass::~SkyClass()
{
}

/*---------------------------------------------------------------------------------------------
�̸�: Initialize()
�뵵: 
- ��ī�̵� ���� �����ꤤ��.
- ��ī�̵� �� �����͸� ����, �ε��� ���ۿ� �ִ´�.
- ��ī�̵��� ���� 2���� ���� �����Ѵ�.

- ������ ���� �ؽ�ó���� �ε��Ѵ�.
- 
-----------------------------------------------------------------------------------------------*/
bool SkyClass::InitializeSky(ID3D11Device* device)
{
	bool result;

	//��ī�̵� ���� �ҷ��´�.
	result = LoadSkyDomeModel("../Dreamy/skydome.txt");
	if (!result) { return false; }

	//�������� ���� ��ī�̵� �����͸� ��������, �ε������ۿ� �ε��Ѵ�.
	result = InitializeSkyBuffers(device);
	if (!result) { return false; }

	// Set the color at the top of the sky dome.
	m_apexColor = D3DXVECTOR4(0.11f, 0.56f, 1.0f, 1.0f);

	// Set the color at the center of the sky dome.
	m_centerColor = D3DXVECTOR4(0.0f, 0.74f, 1.0f, 1.0f);

	return true;

}

bool SkyClass::InitializeCloud(ID3D11Device* device, WCHAR* textureFilename1, WCHAR* textureFilename2)
{
	bool result;

	int skyPlaneResolution; //skyplane�� x,z�������� �����Ǿ�� �ϴ� ���� ���� �����ϴµ� ���(�������� �� ������)
	int textureRepeat; //skyplane ���� �ؽ�ó�� �ݺ��� Ƚ���� �����Ѵ�. uv��ǥ�� �����۴µ� ���ȴ�.
	float skyPlaneWidth, skyPlaneTop, skyPlaneBottom;

	// �ϴ� ��� �Ű� ������ �����մϴ�.
	skyPlaneResolution = 20;
	skyPlaneWidth = 10.0f;
	skyPlaneTop = 0.5f;
	skyPlaneBottom = -0.5f;
	textureRepeat = 4;

	// 0~255������ ��Ʈ���� ����� �� ��⸦ ����ϸ� ����.
	// 0.0f~1.0f������ ���� ���´�.
	// ������ �󸶳� �Ͼ�� ������ ������ �� �ִ�.
	m_brightness = 0.50f;


									   //���� ��ġ. 
	m_textureTranslation[0] = 0.0f;
	m_textureTranslation[1] = 0.0f;
	m_textureTranslation[2] = 0.0f;
	m_textureTranslation[3] = 0.0f;

	//skyplane����
	result = InitializeSkyPlane(skyPlaneResolution, skyPlaneWidth, skyPlaneTop, skyPlaneBottom, textureRepeat);
	if (!result) { return false; }

	//skyplane ��������, �ε������� ����
	result = InitializeCloudBuffers(device, skyPlaneResolution);
	if (!result) { return false; }

	//skyplane �ؽ�ó�� �ε��Ѵ�.
	result = LoadTextures(device, textureFilename1, textureFilename2);
	if (!result) { return false; }
}
/*---------------------------------------------------------------------------------------------
�̸�: LoadSkyDomeModel()
�뵵: .txt���Ͽ��� �о�´�. ModelClass�� �ִ� �װŶ� ����..? �������� ���߿� Ȯ��.
-----------------------------------------------------------------------------------------------*/
bool SkyClass::LoadSkyDomeModel(char* filename)
{
	ifstream fin;
	char input;
	int i;


	// Open the model file.
	fin.open(filename);

	// If it could not open the file then exit.
	if (fin.fail())
	{
		return false;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return false;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i<m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	return true;
}

/*---------------------------------------------------------------------------------------------
�̸�: InitializeBuffers()
�뵵: ���� ����, �ε��� ���ۿ� ��ī�̵� ���� �ε����ش�.
-----------------------------------------------------------------------------------------------*/
bool SkyClass::InitializeSkyBuffers(ID3D11Device* device)
{
	VertexType* vertices; //��ġ
	unsigned long* indices;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; //���� ����, �ε��� ���� desc

	D3D11_SUBRESOURCE_DATA vertexData, indexData; // ���긮�ҽ�(LoadModel���� ������ ������ ��ġ, �ε���)

	HRESULT result;


	// ���� �迭(position) ����
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// �ε��� �迭 ����
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// ���긮�ҽ��� vertices�� �� �����Ϳ��� ������ �����ͼ� �ִ´�.
	// ���긮�ҽ��� indices�� ���ؽ�ī��Ʈ��ŭ �ε����� �ִ´�.
	for (int i = 0; i<m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		indices[i] = i;
	}

	// ���� ���� desc ����
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� ���ۿ� ���굥���� ����.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ����.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �ε��� ���� desc ����
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// �ε��� ���ۿ� ���굥���� ����.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// �ε��� ���� ����
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// �� �־����� ���굥���͵� ����.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

bool SkyClass::InitializeCloudBuffers(ID3D11Device* device, int skyPlaneResolution)
{
	SkyPlaneVertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index, index1, index2, index3, index4;


	// Calculate the number of vertices in the sky plane mesh.
	m_SkyPlanevertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;

	// Set the index count to the same as the vertex count.
	m_SkyPlaneindexCount = m_SkyPlanevertexCount;

	// Create the vertex array.
	vertices = new SkyPlaneVertexType[m_SkyPlanevertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_SkyPlaneindexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize the index into the vertex array.
	index = 0;

	// Load the vertex and index array with the sky plane array data.
	for (int j = 0; j<skyPlaneResolution; j++)
	{
		for (int i = 0; i<skyPlaneResolution; i++)
		{
			index1 = j * (skyPlaneResolution + 1) + i;
			index2 = j * (skyPlaneResolution + 1) + (i + 1);
			index3 = (j + 1) * (skyPlaneResolution + 1) + i;
			index4 = (j + 1) * (skyPlaneResolution + 1) + (i + 1);

			// Triangle 1 - Upper Left
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index1].x, m_skyPlane[index1].y, m_skyPlane[index1].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index1].tu, m_skyPlane[index1].tv);
			indices[index] = index;
			index++;

			// Triangle 1 - Upper Right
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index2].x, m_skyPlane[index2].y, m_skyPlane[index2].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index2].tu, m_skyPlane[index2].tv);
			indices[index] = index;
			index++;

			// Triangle 1 - Bottom Left
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index3].x, m_skyPlane[index3].y, m_skyPlane[index3].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index3].tu, m_skyPlane[index3].tv);
			indices[index] = index;
			index++;

			// Triangle 2 - Bottom Left
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index3].x, m_skyPlane[index3].y, m_skyPlane[index3].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index3].tu, m_skyPlane[index3].tv);
			indices[index] = index;
			index++;

			// Triangle 2 - Upper Right
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index2].x, m_skyPlane[index2].y, m_skyPlane[index2].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index2].tu, m_skyPlane[index2].tv);
			indices[index] = index;
			index++;

			// Triangle 2 - Bottom Right
			vertices[index].position = D3DXVECTOR3(m_skyPlane[index4].x, m_skyPlane[index4].y, m_skyPlane[index4].z);
			vertices[index].texture = D3DXVECTOR2(m_skyPlane[index4].tu, m_skyPlane[index4].tv);
			indices[index] = index;
			index++;
		}
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SkyPlaneVertexType) * m_SkyPlanevertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_SkyPlanevertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_SkyPlaneindexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_SkyPlaneindexBuffer);
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


/*---------------------------------------------------------------------------------------------
�̸�:InitializeSkyPlane()
�뵵:
- ���� ��鿡 ���� ������Ʈ���� �����Ѵ�.
-
-----------------------------------------------------------------------------------------------*/
bool SkyClass::InitializeSkyPlane(int skyPlaneResolution, float skyPlaneWidth, float skyPlaneTop, float skyPlaneBottom, int textureRepeat)
{
	float quadSize, radius, constant, textureDelta;
	int index;
	float positionX, positionY, positionZ, tu, tv;


	// Create the array to hold the sky plane coordinates.
	m_skyPlane = new SkyPlaneType[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];
	if (!m_skyPlane)
	{
		return false;
	}

	// �ϴ� ��� ���� �� ������ ũ�⸦ �����Ѵ�.
	quadSize = skyPlaneWidth / (float)skyPlaneResolution;

	// �ʺ� �������� �ϴ� �ϴ� ����� �������� ����Ѵ�.
	radius = skyPlaneWidth / 2.0f;

	// ���� �� ���� ����� ����Ѵ�.
	constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	//�ؽ�ó ��ǥ �������� ����Ѵ�.
	textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

	// �ϴ� ����� �ݺ��ϰ� ������ ���� ���� ������� ��ǥ�� �����Ѵ�.
	for (int j = 0; j <= skyPlaneResolution; j++)
	{
		for (int i = 0; i <= skyPlaneResolution; i++)
		{
			// Calculate the vertex coordinates.
			positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

			// �ؽ�ó ��ǥ�� ����Ѵ�.
			tu = (float)i * textureDelta;
			tv = (float)j * textureDelta;

			// �� ��ǥ�� �߰��ϱ� ���� �ϴ� ��� �迭�� �ε����� ����Ѵ�.
			index = j * (skyPlaneResolution + 1) + i;

			// �ϴ� ��� �迭�� ��ǥ�� �߰��Ѵ�.
			m_skyPlane[index].x = positionX;
			m_skyPlane[index].y = positionY;
			m_skyPlane[index].z = positionZ;
			m_skyPlane[index].tu = tu;
			m_skyPlane[index].tv = tv;
		}
	}

	return true;
}


void SkyClass::RenderSky(ID3D11DeviceContext* deviceContext)
{
	// Render the sky dome.
	RenderSkyBuffers(deviceContext);

	return;
}

void SkyClass::RenderCloud(ID3D11DeviceContext* deviceContext)
{
	RenderCloudBuffers(deviceContext);

	return;
}

/*---------------------------------------------------------------------------------------------
�̸�: RenderBuffers()
�뵵: �������� ���� �׷��Ƚ� ���������ο� ���� ����, �ε��� ���۸� ���´�.
-----------------------------------------------------------------------------------------------*/
void SkyClass::RenderSkyBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}
void SkyClass::RenderCloudBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride = sizeof(SkyPlaneVertexType);
	unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_SkyPlanevertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_SkyPlaneindexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

/*---------------------------------------------------------------------------------------------
�̸�: LoadTextures()
�뵵: ���� �ؽ�ó�� �ε��Ѵ�.
-----------------------------------------------------------------------------------------------*/
bool SkyClass::LoadTextures(ID3D11Device* device, WCHAR* textureFilename1, WCHAR* textureFilename2)
{
	bool result;


	// Create the first cloud texture object.
	m_CloudTexture1 = new TextureClass;
	if (!m_CloudTexture1)
	{
		return false;
	}

	// Initialize the first cloud texture object.
	result = m_CloudTexture1->Initialize(device, textureFilename1);
	if (!result)
	{
		return false;
	}

	// Create the second cloud texture object.
	m_CloudTexture2 = new TextureClass;
	if (!m_CloudTexture2)
	{
		return false;
	}

	// Initialize the second cloud texture object.
	result = m_CloudTexture2->Initialize(device, textureFilename2);
	if (!result)
	{
		return false;
	}

	return true;
}

int SkyClass::GetIndexCount()
{
	return m_indexCount;
}


D3DXVECTOR4 SkyClass::GetApexColor()
{
	return m_apexColor;
}


D3DXVECTOR4 SkyClass::GetCenterColor()
{
	return m_centerColor;
}

void SkyClass::Frame(float Speed0, float Speed1, float Speed2, float Speed3)
{
	// Increment the translation values to simulate the moving clouds.
	m_textureTranslation[0] += Speed0;
	m_textureTranslation[1] += Speed1;
	m_textureTranslation[2] += Speed2;
	m_textureTranslation[3] += Speed3;

	// Keep the values in the zero to one range.
	if (m_textureTranslation[0] > 1.0f) { m_textureTranslation[0] -= 1.0f; }
	if (m_textureTranslation[1] > 1.0f) { m_textureTranslation[1] -= 1.0f; }
	if (m_textureTranslation[2] > 1.0f) { m_textureTranslation[2] -= 1.0f; }
	if (m_textureTranslation[3] > 1.0f) { m_textureTranslation[3] -= 1.0f; }

	return;
}


int SkyClass::GetCloudIndexCount()
{
	return m_SkyPlaneindexCount;
}


ID3D11ShaderResourceView* SkyClass::GetCloudTexture1()
{
	return m_CloudTexture1->GetTexture();
}


ID3D11ShaderResourceView* SkyClass::GetCloudTexture2()
{
	return m_CloudTexture2->GetTexture();
}

float SkyClass::GetBrightness()
{
	return m_brightness;
}

float SkyClass::GetTranslation(int index)
{
	return m_textureTranslation[index];
}


/*---------------------------------------------------------------------------------------------
�̸�: Release()
�뵵: ������ ����
-----------------------------------------------------------------------------------------------*/
void SkyClass::ReleaseBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the index buffer.
	if (m_SkyPlaneindexBuffer)
	{
		m_SkyPlaneindexBuffer->Release();
		m_SkyPlaneindexBuffer = 0;
	}

	if (m_SkyPlanevertexBuffer)
	{
		m_SkyPlanevertexBuffer->Release();
		m_SkyPlanevertexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void SkyClass::ReleaseSkyDomeModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}

void SkyClass::ShutdownSkyPlane()
{
	// �ϴ� ��� �迭�� �����մϴ�. 
	if (m_skyPlane)
	{
		delete[] m_skyPlane;
		m_skyPlane = 0;
	}

	return;
}

void SkyClass::Shutdown()
{

	ReleaseTextures();

	// Release the vertex and index buffer that were used for rendering the sky dome.
	ReleaseBuffers();

	ShutdownSkyPlane();

	// Release the sky dome model.
	ReleaseSkyDomeModel();

	return;
}


void SkyClass::ReleaseTextures()
{
	// Release the texture objects.
	if (m_CloudTexture1)
	{
		m_CloudTexture1->Shutdown();
		delete m_CloudTexture1;
		m_CloudTexture1 = 0;
	}

	if (m_CloudTexture2)
	{
		m_CloudTexture2->Shutdown();
		delete m_CloudTexture2;
		m_CloudTexture2 = 0;
	}

	return;
}
