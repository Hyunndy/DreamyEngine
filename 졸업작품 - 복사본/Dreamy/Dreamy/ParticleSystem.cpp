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
�̸�: Initialize()
�뵵: 
- �ؽ�ó->��ƼŬ�ý���-> Empty���۸� �ʱ�ȭ �Ѵ�.
------------------------------------------------------------------------*/
bool ParticleSystem::Initialize( WCHAR* textureFilename)
{
	bool result;


	result = LoadTexture( textureFilename);
	if (!result) { return false; }


	result = InitializeParticleSystem();
	if (!result) {return false;}

	result = InitializeBuffers();
	if (!result) { return false; }

	return true;
}
/*------------------------------------------------------------------------
�̸�: Frame()
�뵵:
- ������ ��ƼŬ�� �����Ѵ�.
- �ð��� �Ǹ� ���ο� ��ƼŬ���� �����Ѵ�.
- ���ο� ��ƼŬ�� ������ �� ������ ��ƼŬ���� ������Ʈ �Ѵ�.
- �������� ����Ʈ�� ���� �׵��� height position�� �����Ѵ�.
- ��ƼŬ�� ������Ʈ �Ǹ� ������ ��ƼŬ�� ������Ʈ�� ��ġ�� ���� ���ؽ� ���۸� ������Ʈ �ؾ� �Ѵ�.
- ���ؽ� ���۴� �����̿��� ������Ʈ�ϱ� ��������.
------------------------------------------------------------------------*/
bool ParticleSystem::Frame(float frameTime)
{
	bool result;
	

	KillParticles();

	if (active == true)
	{
		EmitParticles(frameTime);

		UpdateParticles(frameTime);

		// Update the dynamic vertex buffer with the new position of each particle.
		result = UpdateBuffers();
		if (!result) { return false; }


		time += 0.5f;

		if ((time > 100.0f && active == true))
		{
			active = false;
			time = 0.0f;
			InitializeParticleSystem();
		}
	}

	if (active == false)
	{
		active = false;
		time = 0.0f;
		InitializeParticleSystem();
	}



	return true;
}

/*------------------------------------------------------------------------
�̸�: InitializeParticleSystem()
�뵵: 
- ������ ��ƼŬ ��ҵ鿡 ���� ���ڵ��� �ʱ�ȭ�Ѵ�.
- �� ��ƼŬ �ý��ۿ��� ��ƼŬ�� ������ ��ҿ� �����Բ� ����deviation(����)
------------------------------------------------------------------------*/
bool ParticleSystem::InitializeParticleSystem()
{
	// ��ƼŬ�� ����Ǵ� ��Ҹ� random�ϰ� �Ѵ�.
	m_particleDeviationX = 12.0f;
	m_particleDeviationY = 0.0f;
	m_particleDeviationZ = 3.0f;

	// ��ƼŬ�� �ӵ��� �ӵ� ��ȭ ������ �����Ѵ�.
	m_particleVelocity = 15.0f;
	m_particleVelocityVariation = 0.1f;

	// ��ƼŬ�� ����� �����Ѵ�.
	m_particleSize = 1.3f;

	// �ʴ� ����Ǵ� ��ƼŬ�� �����Ѵ�.
	m_particlesPerSecond = 1000.0f;

	// ��ƼŬ �ý��ۿ��� �ƽ� ��ƼŬ�� ���Ѵ�.
	m_maxParticles = 5000;

	// �ƽ� ��ƼŬ ��ƼŬŸ�Ը���Ʈ�� �����Ѵ�.
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

	// ���� ������� �ʾұ� ������ ���� ��ƼŬ ī��Ʈ�� 0���� �ʱ�ȭ�Ѵ�.
	m_currentParticleCount = 0;
	// �ʴ� ��ƼŬ ���⿡ ���� �ð��� �ʱ�ȭ�Ѵ�.
	m_accumulatedTime = 0.0f;

	return true;

}

