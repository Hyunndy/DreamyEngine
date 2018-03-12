#include "AlphaMapShaderClass.h"



AlphaMapShaderClass::AlphaMapShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;

}

AlphaMapShaderClass::AlphaMapShaderClass(const AlphaMapShaderClass& other) { }

AlphaMapShaderClass::~AlphaMapShaderClass() {}

/*----------------------------------------------------------------------------------------------------------------------
이름: Initialize()
용도: HLSL파일을 로드한다.
------------------------------------------------------------------------------------------------------------------------*/
bool AlphaMapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Dreamy/alphamap.vs", L"../Dreamy/alphamap.ps");
	if (!result) { return false; }

	return true;
}
/*----------------------------------------------------------------------------------------------------------------------
이름:Render()
용도:
- !!!텍스쳐배열!!!의 포인터를 인자로 받는다.
- 이를 통해 셰이더가 혼합할 때 두 텍스처에 접근할 수 있게 한다.
- indexCount = texturearray의 갯수
------------------------------------------------------------------------------------------------------------------------*/
bool AlphaMapShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray);
	if (!result) { return false; }

	RenderShader(deviceContext, indexCount);

	return true;
}
/*----------------------------------------------------------------------------------------------------------------------
이름:InitializeShader()
용도:
- 정점/픽셀셰이더를 로드하고 셰이더의 vertextype레이아웃, Matrix버퍼, 텍스처의 샘플 상태가 이 app에서 어떻게 쓰일건지!! 렌더링 파이프라인에 알려주는 desc들을 정의한다.
- *레이아웃 : 정점 셰이더에서 vertextype이 어떻게 쓰일지 렌더링파이프라인에 알려주는 DESC구조체.
(LightShader는 정점에 노말벡터까지 있어서 레이아웃이 3개인데, 텍스쳐는 2개이다.)
------------------------------------------------------------------------------------------------------------------------*/
bool AlphaMapShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	ID3D10Blob* errorMessage;

	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];

	unsigned int numElements;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 셰이더를 로드한다.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "AlphaMapVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		else
		{
			MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
		}
		return false;
	}

	// 픽셀 셰이더를 로드한다.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "AlphaMapPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
		}
		else
		{
			MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
		}

		return false;
	}
	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) { return false; }

	// Create the vertex shader from the buffer. 
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) { return false; }



	// 정점셰이더에 있는 VertexInput 레이아웃을 설정한다.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
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

	//레이아웃의 갯수를 센다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// 정점 인풋 레이아웃을 생성한다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) { return false; }

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed. 
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	// 정점셰이더의 matrix 버퍼 description.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;


	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) { return false; }

	// 텍스처 샘플러 상태 생성.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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

	// Create the texture sampler state. 
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result)) { return false; }


	return true;

}

/*----------------------------------------------------------------------------------------------------------------------
이름: SetShaderParameters()
용도:
- 렌더링 하기 전에 행렬과 텍스처 배열을 셰이더에 전달한다.
- 1. m_MatrixBuffer에 셰이더에 필요한 행렬들을 넣어서 셰이더에 전달한다.
m_MatrixBuffer = 정점셰이더에 있는 상수 버퍼(constant Buffer)
- 2. 픽셀셰이더에 텍스처 배열을 전달한다 -> PSSetShaderResources()

******멀티텍스처에 텍스처가 얼마나 들어갈건지 정해준다. 이것을 활용해서...멀티텍스처 갯수를 조정해보는것도..? 좋은 생각!
------------------------------------------------------------------------------------------------------------------------*/
bool AlphaMapShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray)
{
	HRESULT result;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the matrix constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	// Get a pointer to the data in the constant buffer. 
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// Unlock the matrix constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the matrix constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the matrix constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	//PSSetShaderResource(셰이더에서의 배열 시작 시점, 얼마나 많은 텍스처를 전달할건지, 텍스처 배열의 주소(pointer)).
	deviceContext->PSSetShaderResources(0, 3, textureArray);

	return true;
}



void AlphaMapShaderClass::ShutdownShader()
{
	// Release the sampler state.
	if (m_sampleState) { m_sampleState->Release(); m_sampleState = 0; }

	// Release the matrix constant buffer.
	if (m_matrixBuffer) { m_matrixBuffer->Release(); m_matrixBuffer = 0; }

	// Release the layout.
	if (m_layout) { m_layout->Release(); m_layout = 0; }

	// Release the pixel shader.
	if (m_pixelShader) { m_pixelShader->Release(); m_pixelShader = 0; }

	// Release the vertex shader.
	if (m_vertexShader) { m_vertexShader->Release(); m_vertexShader = 0; }

	return;
}

