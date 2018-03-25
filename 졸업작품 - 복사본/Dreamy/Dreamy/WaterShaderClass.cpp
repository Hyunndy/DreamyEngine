#include "WaterShaderClass.h"



WaterShaderClass::WaterShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_matrixBuffer = 0;
	m_camNormBuffer = 0;
	m_waterBuffer = 0;
}

WaterShaderClass::WaterShaderClass(const WaterShaderClass& other)
{
}

WaterShaderClass::~WaterShaderClass()
{
}

bool WaterShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Dreamy/Data/water.vs", L"../Dreamy/Data/water.ps");
	if (!result) { return false; }

	return true;
}

void WaterShaderClass::Shutdown()
{
	ShutdownShader();

	return;
}

bool WaterShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, D3DXMATRIX reflectionMatrix, ID3D11ShaderResourceView* refractionTexture,
	/*ID3D11ShaderResourceView* reflectionTexture,*/ ID3D11ShaderResourceView* normalTexture, D3DXVECTOR3 cameraPosition,
	D3DXVECTOR2 normalMapTiling, float waterTranslation, float reflectRefractScale, D3DXVECTOR4 refractionTint,
	D3DXVECTOR3 lightDirection, float specularShininess)
{
	bool result; 
	
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, reflectionMatrix, refractionTexture,/* reflectionTexture,*/
		normalTexture, cameraPosition, normalMapTiling, waterTranslation, reflectRefractScale, refractionTint, lightDirection,
		specularShininess);
	if(!result) { return false; }

	
	RenderShader(deviceContext, indexCount);
	
	return true;


}

