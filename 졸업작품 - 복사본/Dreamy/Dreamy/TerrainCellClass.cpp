#include "TerrainCellClass.h"


/*---------------------------------------------------------------------------------------------
이름:
용도:
-----------------------------------------------------------------------------------------------*/
TerrainCellClass::TerrainCellClass()
{
	m_vertexList = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_lineVertexBuffer = 0;
	m_lineIndexBuffer = 0;
	
}

TerrainCellClass::TerrainCellClass(const TerrainCellClass& other)
{
}

TerrainCellClass::~TerrainCellClass()
{
}
/*---------------------------------------------------------------------------------------------
이름:Initialize()
용도:
- TerrainCell 초기화.
- ModelType생성 -> 지형 데이터를 넣을 버퍼 생성 -> cell의 면적 계산
-----------------------------------------------------------------------------------------------*/
bool TerrainCellClass::Initialize(ID3D11Device* device, void* TerrainModelPtr, int nodeIndexX, int nodeIndexY,
	int cellHeight, int cellWidth, int terrainWidth)
{
	ModelType* TerrainModel;
	bool result;
	

	//TerrainClass로 부터 Terrain 지형 모델의 포인터를 불러와 생성한다.
	//이렇게 하는 이유는 cell안에 들어있는 일부 지형만 로드하기 위해서이다.
	TerrainModel = (ModelType*)TerrainModelPtr;

	//셀 안에 있는 Terrain 데이터를 채울 버퍼를 로드한다.
	result = InitializeBuffers(device, nodeIndexX, nodeIndexY, cellHeight, cellWidth, terrainWidth, TerrainModel);
	if (!result) { return false; }

	// 필요 없으니까 TerrainModel을 해제한다.
	TerrainModel = 0;

	// 셀의 dimension(면적)을 계산한다.
	CalculateCellDimensions();

	// 셀주위의 바운딩박스를 생성한다.
	result = BuildLineBuffers(device);

	if (!result) { return false; }

	return true;

}
/*---------------------------------------------------------------------------------------------
이름: InitializeBuffers()
용도: 
- Terrain Cell안에 들어갈 정점, 인덱스에 버퍼를 생성해준다.
- 셀의 크기를 결정하는 것과 같고 다른 계산에 사용되는 정점 목록을 만든다.
- TerrainClass에서 얻어온 ㅁ지형모델이 이 함수로 전달되고 지형 모델에 대한 인덱스가
  nodeIndexX, nodeIndexY를 사용하여 이 셀의 물리적 위치를 기반으로 만들어진다.
- ModelIndex가 뭔지 모르겠음....아마 셀 하나인거같음
-----------------------------------------------------------------------------------------------*/
bool TerrainCellClass::InitializeBuffers(ID3D11Device* device, int nodeIndexX, int nodeIndexY, int cellHeight, int cellWidth,
	int terrainWidth, ModelType * terrainModel)
{
	VertexType* vertices;
	unsigned long* indices;
	int modelIndex, index;

	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	//TerrainClass에서 Terrain에 필요한 Cell갯수를 계산하고, 그걸 여기서 받아서 정점 갯수를 계산한다.
	m_vertexCount = (cellHeight - 1)*(cellWidth - 1) *6;
	
	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];
	if (!vertices) { return false; }

	indices = new unsigned long[m_indexCount];
	if (!indices) { return false; }

	// 지형 모델 데이터와 로컬 정점/인덱스 배열에 Terrain 지형 데이터의 인덱스를 셋업해준다. ???????????
	modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;
	index = 0;

	//TerrainClass로 부터 받은 Terrian 데이터를 정점 배열에 넣어준다.
	//*****셀 안에 들어있는 Terrain지형의 일!부!이다.
	for (int j = 0; j<(cellHeight - 1); j++)
	{
		for (int i = 0; i<((cellWidth - 1) * 6); i++)
		{
			vertices[index].position = D3DXVECTOR3(terrainModel[modelIndex].x, terrainModel[modelIndex].y, terrainModel[modelIndex].z);
			vertices[index].texture = D3DXVECTOR2(terrainModel[modelIndex].tu, terrainModel[modelIndex].tv);
			vertices[index].normal = D3DXVECTOR3(terrainModel[modelIndex].nx, terrainModel[modelIndex].ny, terrainModel[modelIndex].nz);
			vertices[index].tangent = D3DXVECTOR3(terrainModel[modelIndex].tx, terrainModel[modelIndex].ty, terrainModel[modelIndex].tz);
			vertices[index].binormal = D3DXVECTOR3(terrainModel[modelIndex].bx, terrainModel[modelIndex].by, terrainModel[modelIndex].bz);
			vertices[index].color = D3DXVECTOR3(terrainModel[modelIndex].r, terrainModel[modelIndex].g, terrainModel[modelIndex].b);
			indices[index] = index;
			modelIndex++;
			index++;
		}
		modelIndex += (terrainWidth * 6) - (cellWidth * 6);
	}

	// Set up the description of the static vertex buffer.
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

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
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

	// 셀의 정점 정보에 접근하는데 이용될 정점 배열을 생성한다.
	m_vertexList = new VectorType[m_vertexCount];
	if (!m_vertexList) { return false; }

	// 셀의 정점 위치의 로컬을 카피한다.
	for (int i = 0; i<m_vertexCount; i++)
	{
		m_vertexList[i].nx = vertices[i].position.x;
		m_vertexList[i].ny = vertices[i].position.y;
		m_vertexList[i].nz = vertices[i].position.z;
	}


	// Release the arrays now that the buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

