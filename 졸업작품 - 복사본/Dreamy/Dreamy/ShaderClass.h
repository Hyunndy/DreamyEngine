#pragma once
/*
	#ifndef
	#endif
	#ifdef
	#pragma once

	-전처리기들
*/



/*
	추상 클래스(abstract Class)
	- 기초 클래스로서만 의미를 가질 뿐, 객체의 생성을 목적으로 정의된 클래스가 아니다.
	- 클래스 중에서 객체 생성을 목적으로 정의되지 않은 클래스.
	- 순수 가상 함수를 하나라도 가지고 있으면 추상 클래스 이다.

	순수 가상 함수(pure virtual function)
	- 함수의 몸체가 정의되지 않은 함수
	- virtual이 붙은 가상함수의 원형 뒤에 = 0;이 붙은 함수
	- 자식 클래스에서 "무조건" 재정의가 되어야한다.

*/
#include "D3D.h"

class ShaderClass
{
private:
	
protected:

	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	ID3D11VertexShader* m_vertexShader = nullptr;
	ID3D11PixelShader* m_pixelShader = nullptr;
	ID3D11InputLayout* m_layout = nullptr;
	ID3D11Buffer* m_matrixBuffer = nullptr;
	ID3D11SamplerState* m_sampleState = nullptr;


	virtual bool InitializeShader( HWND, WCHAR*, WCHAR*) =0;
	virtual void ShutdownShader() = 0;
	
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	virtual void RenderShader( int);

public:
	ShaderClass();
	ShaderClass(const ShaderClass&);
	~ShaderClass();


	virtual bool Initialize(HWND) = 0;
	void Shutdown();

};

