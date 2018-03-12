#include "ImageClass.h"

ImageClass::ImageClass()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
}

ImageClass::ImageClass(const ImageClass&)
{
}

ImageClass::~ImageClass()
{
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
 이름 : Initialize()
 용도 :
 - 화면 크기와 이미지 크기를 저장한다.
 - 렌더링을 할 정확한 정점 위치가 필요하기 때문에(정확 할 필요는 없다.)
 - 크기 상관x

 - 이전 렌더링 위치 변수는 이전 프레임과 위치를 비교하여 위치가 변하지 않았다면 동적 정점 버퍼를 바꾸지 않기 때문에 성능 향상을 꾀할 수 있다.
 -------------------------------------------------------------------------------------------------------------------------------------------------------------
*/ 
bool ImageClass::Initialize(ID3D11Device* device, int screenWidth, int screenHegiht, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;

	//화면 크기 저장
	m_screenWidth = screenWidth;
	m_screenHeight = screenHegiht;

	//이미지 크기 저장
	m_bitmapWidth = bitmapWidth;
	m_bitmapHeight = bitmapHeight;

	//이전 위치 저장
	m_previousPosX = -1;
	m_previousPosY = -1;

	//정점&인덱스 버퍼 초기화(생성)
	result = InitializeBuffers(device);
	if (!result) { return false; }

	//이미지 불러오기
	result = LoadTexture(device, textureFilename);
	if (!result) { return false; }

	return true;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : Shutdown()
용도 :
- 나중에 GraphicsClass에서 호출된다.
- 2D이미지를 렌더링하기 위해 필요했던 포인터들을 모두 해제한다.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void ImageClass::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();

	return;
}
/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : Render()
용도 :
- 나중에 GraphicsClass에서 호출된다.
- 2D이미지의 버퍼를 그래픽카드에 넣는다.
- 입력으로 이미지가 화면에 그려질 위치를 받는다.
- UpdateBuffer() : 위치를 지정하는데 사용되어 이전 프레임과 위치가 바뀌었으면 동적 정점 버퍼를 새로 갱신한다.
- RenderBuffer() : 최종적으로 파이프라인에 보내질 정점/인덱스 버퍼를 준비한다.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
bool ImageClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	// 동적 정점 버퍼 리빌딩
	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result) { return false; }

	// 그래픽 파이프라인에 최종 정점/인덱스 버퍼를 보냄
	RenderBuffers(deviceContext);

	return true;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : GetIndexCount()
