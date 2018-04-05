#include "ParticleSystem.h"



ParticleSystem::ParticleSystem()
{
	m_Texture = 0;
	m_particleList = 0;
	m_vertices = 0;
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	active = false;
}

ParticleSystem::ParticleSystem(const ParticleSystem&) {}

ParticleSystem::~ParticleSystem()
{
}

/*------------------------------------------------------------------------
이름: Initialize()
용도: 
- 텍스처->파티클시스템-> Empty버퍼를 초기화 한다.
------------------------------------------------------------------------*/
bool ParticleSystem::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;


	result = LoadTexture(device, textureFilename);
	if (!result) { return false; }


	result = InitializeParticleSystem();
	if (!result) {return false;}

	result = InitializeBuffers(device);
	if (!result) { return false; }

	return true;
}
/*------------------------------------------------------------------------
이름: Frame()
용도:
- 오래된 파티클을 제거한다.
- 시간이 되면 새로운 파티클들을 방출한다.
- 새로운 파티클을 방출한 후 방출한 파티클들을 업데이트 한다.
- 떨어지는 이펙트를 위해 그들의 height position을 조정한다.
- 파티클이 업데이트 되면 각각의 파티클의 업데이트된 위치에 따른 버텍스 버퍼를 업데이트 해야 한다.
- 버텍스 버퍼는 동적이여야 업데이트하기 쉬워진다.
------------------------------------------------------------------------*/
bool ParticleSystem::Frame(float frameTime, ID3D11DeviceContext* deviceContext)
{
	bool result;
	

	KillParticles();

	if (active == true)
	{
		EmitParticles(frameTime);

		UpdateParticles(frameTime);

		// Update the dynamic vertex buffer with the new position of each particle.
		result = UpdateBuffers(deviceContext);
		if (!result) { return false; }


		time += 0.5f;

		if ((time > 100.0f && active == true))
		{
			active = false;
			time = 0.0f;
		//	InitializeParticleSystem();
			m_particleList = new ParticleType[m_maxParticles];
			InitializeParticleSystem();
		}
	}

	if (active == false)
	{
		active = false;
		time = 0.0f;
		//	InitializeParticleSystem();
		m_particleList = new ParticleType[m_maxParticles];
		InitializeParticleSystem();
	}



	return true;
}

/*------------------------------------------------------------------------
이름: InitializeParticleSystem()
용도: 
- 각각의 파티클 요소들에 사용될 입자들을 초기화한다.
- 이 파티클 시스템에선 파티클이 랜덤한 장소에 나오게끔 랜덤deviation(편차)
------------------------------------------------------------------------*/
bool ParticleSystem::InitializeParticleSystem()
{
	// 파티클이 방출되는 장소를 random하게 한다.
	m_particleDeviationX = 2.0f;
	m_particleDeviationY = 0.1f;
	m_particleDeviationZ = 2.0f;

	// 파티클의 속도와 속도 변화 변수를 세팅한다.
	m_particleVelocity = 3.0f;
	m_particleVelocityVariation = 0.1f;

	// 파티클의 사이즈를 설정한다.
	m_particleSize = 0.4f;

	// 초당 방출되는 파티클을 설정한다.
	m_particlesPerSecond = 500.0f;

	// 파티클 시스템에서 맥스 파티클을 정한다.
	m_maxParticles = 1000;

	// 맥스 파티클 파티클타입리스트를 정의한다.
	m_particleList = new ParticleType[m_maxParticles];
	if (!m_particleList)
	{
		return false;
	}

	// Initialize the particle list.
	for (int i = 0; i<m_maxParticles; i++)
	{
		m_particleList[i].active = false;
	}

	// 아직 방출되지 않았기 때문에 현재 파티클 카운트를 0으로 초기화한다.
	m_currentParticleCount = 0;
	// 초당 파티클 방출에 대한 시간을 초기화한다.
	m_accumulatedTime = 0.0f;

	return true;

}

/*------------------------------------------------------------------------
이름: InitializeBuffers()
용도:
- 정점 버퍼, 인덱스 버퍼를 준비해놓는다. 
- 파티클이 매 프레임마다 업데이트될것이기 때문에 정점 버퍼는 동적 버퍼로 생서오디어야 한다.
- 시작에선 방출된 파티클이 없기 때문에 정점 버퍼는 빈 상태로 생성되어야 한다.
------------------------------------------------------------------------*/
bool ParticleSystem::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	// 쿼드니까 삼각형 2개 = 정점 6개
	m_vertexCount = m_maxParticles * 6;

	// Set the maximum number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array for the particles that will be rendered.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
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
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Initialize the index array.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
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

	// Release the index array since it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}

