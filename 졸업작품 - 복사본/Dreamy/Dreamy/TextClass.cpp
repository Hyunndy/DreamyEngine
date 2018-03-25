///////////////////////////////////////////////////////////////////////////////
// Filename: textclass.cpp
///////////////////////////////////////////////////////////////////////////////
#include "textclass.h"


TextClass::TextClass()
{
	m_Font = 0;
	m_FontShader = 0;

	m_sentence1 = 0;
	m_sentence2 = 0;
	m_sentence3 = 0;

	m_sentence4 = 0;
	m_sentence5 = 0;
	m_sentence6 = 0;

	m_sentence7 = 0;
	m_sentence8 = 0;
}


TextClass::TextClass(const TextClass& other)
{
}


TextClass::~TextClass()
{
}

/*----------------------------------------------------------------------------------------------------------------------------
이름 : Initialize()
용도 :
- 문장을 화면에 그려내기 위해 화면 크기와 기본 뷰 행렬을 저장한다.
- FontClass와 FontShaderClass를 생성&초기화 한다.
- 화면에 출력할 문장을 생성&초기화 한다.
- 화면에 출력할 문자는 위치,색을 조정할 수 있다.

- 폰트->셰이더->문장 순으로 생성&초기화되어야 한다.
- 폰트의 정점/인덱스 버퍼, 셰이더를 문장이 써먹어야 하니까!
------------------------------------------------------------------------------------------------------------------------------*/
bool TextClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd, int screenWidth, int screenHeight,
	D3DXMATRIX baseViewMatrix)
{
	bool result;


	// Store the screen width and height.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Store the base view matrix.
	m_baseViewMatrix = baseViewMatrix;

	// Create the font object.
	m_Font = new FontClass;
	if (!m_Font){ return false;	}

	// Initialize the font object.
	result = m_Font->Initialize(device, "../Dreamy/fontdata.txt", L"../Dreamy/font.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font object.", L"Error", MB_OK);
		return false;
	}

	// Create the font shader object.
	m_FontShader = new FontShaderClass;
	if (!m_FontShader) {	return false;	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize(device, hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the font shader object.", L"Error", MB_OK);
		return false;
	}

	// 문장을 생성&초기화 한다.
	result = InitializeSentence(&m_sentence1, 16, device);
	if (!result)	{ return false;	}

	result = InitializeSentence(&m_sentence2, 16, device);
	if (!result) { return false; }

	result = InitializeSentence(&m_sentence3, 16, device);
	if (!result) { return false; }

	result = InitializeSentence(&m_sentence4, 16, device);
	if (!result) { return false; }

	result = InitializeSentence(&m_sentence5, 16, device);
	if (!result) { return false; }

	result = InitializeSentence(&m_sentence6, 16, device);
	if (!result) { return false; }

	result = InitializeSentence(&m_sentence7, 16, device);
	if (!result) { return false; }

	result = InitializeSentence(&m_sentence8, 16, device);
	if (!result) { return false; }

	// 사용할 문장을 업데이트 한다.
	result = UpdateSentence(m_sentence1, "Dreamy", 30, 30, 1.0f, 0.0f, 0.0f, deviceContext);
	if (!result) { return false;}

	return true;
}

