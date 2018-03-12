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
�̸�: Initialize()
�뵵:
- ��->����->�ؽ�ó������ �ʱ�ȭ ��Ų��.
- LoadModel�� ModelFilename�� �ִ� ���뵹�� m_model�� �迭�� �ִ´�.
- m_model�迭�� ä������ �̸� �̿��� ����/�ε��� ����(���ϱ���)�� �����Ѵ�.
- LoadModel�� ȣ���ؼ� �����͸� �޾ƿ����ν� InitializeBuffer�Լ��� �ҷ����� �� �����Ϳ� �����Ѵ�.
------------------------------------------------------------------------------------------------*/
bool ModelClass::Initialize(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	
	//���� ������ �����͸� �����´�.
	result = LoadModel(modelFilename);
	if (!result) { return false; }

	//��������, �ε��� ���۸� �ʱ�ȭ �Ѵ�.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//�𵨿��� ����� �ؽ�ó ������ �̸��� ���ڷ� �޴´�.
	result = LoadTexture(device, textureFilename);
	if (!result) { return false; }
	

	return true;
}

bool ModelClass::InitializeMulti(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, WCHAR* textureFilename2)
{
	bool result;


	//���� ������ �����͸� �����´�.
	result = LoadModel(modelFilename);
	if (!result) { return false; }


	//��������, �ε��� ���۸� �ʱ�ȭ �Ѵ�.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//�𵨿��� ����� �ؽ�ó ������ �̸��� ���ڷ� �޴´�.
	result = LoadMultiTexture(device, textureFilename, textureFilename2);
	if (!result) { return false; }


	return true;
}
bool ModelClass::InitializeTriple(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, WCHAR* textureFilename2, WCHAR* textureFilename3)
{
	bool result;


	//���� ������ �����͸� �����´�.
	result = LoadModel(modelFilename);
	if (!result) { return false; }


	//��������, �ε��� ���۸� �ʱ�ȭ �Ѵ�.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//�𵨿��� ����� �ؽ�ó ������ �̸��� ���ڷ� �޴´�.
	result = LoadTripleTexture(device, textureFilename, textureFilename2, textureFilename3);
	if (!result) { return false; }


	return true;
}
bool ModelClass::InitializeBumpMap(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, WCHAR* textureFilename2)
{
	bool result;


	//���� ������ �����͸� �����´�.
	result = LoadModel(modelFilename);
	if (!result) { return false; }

	//�� �����Ͱ� �ε�Ǹ� CalculateModelVector�Լ��� ȣ���Ͽ� tangent�� binormal�� ����Ѵ�.
	//�� �Լ��� ���� ���� ���͵� ����Ѵ�.
	CalculateModelVectors();

	//��������, �ε��� ���۸� �ʱ�ȭ �Ѵ�.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//�𵨿��� ����� �ؽ�ó ������ �̸��� ���ڷ� �޴´�.
	result = LoadMultiTexture(device, textureFilename, textureFilename2);
	if (!result) { return false; }


	return true;
}

bool ModelClass::InitializeSpecMap(ID3D11Device* device, char* modelFilename, WCHAR* textureFilename, WCHAR* textureFilename2, WCHAR* textureFilename3)
{
	bool result;


	//���� ������ �����͸� �����´�.
	result = LoadModel(modelFilename);
	if (!result) { return false; }

	//�� �����Ͱ� �ε�Ǹ� CalculateModelVector�Լ��� ȣ���Ͽ� tangent�� binormal�� ����Ѵ�.
	//�� �Լ��� ���� ���� ���͵� ����Ѵ�.
	CalculateModelVectors();

	//��������, �ε��� ���۸� �ʱ�ȭ �Ѵ�.
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//�𵨿��� ����� �ؽ�ó ������ �̸��� ���ڷ� �޴´�.
	result = LoadTripleTexture(device, textureFilename, textureFilename2, textureFilename3);
	if (!result) { return false; }


	return true;
}

/*----------------------------------------------------------------------------------------------
�̸�: Render()
�뵵:
- GraphicsClass�� Render()���� ȣ��ȴ�.
- RenderBuffer()�� ȣ���ؼ� ����/�ε��� ���۸� �׷��Ƚ� ���������ο� �־� �÷� ���̴��� �̵��� �׸� �� �ְ� �Ѵ�.

- ***����Ұ��� "���̴�, ���̴����� Ȱ��� �������۵��� DESC LAYOUT" �� I/A�ܰ迡 ���̴�Ŭ�������� ����,
  ����/�ε�������(���ϱ���)�� MODELCLASS���� I/A�ܰ迡 ����.
------------------------------------------------------------------------------------------------*/
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//���� ���ۿ� �ε��� ���۸� �׷��Ƚ� ���������ο� �־� ȭ�鿡 �׸� �غ� �Ѵ�.
	RenderBuffers(deviceContext);

	return;
}

/*----------------------------------------------------------------------------------------------
�̸�: GetIndexCount()
�뵵: �ȼ� ���̴����� ���� �׸��� ���� �� ������ �ʿ��ϴ�.
------------------------------------------------------------------------------------------------*/
int ModelClass::GetIndexCount()
{
	return m_indexCount;
}


