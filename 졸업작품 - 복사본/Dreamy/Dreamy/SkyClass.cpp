#include "SkyClass.h"


/*---------------------------------------------------------------------------------------------
이름:
용도:
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
이름: Initialize()
용도: 
- 스카이돔 모델을 오르닿ㄴ다.
- 스카이돔 모델 데이터를 정점, 인덱스 버퍼에 넣는다.
- 스카이돔에 쓰일 2가지 색을 설정한다.

- 구름에 쓰일 텍스처들을 로드한다.
- 
-----------------------------------------------------------------------------------------------*/
bool SkyClass::InitializeSky(ID3D11Device* device)
{
	bool result;

	//스카이돔 모델을 불러온다.
	result = LoadSkyDomeModel("../Dreamy/skydome.txt");
	if (!result) { return false; }

	//렌더링을 위해 스카이돔 데이터를 정점버퍼, 인덱스버퍼에 로드한다.
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

	int skyPlaneResolution; //skyplane이 x,z방향으로 구성되어야 하는 쿼드 수를 지정하는데 사용(높아지면 더 높아짐)
	int textureRepeat; //skyplane 위로 텍스처를 반복할 횟수를 결정한다. uv좌표를 생서앟는데 사용된다.
	float skyPlaneWidth, skyPlaneTop, skyPlaneBottom;

	// 하늘 평면 매개 변수를 설정합니다.
	skyPlaneResolution = 20;
	skyPlaneWidth = 10.0f;
	skyPlaneTop = 0.5f;
	skyPlaneBottom = -0.5f;
	textureRepeat = 4;

	// 0~255사이의 비트맵을 사용할 때 밝기를 사용하면 좋다.
	// 0.0f~1.0f사이의 값을 갖는다.
	// 구름이 얼마나 하얗게 보일지 조절할 수 있다.
	m_brightness = 0.50f;


									   //구름 위치. 
	m_textureTranslation[0] = 0.0f;
	m_textureTranslation[1] = 0.0f;
	m_textureTranslation[2] = 0.0f;
	m_textureTranslation[3] = 0.0f;

	//skyplane형성
	result = InitializeSkyPlane(skyPlaneResolution, skyPlaneWidth, skyPlaneTop, skyPlaneBottom, textureRepeat);
	if (!result) { return false; }

	//skyplane 정점버퍼, 인덱스버퍼 생성
	result = InitializeCloudBuffers(device, skyPlaneResolution);
	if (!result) { return false; }

	//skyplane 텍스처를 로드한다.
	result = LoadTextures(device, textureFilename1, textureFilename2);
	if (!result) { return false; }
}
/*---------------------------------------------------------------------------------------------
이름: LoadSkyDomeModel()
용도: .txt파일에서 읽어온다. ModelClass에 있는 그거랑 같나..? 귀찮으니 나중에 확인.
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
이름: InitializeBuffers()
용도: 정점 버퍼, 인덱스 버퍼에 스카이돔 모델을 로드해준다.
-----------------------------------------------------------------------------------------------*/
bool SkyClass::InitializeSkyBuffers(ID3D11Device* device)
{
	VertexType* vertices; //위치
	unsigned long* indices;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc; //정점 버퍼, 인덱스 버퍼 desc

	D3D11_SUBRESOURCE_DATA vertexData, indexData; // 서브리소스(LoadModel에서 가져온 정점의 위치, 인덱스)

	HRESULT result;


	// 정점 배열(position) 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// 인덱스 배열 생성
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// 서브리소스인 vertices에 모델 데이터에서 정보를 가져와서 넣는다.
	// 서브리소스인 indices에 버텍스카운트만큼 인덱스를 넣는다.
	for (int i = 0; i<m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		indices[i] = i;
	}

	// 정점 버퍼 desc 설정
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 버퍼에 서브데이터 설정.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 인덱스 버퍼 desc 설정
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// 인덱스 버퍼에 서브데이터 설정.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// 인덱스 버퍼 생성
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// 다 넣었으니 서브데이터들 해제.
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
이름:InitializeSkyPlane()
용도:
- 구름 평면에 대한 지오메트리를 빌드한다.
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

	// 하늘 평면 에서 각 쿼드의 크기를 결정한다.
	quadSize = skyPlaneWidth / (float)skyPlaneResolution;

	// 너비를 기준으로 하는 하늘 평면의 반지름을 계산한다.
	radius = skyPlaneWidth / 2.0f;

	// 증가 할 높이 상수를 계산한다.
	constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

	//텍스처 좌표 증가값을 계산한다.
	textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

	// 하늘 평면을 반복하고 제공된 증가 값을 기반으로 좌표를 생성한다.
	for (int j = 0; j <= skyPlaneResolution; j++)
	{
		for (int i = 0; i <= skyPlaneResolution; i++)
		{
			// Calculate the vertex coordinates.
			positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
			positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
			positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

			// 텍스처 좌표를 계산한다.
			tu = (float)i * textureDelta;
			tv = (float)j * textureDelta;

			// 이 좌표를 추가하기 위해 하늘 평면 배열의 인덱스를 계산한다.
			index = j * (skyPlaneResolution + 1) + i;

			// 하늘 평면 배열에 좌표를 추가한다.
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
이름: RenderBuffers()
용도: 렌더링을 위해 그래픽스 파이프라인에 정전 버퍼, 인덱스 버퍼를 놓는다.
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
이름: LoadTextures()
용도: 구름 텍스처를 로드한다.
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
이름: Release()
용도: 데이터 해제
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
	// 하늘 평면 배열을 해제합니다. 
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