/*----------------------------------------------------------------------------------------------------------------------------
이름 : InitializeSentence()
용도 :
- Initialize()함수에서 문장을 생성&초기화할 때 호출되는 함수.
- 문자를 저장하고 그리기 위해 빈 정점 버퍼로 이루어진 SentenceType형의 변수를 생성한다.

- MaxLength는 정점 버퍼가 얼마나 커야하는지 결정한다.
- 모든 문장은 관련된 정점/인덱스 버퍼를 가지며 버퍼들은 이 함수의 앞부분에서 초기화된다.
- 문장의 내용을 언제든 바꿀 수 있도록 정점 버퍼의 description을 작성할 때 Usage를 dynamic으로 설정한다.
- 인덱스 버퍼는 항상 6으로 바뀔리 없기 때문에 정적 버퍼로 생성한다.
------------------------------------------------------------------------------------------------------------------------------*/
bool TextClass::InitializeSentence(SentenceType** sentence, int maxLength, ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create a new sentence object.
	*sentence = new SentenceType;
	if (!*sentence)
	{
		return false;
	}

	// Initialize the sentence buffers to null.
	(*sentence)->vertexBuffer = 0;
	(*sentence)->indexBuffer = 0;

	// Set the maximum length of the sentence.
	(*sentence)->maxLength = maxLength;

	// Set the number of vertices in the vertex array.
	(*sentence)->vertexCount = 6 * maxLength;

	// Set the number of indexes in the index array.
	(*sentence)->indexCount = (*sentence)->vertexCount;

	// Create the vertex array.
	vertices = new VertexType[(*sentence)->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[(*sentence)->indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * (*sentence)->vertexCount));

	// Initialize the index array.
	for (i = 0; i<(*sentence)->indexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * (*sentence)->vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &(*sentence)->vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * (*sentence)->indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &(*sentence)->indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}


/*----------------------------------------------------------------------------------------------------------------------------
이름 : UpdateSentence()
용도 :
- 입력된 문장에 맞게 정점 버퍼의 내용을 바꾼다.
- 왜냐면 프레임 마다 글씨가 바뀔 수도 있으니까!(마우스 포인터의 위치라던가)

- FontClass의 BuildVertexArray를 여기서 써먹는다.
------------------------------------------------------------------------------------------------------------------------------*/
bool TextClass::UpdateSentence(SentenceType* sentence, char* text, int positionX, int positionY, float red, float green, float blue,
	ID3D11DeviceContext* deviceContext)
{
	int numLetters;
	VertexType* vertices;
	float drawX, drawY;
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;


	// 문장의 길이와 색상을 정한다.
	sentence->red = red;
	sentence->green = green;
	sentence->blue = blue;


	numLetters = (int)strlen(text);


	if (numLetters > sentence->maxLength)
	{
		return false;
	}

	//정점 배열을 생성한다.
	vertices = new VertexType[sentence->vertexCount];
	if (!vertices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(VertexType) * sentence->vertexCount));

	// Calculate the X and Y pixel position on the screen to start drawing to.
	drawX = (float)(((m_screenWidth / 2) * -1) + positionX);
	drawY = (float)((m_screenHeight / 2) - positionY);

	//FONT클래스의 BUILDVERTEXARRY로 정점 배열을 만든다.
	m_Font->BuildVertexArray((void*)vertices, text, drawX, drawY);

	// 정점 배열의 정보를 문장 정점버퍼로 복사한다.
	result = deviceContext->Map(sentence->vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * sentence->vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(sentence->vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	return true;
}

/*----------------------------------------------------------------------------------------------------------------------------
이름 : RenderSentence()
용도 :
- 문장의 정점/인덱스 버퍼를 입력 어셈블러(I/A)에 넣고 FontShaderClass를 호출하여 입력된 버퍼들을 그리게 한다.
- m_BaseViewMatrix가 아니라 현재의 뷰 행렬을 사용한다.

- 이를 통해 현재 카메라가 어디있든 상관없이 화면의 같은 위치에 글자가 그려진다.
- 2d좌표기 때문에 정사영 행렬을 사용한다.

- 글꼴에 관련된 정점/인덱스 버퍼들과 그리는 함수들을 처음으로 I/A(입력조립기)단계에 넣는다.
- 입력조립기 다음 정점셰이더가 나온다는것 잊지말기!
------------------------------------------------------------------------------------------------------------------------------*/

bool TextClass::RenderSentence(ID3D11DeviceContext* deviceContext, SentenceType* sentence, D3DXMATRIX worldMatrix,
	D3DXMATRIX orthoMatrix)
{
	unsigned int stride, offset;
	D3DXVECTOR4 pixelColor;
	bool result;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &sentence->vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(sentence->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create a pixel color vector with the input sentence color.
	pixelColor = D3DXVECTOR4(sentence->red, sentence->green, sentence->blue, 1.0f);

	// Render the text using the font shader.
	result = m_FontShader->Render(deviceContext, sentence->indexCount, worldMatrix, m_baseViewMatrix, orthoMatrix, m_Font->GetTexture(),
		pixelColor);
	if (!result)
	{
		false;
	}

	return true;
}

/*----------------------------------------------------------------------------------------------------------------------------
이름 : Render()
용도 :
- GraphicsClass에서 호출되는 함수.
- RenderSentence()를 호출해서 문장을 화면에 그린다.
------------------------------------------------------------------------------------------------------------------------------*/
bool TextClass::Render(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX orthoMatrix)
{
	bool result;

	// Draw the first sentence.
	result = RenderSentence(deviceContext, m_sentence1, worldMatrix, orthoMatrix);
	result = RenderSentence(deviceContext, m_sentence2, worldMatrix, orthoMatrix);
	result = RenderSentence(deviceContext, m_sentence3, worldMatrix, orthoMatrix);
	result = RenderSentence(deviceContext, m_sentence4, worldMatrix, orthoMatrix);
	result = RenderSentence(deviceContext, m_sentence5, worldMatrix, orthoMatrix);
	result = RenderSentence(deviceContext, m_sentence6, worldMatrix, orthoMatrix);
	result = RenderSentence(deviceContext, m_sentence7, worldMatrix, orthoMatrix);
	result = RenderSentence(deviceContext, m_sentence8, worldMatrix, orthoMatrix);
	if (!result) {	return false;	}

	return true;
}

void TextClass::ReleaseSentence(SentenceType** sentence)
{
	if (*sentence)
	{
		// Release the sentence vertex buffer.
		if ((*sentence)->vertexBuffer)
		{
			(*sentence)->vertexBuffer->Release();
			(*sentence)->vertexBuffer = 0;
		}

		// Release the sentence index buffer.
		if ((*sentence)->indexBuffer)
		{
			(*sentence)->indexBuffer->Release();
			(*sentence)->indexBuffer = 0;
		}

		// Release the sentence.
		delete *sentence;
		*sentence = 0;
	}

	return;
}

void TextClass::Shutdown()
{
	// Release the first sentence.
	ReleaseSentence(&m_sentence1);
	ReleaseSentence(&m_sentence2);
	ReleaseSentence(&m_sentence3);
	ReleaseSentence(&m_sentence4);
	ReleaseSentence(&m_sentence5);
	ReleaseSentence(&m_sentence6);
	ReleaseSentence(&m_sentence7);
	ReleaseSentence(&m_sentence8);

	// Release the font shader object.
	if (m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the font object.
	if (m_Font)
	{
		m_Font->ShutDown();
		delete m_Font;
		m_Font = 0;
	}

	return;
}

bool TextClass::SetMousePosition(int mouseX, int mouseY, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char mouseString[16];
	bool result;


	// Convert the mouseX integer to string format.
	_itoa_s(mouseX, tempString, 10);

	// Setup the mouseX string.
	strcpy_s(mouseString, "MouseX: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence7, mouseString, 30, 130, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// Convert the mouseY integer to string format.
	_itoa_s(mouseY, tempString, 10);

	// Setup the mouseY string.
	strcpy_s(mouseString, "MouseY: ");
	strcat_s(mouseString, tempString);

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence8, mouseString, 110, 130, 1.0f, 1.0f, 1.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}



bool TextClass::SetPosition(float posX, float posY, float posZ, ID3D11DeviceContext* deviceContext)
{
	int positionX, positionY, positionZ;
	char tempString[16];
	char dataString[16];
	bool result;


	// Convert the position from floating point to integer.
	positionX = (int)posX;
	positionY = (int)posY;
	positionZ = (int)posZ;

	// Truncate the position if it exceeds either 9999 or -9999.
	if (positionX > 9999) { positionX = 9999; }
	if (positionY > 9999) { positionY = 9999; }
	if (positionZ > 9999) { positionZ = 9999; }

	if (positionX < -9999) { positionX = -9999; }
	if (positionY < -9999) { positionY = -9999; }
	if (positionZ < -9999) { positionZ = -9999; }

	// Setup the X position string.
	_itoa_s(positionX, tempString, 10);
	strcpy_s(dataString, "X: ");
	strcat_s(dataString, tempString);

	// 1535, 35
	// 1535, 55, 0.0f, 1.0f, 0.0f
	result = UpdateSentence(m_sentence4, dataString, 30, 105, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// Setup the Y position string.
	_itoa_s(positionY, tempString, 10);
	strcpy_s(dataString, "Y: ");
	strcat_s(dataString, tempString);

	result = UpdateSentence(m_sentence5, dataString, 78, 105, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	// Setup the Z position string.
	_itoa_s(positionZ, tempString, 10);
	strcpy_s(dataString, "Z: ");
	strcat_s(dataString, tempString);

	result = UpdateSentence(m_sentence6, dataString, 120, 105, 0.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;


}

/*----------------------------------------------------------------------------------------------------------------------------
이름 : SetFps(), SetCpu()
용도 :
- 입력으로 정수(int fps)를 받고 문자열(_itoa_s tempString)로 반환한다.
- fps값이 문자열로 변환되고 나면 이 숫자가 fps라는것을 나타내는 다른 문자열 뒤에 붙는다.
- 화면에 그려지기 위해 마지막에 UpdateSentence로 불려간다.

- Sentence2에 FPS그려지게했음!
- Sentence3에 Cpu사용량이 그려진다.
------------------------------------------------------------------------------------------------------------------------------*/
bool TextClass::SetFps(int fps, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char fpsString[16];
	float red, green, blue;
	bool result;


	// Truncate the fps to below 10,000.
	if (fps > 9999)
	{
		fps = 9999;
	}

	// Convert the fps integer to string format.
	_itoa_s(fps, tempString, 10);

	// Setup the fps string.
	strcpy_s(fpsString, "Fps: ");
	strcat_s(fpsString, tempString);

	// If fps is 60 or above set the fps color to green.
	if (fps >= 60)
	{
		red = 1.0f;
		green = 0.58039215686;
		blue = 0.21176470588;
	}

	// If fps is below 60 set the fps color to yellow.
	if (fps < 60)
	{
		red = 1.0f;
		green = 1.0f;
		blue = 0.0f;
	}

	// If fps is below 30 set the fps color to red.
	if (fps < 30)
	{
		red = 1.0f;
		green = 0.0f;
		blue = 0.0f;
	}

	//
	result = UpdateSentence(m_sentence2, fpsString, 30, 55, red, green, blue, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}


bool TextClass::SetCpu(int cpu, ID3D11DeviceContext* deviceContext)
{
	char tempString[16];
	char cpuString[16];
	bool result;


	// Convert the cpu integer to string format.
	_itoa_s(cpu, tempString, 10);

	// Setup the cpu string.
	strcpy_s(cpuString, "Cpu: ");
	strcat_s(cpuString, tempString);
	strcat_s(cpuString, "%");

	// Update the sentence vertex buffer with the new string information.
	result = UpdateSentence(m_sentence3, cpuString, 30, 80, 1.0f, 1.0f, 0.0f, deviceContext);
	if (!result)
	{
		return false;
	}

	return true;
}