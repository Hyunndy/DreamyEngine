////////////////////////////////////////////////////////////////////////////////
// Filename: LightShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "LightShaderClass.h"


LightShaderClass::LightShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
	m_lightBuffer = 0;
	m_cameraBuffer = 0;
}


LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}


LightShaderClass::~LightShaderClass()
{
}

// 셰이더의 초기화를 수행하는 함수를 호출한다.
bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// HLSL셰이더 파일의 이름을 넘겨준다.

	//1. 초록색 삼각형 render하는 셰이더
	//result = InitializeShader(device, hwnd, L"../Dreamy/color.vs", L"../Dreamy/color.ps");
	
	//2. 텍스처 붙어있는 삼각형 render하는 셰이더
	//result = InitializeShader(device, hwnd, L"../Dreamy/texture.vs", L"../Dreamy/texture.ps");
	//if (!result) { return false; }
	
	//3. 방향광 & 정반사광초기화
	result = InitializeShader(device, hwnd, L"../Dreamy/light.vs", L"../Dreamy/light.ps");
	if (!result) { return false; }

	return true;
}


// SetShaderParameter함수를 사용하여 셰이더에서 사용될 인자들을 설정한다.
// 인자들이 설정되면 RenderShader함수를 호출하여 HLSL셰이더를 이용한 물체 그리기를 수행한다.
// 텍스처 자원을 가리키는 포인터인 texture을 받는다. 이것은 setShaderParameter로 연결되어 렌더링에 사용될 셰이더에 연결된다.
bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix
						, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor
						, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	bool result;

	// 렌더링에 사용할 셰이더의 인자를 입력한다.
	// 빛의 방향과 조명 색상을 입력 받는다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture,lightDirection, ambientColor, diffuseColor
								, cameraPosition, specularColor, specularPower);
	if (!result)
	{
		return false;
	}

	// 셰이더를 이용하여 준비된 버퍼를 그린다.
	RenderShader(deviceContext, indexCount);

	return true;
}

