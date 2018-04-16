#include "InstancingClass.h"



InstancingClass::InstancingClass()
{
	m_vertexBuffer = 0;
	m_instanceBuffer = 0;
	m_Texture = 0;
	m_model = 0;
	InstancePosition = 0;
}

InstancingClass::InstancingClass(const InstancingClass& other) {}

InstancingClass::~InstancingClass() {}


bool InstancingClass::Initialize(char* modelFilename, WCHAR* textureFilename)
{
	bool result;


	//���� ������ �����͸� �����´�.
	result = LoadModel(modelFilename);
	if (!result) { return false; }

	//��������, �ν��Ͻ� ���۸� �ʱ�ȭ �Ѵ�.
	result = InitializeBuffers();
	if (!result) { return false; }

	//�𵨿��� ����� �ؽ�ó ������ �̸��� ���ڷ� �޴´�.
	result = LoadTexture( textureFilename);
	if (!result) { return false; }


	return true;
}

int InstancingClass::GetVertexCount()
{
	return m_vertexCount;
}

int InstancingClass::GetInstanceCount()
{
	return m_instanceCount;
}

/*----------------------------------------------------------------------------------------------
�̸�: InitializeBuffers()
�뵵:
- ����/�ε��� ���۸� �����Ѵ�.
- ���� �迭�� ModelType �迭���� �ε�Ǵ� ������ �ٲ����.
- ModelType�迭�� tangent�� binormal���� ������ �ֱ� ������ �̸� �����ϰ� ���߿� ���� ���۷� �����Ѵ�.
------------------------------------------------------------------------------------------------*/
bool InstancingClass::InitializeBuffers()
{

	VertexType* vertices;
	InstanceType* instances;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	HRESULT result;
	int i;


	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices) { return false; }


	// ����/�ε��� �迭�� �ﰢ���� �� ���� �� ������ ä���ִ´�.
	// Direct3D ������ȭ ������ ����Ʈ�� �ð� ������ �������̱� ������, ������ �ð� �������� ä�����Ѵ�.
	// �̴� ���߿� ���� ���¸� ������ �� Rasterizer�κп��� �ݽð�������� ������ ���� �ִ�.


	// ������ �ε��� �迭�� �������ش�.
	// �� ó�� ���� �� ���� �־��ִ� ���, for������ ���鼭 m_model�� �ִ� �������� �����Ѵ�.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

	}
	/*
	���� �迭�� �ε��� �迭���� �� ä�������� �̸� �̿��� ���� ���ۿ� �ε��� ���۸� �����Ѵ�.

	1. vertex���� ����ü
	2. vertex �Է� �迭 ����(���� ���̴����� �����)
	3. �Է� ��ġ

	4. ������ �ߵ��� vertex���� ����ü�� ������ ä���ִ´�.
	5. ������ ������ description�� �ۼ��Ѵ�.
	(�� desc�� ������ ũ��, ������ Ÿ�� ���� ����.)

	6. description�� �� �Ŀ� subresource.psystem���� ���� �迭&�ε��� �迭�� subresource �����Ϳ� �����Ѵ�.
	7. ���������� description, subresource, ����̽� context�� CreateBuffer�Լ��� �̿��� �� ������ �����͸� �޾ƿ´�.
	*/

	// ���� ������ description�� �ۼ��Ѵ�.
	/*
	USAGE 4��
	1. ����Ʈ = GPU�� ������ �ڿ��� �а� ����Ѵٸ� �� �뵵�� �����Ѵ�.
	2. IMMUTABLE = ���۸� ������ �Ŀ� ���� �������� ���� ���̶�� �� �뵵�� ����. ����ȭ�� ������ �ִ�.
	3. DYNAMIC = ���� ���α׷��� �ڿ��� �ڷ� ������ �� �����Ӹ��� �����ؾ� �ȴٸ� �� �뵵�� �����Ѵ�. (����õ �뵵)
	�� ����õ�̳ĸ� �̰� �ý���RAM(CPU)�޸𸮿��� GPU(����RAM)�޸𸮷� �����ؾ� �ϱ� �����̴�.
	4. STAGING = CPU�� �ڿ��� ���纻�� �о�� �Ѵٸ��� �뵵�� ���� (����õ)
	*/
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // ���۰� ���̴� ���
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount; //������ ������ ũ��
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // ���� ��������
	vertexBufferDesc.CPUAccessFlags = 0; // cpu�� ���ۿ� �����ϴ� ���
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// ���� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��Ѵ�.
	// = ���� �迭�� �����´�.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// ���� ���� ����
	result = D3D::GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}


	// Create the instance array.
	instances = new InstanceType[m_instanceCount];
	if (!instances)
	{
		return false;
	}

	for (int i = 0; i < m_instanceCount; i++)
	{
		instances[i].position = InstancePosition[i];
	}
	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	instanceBufferDesc.ByteWidth = sizeof(InstanceType) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = 0;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;
	
	delete[] InstancePosition;
	InstancePosition = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = D3D::GetDevice()->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the instance array now that the instance buffer has been created and loaded.
	delete[] instances;
	instances = 0;

	// �ʿ� ������ ���� �迭, �ε��� �迭�� ���� �Ѵ�.
	delete[] vertices;
	vertices = 0;


	return true;
}

void InstancingClass::SetInstanceCount(int InstanceCount)
{
	m_instanceCount = InstanceCount;
}

void InstancingClass::SetInstancePosition(float x, float y, float z)
{
	InstancePosition = new D3DXVECTOR3[m_instanceCount];
	
	for (int i = 0; i < m_instanceCount; i++)
	{
		InstancePosition[i] = { x+(i*40.0f), y, z  };
	}
	
	InstancePosition[2].y - 10.0f;
	
}

ID3D11ShaderResourceView* InstancingClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool InstancingClass::LoadModel(char* filename)
{
	ifstream fin; // �ؽ�Ʈ ����
	char input;
	int i;

	fin.open(filename);
	if (fin.fail()) { return false; }

	// ���� ������ ���� �д´�?
	// :�� ���� �� ���� ���ڸ� �д´ٴ°� ����.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	// fin���� ���� �����͸� m_vetrexcount�� �ִ°��̴�.
	fin >> m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if (!m_model) { return false; }


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
bool InstancingClass::LoadTexture( WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture) { return false; }

	result = m_Texture->Initialize( filename);
	if (!result) { return false; }

	return true;
}

void InstancingClass::RenderBuffers()
{
	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer strides.
	strides[0] = sizeof(VertexType);
	strides[1] = sizeof(InstanceType);


	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;
	bufferPointers[1] = m_instanceBuffer;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	D3D::GetDeviceContext()->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	D3D::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void InstancingClass::Render()
{
	//���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �־� ȭ�鿡 �׸� �غ� �Ѵ�.
	RenderBuffers();

	return;
}

void InstancingClass::Shutdown()
{

	//�ؽ�ó ��ü�� ��ȯ�Ѵ�.
	ReleaseTexture();
	//����/���̴� ��ü�� ��ȯ�Ѵ�.
	ShutdownBuffers();
	//�𵨵����͸� ��ȯ�Ѵ�.
	ReleaseModel();

	return;
}
void InstancingClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
	return;
}
void InstancingClass::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}
void InstancingClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}