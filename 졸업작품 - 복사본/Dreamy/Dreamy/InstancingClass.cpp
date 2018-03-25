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


bool InstancingClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	bool result;


	//모델의 기하학 데이터를 가져온다.
	result = LoadModel(modelFilename);
	if (!result) { return false; }

	//정점버퍼, 인스턴스 버퍼를 초기화 한다.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//모델에서 사용할 텍스처 파일의 이름을 인자로 받는다.
	result = LoadTexture(device, textureFilename);
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
이름: InitializeBuffers()
용도:
- 정점/인덱스 버퍼를 생성한다.
- 정점 배열이 ModelType 배열에서 로드되는 것으로 바뀌었다.
- ModelType배열은 tangent와 binormal값을 가지고 있기 때문에 이를 복사하고 나중에 정점 버퍼로 복사한다.
------------------------------------------------------------------------------------------------*/
bool InstancingClass::InitializeBuffers(ID3D11Device* device)
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


	// 정점/인덱스 배열에 삼각형의 각 점과 그 순서를 채워넣는다.
	// Direct3D 래스터화 상태의 디폴트는 시계 방향이 정방향이기 때문에, 점들을 시계 방향으로 채워야한다.
	// 이는 나중에 렌더 상태를 조종할 때 Rasterizer부분에서 반시계방향으로 설정할 수도 있다.


	// 정점과 인덱스 배열을 지정해준다.
	// 위 처럼 직접 그 값을 넣어주는 대신, for루프를 돌면서 m_model에 있는 정보들을 복사한다.
	for (i = 0; i < m_vertexCount; i++)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);

	}
	/*
	정점 배열과 인덱스 배열들이 다 채워졌으니 이를 이용해 정점 버퍼와 인덱스 버퍼를 생성한다.

	1. vertex형식 구조체
	2. vertex 입력 배열 서술(정점 셰이더에서 사용할)
	3. 입력 배치

	4. 위에서 했듯이 vertex형식 구조체에 정보를 채워넣는다.
	5. 생성될 버퍼의 description을 작성한다.
	(이 desc엔 버퍼의 크기, 버퍼의 타입 등이 들어간다.)

	6. description을 한 후에 subresource.psystem으로 정점 배열&인덱스 배열을 subresource 포인터에 연결한다.
	7. 최종적으로 description, subresource, 디바이스 context의 CreateBuffer함수를 이용해 새 버퍼의 포인터를 받아온다.
	*/

	// 정점 버퍼의 description을 작성한다.
	/*
	USAGE 4개
	1. 디폴트 = GPU가 버퍼의 자원을 읽고 써야한다면 이 용도를 지원한다.
	2. IMMUTABLE = 버퍼를 생성한 후에 전혀 변경하지 않을 것이라면 이 용도를 지정. 최적화의 여지가 있다.
	3. DYNAMIC = 응용 프로그램이 자원의 자료 내용을 매 프레임마다 갱신해야 된다면 이 용도를 지정한다. (비추천 용도)
	왜 비추천이냐면 이건 시스템RAM(CPU)메모리에서 GPU(비디오RAM)메모리로 전송해야 하기 때문이다.
	4. STAGING = CPU가 자원의 복사본을 읽어야 한다면이 용도를 지정 (비추천)
	*/
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT; // 버퍼가 쓰이는 방식
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount; //생성할 버퍼의 크기
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 무슨 버퍼인지
	vertexBufferDesc.CPUAccessFlags = 0; // cpu가 버퍼에 접근하는 방식
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// 정점 데이터를 가리키는 보조 리소스 구조체를 작성한다.
	// = 정점 배열을 가져온다.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// 정점 버퍼 생성
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
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
	result = device->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the instance array now that the instance buffer has been created and loaded.
	delete[] instances;
	instances = 0;

	// 필요 없어진 정점 배열, 인덱스 배열을 제거 한다.
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
	ifstream fin; // 텍스트 파일
	char input;
	int i;

	fin.open(filename);
	if (fin.fail()) { return false; }

	// 정점 갯수의 값을 읽는다?
	// :가 나올 때 까지 문자를 읽는다는것 같다.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	// fin으로 부터 데이터를 m_vetrexcount에 넣는것이다.
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
bool InstancingClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture) { return false; }

	result = m_Texture->Initialize(device, filename);
	if (!result) { return false; }

	return true;
}

void InstancingClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
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
	deviceContext->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

void InstancingClass::Render(ID3D11DeviceContext* deviceContext)
{
	//정점 버퍼와 인덱스 버퍼를 그래픽스 파이프라인에 넣어 화면에 그릴 준비를 한다.
	RenderBuffers(deviceContext);

	return;
}

void InstancingClass::Shutdown()
{

	//텍스처 객체를 반환한다.
	ReleaseTexture();
	//정점/셰이더 객체를 반환한다.
	ShutdownBuffers();
	//모델데이터를 반환한다.
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