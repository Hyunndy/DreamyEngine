#include "AlphaMapShaderClass.h"



DiffuseShaderClass::DiffuseShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
	m_lightBuffer = 0;

}

DiffuseShaderClass::DiffuseShaderClass(const DiffuseShaderClass& other) { }

DiffuseShaderClass::~DiffuseShaderClass() {}

/*----------------------------------------------------------------------------------------------------------------------
�̸�: Initialize()
�뵵: HLSL������ �ε��Ѵ�.
------------------------------------------------------------------------------------------------------------------------*/
bool DiffuseShaderClass::Initialize( HWND hwnd)
{
	bool result;

	result = InitializeShader( hwnd, L"../Dreamy/shader/diffuse.vs", L"../Dreamy/shader/diffuse.ps");
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
bool DiffuseShaderClass::Render(int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 diffuseColor)
{
	bool result;

	result = SetShaderParameters( worldMatrix, viewMatrix, projectionMatrix, texture, lightDirection, diffuseColor);
	if (!result) { return false; }

	RenderShader( indexCount);

	return true;
}
/*----------------------------------------------------------------------------------------------------------------------
�̸�:InitializeShader()
�뵵:
- ����/�ȼ����̴��� �ε��ϰ� ���̴��� vertextype���̾ƿ�, Matrix����, �ؽ�ó�� ���� ���°� �� app���� ��� ���ϰ���!! ������ ���������ο� �˷��ִ� desc���� �����Ѵ�.
- *���̾ƿ� : ���� ���̴����� vertextype�� ��� ������ ���������������ο� �˷��ִ� DESC����ü.
(LightShader�� ������ �븻���ͱ��� �־ ���̾ƿ��� 3���ε�, �ؽ��Ĵ� 2���̴�.)
------------------------------------------------------------------------------------------------------------------------*/
bool DiffuseShaderClass::InitializeShader( HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;

	ID3D10Blob* errorMessage;

	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

	unsigned int numElements;

	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���� ���̴��� �ε��Ѵ�.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "DiffuseVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
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
	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "DiffusePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
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
	result = D3D::GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result)) { return false; }

	// Create the vertex shader from the buffer. 
	result = D3D::GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
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

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	//���̾ƿ��� ������ ����.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� ��ǲ ���̾ƿ��� �����Ѵ�.
	result = D3D::GetDevice()->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
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
	result = D3D::GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) { return false; }

	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = D3D::GetDevice()->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer);
	if (FAILED(result))
	{
		return false;
	}


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
	result = D3D::GetDevice()->CreateSamplerState(&samplerDesc, &m_sampleState);
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
bool DiffuseShaderClass::SetShaderParameters(D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection,
	D3DXVECTOR4 diffuseColor)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	unsigned int bufferNumber;
	MatrixBufferType* dataPtr;
	LightBufferType* dataPtr2;


	// Transpose the matrices to prepare them for the shader.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// Lock the constant buffer so it can be written to.
	result = D3D::GetDeviceContext()->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
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

	// Unlock the constant buffer.
	D3D::GetDeviceContext()->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Now set the constant buffer in the vertex shader with the updated values.
	D3D::GetDeviceContext()->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	D3D::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);
	//The light constant buffer is setup the same way as the matrix constant buffer.We first lock the buffer and get a pointer to it.After that we set the diffuse color and light direction using that pointer.Once the data is set we unlock the buffer and then set it in the pixel shader.Note that we use the PSSetConstantBuffers function instead of VSSetConstantBuffers since this is a pixel shader buffer we are setting.

	// Lock the light constant buffer so it can be written to.
	result = D3D::GetDeviceContext()->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// Copy the lighting variables into the constant buffer.
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->padding = 0.0f;

	// Unlock the constant buffer.
	D3D::GetDeviceContext()->Unmap(m_lightBuffer, 0);

	// Set the position of the light constant buffer in the pixel shader.
	bufferNumber = 0;

	// Finally set the light constant buffer in the pixel shader with the updated values.
	D3D::GetDeviceContext()->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}



void DiffuseShaderClass::ShutdownShader()
{
	// Release the light constant buffer.
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}
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