/*
	용도:
	1. 실제로 셰이더 파일을 불러온다.
	2. DirectX와 GPU에서 사용가능하도록 한다.
	
	3. 레이아웃을 세팅하고 어떻게 정점 버퍼의 데이터가 GPU에서 사용되는지 볼 수 있다.
	(이 레이아웃은 modelclass와 color.vs에 선언된 VertexType과 일치해야 한다.)
*/
bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	//https://blog.naver.com/lifeisforu/80103346516
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3]; // 레이아웃에 법선 벡터를 사용할 수 있게 3개로 바꾼다.
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc; //조명 상수 버퍼
	D3D11_BUFFER_DESC cameraBufferDesc; // 카메라 상수 버퍼

	
	//텍스처 샘플러의 description 변수를 사용한다.
	D3D11_SAMPLER_DESC samplerDesc;

	// 함수에서 사용하는 포인터들을 0으로 초기화한다.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 셰이더를 컴파일한다.
	// 셰이더 프로그램을 버퍼로 컴파일 한다.
	// 셰이더 파일의 이름, 셰이더의 이름, 셰이더 버전(Direct11은 5.0), 셰이더가 컴파일 될 버퍼를 인자로 넘겨준다.)
	// 컴파일 과정이 실패하면 에러메세지를 errormessage문자열에 출력하고 이것은 다른 함수로 볼 수 있다.

	//새 조명 정점 셰이더를 로드한다.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			// 셰이더가 컴파일에 실패하면 에러 메세지를 기록한다.
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// 에러 메세지가 없다면 셰이더 파일을 찾지 못한것.
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		// If there was nothing in the error message then it simply could not find the file itself.
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	/*
	정점 셰이더와 픽셀 셰이더가 버퍼로 잘 컴파일되면 이를 이용해 셰이더 객체를 생성한다.
	*/

	// 정점 셰이더 생성
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// 픽셀 셰이더를 생성
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	/*
	셰이더에서 사용할 정점 데이터의 레이아웃을 생성한다.
	정점 구조체의 입력 설명 배열임!!!!!!!!-> 정점 구조체가 어떻게 쓰일지 Direct3D에 알려주는것.

	1. 이 셰이더에선 위치 벡터와 색상 벡터를 사용하므로 레이아웃에 각각의 벡터의 크기를 포함하는 두 레이아웃을 생성한다.
	2. SEMATICNAME : 이 요소가 레이아웃에서 어떻게 사용될지 알려준다.
	3. Format : 위치 벡터에 대해서는 32비트 3차원, 색상 벡터에선 알파값까지 32비트를 쓰게 한다.
	4. alignedbyteoffset : 각 요소가 어디서 부터 시작하는지 보여준다.

	1.정점 입력 레이아웃 description을 작성한다.
	이 설정은 modelclass와 셰이더에 있는 vertextype과 일치해야한다.

	2. 레이아웃의 description이 채워지면 이것의 크기를 갖고 D3D디바이스를 사용하여 입력 레이아웃을 생성한다.

	3. 레이아웃이 생성되면 정점/픽셀 셰이더 버퍼들은 더이상 사용되지 않으므ㅏ로 할당을 해제한다.

	SemanticName과 Folrmat이 ModelClass의 정의와 셰이더 파일에 있는 VertexTpye과 일치하도록 만들어야한다.
	*/
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	//polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	//polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	/*
	라이팅이 들어왔을 때 중요한 변경사항이 polygonLayout이다.
	여기서 조명 효과를 위하여 법선 벡터에 담을 세번째 요소를 추가한다.
	이 구조는 HLSL정점 세이더가 입력으로 받아들일 입력과 잘 맞아떨어지게 된다.
	*/

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT; // 자료형은 법선 벡터의 X,Y,Z,부동 소수점 변수를 담을 수 있는 자료형이다.
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;


	// 레이아웃의 요소 갯수를 가져온다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 입력 레이아웃을 생성한다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// 더이상 사용되지 않는 정점 셰이더 버퍼와 픽셀 셰이더 버퍼를 해제한다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	/*
	샘플러 상태의 description도 여기서 설정되며 나중에 픽셀 셰이더로 전달된다.
	샘플러 description에서 중요한것은 Filter이다.

	텍스처 표본(sample) 추출
	표본 추출기(sampler)는 텍스처에 적용할 필터링(확대,축소,비등방 필터링)을 정할 수 있다.

	Filter = 최종 도형 표면에서 텍스처의 어느 픽셀이 사용되거나 혼합될 것인지 결정하는 방법을 정한다.
	ex) D3D11_FILTER_MIN_MAG_MIP_LINEAR 옵션을 사용 : 이것은 축소, 확대, 밉레벨 샘플링 시 선형보간을 사용한다는 것이다.
	*/
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // 선형 보간 사용
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1; 
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; 
	samplerDesc.BorderColor[0] = 0; 
	samplerDesc.BorderColor[1] = 0; 
	samplerDesc.BorderColor[2] = 0; 
	samplerDesc.BorderColor[3] = 0; 
	samplerDesc.MinLOD = 0; 
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// 텍스처 렌더 상태를 생성한다.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result)) { return false; }

	/*
	정점 셰이더에서 보았던 것처럼 하나의 상수 버퍼를 갖고 있기 때문에 셰이더에 대한 인터페이스를 사용할 수 있다.
	매 프레임마다 상수 버퍼를 업데이트 하기 때문에 이것은 동적으로 한다.
	
	정점 버퍼에서 버퍼 description을 작성했듯이 상수 버퍼에서도 세팅할 수 있다.
	Bindflags로 상수 버퍼를 이 버퍼로 한다는것을 설정한다.

	여기서 matrixbuffer는 월드, 뷰, 투영행렬이다.
	*/
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // 응용 프로그램이 자원의 자료 내용을 매 프레임마다 갱신해야되기 때문에 이 용도를 지정한다.
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 셰이더 상수 버퍼에 접근할 수 있게 한다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))	{ return false; }

	/*
	01.22 정반사광(specular)을 위해 정점 셰이더에 카메라에 대한 상수 버퍼를 지정해주었으니까
	카메라 상수버퍼의 쓰임에 대해 작성해야 한다!
	용도: 정점 셰이더에 카메라의 위치를 정할 수 있게 해준다.
	*/
	// Setup the description of the camera dynamic constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if (FAILED(result))
	{
		return false;
	}


	/*
	조명의 색상과 방향을 다루는 조명 상수 버퍼의 description을 작성한다.
	특히 상수 버퍼의 크기가 16의 배수인지에 주의를 기울여야 한다.
	여기선 상수 버퍼의 크기가 28바이트기 때문에 4바이트의 padding을 붙여 32바이트로 만들었다.
	*/
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}


	return true;
}


