#pragma once
#include "ShaderClass.h"

/*-------------------------------------------------------------------------------------------------------------
�̸�: �׸��ڽ��̴�
���� ����: 
1) �ؽ�ó ������(RTT)
: ����۰� �ƴ� �ؽ�ó���� ����� �׸� �� �ְ� �Ѵ�.(�ſ�, TVȭ�� ��)

2) ���� ����(Z����)
: �������Ҿ��� ��� �ȼ��� ���̸� ����Ѵ�.
: ��κ��� ���� ī��� 32��Ʈ�� ���� ���۸� ������.
: DepthStencil�� ���� ���� ������ DXGI_FORMAT_D24_UNORM_S8_UINT��� �����Ȱ� ���̹���=���ٽ� ���۶�� ��, ���� ä���� 24��Ʈ, ���ٽ� ä���� ��� 8��Ʈ�� �����Ѵٴ°��̴�.
: ���� ������ ���� �ε� �Ҽ����̴�. ������ ����� Ŭ���� ��鿡�� 0.0~1.0�̴�.
: ������ ���� ������ �ƴϰ�, �ε� �Ҽ��� ���� �� 90%�� ����� Ŭ���� ��鿡 ����� ���� ������ ó�� 10%���� �߻��Ѵ�.
  ������ 10%(0.9���� 1.0����)�� ���� ������ ������ 90%�� �����Ѵ�.
: ���� ������ �ƴ� ������ ��ټ��� ���α׷��� ����� ���� ��Ȯ�ϰ�, �ָ��ִ� ���� ���е��� ������� �ʱ� �����̴�.

�׸��ڸ���
: ���� �������� ����� �������Ѵ�.
: ���� �������� ������ �� �� ���� ���� ������ �ؽ�ó ���������� �������Ѵ�. -> ���� ������ ä���� �� �ؽ�ó�� �׸��� ���̶�� �Ѵ�.

*****************
: ����� ������ �� �� �׸��� �� �ؽ�ó�� �ٽ� ���� �����Ͽ� �׸��ڸ� �����ϴ� ��ü�� ���̸� ��� �ȼ� ���̴����� �ȼ� ������ ���� ��ġ�� ���Ѵ�.
: ���� ī�޶� �� �����ٴ°��� �˰ԵǸ� �ȼ��� ���߰�, ��ü�� ī�޶� �� �����ٴ°��� �˰ԵǸ� �ȼ��� �׸��� ó���Ѵ�.
*****************


-------------------------------------------------------------------------------------------------------------*/




class ShadowShaderClass :
	public ShaderClass
{
private:
	struct MatrixBufferType2
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX lightView;
		D3DXMATRIX lightProjection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
	};

	struct LightBufferType2
	{
		D3DXVECTOR3 lightPosition;
		float padding;
	};


public:
	ShadowShaderClass();
	ShadowShaderClass(const ShadowShaderClass&);
	~ShadowShaderClass();

	virtual bool Initialize(HWND);
	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);

private:
	virtual bool InitializeShader(HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();
	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR4);
	virtual void RenderShader(int);

private:
	ID3D11SamplerState* m_sampleStateWrap = nullptr;
	ID3D11SamplerState* m_sampleStateClamp = nullptr;
	
	ID3D11Buffer* m_lightBuffer = nullptr;
	ID3D11Buffer* m_lightBuffer2 = nullptr;
};

