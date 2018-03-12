////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "modelclass.h"


ModelClass::ModelClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;

}
ModelClass::ModelClass(const ModelClass& other){}

ModelClass::~ModelClass(){}
/*----------------------------------------------------------------------------------------------
이름: Initialize()
용도:
- 모델->버퍼->텍스처순으로 초기화 시킨다.
- LoadModel로 ModelFilename에 있는 내용돌을 m_model의 배열에 넣는다.
- m_model배열이 채워지면 이를 이용해 정점/인덱스 버퍼(기하구조)를 생성한다.
- LoadModel을 호출해서 데이터를 받아옴으로써 InitializeBuffer함수는 불러오는 모델 데이터에 의존한다.
------------------------------------------------------------------------------------------------*/
bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	
	//모델의 기하학 데이터를 가져온다.
	result = LoadModel(modelFilename);
	if (!result) { return false; }

	//정점버퍼, 인덱스 버퍼를 초기화 한다.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//모델에서 사용할 텍스처 파일의 이름을 인자로 받는다.
	result = LoadTexture(device, textureFilename);
	if (!result) { return false; }
	

	return true;
}

bool ModelClass::InitializeMulti(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, WCHAR* textureFilename2)
{
	bool result;


	//모델의 기하학 데이터를 가져온다.
	result = LoadModel(modelFilename);
	if (!result) { return false; }


	//정점버퍼, 인덱스 버퍼를 초기화 한다.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//모델에서 사용할 텍스처 파일의 이름을 인자로 받는다.
	result = LoadMultiTexture(device, textureFilename, textureFilename2);
	if (!result) { return false; }


	return true;
}
bool ModelClass::InitializeTriple(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, WCHAR* textureFilename2, WCHAR* textureFilename3)
{
	bool result;


	//모델의 기하학 데이터를 가져온다.
	result = LoadModel(modelFilename);
	if (!result) { return false; }


	//정점버퍼, 인덱스 버퍼를 초기화 한다.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//모델에서 사용할 텍스처 파일의 이름을 인자로 받는다.
	result = LoadTripleTexture(device, textureFilename, textureFilename2, textureFilename3);
	if (!result) { return false; }


	return true;
}
bool ModelClass::InitializeBumpMap(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, WCHAR* textureFilename2)
{
	bool result;


	//모델의 기하학 데이터를 가져온다.
	result = LoadModel(modelFilename);
	if (!result) { return false; }

	//모델 데이터가 로드되면 CalculateModelVector함수를 호출하여 tangent와 binormal을 계산한다.
	//이 함수는 또한 법선 벡터도 계산한다.
	CalculateModelVectors();

	//정점버퍼, 인덱스 버퍼를 초기화 한다.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//모델에서 사용할 텍스처 파일의 이름을 인자로 받는다.
	result = LoadMultiTexture(device, textureFilename, textureFilename2);
	if (!result) { return false; }


	return true;
}

bool ModelClass::InitializeSpecMap(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, WCHAR* textureFilename2, WCHAR* textureFilename3)
{
	bool result;


	//모델의 기하학 데이터를 가져온다.
	result = LoadModel(modelFilename);
	if (!result) { return false; }

	//모델 데이터가 로드되면 CalculateModelVector함수를 호출하여 tangent와 binormal을 계산한다.
	//이 함수는 또한 법선 벡터도 계산한다.
	CalculateModelVectors();

	//정점버퍼, 인덱스 버퍼를 초기화 한다.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//모델에서 사용할 텍스처 파일의 이름을 인자로 받는다.
	result = LoadTripleTexture(device, textureFilename, textureFilename2, textureFilename3);
	if (!result) { return false; }


	return true;
}

/*----------------------------------------------------------------------------------------------
이름: Render()
용도:
- GraphicsClass의 Render()에서 호출된다.
- RenderBuffer()을 호출해서 정점/인덱스 버퍼를 그래픽스 파이프라인에 넣어 컬러 셰이더가 이들을 그릴 수 있게 한다.

- ***기억할것은 "세이더, 셰이더에서 활용될 정점버퍼들의 DESC LAYOUT" 은 I/A단계에 셰이더클래스에서 들어가고,
  정점/인덱스버퍼(기하구조)는 MODELCLASS에서 I/A단계에 들어간다.
------------------------------------------------------------------------------------------------*/
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//정점 버퍼와 인덱스 버퍼를 그래픽스 파이프라인에 넣어 화면에 그릴 준비를 한다.
	RenderBuffers(deviceContext);

	return;
}