/*------------------------------------------------------------------------
�̸�: InitializeBuffers()
�뵵:
- ���� ����, �ε��� ���۸� �غ��س��´�. 
- ��ƼŬ�� �� �����Ӹ��� ������Ʈ�ɰ��̱� ������ ���� ���۴� ���� ���۷� ���������� �Ѵ�.
- ���ۿ��� ����� ��ƼŬ�� ���� ������ ���� ���۴� �� ���·� �����Ǿ�� �Ѵ�.
------------------------------------------------------------------------*/
bool ParticleSystem::InitializeBuffers()
{
	unsigned long* indices;
	int i;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;


	// Set the maximum number of vertices in the vertex array.
	// ����ϱ� �ﰢ�� 2�� = ���� 6��
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
	result = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
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
	result = D3D::GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
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
�̸�: EmitParticles()
�뵵:
- ��ƼŬ�� �����Ѵ�.
------------------------------------------------------------------------*/
void ParticleSystem::EmitParticles(float frameTime)
{
	bool emitParticle, found;
	float positionX, positionY, positionZ, velocity, red, green, blue;
	int index, i, j;


	// ���⿡ ������ �ð��� �����ش�.
	m_accumulatedTime += frameTime;

	// Set emit particle to false for now.
	emitParticle = false;

	// �� ��ƼŬ�� ������ �ð����� �ƴ��� �����Ѵ�.(per second)
	if (m_accumulatedTime > (1000.0f / m_particlesPerSecond))
	{
		m_accumulatedTime = 0.0f;
		emitParticle = true;
	}

	// ������ ��ƼŬ�� �ִٸ� �� ������ ���� �����ϵ��� �Ѵ�.
	if ((emitParticle == true) && (m_currentParticleCount < (m_maxParticles - 1)))
	{
		m_currentParticleCount++;

		// Now generate the randomized particle properties.
		positionX = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationX;
		positionY = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationY;
		positionZ = (((float)rand() - (float)rand()) / RAND_MAX) * m_particleDeviationZ;

		velocity = m_particleVelocity + (((float)rand() - (float)rand()) / RAND_MAX) * m_particleVelocityVariation;

		//red = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		green = (((float)rand() - (float)rand()) / RAND_MAX) + 0.5f;
		blue = (((float)rand() - (float)rand()) / RAND_MAX) + 1.0f;
		red = 0.0f;
		//green = 0.0f;


		// ��ƼŬ�� �ڿ��� ���� ������ �����Ǿ� �ϱ� ������ ��ƼŬ �迭�� ���� ����� �Ѵ�.
		// z���۸� ����Ͽ� �����ϹǷ� ��Ͽ��� ���ڸ� �����ؾ� �ϴ� ��ġ�� ã�ƾ��Ѵ�.
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
�̸�: UpdateParticles()
�뵵:
- ��ƼŬ ���� ����!!! ( �� ���� �����Ѵ�!!)
------------------------------------------------------------------------*/
void ParticleSystem::UpdateParticles(float frameTime)
{
	int i;

	// Each frame we update all the particles by making them move downwards using their position, velocity, and the frame time.
	for (i = 0; i<m_currentParticleCount; i++)
	{
		//m_particleList[i].posX = m_particleList[i].posX + (m_particleList[i].velocity * frameTime * 0.001f);
		m_particleList[i].posY = m_particleList[i].posY + (m_particleList[i].velocity * frameTime * 0.001f);
	}

	return;
}

/*------------------------------------------------------------------------
�̸�: KillParticles()
�뵵:
- ������ ������Ÿ���� ���� �ֵ�(���⼱ UpdateParticle������ ������ �ֵ�)�� ���δ�.
------------------------------------------------------------------------*/
void ParticleSystem::KillParticles()
{
	int i, j;


	// Kill all the particles that have gone below a certain height range.
	for (i = 0; i<m_maxParticles; i++)
	{
		if ((m_particleList[i].active == true) && (m_particleList[i].posY > 50.0f))
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
�̸�: UpdateBuffers()
�뵵:
- ���� ���۸� ���� �����Ѵ�.
- ��? ������ �� �� ���� ��ƼŬ�� ��ġ�� ������Ʈ �Ǵϱ�!
------------------------------------------------------------------------*/
bool ParticleSystem::UpdateBuffers()
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
	result = D3D::GetDeviceContext()->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)m_vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	D3D::GetDeviceContext()->Unmap(m_vertexBuffer, 0);

	return true;
}

void ParticleSystem::RenderBuffers()
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	D3D::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	D3D::GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer.
	D3D::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}


bool ParticleSystem::LoadTexture( WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize( filename);
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

void ParticleSystem::Render()
{

		RenderBuffers();
	
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