/*------------------------------------------------------------------------
이름: EmitParticles()
용도:
- 파티클을 방출한다.
------------------------------------------------------------------------*/
void ParticleSystem::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;


	// 방출에 축적된 시간을 더해준다.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// 새 파티클을 방출할 시간인지 아닌지 결정한다.(per second)
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// 방출할 파티클이 있다면 한 프레임 마다 방출하도록 한다.
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;


		// 파티클은 뒤에서 부터 앞으로 렌더되야 하기 때문에 파티클 배열을 정렬 해줘야 한다.
		// z버퍼를 사용하여 정렬하므로 목록에서 입자를 삽입해야 하는 위치를 찾아야한다.
		index = 0;
		found = false;
		while (!found)
		{
			if ((m_particleList[index].active == false) || (m_particleList[index].posZ < positionZ))
			{
				found = true;
			}
			else
			{
				index++;
			}
		}


		i = m_currentParticleCount;
		j = i - 1;

		while (i != index)
		{
			m_particleList[i].posX = m_particleList[j].posX;
			m_particleList[i].posY = m_particleList[j].posY;
			m_particleList[i].posZ = m_particleList[j].posZ;
			m_particleList[i].red = m_particleList[j].red;
			m_particleList[i].green = m_particleList[j].green;
			m_particleList[i].blue = m_particleList[j].blue;
			m_particleList[i].velocity = m_particleList[j].velocity;
			m_particleList[i].active = m_particleList[j].active;
			i--;
			j--;
		}

		// Now insert it into the particle array in the correct depth order.
		m_particleList[index].posX = positionX;
		m_particleList[index].posY = positionY;
		m_particleList[index].posZ = positionZ;
		m_particleList[index].red = red;
		m_particleList[index].green = green;
		m_particleList[index].blue = blue;
		m_particleList[index].velocity = velocity;
		m_particleList[index].active = true;
	}

	return;
}


/*------------------------------------------------------------------------
이름: UpdateParticles()
용도:
- 파티클 진행 방향!!! ( 난 위로 가게한다!!)
------------------------------------------------------------------------*/
void ParticleSystem::UpdateParticles(float frameTime)
{
	int i;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (i = 0; i<m_currentParticleCount; i++)
	{
		m_particleList[i].posY = m_particleList[i].posY + (m_particleList[i].velocity * frameTime * 0.001f);
	}

	return;
}

/*------------------------------------------------------------------------
이름: KillParticles()
용도:
- 렌더링 라이프타임이 끝난 애들(여기선 UpdateParticle조건이 충족된 애들)을 죽인다.
------------------------------------------------------------------------*/
void ParticleSystem::KillParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for (i = 0; i<m_maxParticles; i++)
	{
		if ((m_particleList[i].active == true) && (m_particleList[i].posY > 6.0f))
		{
			m_particleList[i].active = false;
			m_currentParticleCount--;

			// Now shift all the live particles back up the array to erase the destroyed particle and keep the array sorted correctly.
			for (j = i; j<m_maxParticles - 1; j++)
			{
				m_particleList[j].posX = m_particleList[j + 1].posX;
				m_particleList[j].posY = m_particleList[j + 1].posY;
				m_particleList[j].posZ = m_particleList[j + 1].posZ;
				m_particleList[j].red = m_particleList[j + 1].red;
				m_particleList[j].green = m_particleList[j + 1].green;
				m_particleList[j].blue = m_particleList[j + 1].blue;
				m_particleList[j].velocity = m_particleList[j + 1].velocity;
				m_particleList[j].active = m_particleList[j + 1].active;
			}
		}
	}

	return;
}

/*------------------------------------------------------------------------
이름: UpdateBuffers()
용도:
- 정점 버퍼를 동적 생성한다.
- 왜? 렌더링 될 때 마다 파티클의 위치가 업데이트 되니까!
------------------------------------------------------------------------*/
bool ParticleSystem::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	int index, i;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	// Initialize vertex array to zeros at first.
	memset(m_vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Now build the vertex array from the particle list array.  Each particle is a quad made out of two triangles.
	index = 0;

	for (i = 0; i<m_currentParticleCount; i++)
	{
		// Bottom left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].posX - m_particleSize, m_particleList[i].posY - m_particleSize, m_particleList[i].posZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].posX - m_particleSize, m_particleList[i].posY + m_particleSize, m_particleList[i].posZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].posX + m_particleSize, m_particleList[i].posY - m_particleSize, m_particleList[i].posZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Bottom right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].posX + m_particleSize, m_particleList[i].posY - m_particleSize, m_particleList[i].posZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 1.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top left.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].posX - m_particleSize, m_particleList[i].posY + m_particleSize, m_particleList[i].posZ);
		m_vertices[index].texture = D3DXVECTOR2(0.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;

		// Top right.
		m_vertices[index].position = D3DXVECTOR3(m_particleList[i].posX + m_particleSize, m_particleList[i].posY + m_particleSize, m_particleList[i].posZ);
		m_vertices[index].texture = D3DXVECTOR2(1.0f, 0.0f);
		m_vertices[index].color = D3DXVECTOR4(m_particleList[i].red, m_particleList[i].green, m_particleList[i].blue, 1.0f);
		index++;
	}

	// Lock the vertex buffer.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	return true;
}

void ParticleSystem::RenderBuffers(ID3D11DeviceContext* deviceContext)
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

	// Set the type of primitive that should be rendered from this vertex buffer.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ParticleSystem::LoadTexture(ID3D11Device* device, WCHAR* filename)
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

int ParticleSystem::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ParticleSystem::GetTexture()
{
	return m_Texture->GetTexture();
}

void ParticleSystem::Render(ID3D11DeviceContext* deviceContext)
{

		RenderBuffers(deviceContext);
	
	return;
}

void ParticleSystem::ReleaseTexture()
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

void ParticleSystem::Shutdown()
{
	// Release the buffers.
	ShutdownBuffers();

	// Release the particle system.
	ShutdownParticleSystem();

	// Release the texture used for the particles.
	ReleaseTexture();

	return;
}
void ParticleSystem::ShutdownParticleSystem()
{
	// Release the particle list.
	if (m_particleList)
	{
		delete[] m_particleList;
		m_particleList = 0;
	}

	return;
}
void ParticleSystem::ShutdownBuffers()
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