/*----------------------------------------------------------------------------------------------
이름: GetIndexCount()
용도: 픽셀 셰이더에서 모델을 그리기 위해 이 정보가 필요하다.
------------------------------------------------------------------------------------------------*/
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


/*----------------------------------------------------------------------------------------------
이름: InitializeBuffers()
용도:
- 정점/인덱스 버퍼를 생성한다.
- 정점 배열이 ModelType 배열에서 로드되는 것으로 바뀌었다.
- ModelType배열은 tangent와 binormal값을 가지고 있기 때문에 이를 복사하고 나중에 정점 버퍼로 복사한다.
------------------------------------------------------------------------------------------------*/
bool ModelClass::InitializeBuffers(ID3D11Device* device)
{

VertexType* vertices;
unsigned long* indices;
D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
D3D11_SUBRESOURCE_DATA vertexData, indexData;
HRESULT result;
int i;


// Create the vertex array.
vertices = new VertexType[m_vertexCount];
if (!vertices)	{	return false;	}

// Create the index array.
indices = new unsigned long[m_indexCount];
if (!indices)	{	return false;	}

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
	vertices[i].tangent = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
	vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);

//인덱스 배열은 불러올 때 해당 배열에서의 위치가 곧 인덱스 번호이기 때문에 간단히 지정한다.
	indices[i] = i;			
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

// 필요 없어진 정점 배열, 인덱스 배열을 제거 한다.
delete[] vertices;
vertices = 0;

delete[] indices;
indices = 0;

return true;
}


 /*----------------------------------------------------------------------------------------------
 이름: RenderBuffers()
 용도:
 - Render()에서 호출된다.
 - 이 함수의 목적은 InitializeBuffer()에서 생성한 정점/인덱스 버퍼를 GPU의 버퍼로서 활성화 시키는것.
 - 일단 GPU가 활성화 된 정점 버퍼를 가지게 되면 세이더를 이용해 버퍼의 내용을 그릴 수 있다.
 - 또한 정점들을 삼각형, 선분, 부채꼴 등 어떤 모양으로 그릴지 정의한다.
 ------------------------------------------------------------------------------------------------*/
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// 정점 버퍼의 단위와 오프셋을 설정한다.
	stride = sizeof(VertexType);
	offset = 0;

	// input assmbler(렌더링 파이프라인의 첫번째 단계)에 정점 버퍼를 활성화 하여 그려질 수 있게 한다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// 입력 조립기(I/A)에 인덱스 버퍼를 활성화 하여 그려질 수 있게 한다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// 정점 버퍼로 그릴 기본형을 설정한다. 여기선 삼각형. 부채꼴이 될 수도 있고 그렇다.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

/*----------------------------------------------------------------------------------------------
이름: LoadTexture()
용도: 제공된 파일 이름으로 텍스처 개체를 만들고 초기화한다.
------------------------------------------------------------------------------------------------*/

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture) { return false; }

	result = m_Texture->Initialize(device, filename);
	if (!result) { return false; }

	return true;
}
bool ModelClass::LoadMultiTexture(ID3D11Device* device, WCHAR* filename, WCHAR* filename2)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture) { return false; }

	result = m_Texture->InitializeMulti(device, filename, filename2);
	if (!result) { return false; }

	return true;
}
bool ModelClass::LoadTripleTexture(ID3D11Device* device, WCHAR* filename, WCHAR* filename2, WCHAR* filename3)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture) { return false; }

	result = m_Texture->InitializeTriple(device, filename, filename2, filename3);
	if (!result) { return false; }

	return true;
}

