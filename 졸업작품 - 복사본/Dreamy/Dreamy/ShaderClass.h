#pragma once
/*
	#ifndef
	#endif
	#ifdef
	#pragma once

	-��ó�����
*/



/*
	�߻� Ŭ����(abstract Class)
	- ���� Ŭ�����μ��� �ǹ̸� ���� ��, ��ü�� ������ �������� ���ǵ� Ŭ������ �ƴϴ�.
	- Ŭ���� �߿��� ��ü ������ �������� ���ǵ��� ���� Ŭ����.
	- ���� ���� �Լ��� �ϳ��� ������ ������ �߻� Ŭ���� �̴�.

	���� ���� �Լ�(pure virtual function)
	- �Լ��� ��ü�� ���ǵ��� ���� �Լ�
	- virtual�� ���� �����Լ��� ���� �ڿ� = 0;�� ���� �Լ�
	- �ڽ� Ŭ�������� "������" �����ǰ� �Ǿ���Ѵ�.

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

