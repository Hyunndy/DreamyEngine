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

// ���̴��� �ʱ�ȭ�� �����ϴ� �Լ��� ȣ���Ѵ�.
bool LightShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	// HLSL���̴� ������ �̸��� �Ѱ��ش�.

	//1. �ʷϻ� �ﰢ�� render�ϴ� ���̴�
	//result = InitializeShader(device, hwnd, L"../Dreamy/color.vs", L"../Dreamy/color.ps");
	
	//2. �ؽ�ó �پ��ִ� �ﰢ�� render�ϴ� ���̴�
	//result = InitializeShader(device, hwnd, L"../Dreamy/texture.vs", L"../Dreamy/texture.ps");
	//if (!result) { return false; }
	
	//3. ���Ɽ & ���ݻ籤�ʱ�ȭ
	result = InitializeShader(device, hwnd, L"../Dreamy/light.vs", L"../Dreamy/light.ps");
	if (!result) { return false; }

	return true;
}


// SetShaderParameter�Լ��� ����Ͽ� ���̴����� ���� ���ڵ��� �����Ѵ�.
// ���ڵ��� �����Ǹ� RenderShader�Լ��� ȣ���Ͽ� HLSL���̴��� �̿��� ��ü �׸��⸦ �����Ѵ�.
// �ؽ�ó �ڿ��� ����Ű�� �������� texture�� �޴´�. �̰��� setShaderParameter�� ����Ǿ� �������� ���� ���̴��� ����ȴ�.
bool LightShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,D3DXMATRIX projectionMatrix
						, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor
						, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	bool result;

	// �������� ����� ���̴��� ���ڸ� �Է��Ѵ�.
	// ���� ����� ���� ������ �Է� �޴´�.
	result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture,lightDirection, ambientColor, diffuseColor
								, cameraPosition, specularColor, specularPower);
	if (!result)
	{
		return false;
	}

	// ���̴��� �̿��Ͽ� �غ�� ���۸� �׸���.
	RenderShader(deviceContext, indexCount);

	return true;
}

