#include "ShaderClass.h"



ShaderClass::ShaderClass()
{
}

ShaderClass::ShaderClass(const ShaderClass&)
{
}

ShaderClass::~ShaderClass()
{
}
/*----------------------------------------------------------------------------------------------------------------------
�̸�:OutputShaderErrorMessage()
�뵵: ���� �޼��� ���
------------------------------------------------------------------------------------------------------------------------*/
void ShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;

	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message. 
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to. 
	fout.open("shader-error.txt");

	// Write out the error message.
	for (i = 0; i<bufferSize; i++) { fout << compileErrors[i]; }

	// Close the file. 
	fout.close();

	// Release the error message. 
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.txt for message.", shaderFilename, MB_OK);

	return;
}

/*----------------------------------------------------------------------------------------------------------------------
�̸�: RenderShader()
�뵵:
- InitializeShader()���� Desc�� �����ϰ� Create�س��� �ֵ��� ����������������(I/A)�� ������.
- ����Ұ���!!! ��״� ������ ���������ο� "���̴�"�� �������ִ°Ű�, "����/�ε��� ����"���� ModelClass���� ���ش�.
- �׸��� ���̴��� �̿��� ���� �׸���.

1. �Է� ���̾ƿ��� �Է� �����(�׷��Ƚ� ������ ���������� ù��° �ܰ�)�� �����Ѵ�.
�̷ν� GPU ���� ������ �ڷᱸ���� �˰� �ȴ�.

2.���� ���۸� �׸��� ���� ���� ���̴��� �ȼ� ���̴��� �����Ѵ�.
���̴��� ���� �Ǹ� D3D ����̽� ���ؽ�Ʈ���� DirectX11�� DrawIndexed �Լ��� ����Ͽ� �ﰢ���� �׷�����.
------------------------------------------------------------------------------------------------------------------------*/
void ShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// �������̴� INPUT ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	// �ﰢ���� �׸� ���� ���̴�, �ȼ� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// �ؽ�ó ���÷� ���¸� �ȼ� ���̴��� �����Ѵ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// �ﰢ���� �׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}

void ShaderClass::Shutdown()
{
	ShutdownShader();
}