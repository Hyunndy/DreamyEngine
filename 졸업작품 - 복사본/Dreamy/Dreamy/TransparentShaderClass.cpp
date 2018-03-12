////////////////////////////////////////////////////////////////////////////////
// Filename: TransparentShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "TransparentShaderClass.h"


TransparentShaderClass::TransparentShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
	m_transparentBuffer = 0;

}


TransparentShaderClass::TransparentShaderClass(const TransparentShaderClass& other)
{
}


TransparentShaderClass::~TransparentShaderClass()
{
}

// 셰이더의 초기화를 수행하는 함수를 호출한다.
bool TransparentShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Dreamy/transparent.vs", L"../Dreamy/transparent.ps");
	if (!result) { return false; }


	return true;
}


bool TransparentShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix,
									D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float blendAmount)
{
	bool result;

	// 렌더링에 사용할 셰이더의 인자를 입력한다.
	// 빛의 방향과 조명 색상을 입력 받는다.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, blendAmount);
	if (!result) { return false; }

	// 셰이더를 이용하여 준비된 버퍼를 그린다.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TransparentShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	//https://blog.naver.com/lifeisforu/80103346516
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC transparentBufferDesc;


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
	//-------텍스쳐 씌워져있는 삼각형을 다룰 때 쓰는 것-----------
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "TransparentVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
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

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "TransparentPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
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

	// 픽셀 셰이더에 포함된 투명도 동적 상수 버퍼의 description을 작성합니다.
	transparentBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	transparentBufferDesc.ByteWidth = sizeof(TransparentBufferType);
	transparentBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	transparentBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	transparentBufferDesc.MiscFlags = 0;
	transparentBufferDesc.StructureByteStride = 0;
	
	// 동적 상수 버퍼의 포인터를 생성하여 픽셀 셰이더의 상수 버퍼에 접근할 수 있게 합니다.
	result = device->CreateBuffer(&transparentBufferDesc, NULL, &m_transparentBuffer);
	if(FAILED(result)) { return false; } 



	return true;
}


void TransparentShaderClass::ShutdownShader()
{

	// 상수 버퍼 해제
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	if (m_transparentBuffer)
	{
		m_transparentBuffer->Release();
		m_transparentBuffer = 0;
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
bool TransparentShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, float blendAmount)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr; // 월드, 뷰, 프로젝션 행렬을 묶어놓은 행렬 상수 버퍼
	TransparentBufferType* dataPtr2;
	unsigned int bufferNumber;

	// 행렬을 transpose하여 셰이더에서 사용할 수 있게 한다.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

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

	result = deviceContext->Map(m_transparentBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	dataPtr2 = (TransparentBufferType*)mappedResource.pData;

	dataPtr2->blendAmount = blendAmount;

	deviceContext->Unmap(m_transparentBuffer, 0);


	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_transparentBuffer);



	return true;
}

