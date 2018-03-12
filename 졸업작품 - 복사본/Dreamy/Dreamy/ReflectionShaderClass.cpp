////////////////////////////////////////////////////////////////////////////////
// Filename: ReflectionShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "ReflectionShaderClass.h"


ReflectionShaderClass::ReflectionShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;

	m_reflectionBuffer = 0;

}


ReflectionShaderClass::ReflectionShaderClass(const ReflectionShaderClass& other)
{
}


ReflectionShaderClass::~ReflectionShaderClass()
{
}

/*-----------------------------------------------------------------------------------------------------
이름: Initialize()
용도: 초기화
-----------------------------------------------------------------------------------------------------*/
bool ReflectionShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Dreamy/reflection.vs", L"../Dreamy/reflection.ps");
	if (!result) { return false; }


	return true;
}

/*-----------------------------------------------------------------------------------------------------
이름: Render()
용도:
- SetShaderParameter함수를 사용하여 셰이더에서 사용될 인자들을 설정한다.
- 인자들이 설정되면 RenderShader함수를 호출하여 HLSL셰이더를 이용한 물체 그리기를 수행한다.
-----------------------------------------------------------------------------------------------------*/

bool ReflectionShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture
									, ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, reflectionTexture, reflectionMatrix);
	if (!result) { return false; }

	RenderShader(deviceContext, indexCount);

	return true;
}

/*-----------------------------------------------------------------------------------------------------
이름: InitializeShader()
용도: 정점셰이더, 픽셀셰이더와 셰이더들 안에 선언되어있는 VERTEXINPUT에 대한 LAYOUT, 상수 버퍼들이 어떤식으로 쓰이는지 GPU에 알려준다.
-----------------------------------------------------------------------------------------------------*/
bool ReflectionShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	//https://blog.naver.com/lifeisforu/80103346516
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC reflectionBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// 함수에서 사용하는 포인터들을 0으로 초기화한다.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "ReflectionVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
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

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "ReflectionPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
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


	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;


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
	if (FAILED(result)) { return false; }

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


	// 정점 셰이더에 들어가는 반사 행렬의 동적 상수 버퍼에 대한 description을 작성합니다.
	reflectionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	reflectionBufferDesc.ByteWidth = sizeof(ReflectionBufferType);
	reflectionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	reflectionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	reflectionBufferDesc.MiscFlags = 0; 
	reflectionBufferDesc.StructureByteStride = 0;

	// 버퍼를 생성하여 반사 상수 버퍼가 정점 셰이더의 상수 버퍼에 접근할 수 있게 합니다.
	result = device->CreateBuffer(&reflectionBufferDesc, NULL, &m_reflectionBuffer);
	if(FAILED(result)) { return false; } 



	return true;
}


void ReflectionShaderClass::ShutdownShader()
{

	// 상수 버퍼 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	if (m_reflectionBuffer)
	{
		m_reflectionBuffer->Release();
		m_reflectionBuffer = 0;
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


/*-----------------------------------------------------------------------------------------------------
이름: SetShaderParameters()
용도: 셰이더 안에 선언된 상수 버퍼들에 실제 값을 넣어준다.
********************DX11에선 메모리 정렬 문제로 모든 입력 행렬들이 변환되어 있어야 하므로 반사 행렬도 변환해 준다***********************************************
-----------------------------------------------------------------------------------------------------*/
bool ReflectionShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* reflectionTexture, D3DXMATRIX reflectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr; // 월드, 뷰, 프로젝션 행렬을 묶어놓은 행렬 상수 버퍼
	ReflectionBufferType* dataPtr2; //반사 행렬 상수 버퍼
	unsigned int bufferNumber;

	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 한다.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);
	D3DXMatrixTranspose(&reflectionMatrix, &reflectionMatrix);

	// 상수 버퍼의 내용을 쓸 수 있도록 잠근다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

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

	
	result = deviceContext->Map(m_reflectionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	dataPtr2 = (ReflectionBufferType*)mappedResource.pData;
	
	dataPtr2->reflectionMatrix = reflectionMatrix;

	deviceContext->Unmap(m_reflectionBuffer, 0);

	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_reflectionBuffer);


	//픽셀 셰이더에 텍스처를 설정한다.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	
	//픽셀 셰이더에 반사 텍스처를 설정한다. 아 이거 순서가 중요하군다
	deviceContext->PSSetShaderResources(1, 1, &reflectionTexture);

	return true;
}