/*---------------------------------------------------------------------------------------------
이름: CalculateCellDimensions()
용도: 
- 셀의 사이즈를 결정하는데 사용된다.
- InitializeBuffer에서 생성한 m_vertexlist를 여기서 사용한다.
- 정점 배열을 돌면서!!!! x좌표 = width, y좌표 = height, z좌표 = depth로 계속 돌면서 각 성분이 max가 될 때 까지 돌려서 max가 어딘지 찾아낸다.
-----------------------------------------------------------------------------------------------*/
void TerrainCellClass::CalculateCellDimensions()
{
	float width, height, depth;

	// Initialize the dimensions of the node.
	m_maxWidth = -1000000.0f;
	m_maxHeight = -1000000.0f;
	m_maxDepth = -1000000.0f;

	m_minWidth = 1000000.0f;
	m_minHeight = 1000000.0f;
	m_minDepth = 1000000.0f;

	//정점 배열을 돌면서!!!!
	//x좌표 = width, y좌표 = height, z좌표 = depth로 계속 돌면서
	// 각 성분이 max가 될 때 까지 돌려서 max가 어딘지 찾아낸다.
	for (int i = 0; i<m_vertexCount; i++)
	{
		width = m_vertexList[i].nx;
		height = m_vertexList[i].ny;
		depth = m_vertexList[i].nz;

		// Check if the width exceeds the minimum or maximum.
		if (width > m_maxWidth)
		{
			m_maxWidth = width;
		}
		if (width < m_minWidth)
		{
			m_minWidth = width;
		}

		// Check if the height exceeds the minimum or maximum.
		if (height > m_maxHeight)
		{
			m_maxHeight = height;
		}
		if (height < m_minHeight)
		{
			m_minHeight = height;
		}

		// Check if the depth exceeds the minimum or maximum.
		if (depth > m_maxDepth)
		{
			m_maxDepth = depth;
		}
		if (depth < m_minDepth)
		{
			m_minDepth = depth;
		}
	}

	// Calculate the center position of this cell.
	m_positionX = (m_maxWidth - m_minWidth) + m_minWidth;
	m_positionY = (m_maxHeight - m_minHeight) + m_minHeight;
	m_positionZ = (m_maxDepth - m_minDepth) + m_minDepth;

	return;
}

/*---------------------------------------------------------------------------------------------
이름: 라인그리는애들
용도:
-----------------------------------------------------------------------------------------------*/
bool TerrainCellClass::BuildLineBuffers(ID3D11Device* device)
{
	ColorVertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	D3DXVECTOR4 lineColor;
	int index, vertexCount, indexCount;


	// Set the color of the lines to orange.
	lineColor = D3DXVECTOR4(1.0f, 0.5f, 0.0f, 1.0f);

	// Set the number of vertices in the vertex array.
	vertexCount = 24;

	// Set the number of indices in the index array.
	indexCount = vertexCount;

	// Create the vertex array.
	vertices = new ColorVertexType[vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[indexCount];
	if (!indices)
	{
		return false;
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(ColorVertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Load the vertex and index array with data.
	index = 0;

	// 8 Horizontal lines.
	vertices[index].position = D3DXVECTOR3(m_minWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	// 4 Verticle lines.
	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_maxHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_minHeight, m_maxDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_maxWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_maxHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;
	index++;

	vertices[index].position = D3DXVECTOR3(m_minWidth, m_minHeight, m_minDepth);
	vertices[index].color = lineColor;
	indices[index] = index;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_lineVertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_lineIndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Store the index count for rendering.
	m_lineIndexCount = indexCount;

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	return true;
}

void TerrainCellClass::RenderLineBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(ColorVertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_lineVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_lineIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case lines.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	return;
}

/*---------------------------------------------------------------------------------------------
이름: Get()
용도: 셀 안의 정점, 인덱스 갯수를 반환한다.
-----------------------------------------------------------------------------------------------*/
int TerrainCellClass::GetVertexCount()
{
	return m_vertexCount;
}

int TerrainCellClass::GetIndexCount()
{
	return m_indexCount;
}

int TerrainCellClass::GetLineBuffersIndexCount()
{
	return m_lineIndexCount;
}

void TerrainCellClass::GetCellDimensions(float& maxWidth, float& maxHeight, float& maxDepth,
	float& minWidth, float& minHeight, float& minDepth)
{
	maxWidth = m_maxWidth;
	maxHeight = m_maxHeight;
	maxDepth = m_maxDepth;
	minWidth = m_minWidth;
	minHeight = m_minHeight;
	minDepth = m_minDepth;
	return;
}

/*---------------------------------------------------------------------------------------------
이름: Render()
용도: 정점, 인덱스 버퍼를 그래픽스 파이프라인에 등록한다.
-----------------------------------------------------------------------------------------------*/
void TerrainCellClass::Render(ID3D11DeviceContext* deviceContext)
{
	// 정점, 인덱스 버퍼를 그래픽스 파이프라인에 등록한다.
	RenderBuffers(deviceContext);

	return;
}
void TerrainCellClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

/*---------------------------------------------------------------------------------------------
이름:Shutdown()
용도:해제
-----------------------------------------------------------------------------------------------*/
void TerrainCellClass::Shutdown()
{
	// Release the line rendering buffers.
	ShutdownLineBuffers();

	// Release the cell rendering buffers.
	ShutdownBuffers();

	return;
}
void TerrainCellClass::ShutdownBuffers()
{
	// Release the public vertex list.
	if (m_vertexList)
	{
		delete[] m_vertexList;
		m_vertexList = 0;
	}

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
void TerrainCellClass::ShutdownLineBuffers()
{
	// Release the index buffer.
	if (m_lineIndexBuffer)
	{
		m_lineIndexBuffer->Release();
		m_lineIndexBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_lineVertexBuffer)
	{
		m_lineVertexBuffer->Release();
		m_lineVertexBuffer = 0;
	}

	return;
}