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
이름:OutputShaderErrorMessage()
용도: 에러 메세지 출력
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
이름: RenderShader()
용도:
- InitializeShader()에서 Desc를 설정하고 Create해놓은 애들을 렌더링파이프라인(I/A)에 보낸다.
- 기억할것은!!! 얘네는 렌더링 파이프라인에 "셰이더"를 세팅해주는거고, "정점/인덱스 버퍼"들은 ModelClass에서 해준다.
- 그리고 셰이더를 이용해 모델을 그린다.

1. 입력 레이아웃을 입력 어셈블러(그래픽스 렌더링 파이프라인 첫번째 단계)에 연결한다.
이로써 GPU 정점 버퍼의 자료구조를 알게 된다.

2.정점 버퍼를 그리기 위한 정점 셰이더와 픽셀 셰이더를 설정한다.
셰이더가 설정 되면 D3D 디바이스 컨텍스트에서 DirectX11의 DrawIndexed 함수를 사용하여 삼각형을 그려낸다.
------------------------------------------------------------------------------------------------------------------------*/
void ShaderClass::RenderShader( int indexCount)
{
	// 정점쉐이더 INPUT 레이아웃을 설정한다.
	D3D::GetDeviceContext()->IASetInputLayout(m_layout);

	// 삼각형을 그릴 정점 셰이더, 픽셀 셰이더를 설정한다.
	D3D::GetDeviceContext()->VSSetShader(m_vertexShader, NULL, 0);
	D3D::GetDeviceContext()->PSSetShader(m_pixelShader, NULL, 0);

	// 텍스처 샘플러 상태를 픽셀 셰이더에 설정한다.
	D3D::GetDeviceContext()->PSSetSamplers(0, 1, &m_sampleState);

	// 삼각형을 그린다.
	D3D::GetDeviceContext()->DrawIndexed(indexCount, 0, 0);

	return;
}

void ShaderClass::Shutdown()
{
	ShutdownShader();
}