/*----------------------------------------------------------------------------------------------
�̸�: InitializeBuffers()
�뵵:
- ����/�ε��� ���۸� �����Ѵ�.
- ���� �迭�� ModelType �迭���� �ε�Ǵ� ������ �ٲ����.
- ModelType�迭�� tangent�� binormal���� ������ �ֱ� ������ �̸� �����ϰ� ���߿� ���� ���۷� �����Ѵ�.
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
	vertices[i].tangent = D3DXVECTOR3(m_model[i].tx, m_model[i].ty, m_model[i].tz);
	vertices[i].binormal = D3DXVECTOR3(m_model[i].bx, m_model[i].by, m_model[i].bz);

//�ε��� �迭�� �ҷ��� �� �ش� �迭������ ��ġ�� �� �ε��� ��ȣ�̱� ������ ������ �����Ѵ�.
	indices[i] = i;			
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

// �ʿ� ������ ���� �迭, �ε��� �迭�� ���� �Ѵ�.
delete[] vertices;
vertices = 0;

delete[] indices;
indices = 0;

return true;
}


 /*----------------------------------------------------------------------------------------------
 �̸�: RenderBuffers()
 �뵵:
 - Render()���� ȣ��ȴ�.
 - �� �Լ��� ������ InitializeBuffer()���� ������ ����/�ε��� ���۸� GPU�� ���۷μ� Ȱ��ȭ ��Ű�°�.
 - �ϴ� GPU�� Ȱ��ȭ �� ���� ���۸� ������ �Ǹ� ���̴��� �̿��� ������ ������ �׸� �� �ִ�.
 - ���� �������� �ﰢ��, ����, ��ä�� �� � ������� �׸��� �����Ѵ�.
 ------------------------------------------------------------------------------------------------*/
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// ���� ������ ������ �������� �����Ѵ�.
	stride = sizeof(VertexType);
	offset = 0;

	// input assmbler(������ ������������ ù��° �ܰ�)�� ���� ���۸� Ȱ��ȭ �Ͽ� �׷��� �� �ְ� �Ѵ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// �Է� ������(I/A)�� �ε��� ���۸� Ȱ��ȭ �Ͽ� �׷��� �� �ְ� �Ѵ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// ���� ���۷� �׸� �⺻���� �����Ѵ�. ���⼱ �ﰢ��. ��ä���� �� ���� �ְ� �׷���.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

/*----------------------------------------------------------------------------------------------
�̸�: LoadTexture()
�뵵: ������ ���� �̸����� �ؽ�ó ��ü�� ����� �ʱ�ȭ�Ѵ�.
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
�̸�: LoadModel()
�뵵:
- �ؽ�Ʈ ������ �� �����͸� m_model�迭�� �ε��Ѵ�.
  1. ������ ���� ���� ���� �д´�.
  2. ModelType�� �迭�� �����ϰ� �� ������ �о� �� ���� �迭�� �ִ´�.
  3. m_vertexCount�� m_IndexCount�� ���⼭ �����ȴ�.
------------------------------------------------------------------------------------------------*/

bool ModelClass::LoadModel(char* filename)
{
	ifstream fin; // �ؽ�Ʈ ����
	char input;
	int i;

	fin.open(filename);
	if (fin.fail()) { return false; }

	// ���� ������ ���� �д´�?
	// :�� ���� �� ���� ���ڸ� �д´ٴ°� ����.
	fin.get(input);
	while(input != ':') 
	{ fin.get(input); }

	// Read in the vertex count.
	// fin���� ���� �����͸� m_vetrexcount�� �ִ°��̴�.
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
�̸�: ShutDown()
�뵵: �����Ѵ�.
------------------------------------------------------------------------------------------------*/
void ModelClass::Shutdown()
{

	//�ؽ�ó ��ü�� ��ȯ�Ѵ�.
	ReleaseTexture();
	//����/���̴� ��ü�� ��ȯ�Ѵ�.
	ShutdownBuffers();
	//�𵨵����͸� ��ȯ�Ѵ�.
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
�̸�: GetTexture()
�뵵:
-TextureClass���� �ؽ��ĸ� �����´�!
-�ؽ�ó ���̴��� ���� �׸��� ���� �� �ؽ�ó�� ������ �� �־�� �ϱ� �����̴�.
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
�̸�: CalculateModelVectors()
�뵵:
- tangent, binormal, ���� ���͸� ����Ѵ�.
- 1. �𵨿� �ִ� �ﰢ������ ���� ����Ѵ�.
- 2. �� �ﰢ���� �� ������ ������ tangent, binormal, ������ ����Ѵ�.
- 3. �� ���͵��� �� ����ü�� ���� �����Ѵ�.
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
�̸�: CalculateTangentBinormal()
�뵵:
- �Էµ� �� �������� tangent�� binormal�� ����Ѵ�.
- �ϳ��� ������ �����ϴ� ����� t�� b�� ���ȭ�ؼ� ��� t, b�� �����Ѵ�.
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
�̸�: CalculateNormal()
�뵵:
- tangent�� binormal�� �Է����� �޾� ���� ������ �Ͽ� ���� ���͸� ���Ѵ�.
- �� �����̳ĸ� t�� b�� �־ n�� �׻� �����̱� �����̴�.
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