bool WaterShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix, D3DXMATRIX projectionMatrix,
	D3DXMATRIX reflectionMatrix, ID3D11ShaderResourceView* refractionTexture,
	/*ID3D11ShaderResourceView* reflectionTexture,*/ID3D11ShaderResourceView* normalTexture,
	D3DXVECTOR3 cameraPosition, D3DXVECTOR2 normalMapTiling, float waterTranslation, float reflectRefractScale,
	D3DXVECTOR4 refractionTint, D3DXVECTOR3 lightDirection, float specularShininess)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;

	// water.vs, water.ps에 있는 cbuffer들
	MatrixBufferType* dataPtr;
	CamNormBufferType* dataPtr2;
	WaterBufferType* dataPtr3;

	// cbuffer에 있는 행렬들을 전치시킨다.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);
	D3DXMatrixTranspose(&reflectionMatrix, &reflectionMatrix);

	// water.vs에 행렬 버퍼를 설정한다.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	// cbuffer에 접근할 포인터를 얻어온다.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// water.vs의 cbuffer인 MatrixBuffer의 값들에 실제 값들을 넣어준다.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;
	dataPtr->reflection = reflectionMatrix;


	deviceContext->Unmap(m_matrixBuffer, 0);

	bufferNumber = 0;

	// water.vs의 cbuffer에 matrixBuffer을 넣어주었다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// water.vs의 cbuffer에 있는 camnormBuffer에 접근한다.
	result = deviceContext->Map(m_camNormBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// CamNormBufferType에 접근하는 포인터를 가져온다.
	dataPtr2 = (CamNormBufferType*)mappedResource.pData;

	// cbuffer에 데이터를 넣어준다.
	dataPtr2->cameraPosition = cameraPosition;
	dataPtr2->padding1 = 0.0f;
	dataPtr2->normalMapTiling = normalMapTiling;
	dataPtr2->padding2 = D3DXVECTOR2(0.0f, 0.0f);


	deviceContext->Unmap(m_camNormBuffer, 0);

	bufferNumber = 1;

	// water.vs의 cbuffer에 m_camNormBuffer을 넣어주었다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_camNormBuffer);

	// water.ps의 맨 위에 써있는 Texture들에 실제 텍스처들을 넣어준다.
	deviceContext->PSSetShaderResources(0, 1, &refractionTexture);
	//deviceContext->PSSetShaderResources(1, 1, &reflectionTexture);
	deviceContext->PSSetShaderResources(1, 1, &normalTexture);

	result = deviceContext->Map(m_waterBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	// cbuffer에 접근할 포인터를 얻어온다.
	dataPtr3 = (WaterBufferType*)mappedResource.pData;

	// water.ps의 cbuffer인 WaterBuffer에 실제 값을 넣어준다.
	dataPtr3->waterTranslation = waterTranslation;
	dataPtr3->reflectRefractScale = reflectRefractScale;
	dataPtr3->refractionTint = refractionTint;
	dataPtr3->lightDirection = lightDirection;
	dataPtr3->specularShininess = specularShininess;
	dataPtr3->padding = D3DXVECTOR2(0.0f, 0.0f);


	deviceContext->Unmap(m_waterBuffer, 0);

	bufferNumber = 0;

	// water.ps의 cbuffer인 WaterBuffer에 m_waterBuffer을 넣어준다.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_waterBuffer);

	return true;
}

//void WaterShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
//{
//	// water.vs의 VERTEXINPUT의 Layout부터 I/A단계에 넣어준다.
//	deviceContext->IASetInputLayout(m_layout);
//	
//	// water.vs, water.ps의 셰이더들을 I/A단계에 넣어준다.
//	deviceContext->VSSetShader(m_vertexShader,NULL,0);
//	deviceContext->PSSetShader(m_pixelShader,NULL,0);
//
//	// water.ps의 SampleState를 I/A단계에 넣어준다.
//	deviceContext->PSSetSamplers(0, 1, &m_sampleState);
//
//	// 그려준다.
//	deviceContext->DrawIndexed(indexCount, 0, 0);
//
//	return;
//}

bool WaterShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	
	ID3D10Blob* vertexShaderBuffer; //정점셰이더 불러와서 생성
	ID3D10Blob* pixelShaderBuffer; // 픽셀셰이더 불러와서 생성
	
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2]; // water.vs파일에 있는 VERTEXINPUT구조체 설명
	unsigned int numElements;
	
	D3D11_SAMPLER_DESC samplerDesc; // 픽셀셰이더에서 텍스처 샘플 뽑을 때 samplerDesc
	D3D11_BUFFER_DESC matrixBufferDesc; // 정점셰이더 매트릭스버퍼 Desc.
	D3D11_BUFFER_DESC camNormBufferDesc; // 반사버퍼 Desc
	D3D11_BUFFER_DESC waterBufferDesc; // 물 버퍼 Desc.


	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// water.vs파일을 불러온다.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "WaterVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if(FAILED(result)) { if(errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, vsFilename); } else { MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK); } return false; }

	// water.ps파일을 불러온다.
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "WaterPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
	if(FAILED(result)) { if(errorMessage) { OutputShaderErrorMessage(errorMessage, hwnd, psFilename); } else { MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK); } return false; } 

	// ShaderClass에서 만들어준 VertexShader m_vertexShader에 water.vs에서 만든 VertexShader을 넣는다.
	// 정점셰이더 : 정점 하나를 출력하는 함수
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) { return false; }

	// ShaderClass에서 만들어준 PixelShader m_pixerlShader에 water.ps에서 만든 PixelShader을 넣는다.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result)) { return false; }


	// water.vs에서 만들어준 VERTEXINPUT구조체에 있는애들의 desc를 작성해준다.
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

	//polygonLayout이 몇 개인지 계산한다.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//ShaderClass에서 만들어준ID3D11InputLayout  m_layout에 VERTEXINPUT의 Layout을 넣어준다.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) { return false; }

	//다 사용했으니 해제해준다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// water.ps에서 만들어준 SampleState의 Desc를 작성해준다.
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
	
	// ShaderClass에서 만들어준 ID3D11SamplerState* m_sampleState에 saplerDesc를 넣어준다.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result)) { return false; }

	// water.vs와 ShaderClass에서 만들어준 cbuffer인 MatrixBuffer의 Desc를 작성해준다.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ID3D11Buffer* m_matrixBuffer에 Desc를 넣어서 최종적으로 버퍼를 생성해준다.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) { return false; }

	// Setup the description of the camera and normal tiling dynamic constant buffer that is in the vertex shader.
	camNormBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	camNormBufferDesc.ByteWidth = sizeof(CamNormBufferType);
	camNormBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	camNormBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	camNormBufferDesc.MiscFlags = 0;
	camNormBufferDesc.StructureByteStride = 0;;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&camNormBufferDesc, NULL, &m_camNormBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// water.ps에 있는 cbuffer인 WaterBuffer의 Desc를 작성해준다.
	waterBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	waterBufferDesc.ByteWidth = sizeof(WaterBufferType);
	waterBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	waterBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	waterBufferDesc.MiscFlags = 0;
	waterBufferDesc.StructureByteStride = 0;

	// ID3D11Buffer* m_waterBuffer에 Desc를 넣어서 최종적으로 버퍼를 생성해준다.
	result = device->CreateBuffer(&waterBufferDesc, NULL, &m_waterBuffer);
	if (FAILED(result)) { return false; }

	return true;
}

void WaterShaderClass::ShutdownShader()
{
	if(m_waterBuffer) { m_waterBuffer->Release(); m_waterBuffer = 0; }
	if (m_camNormBuffer) { m_camNormBuffer->Release(); m_camNormBuffer = 0; }
	if (m_matrixBuffer) { m_matrixBuffer->Release(); m_matrixBuffer = 0; }
	if (m_sampleState) { m_sampleState->Release(); m_sampleState = 0; }
	if (m_layout) { m_layout->Release(); m_layout = 0; }
	if (m_pixelShader) { m_pixelShader->Release(); m_pixelShader = 0; }
	if (m_vertexShader) { m_vertexShader->Release(); m_vertexShader = 0; }

}