용도 :
- 2D 이미지의 정점 수를 리턴한다.
- 2D기 때문에 항상 6을 반환한다!
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
int ImageClass::GetIndexCount()
{
	return m_indexCount;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : GetIndexCount()
용도 :
- 2D 이미지의 정점 수를 리턴한다.
- 2D기 때문에 항상 6을 반환한다!
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
ID3D11ShaderResourceView* ImageClass::GetTexture()
{ 
	return m_Texture->GetTexture();
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : InitializeBuffers()
용도 :
- 2D 이미지를 그리는데 사용하는 정점/인덱스 버퍼를 만드는 데 사용된다.
- D3D11_BUFFER_DESC 구조체로 VertexType에 있는 정점들의 정보를 렌더링 파이프라인에 알려준다.
- D3D11_SUBRESOURCE_DATA 구조체는 서브리소스이다.

- 서브 리소스란? Direct3D는 리소스 전체 또는 리소스의 일부를 참조할 수 있는데, 이는 리소스의 일부분을 나타내기 위한 리소스의 부분집합이다.
- 텍스처에 대한 서브리소스는 텍스처가 가질 수 있는 밉맵 레벨과 텍스처 배열 때문에 조금 복잡하다.
- 하나의 버퍼는 기본적으로 하나의 서브 리소스로 정의된다.(버퍼=서브리소스)

- ImageClass는 ModelClass를 변형한것인데, 가장 큰 차이는 ImageClass에선 **동적정점버퍼**를 사용한다.
- 하지만 인덱스는 2D이미지이가 고정이기 때문에 항상 6이다. 따라서 인덱스 버퍼는 정적버퍼로 만든다.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
bool ImageClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// 2개의 삼각형을 만들어야 하므로 정점 수가 6.
	m_vertexCount = 6;

	// Set the number of indices in the index array.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
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
	memset(vertices, 0, (sizeof(VertexType) * m_vertexCount));

	// Load the index array with data.
	for (i = 0; i<m_indexCount; i++)
	{
		indices[i] = i;
	}

	// 동적 정점 버퍼 생성
	// D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

	//인덱스 버퍼는 정적 버퍼로 만든다.
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

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : ShutdwonBuffers()
용도 : 정점&인덱스 버퍼를 해제한다.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void ImageClass::ShutdownBuffers()
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

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : UpdateBuffers()
용도 : 
- 움직이지 않는 ModelClass와 다른점!

- InitializeBuffer()에서 동적 정점 버퍼를 생성해 놓은것을 UpdateBuffer()을 매 프레임 마다 돌아가는 Render()에 넣음으로써
  매 프레임 마다 불려 필요할 경우 동적 정점 버퍼의 내용을 새 위치로 변경하게 한다.
- 필요한 경우 : 좌표값이 바뀌는 경우

- 1. 위치가 바뀌었는지 검사한다.
- 2. 바뀌었다면 임시로 정점 배열을 생성하고, 그 안에 바뀐 데이터를 넣는다.
- 3. InitializeBuffer()에서 동적 정점 버퍼로 만들어 놓은것을 Map함수를 통해 내용을 변경할 수 있게 한다.
- 4. 버퍼 안의 데이터에 접근할 수 있는 포인터(verticesPtr)을 만들어 포인터에 memcpy로 2번에 만들어놓은 정점 배열의 데이터를 복사한다.
- 5. 버퍼를 해제시킨다.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
bool ImageClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;


	// 이미지의 위치가 이전과 비교하여 달라지지 않았다면 return true;로 그냥 빠져 나간다.
	// 이 확인은 많은 연산을 감소시킨다.
	if ((positionX == m_previousPosX) && (positionY == m_previousPosY))
	{
		return true;
	}

	// 바꼈다면 검사해보자!
	m_previousPosX = positionX;
	m_previousPosY = positionY;

	// 이미지의 네 변의 위치를 계산한다.
	// Direct3D에선 스크린 중앙이 (0.0)이다.
	// M_SCREENHEIGHT이런걸로 알아서 출력이 잘되게 해놓았다 이건.
	// TOP,BOTTOM은!!! 2차원 텍스처 좌표에서 오른쪽위가 (0,0)이다. 거기서부터 네모가 그려지기 때문에 POSITIONY를 -을 해줘야 왼쪽 아래로 이미지가 나타난다.
	left = (float)((m_screenWidth / 2) * -1) + (float)positionX;
	right = left + (float)m_bitmapWidth;

	top = (float)(m_screenHeight / 2) - (float)positionY;
	bottom = top - (float)m_bitmapHeight;

	// 임시 저장 정점 배열
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Load the vertex array with data.
	// First triangle.
	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);  // Bottom left.
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	// Second triangle.
	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);  // Top left.
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);  // Top right.
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);  // Bottom right.
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	//InitializeBuffer()에서 생성한 정점 버퍼를 Map함수를 통해 열어 내용 변경을 할 수 있게 한다. (lock함수랑 기능이 같은듯)
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// 버텍스 버퍼에 있는 데이터를 향한 포인터를 얻어온다.
	verticesPtr = (VertexType*)mappedResource.pData;

	// 위에 얻어온 포인터에 위에서 임시 저장한 vertices 정점 배열에 있는 내용을 복사한다.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// 포인터를 얻어와서 값을 바꿨기 때문에 값이 바뀐다.
	// 값을 바꾼 후 버텍스 버퍼를 다시 unlock해준다.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// 정점을 해제한다.
	delete[] vertices;
	vertices = 0;

	return true;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : RenderBuffers()
용도 : 
- 셰이더가 GPU에서 정점/인덱스 버퍼를 사용할 수 있도록 렌더링 파이프라인에 보낸다.
- 렌더링 파이프 라인 1단계 입력조립기(I/A)에 보낸다.
- I/A다음에 정점 셰이더 단계에 들어가게된다!
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void ImageClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : LoadTexture()
용도 : 2D 이미지로 그릴 텍스쳐를 로드한다. (텍스처 클래스로 부터)
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
bool ImageClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new TextureClass;
	if (!m_Texture)	{	return false;	}

	result = m_Texture->Initialize(device, filename);
	if (!result){	return false;	}

	return true;
}

/*
-------------------------------------------------------------------------------------------------------------------------------------------------------------
이름 : ReleaseTexuture()
용도 : 텍스처를 해제한다.
-------------------------------------------------------------------------------------------------------------------------------------------------------------
*/
void ImageClass::ReleaseTexture()
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



