#include "WaterClass.h"

/*---------------------------------------------------------------------------------------
이름:
용도:
---------------------------------------------------------------------------------------*/

WaterClass::WaterClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;

}


WaterClass::WaterClass(const WaterClass& other)
{
}

WaterClass::~WaterClass()
{
}

/*---------------------------------------------------------------------------------------
이름: Initialize()
용도: 
- 호수의 normalmap, 높이, 반지름을 인자로 받는다.
- 셰이더 관련 설정을 세팅한다.
**셰이더 관련 설정: 노말맵을 얼마나 타일링할지, 노말맵의 크기, 물 이동속도, 굴절세기, 정반사광세기
---------------------------------------------------------------------------------------*/
bool WaterClass::Initialize(ID3D11Device* device, WCHAR* textureFilename, float waterHeight, float waterRadius)
{
	bool result;

	m_waterHeight = waterHeight;

	//호수 면적의 vertex, index버퍼를 만든다.
	result = InitializeBuffers(device, waterRadius);
	if (!result) { return false; }

	//호수의 텍스처를 로드한다.
	result = LoadTexture(device, textureFilename);
	if (!result) { return false; }

	//노말맵을 얼마나 깔것인지.
	m_normalMapTiling.x = 0.01f; // 쿼드에 10번 노말맵을 깐다.
	m_normalMapTiling.y = 0.02f; // 쿼드에 5번 노말맵을 깐다.

	m_waterTranslation = 0.0f;

	//노말맵의 스케일링을 세팅한다.
	m_reflectRefractScale = 0.03f;

	//굴절의 청명함(tint)정도를 세팅한다.
	m_refractionTint = D3DXVECTOR4(0.0f, 0.8f, 1.0f, 0.0f);

	// specular(정반사광) 세기를 세팅한다.
	m_specularShininess = 100.0f;

	return true;

}
//bool WaterClass::Initialize(ID3D11Device* device, WCHAR* textureFilename, float waterRadius)
//{
//	bool result;
//
//	//호수 면적의 vertex, index버퍼를 만든다.
//	result = InitializeBuffers(device, waterRadius);
//	if (!result) { return false; }
//
//	//호수의 텍스처를 로드한다.
//	result = LoadTexture(device, textureFilename);
//	if (!result) { return false; }
//
//}
/*---------------------------------------------------------------------------------------
이름: InitializeBuffers()
용도: 호수의 실제 물리 쿼드를 생성한다. (버텍스, 인덱스 버퍼를 사용해서)
---------------------------------------------------------------------------------------*/
bool WaterClass::InitializeBuffers(ID3D11Device* device, float waterRadius)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//호수를 쿼드로 나타낼것이기 때문에 정점을 6개로 둔다.(=삼각형 2개=정점6개)
	m_vertexCount = 6;
	m_indexCount = 6;

	//정점 배열 생성
	vertices = new VertexType[m_vertexCount];
	if (!vertices) { return false; }

	//인덱스 배열 생성
	indices = new unsigned long[m_indexCount];
	if (!indices) { return false; }

	// 쿼드 값 채우기~!~!~!
	// 삼각형 2개로 하나의 큰 사각형을 만드는 것이다.
	vertices[0].position = D3DXVECTOR3(-waterRadius, 0.0f, waterRadius);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(waterRadius, 0.0f, waterRadius);  // Top right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[2].position = D3DXVECTOR3(-waterRadius, 0.0f, -waterRadius);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	vertices[3].position = D3DXVECTOR3(-waterRadius, 0.0f, -waterRadius);  // Bottom left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 1.0f);

	vertices[4].position = D3DXVECTOR3(waterRadius, 0.0f, waterRadius);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(waterRadius, 0.0f, -waterRadius);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	// Load the index array with data.
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the index buffer.
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

/*---------------------------------------------------------------------------------------
이름: LoadTexture()
용도: 노말맵을 받아온당.
---------------------------------------------------------------------------------------*/
bool WaterClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

/*---------------------------------------------------------------------------------------
이름: Frame()
용도: 잔물결 효과를 내게한다.
- 이걸 좀..어떻게 바꿔야겠다.
---------------------------------------------------------------------------------------*/
void WaterClass::Frame(int frametime)
{
	// Update the position of the water to simulate motion.
	m_waterTranslation += frametime*0.003f;
	if (m_waterTranslation > 1.0f)
	{
		m_waterTranslation -= 1.0f;
	}

	return;
}

void WaterClass::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	return;
}

void WaterClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

/*---------------------------------------------------------------------------------------
이름: Get()
용도: 반환하는 애들.
---------------------------------------------------------------------------------------*/
int WaterClass::GetIndexCount()
{
	return m_indexCount;
}


ID3D11ShaderResourceView* WaterClass::GetTexture()
{
	return m_Texture->GetTexture();
}


float WaterClass::GetWaterHeight()
{
	return m_waterHeight;
}


D3DXVECTOR2 WaterClass::GetNormalMapTiling()
{
	return m_normalMapTiling;
}


float WaterClass::GetWaterTranslation()
{
	return m_waterTranslation;
}


float WaterClass::GetReflectRefractScale()
{
	return m_reflectRefractScale;
}


D3DXVECTOR4 WaterClass::GetRefractionTint()
{
	return m_refractionTint;
}


float WaterClass::GetSpecularShininess()
{
	return m_specularShininess;
}

void WaterClass::ReleaseTexture()
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

void WaterClass::ShutdownBuffers()
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


void WaterClass::Shutdown()
{
	// Release the model texture.
	ReleaseTexture();

	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}