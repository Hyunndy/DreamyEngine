#include "TerrainCellClass.h"


/*---------------------------------------------------------------------------------------------
�̸�:
�뵵:
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
�̸�:Initialize()
�뵵:
- TerrainCell �ʱ�ȭ.
- ModelType���� -> ���� �����͸� ���� ���� ���� -> cell�� ���� ���
-----------------------------------------------------------------------------------------------*/
bool TerrainCellClass::Initialize(ID3D11Device* device, void* TerrainModelPtr, int nodeIndexX, int nodeIndexY,
	int cellHeight, int cellWidth, int terrainWidth)
{
	ModelType* TerrainModel;
	bool result;
	

	//TerrainClass�� ���� Terrain ���� ���� �����͸� �ҷ��� �����Ѵ�.
	//�̷��� �ϴ� ������ cell�ȿ� ����ִ� �Ϻ� ������ �ε��ϱ� ���ؼ��̴�.
	TerrainModel = (ModelType*)TerrainModelPtr;

	//�� �ȿ� �ִ� Terrain �����͸� ä�� ���۸� �ε��Ѵ�.
	result = InitializeBuffers(device, nodeIndexX, nodeIndexY, cellHeight, cellWidth, terrainWidth, TerrainModel);
	if (!result) { return false; }

	// �ʿ� �����ϱ� TerrainModel�� �����Ѵ�.
	TerrainModel = 0;

	// ���� dimension(����)�� ����Ѵ�.
	CalculateCellDimensions();

	// �������� �ٿ���ڽ��� �����Ѵ�.
	result = BuildLineBuffers(device);

	if (!result) { return false; }

	return true;

}
/*---------------------------------------------------------------------------------------------
�̸�: InitializeBuffers()
�뵵: 
- Terrain Cell�ȿ� �� ����, �ε����� ���۸� �������ش�.
- ���� ũ�⸦ �����ϴ� �Ͱ� ���� �ٸ� ��꿡 ���Ǵ� ���� ����� �����.
- TerrainClass���� ���� ���������� �� �Լ��� ���޵ǰ� ���� �𵨿� ���� �ε�����
  nodeIndexX, nodeIndexY�� ����Ͽ� �� ���� ������ ��ġ�� ������� ���������.
- ModelIndex�� ���� �𸣰���....�Ƹ� �� �ϳ��ΰŰ���
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

	//TerrainClass���� Terrain�� �ʿ��� Cell������ ����ϰ�, �װ� ���⼭ �޾Ƽ� ���� ������ ����Ѵ�.
	m_vertexCount = (cellHeight - 1)*(cellWidth - 1) *6;
	
	m_indexCount = m_vertexCount;

	vertices = new VertexType[m_vertexCount];
	if (!vertices) { return false; }

	indices = new unsigned long[m_indexCount];
	if (!indices) { return false; }

	// ���� �� �����Ϳ� ���� ����/�ε��� �迭�� Terrain ���� �������� �ε����� �¾����ش�. ???????????
	modelIndex = ((nodeIndexX * (cellWidth - 1)) + (nodeIndexY * (cellHeight - 1) * (terrainWidth - 1))) * 6;
	index = 0;

	//TerrainClass�� ���� ���� Terrian �����͸� ���� �迭�� �־��ش�.
	//*****�� �ȿ� ����ִ� Terrain������ ��!��!�̴�.
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

	// ���� ���� ������ �����ϴµ� �̿�� ���� �迭�� �����Ѵ�.
	m_vertexList = new VectorType[m_vertexCount];
	if (!m_vertexList) { return false; }

	// ���� ���� ��ġ�� ������ ī���Ѵ�.
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
�̸�: CalculateCellDimensions()
�뵵: 
- ���� ����� �����ϴµ� ���ȴ�.
- InitializeBuffer���� ������ m_vertexlist�� ���⼭ ����Ѵ�.
- ���� �迭�� ���鼭!!!! x��ǥ = width, y��ǥ = height, z��ǥ = depth�� ��� ���鼭 �� ������ max�� �� �� ���� ������ max�� ����� ã�Ƴ���.
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

	//���� �迭�� ���鼭!!!!
	//x��ǥ = width, y��ǥ = height, z��ǥ = depth�� ��� ���鼭
	// �� ������ max�� �� �� ���� ������ max�� ����� ã�Ƴ���.
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
�̸�: ���α׸��¾ֵ�
�뵵:
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
�̸�: Get()
�뵵: �� ���� ����, �ε��� ������ ��ȯ�Ѵ�.
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
�̸�: Render()
�뵵: ����, �ε��� ���۸� �׷��Ƚ� ���������ο� ����Ѵ�.
-----------------------------------------------------------------------------------------------*/
void TerrainCellClass::Render(ID3D11DeviceContext* deviceContext)
{
	// ����, �ε��� ���۸� �׷��Ƚ� ���������ο� ����Ѵ�.
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
�̸�:Shutdown()
�뵵:����
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