/*----------------------------------------------------------------------------------------------
이름: LoadModel()
용도:
- 텍스트 파일의 모델 데이터를 m_model배열에 로드한다.
  1. 파일을 열고 정점 수를 읽는다.
  2. ModelType의 배열을 생성하고 각 라인을 읽어 그 값을 배열에 넣는다.
  3. m_vertexCount와 m_IndexCount가 여기서 설정된다.
------------------------------------------------------------------------------------------------*/

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin; // 텍스트 파일
	char input;
	int i;

	fin.open(filename);
	if (fin.fail()) { return false; }

	// 정점 갯수의 값을 읽는다?
	// :가 나올 때 까지 문자를 읽는다는것 같다.
	fin.get(input);
	while(input != ':') 
	{ fin.get(input); }

	// Read in the vertex count.
	// fin으로 부터 데이터를 m_vetrexcount에 넣는것이다.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if(!m_model) { return false; }
	
	
	// Read up to the beginning of the data.
	fin.get(input);
	while(input != ':')
	{ fin.get(input); }
	fin.get(input);
	fin.get(input);
	// Read in the vertex data.
	for(i=0; i<m_vertexCount; i++) 
	{ 
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	} 
	// Close the model file.
	fin.close();

	return true;
}


/*----------------------------------------------------------------------------------------------
이름: ShutDown()
용도: 해제한다.
------------------------------------------------------------------------------------------------*/
void ModelClass::Shutdown()
{

	//텍스처 객체를 반환한다.
	ReleaseTexture();
	//정점/셰이더 객체를 반환한다.
	ShutdownBuffers();
	//모델데이터를 반환한다.
	ReleaseModel();

	return;
}
void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
	return;
}
void ModelClass::ShutdownBuffers()
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
void ModelClass::ReleaseModel()
{
	if (m_model)
	{
		delete[] m_model;
		m_model = 0;
	}

	return;
}


/*----------------------------------------------------------------------------------------------
이름: GetTexture()
용도:
-TextureClass에서 텍스쳐를 가져온다!
-텍스처 셰이더가 모델을 그리기 위해 이 텍스처에 접근할 수 있어야 하기 때문이다.
------------------------------------------------------------------------------------------------*/
ID3D11ShaderResourceView** ModelClass::GetMultiTextureArray()
{
	return m_Texture->GetMultiTextureArray();
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}
ID3D11ShaderResourceView** ModelClass::GetTripleTextureArray()
{
	return m_Texture->GetTripleTextureArray();
}

/*----------------------------------------------------------------------------------------------
이름: CalculateModelVectors()
용도:
- tangent, binormal, 법선 벡터를 계산한다.
- 1. 모델에 있는 삼각형들의 수를 계산한다.
- 2. 각 삼각형의 세 정점을 가지고 tangent, binormal, 법선을 계산한다.
- 3. 세 벡터들을 모델 구조체에 도로 저장한다.
------------------------------------------------------------------------------------------------*/
void ModelClass::CalculateModelVectors()
{
	int faceCount, i, index;
	TempVertexType vertex1, vertex2, vertex3;
	VectorType tangent, binormal, normal;


	// Calculate the number of faces in the model.
	faceCount = m_vertexCount / 3;

	// Initialize the index to the model data.
	index = 0;

	// Go through all the faces and calculate the the tangent, binormal, and normal vectors.
	for (i = 0; i<faceCount; i++)
	{
		// Get the three vertices for this face from the model.
		vertex1.x = m_model[index].x;
		vertex1.y = m_model[index].y;
		vertex1.z = m_model[index].z;
		vertex1.tu = m_model[index].tu;
		vertex1.tv = m_model[index].tv;
		vertex1.nx = m_model[index].nx;
		vertex1.ny = m_model[index].ny;
		vertex1.nz = m_model[index].nz;
		index++;

		vertex2.x = m_model[index].x;
		vertex2.y = m_model[index].y;
		vertex2.z = m_model[index].z;
		vertex2.tu = m_model[index].tu;
		vertex2.tv = m_model[index].tv;
		vertex2.nx = m_model[index].nx;
		vertex2.ny = m_model[index].ny;
		vertex2.nz = m_model[index].nz;
		index++;

		vertex3.x = m_model[index].x;
		vertex3.y = m_model[index].y;
		vertex3.z = m_model[index].z;
		vertex3.tu = m_model[index].tu;
		vertex3.tv = m_model[index].tv;
		vertex3.nx = m_model[index].nx;
		vertex3.ny = m_model[index].ny;
		vertex3.nz = m_model[index].nz;
		index++;

		// Calculate the tangent and binormal of that face.
		CalculateTangentBinormal(vertex1, vertex2, vertex3, tangent, binormal);

		// Calculate the new normal using the tangent and binormal.
		CalculateNormal(tangent, binormal, normal);

		// Store the normal, tangent, and binormal for this face back in the model structure.
		m_model[index - 1].nx = normal.x;
		m_model[index - 1].ny = normal.y;
		m_model[index - 1].nz = normal.z;
		m_model[index - 1].tx = tangent.x;
		m_model[index - 1].ty = tangent.y;
		m_model[index - 1].tz = tangent.z;
		m_model[index - 1].bx = binormal.x;
		m_model[index - 1].by = binormal.y;
		m_model[index - 1].bz = binormal.z;

		m_model[index - 2].nx = normal.x;
		m_model[index - 2].ny = normal.y;
		m_model[index - 2].nz = normal.z;
		m_model[index - 2].tx = tangent.x;
		m_model[index - 2].ty = tangent.y;
		m_model[index - 2].tz = tangent.z;
		m_model[index - 2].bx = binormal.x;
		m_model[index - 2].by = binormal.y;
		m_model[index - 2].bz = binormal.z;

		m_model[index - 3].nx = normal.x;
		m_model[index - 3].ny = normal.y;
		m_model[index - 3].nz = normal.z;
		m_model[index - 3].tx = tangent.x;
		m_model[index - 3].ty = tangent.y;
		m_model[index - 3].tz = tangent.z;
		m_model[index - 3].bx = binormal.x;
		m_model[index - 3].by = binormal.y;
		m_model[index - 3].bz = binormal.z;
	}

	return;
}

