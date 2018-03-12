#pragma once
/*
	#ifndef
	#endif
	#ifdef
	#pragma once

	-��ó�����
*/


#include <d3d11.h>
#include <d3dx11tex.h>
#include <d3dx10math.h>
#include <D3DX11async.h>
#include <fstream>
using namespace std;

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

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;


	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*) =0;
	virtual void ShutdownShader() = 0;
	
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);
	void RenderShader(ID3D11DeviceContext*, int);

public:
	ShaderClass();
	ShaderClass(const ShaderClass&);
	~ShaderClass();


	virtual bool Initialize(ID3D11Device*, HWND) = 0;
	void Shutdown();

};