/*
	�뵵:
	1. ������ ���̴� ������ �ҷ��´�.
	2. DirectX�� GPU���� ��밡���ϵ��� �Ѵ�.
	
	3. ���̾ƿ��� �����ϰ� ��� ���� ������ �����Ͱ� GPU���� ���Ǵ��� �� �� �ִ�.
	(�� ���̾ƿ��� modelclass�� color.vs�� ����� VertexType�� ��ġ�ؾ� �Ѵ�.)
*/
bool LightShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{
	//https://blog.naver.com/lifeisforu/80103346516
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3]; // ���̾ƿ��� ���� ���͸� ����� �� �ְ� 3���� �ٲ۴�.
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC lightBufferDesc; //���� ��� ����
	D3D11_BUFFER_DESC cameraBufferDesc; // ī�޶� ��� ����

	
	//�ؽ�ó ���÷��� description ������ ����Ѵ�.
	D3D11_SAMPLER_DESC samplerDesc;

	// �Լ����� ����ϴ� �����͵��� 0���� �ʱ�ȭ�Ѵ�.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// ���� ���̴��� �������Ѵ�.
	// ���̴� ���α׷��� ���۷� ������ �Ѵ�.
	// ���̴� ������ �̸�, ���̴��� �̸�, ���̴� ����(Direct11�� 5.0), ���̴��� ������ �� ���۸� ���ڷ� �Ѱ��ش�.)
	// ������ ������ �����ϸ� �����޼����� errormessage���ڿ��� ����ϰ� �̰��� �ٸ� �Լ��� �� �� �ִ�.

	//�� ���� ���� ���̴��� �ε��Ѵ�.
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(result))
	{
		if (errorMessage)
		{
			// ���̴��� �����Ͽ� �����ϸ� ���� �޼����� ����Ѵ�.
			OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// ���� �޼����� ���ٸ� ���̴� ������ ã�� ���Ѱ�.
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
	���� ���̴��� �ȼ� ���̴��� ���۷� �� �����ϵǸ� �̸� �̿��� ���̴� ��ü�� �����Ѵ�.
	*/

	// ���� ���̴� ����
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// �ȼ� ���̴��� ����
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if (FAILED(result))
	{
		return false;
	}

	/*
	���̴����� ����� ���� �������� ���̾ƿ��� �����Ѵ�.
	���� ����ü�� �Է� ���� �迭��!!!!!!!!-> ���� ����ü�� ��� ������ Direct3D�� �˷��ִ°�.

	1. �� ���̴����� ��ġ ���Ϳ� ���� ���͸� ����ϹǷ� ���̾ƿ��� ������ ������ ũ�⸦ �����ϴ� �� ���̾ƿ��� �����Ѵ�.
	2. SEMATICNAME : �� ��Ұ� ���̾ƿ����� ��� ������ �˷��ش�.
	3. Format : ��ġ ���Ϳ� ���ؼ��� 32��Ʈ 3����, ���� ���Ϳ��� ���İ����� 32��Ʈ�� ���� �Ѵ�.
	4. alignedbyteoffset : �� ��Ұ� ��� ���� �����ϴ��� �����ش�.

	1.���� �Է� ���̾ƿ� description�� �ۼ��Ѵ�.
	�� ������ modelclass�� ���̴��� �ִ� vertextype�� ��ġ�ؾ��Ѵ�.

	2. ���̾ƿ��� description�� ä������ �̰��� ũ�⸦ ���� D3D����̽��� ����Ͽ� �Է� ���̾ƿ��� �����Ѵ�.

	3. ���̾ƿ��� �����Ǹ� ����/�ȼ� ���̴� ���۵��� ���̻� ������ �����Ǥ��� �Ҵ��� �����Ѵ�.

	SemanticName�� Folrmat�� ModelClass�� ���ǿ� ���̴� ���Ͽ� �ִ� VertexTpye�� ��ġ�ϵ��� �������Ѵ�.
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
	�������� ������ �� �߿��� ��������� polygonLayout�̴�.
	���⼭ ���� ȿ���� ���Ͽ� ���� ���Ϳ� ���� ����° ��Ҹ� �߰��Ѵ�.
	�� ������ HLSL���� ���̴��� �Է����� �޾Ƶ��� �Է°� �� �¾ƶ������� �ȴ�.
	*/

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT; // �ڷ����� ���� ������ X,Y,Z,�ε� �Ҽ��� ������ ���� �� �ִ� �ڷ����̴�.
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;


	// ���̾ƿ��� ��� ������ �����´�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
		vertexShaderBuffer->GetBufferSize(), &m_layout);
	if (FAILED(result))
	{
		return false;
	}

	// ���̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���۸� �����Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	/*
	���÷� ������ description�� ���⼭ �����Ǹ� ���߿� �ȼ� ���̴��� ���޵ȴ�.
	���÷� description���� �߿��Ѱ��� Filter�̴�.

	�ؽ�ó ǥ��(sample) ����
	ǥ�� �����(sampler)�� �ؽ�ó�� ������ ���͸�(Ȯ��,���,���� ���͸�)�� ���� �� �ִ�.

	Filter = ���� ���� ǥ�鿡�� �ؽ�ó�� ��� �ȼ��� ���ǰų� ȥ�յ� ������ �����ϴ� ����� ���Ѵ�.
	ex) D3D11_FILTER_MIN_MAG_MIP_LINEAR �ɼ��� ��� : �̰��� ���, Ȯ��, �ӷ��� ���ø� �� ���������� ����Ѵٴ� ���̴�.
	*/
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // ���� ���� ���
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

	// �ؽ�ó ���� ���¸� �����Ѵ�.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result)) { return false; }

	/*
	���� ���̴����� ���Ҵ� ��ó�� �ϳ��� ��� ���۸� ���� �ֱ� ������ ���̴��� ���� �������̽��� ����� �� �ִ�.
	�� �����Ӹ��� ��� ���۸� ������Ʈ �ϱ� ������ �̰��� �������� �Ѵ�.
	
	���� ���ۿ��� ���� description�� �ۼ��ߵ��� ��� ���ۿ����� ������ �� �ִ�.
	Bindflags�� ��� ���۸� �� ���۷� �Ѵٴ°��� �����Ѵ�.

	���⼭ matrixbuffer�� ����, ��, ��������̴�.
	*/
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // ���� ���α׷��� �ڿ��� �ڷ� ������ �� �����Ӹ��� �����ؾߵǱ� ������ �� �뵵�� �����Ѵ�.
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� ������ �� �ְ� �Ѵ�.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result))	{ return false; }

	/*
	01.22 ���ݻ籤(specular)�� ���� ���� ���̴��� ī�޶� ���� ��� ���۸� �������־����ϱ�
	ī�޶� ��������� ���ӿ� ���� �ۼ��ؾ� �Ѵ�!
	�뵵: ���� ���̴��� ī�޶��� ��ġ�� ���� �� �ְ� ���ش�.
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
	������ ����� ������ �ٷ�� ���� ��� ������ description�� �ۼ��Ѵ�.
	Ư�� ��� ������ ũ�Ⱑ 16�� ��������� ���Ǹ� ��￩�� �Ѵ�.
	���⼱ ��� ������ ũ�Ⱑ 28����Ʈ�� ������ 4����Ʈ�� padding�� �ٿ� 32����Ʈ�� �������.
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


	//���� ���� ����
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
	// ��� ���� ����
	if (m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}
	// ���÷� ���� ����
	if (m_sampleState)
	{ 
		m_sampleState->Release();
		m_sampleState = 0;
	}



	// ���̾ƿ� ����
	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// �ȼ� ���̴� ����
	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// ���� ���̴� ����
	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}


// ���̴��� ���� ������ ���� �ٸ� �� �ֵ��� �ϱ� ���� ������� �Լ�,
// �� �Լ��� ���� ��ĵ��� GraphicsClass���� ���������.
// ���� �ؽ�ó �ڿ��� �����͸� ���ڷ� �ް� �װ��� ���̴��� ����Ѵ�.
// ***�ؽ�ó�� �ݵ�� ���ۿ� �������� �Ͼ�� ���� �����Ǿ� �־�� �Ѵ�***
bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture, D3DXVECTOR3 lightDirection, D3DXVECTOR4 ambientColor, D3DXVECTOR4 diffuseColor
	, D3DXVECTOR3 cameraPosition, D3DXVECTOR4 specularColor, float specularPower)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr; // ����, ��, �������� ����� ������� ��� ��� ����
	unsigned int bufferNumber;
	LightBufferType* dataPtr2; // ���� ��� ����
	CameraBufferType* dataPtr3; // ī�޶� ��� ����


	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))	{	return false;	}

	// ��� ������ �����Ϳ� ���� �����͸� �����´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ۴�.
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

	// ���� ��� ���ۿ� ���(lock)�� �ɰ� ������ ������ ȹ��
	result = deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result)) { return false; }
	dataPtr2 = (LightBufferType*)mappedResource.pData;

	// copy the lighting variables into the constant buffer.
	// �����͸� �̿��Ͽ� ���� ����� ������ �����Ѵ�.
	// ���� ���� ���ۿ� ���εǰ� �׷����� ���� �ȼ����̴��� ��� ���۷� ���õȴ�.
	dataPtr2->ambientColor = ambientColor;
	dataPtr2->diffuseColor = diffuseColor;
	dataPtr2->lightDirection = lightDirection;
	dataPtr2->specularColor = specularColor;
	dataPtr2->specularPower = specularPower;



	// Unlock the constant buffer.
	deviceContext->Unmap(m_lightBuffer, 0);

	// �ȼ� ���̴��� ���� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// �ȼ� ���̴��� �����ϴ°��̱� ������ PS��������̴�.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}
