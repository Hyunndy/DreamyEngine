#include "TranslateShaderClass.h"



TranslateShaderClass::TranslateShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;

	m_matrixBuffer = 0;
	m_lightBuffer = 0;
	m_cameraBuffer = 0;
	m_translateBuffer = 0;
}

TranslateShaderClass::TranslateShaderClass(const TranslateShaderClass& other) { }

TranslateShaderClass::~TranslateShaderClass() {}

/*----------------------------------------------------------------------------------------------------------------------
이름: Initialize()
용도: HLSL파일을 로드한다.
------------------------------------------------------------------------------------------------------------------------*/
bool TranslateShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Dreamy/translate.vs", L"../Dreamy/translate.ps");
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
bool TranslateShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix,
	D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor
	, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower, float translation)
{
	bool result;

	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, textureArray, lightDirection, ambientColor, diffuseColor, cameraPosition, specularColor, specularPower, translation);
	if (!result) { return false; }

	RenderShader(deviceContext, indexCount);

	return true;
}
/*----------------------------------------------------------------------------------------------------------------------
이름:InitializeShader()
용도:
- 정점/픽셀셰이더를 로드하고 셰이더의 vertextype레이아웃, Matrix버퍼, 조명 버퍼, 텍스처의 샘플 상태가 이 app에서 어떻게 쓰일건지!! 렌더링 파이프라인에 알려주는 desc들을 정의한다.
- *레이아웃 : 정점 셰이더에서 vertexinputtype이 어떻게 쓰일지 렌더링파이프라인에 알려주는 DESC구조체.
- 범프맵 셰이더에선 정점셰이더의 인풋 레이아웃이 5개이다. (포지션, 텍스쳐, 노말, T벡터, B벡터)

- vertexinputtype만 여기서 하면 pixelinput은?! 그건 정점셰이더의 함수에서 하는일! return output!
------------------------------------------------------------------------------------------------------------------------*/
bool TranslateShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	ID3D10Blob* errorMessage;

	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[5]; // 정점 셰이더에있는 VERTEXINPUT

	unsigned int numElements;

	D3D11_BUFFER_DESC matrixBufferDesc; // 행렬 변환 상수 버퍼 DESC
	D3D11_BUFFER_DESC lightBufferDesc; // 조명 상수 버퍼 DESC
	D3D11_BUFFER_DESC cameraBufferDesc; // 카메라 상수 버퍼 D3SC
	D3D11_BUFFER_DESC translateBufferDesc;

	D3D11_SAMPLER_DESC samplerDesc; // 텍스처 샘플 상태

									// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// 정점 셰이더를 로드한다.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "TranslateVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
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
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "TranslatePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	polygonLayout[3].SemanticName = "TANGENT";
	polygonLayout[3].SemanticIndex = 0;
	polygonLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[3].InputSlot = 0;
	polygonLayout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[3].InstanceDataStepRate = 0;

	polygonLayout[4].SemanticName = "BINORMAL";
	polygonLayout[4].SemanticIndex = 0;
	polygonLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[4].InputSlot = 0;
	polygonLayout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[4].InstanceDataStepRate = 0;

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

	// 조명 상수 버퍼 DESC설정
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result)) { return false; }

	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(CameraBufferType);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the camera constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &m_cameraBuffer);
	if (FAILED(result)) { return false; }


	// 픽셀 셰이더에 있는 텍스쳐 이동을 위한 동적 상수 버퍼의 description을 작성합니다.
	translateBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	translateBufferDesc.ByteWidth = sizeof(TranslateBufferType);
	translateBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	translateBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
	translateBufferDesc.MiscFlags = 0; 
	translateBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&translateBufferDesc, NULL, &m_translateBuffer);
	if (FAILED(result)) { return false; }


	return true;

}

/*----------------------------------------------------------------------------------------------------------------------
이름: SetShaderParameters()
용도:
- 렌더링 하기 전에 행렬과 텍스처 배열을 셰이더에 전달한다.
- InitializeShader()은 셰이더들 안에있는 상수 버퍼들이 어떻게! 쓰일것인지 하드웨어에 알려주는것이고, 얘는 진짜 거기안에 들어갈 아이들

- 1. m_MatrixBuffer에 셰이더에 필요한 행렬들을 넣어서 정점셰이더에 전달한다.
MatrixBufferType = 정점셰이더에 있는 상수 버퍼(constant Buffer)
- 2. 픽셀셰이더에 텍스처 배열을 전달한다 -> PSSetShaderResources()
- 3. 픽셀셰이더에 있는 조명 상수 버퍼에 값을 넣어 픽셀셰이더에게 전달한다.
LightBufferType = 픽셀셰이더에 있는 조명 상수 버퍼

******멀티텍스처에 텍스처가 얼마나 들어갈건지 정해준다. 이것을 활용해서...멀티텍스처 갯수를 조정해보는것도..? 좋은 생각!
------------------------------------------------------------------------------------------------------------------------*/
bool TranslateShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView** textureArray, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffusecolor,
	D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower, float translation)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;
	CameraBufferType* dataPtr3;
	TranslateBufferType* dataPtr4;
	unsigned int bufferNumber;


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the matrix constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

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

	// Set shader texture array resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 3, textureArray);

	// Lock the light constant buffer so it can be written to.
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffusecolor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;

	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	result = deviceContext->Map(m_cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr3 = (CameraBufferType*)mappedResource.pData;

	dataPtr3->cameraPosition = cameraPosition;

	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_cameraBuffer);


	result = deviceContext->Map(m_translateBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}
	dataPtr4 = (TranslateBufferType*)mappedResource.pData;

	dataPtr4->translation = translation;

	deviceContext->Unmap(m_translateBuffer, 0);

	bufferNumber = 1;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_translateBuffer);


	return true;
}


void TranslateShaderClass::ShutdownShader()
{
	// Release the sampler state.
	if (m_sampleState) { m_sampleState->Release(); m_sampleState = 0; }

	// Release the matrix constant buffer.
	if (m_matrixBuffer) { m_matrixBuffer->Release(); m_matrixBuffer = 0; }

	// Release the matrix constant buffer.
	if (m_lightBuffer) { m_lightBuffer->Release(); m_lightBuffer = 0; }
	
	if (m_translateBuffer) { m_translateBuffer->Release(); m_translateBuffer = 0; }
	// Release the matrix constant buffer.
	if (m_cameraBuffer) { m_cameraBuffer->Release(); m_cameraBuffer = 0; }

	// Release the layout.
	if (m_layout) { m_layout->Release(); m_layout = 0; }

	// Release the pixel shader.
	if (m_pixelShader) { m_pixelShader->Release(); m_pixelShader = 0; }

	// Release the vertex shader.
	if (m_vertexShader) { m_vertexShader->Release(); m_vertexShader = 0; }

	return;
}

