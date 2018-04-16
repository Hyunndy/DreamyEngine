////////////////////////////////////////////////////////////////////////////////
// Filename: InstancingShaderClass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "InstancingShaderClass.h"


InstancingShaderClass::InstancingShaderClass()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;

}


InstancingShaderClass::InstancingShaderClass(const InstancingShaderClass& other)
{
}


InstancingShaderClass::~InstancingShaderClass()
{
}

// ���̴��� �ʱ�ȭ�� �����ϴ� �Լ��� ȣ���Ѵ�.
bool InstancingShaderClass::Initialize( HWND hwnd)
{
	bool result;

	//2. �ؽ�ó �پ��ִ� �ﰢ�� render�ϴ� ���̴�
	result = InitializeShader(hwnd, L"../Dreamy/shader/instancing.vs", L"../Dreamy/shader/instancing.ps");
	if (!result) { return false; }


	return true;
}


bool InstancingShaderClass::Render(int vertexCount, int instanceCount, D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	bool result;

	// �������� ����� ���̴��� ���ڸ� �Է��Ѵ�.
	// ���� ����� ���� ������ �Է� �޴´�.
	result = SetShaderParameters(worldMatrix, viewMatrix, projectionMatrix, texture);
	if (!result) { return false; }

	// ���̴��� �̿��Ͽ� �غ�� ���۸� �׸���.
	RenderShader( vertexCount, instanceCount);

	return true;
}

/*
�뵵:
1. ������ ���̴� ������ �ҷ��´�.
2. DirectX�� GPU���� ��밡���ϵ��� �Ѵ�.

3. ���̾ƿ��� �����ϰ� ��� ���� ������ �����Ͱ� GPU���� ���Ǵ��� �� �� �ִ�.
(�� ���̾ƿ��� modelclass�� color.vs�� ����� VertexType�� ��ġ�ؾ� �Ѵ�.)
*/
bool InstancingShaderClass::InitializeShader(HWND hwnd, WCHAR* vsFilename, WCHAR* psFilename)
{

	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;

	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];

	unsigned int numElements;

	D3D11_BUFFER_DESC matrixBufferDesc;

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
	//-------�ؽ��� �������ִ� �ﰢ���� �ٷ� �� ���� ��-----------
	result = D3DX11CompileFromFile(vsFilename, NULL, NULL, "InstancingVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMessage, NULL);
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

	result = D3DX11CompileFromFile(psFilename, NULL, NULL, "InstancingPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMessage, NULL);
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


	/*
	���� ���̴��� �ȼ� ���̴��� ���۷� �� �����ϵǸ� �̸� �̿��� ���̴� ��ü�� �����Ѵ�.
	*/

	// ���� ���̴� ����
	result = D3D::GetDevice()->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if (FAILED(result))
	{
		return false;
	}

	// �ȼ� ���̴��� ����
	result = D3D::GetDevice()->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
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


	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	/*
	�ν��Ͻ��� ���� ����° ��Ҹ� �߰��Ѵ�.
	�ν��Ͻ��� ��ġ�� �ҰŸ� x,y,z�� ��Ÿ���� �� ���� �ε� �Ҽ������� ������ �߰� ��ġ ���͸� ���̴��� ������ �Ѵ�.
	���� ������ DXGI_FORMAT_R32G32B32_FLOAT�� �����Ѵ�.
	
	�̸��� �׳� TEXCOORD�� �����Ѵ�.
	�ν��Ͻ��̶��� ��Ÿ���� ���� D3D11_INPUT_PER_INSTANCE_DATA�� �����Ѵ�.
	InstanceDataStepRate�� ���Ǿ��� �ν��Ͻ� �����Ϳ��� ������ �ܰ������� �����ϱ� ���� �ϳ��� �ν��Ͻ��� �׸����� �ܰ� �ӵ��� 1�� �����Ѵ�.
	�̰��� �ν��Ͻ� ������ ù��° �����̱� ������ �ν��Ͻ� �����ͷ� ���� ���ۿ� �������� �ʱ� ������ Alignbyteoffset�� �ٽ� 0���� �����Ѵ�.
	*/
	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 1;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 1;
	polygonLayout[2].AlignedByteOffset = 0;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
	polygonLayout[2].InstanceDataStepRate = 1;


	// ���̾ƿ��� ��� ������ �����´�.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	result = D3D::GetDevice()->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
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
	result = D3D::GetDevice()->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if (FAILED(result)) { return false; }

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
	result = D3D::GetDevice()->CreateSamplerState(&samplerDesc, &m_sampleState);
	if (FAILED(result)) { return false; }



	return true;
}


void InstancingShaderClass::ShutdownShader()
{

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
bool InstancingShaderClass::SetShaderParameters( D3DXMATRIX worldMatrix, D3DXMATRIX viewMatrix,
	D3DXMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr; // ����, ��, �������� ����� ������� ��� ��� ����
	unsigned int bufferNumber;

	// ����� transpose�Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
	D3DXMatrixTranspose(&worldMatrix, &worldMatrix);
	D3DXMatrixTranspose(&viewMatrix, &viewMatrix);
	D3DXMatrixTranspose(&projectionMatrix, &projectionMatrix);

	// ��� ������ ������ �� �� �ֵ��� ��ٴ�.
	result = D3D::GetDeviceContext()->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) { return false; }

	// ��� ������ �����Ϳ� ���� �����͸� �����´�.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// ��� ���ۿ� ����� �����Ѵ�.
	dataPtr->world = worldMatrix;
	dataPtr->view = viewMatrix;
	dataPtr->projection = projectionMatrix;

	// ��� ������ ����� Ǭ��.
	D3D::GetDeviceContext()->Unmap(m_matrixBuffer, 0);

	// ���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	bufferNumber = 0;

	// ���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	D3D::GetDeviceContext()->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	D3D::GetDeviceContext()->PSSetShaderResources(0, 1, &texture);



	return true;
}

//�ε��� ���� ��� �ν��Ͻ� ī��Ʈ�� ����Ѵ�.
void InstancingShaderClass::RenderShader(int vertexCount, int instanceCount)
{
	// Set the vertex input layout.
	D3D::GetDeviceContext()->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	D3D::GetDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
	D3D::GetDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	D3D::GetDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	D3D::GetDeviceContext()->DrawInstanced(vertexCount, instanceCount, 0, 0);

	return;
}

