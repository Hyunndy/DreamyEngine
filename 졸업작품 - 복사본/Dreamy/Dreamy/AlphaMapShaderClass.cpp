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
�̸�: Initialize()
�뵵: HLSL������ �ε��Ѵ�.
------------------------------------------------------------------------------------------------------------------------*/
bool AlphaMapShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	result = InitializeShader(device, hwnd, L"../Dreamy/alphamap.vs", L"../Dreamy/alphamap.ps");
	if (!result) { return false; }

	return true;
}
/*----------------------------------------------------------------------------------------------------------------------
�̸�:Render()
�뵵:
- !!!�ؽ��Ĺ迭!!!�� �����͸� ���ڷ� �޴´�.
- �̸� ���� ���̴��� ȥ���� �� �� �ؽ�ó�� ������ �� �ְ� �Ѵ�.
- indexCount = texturearray�� ����
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
�̸�:InitializeShader()
�뵵:
- ����/�ȼ����̴��� �ε��ϰ� ���̴��� vertextype���̾ƿ�, Matrix����, �ؽ�ó�� ���� ���°� �� app���� ��� ���ϰ���!! ������ ���������ο� �˷��ִ� desc���� �����Ѵ�.
- *���̾ƿ� : ���� ���̴����� vertextype�� ��� ������ ���������������ο� �˷��ִ� DESC����ü.
(LightShader�� ������ �븻���ͱ��� �־ ���̾ƿ��� 3���ε�, �ؽ��Ĵ� 2���̴�.)
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

	// ���� ���̴��� �ε��Ѵ�.
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

	// �ȼ� ���̴��� �ε��Ѵ�.
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



	// �������̴��� �ִ� VertexInput ���̾ƿ��� �����Ѵ�.
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

	//���̾ƿ��� ������ ����.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� ��ǲ ���̾ƿ��� �����Ѵ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result)) { return false; }

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed. 
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;
	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;


	// �������̴��� matrix ���� description.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;


	// Create the matrix constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) { return false; }

	// �ؽ�ó ���÷� ���� ����.
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
�̸�: SetShaderParameters()
�뵵:
- ������ �ϱ� ���� ��İ� �ؽ�ó �迭�� ���̴��� �����Ѵ�.
- 1. m_MatrixBuffer�� ���̴��� �ʿ��� ��ĵ��� �־ ���̴��� �����Ѵ�.
m_MatrixBuffer = �������̴��� �ִ� ��� ����(constant Buffer)
- 2. �ȼ����̴��� �ؽ�ó �迭�� �����Ѵ� -> PSSetShaderResources()

******��Ƽ�ؽ�ó�� �ؽ�ó�� �󸶳� ������ �����ش�. �̰��� Ȱ���ؼ�...��Ƽ�ؽ�ó ������ �����غ��°͵�..? ���� ����!
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

	//PSSetShaderResource(���̴������� �迭 ���� ����, �󸶳� ���� �ؽ�ó�� �����Ұ���, �ؽ�ó �迭�� �ּ�(pointer)).
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