/*----------------------------------------------------------------------------------------------
이름: CalculateTangentBinormal()
용도:
- 입력된 세 정점으로 tangent와 binormal을 계산한다.
- 하나의 정점을 공유하는 면들의 t와 b를 평균화해서 평균 t, b를 생성한다.
------------------------------------------------------------------------------------------------*/

void ModelClass::CalculateTangentBinormal(TempVertexType vertex1, TempVertexType vertex2, TempVertexType vertex3,
	VectorType& tangent, VectorType& binormal)
{
	float vector1[3], vector2[3];
	float tuVector[2], tvVector[2];
	float den;
	float length;


	// Calculate the two vectors for this face.
	vector1[0] = vertex2.x - vertex1.x;
	vector1[1] = vertex2.y - vertex1.y;
	vector1[2] = vertex2.z - vertex1.z;

	vector2[0] = vertex3.x - vertex1.x;
	vector2[1] = vertex3.y - vertex1.y;
	vector2[2] = vertex3.z - vertex1.z;

	// Calculate the tu and tv texture space vectors.
	tuVector[0] = vertex2.tu - vertex1.tu;
	tvVector[0] = vertex2.tv - vertex1.tv;

	tuVector[1] = vertex3.tu - vertex1.tu;
	tvVector[1] = vertex3.tv - vertex1.tv;

	// Calculate the denominator of the tangent/binormal equation.
	den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

	// Calculate the cross products and multiply by the coefficient to get the tangent and binormal.
	tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
	tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
	tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

	binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
	binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
	binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

	// Calculate the length of this normal.
	length = sqrt((tangent.x * tangent.x) + (tangent.y * tangent.y) + (tangent.z * tangent.z));

	// Normalize the normal and then store it
	tangent.x = tangent.x / length;
	tangent.y = tangent.y / length;
	tangent.z = tangent.z / length;

	// Calculate the length of this normal.
	length = sqrt((binormal.x * binormal.x) + (binormal.y * binormal.y) + (binormal.z * binormal.z));

	// Normalize the normal and then store it
	binormal.x = binormal.x / length;
	binormal.y = binormal.y / length;
	binormal.z = binormal.z / length;

	return;
}


/*----------------------------------------------------------------------------------------------
이름: CalculateNormal()
용도:
- tangent와 binormal을 입력으로 받아 외적 연산을 하여 법선 벡터를 구한다.
- 왜 외적이냐면 t와 b에 있어서 n은 항상 수직이기 때문이다.
------------------------------------------------------------------------------------------------*/
void ModelClass::CalculateNormal(VectorType tangent, VectorType binormal, VectorType& normal)
{
	float length;


	// Calculate the cross product of the tangent and binormal which will give the normal vector.
	normal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	normal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	normal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);

	// Calculate the length of the normal.
	length = sqrt((normal.x * normal.x) + (normal.y * normal.y) + (normal.z * normal.z));

	// Normalize the normal.
	normal.x = normal.x / length;
	normal.y = normal.y / length;
	normal.z = normal.z / length;

	return;
}