void LightShaderClass::ShutdownShader()
{


	//조명 버퍼 해제
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}
	if (m_cameraBuffer)
	{
		m_cameraBuffer->Release();
		m_cameraBuffer = 0;
	}	
	// 상수 버퍼 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	// 샘플러 상태 해제
	if (m_sampleState)
	{ 
		m_sampleState->Release();
		m_sampleState = 0;
	}



	// 레이아웃 해제
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// 픽셀 셰이더 해제
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// 정점 셰이더 해제
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


// 셰이더의 전역 변수를 쉽게 다를 수 있도록 하기 위해 만들어진 함수,
// 이 함수에 사용된 행렬들은 GraphicsClass에서 만들어졌다.
// 이제 텍스처 자원의 포인터를 인자로 받고 그것을 셰이더에 등록한다.
// ***텍스처는 반드시 버퍼에 렌더링이 일어나기 전에 설정되어 있어야 한다***
bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor
	, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr; // 월드, 뷰, 프로젝션 행렬을 묶어놓은 행렬 상수 버퍼
	unsigned int bufferNumber;
	LightBufferType* dataPtr2; // 조명 상수 버퍼
	CameraBufferType* dataPtr3; // 카메라 상수 버퍼


	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 한다.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠근다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))	{	return false;	}

	// 상수 버퍼의 데이터에 대한 포인터를 가져온다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// 상수 버퍼에 행렬을 복사한다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// 상수 버퍼의 잠금을 푼다.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// 정점 셰이더에서의 상수 버퍼의 위치를 설정한다.
	bufferNumber = 0;

	// 정점 셰이더의 상수 버퍼를 바뀐 값으로 바꾼다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);


	// Lock the camera constant buffer so it can be written to.
	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}


	// Get a pointer to the data in the constant buffer.
	dataPtr3 = (CameraBufferType*)mappedResource.pData;

	// Copy the camera position into the constant buffer.
	dataPtr3->cameraPosition = cameraPosition;
	dataPtr3->padding = 0.0f;

	// Unlock the camera constant buffer.
	deviceContext->Unmap(m_cameraBuffer, 0);

	// Set the position of the camera constant buffer in the vertex shader.
	bufferNumber = 1;

	// Now set the camera constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	// 조명 상수 버퍼에 잠금(lock)을 걸고 버퍼의 포인터 획득
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)) { return false; }
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// copy the lighting variables into the constant buffer.
	// 포인터를 이용하여 빛의 색상과 방향을 설정한다.
	// 빛이 조명 버퍼에 매핑되고 그려지기 전에 픽셀셰이더의 상수 버퍼로 세팅된다.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;



	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// 픽셀 셰이더의 조명 상수 버퍼의 위치를 설정한다.
	bufferNumber = 0;

	// 픽셀 셰이더를 설정하는것이기 때문에 PS상수버퍼이다.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}
