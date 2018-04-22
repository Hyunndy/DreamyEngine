#pragma once
#include "ShaderClass.h"


/*
�̸�: WaterShaderClass
�뵵: 
- water �������̴�, �ȼ����̴��� ���� ���� �������Ѵ�.
- ��= �� ǥ��(�ݻ�) + �� ����(����) + �븻��(���� ����ġ) -> ������������ ������ �̾Ƴ���.
- �� ǥ��: �ݻ� �ؽ�ó(���� ����, ī�޶��� ��ġ, ������ �̿��� �� ���� ������ �ݻ�)
- �� ����: ���� �ؽ�ó(�ݻ��� �ݴ�)
- �� = �ݻ��ؽ�ó+�����ؽ�ó�� ������������ ȥ��

- �� = �븻�� Ÿ�ϸ� 2��(�ܹ���) + �ݻ��ؽ�ó + �����ؽ�ó + ī�޶� ��ġ�� �̿��� ������ + specular light 

- �ݻ罦�̴��� ����� ����� ������������ ������ Ŭ�� ����� ����� �� �Ʒ��� ��� ���� �������Ͽ� ������ �����.
- �̰͵��� ��� �ؽ�ó ���������� �������ǹǷ� ī�޶��� ������ �������� ���� �ؽ�ó�� ����Ͽ� �� ���忡 ������ �� �ִ�.
- �� ���� �������� �ٵ��ǽ����� �迭�� ��ָ� ����� ȥ���� �ܹ����� �ִϸ��̼����� ǥ���Ѵ�.

*/
class WaterShaderClass :
	public ShaderClass
{
private:
	
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX reflection; // ���� ���� �������� �ݻ� ����� ������ֱ� ������ ReflectionBufferType�� �ʿ����.
	};
	//������, specular light�� �̿��ϱ� ���� camnormal����.
	struct CamNormBufferType
	{
		D3DXVECTOR3 cameraPosition;
		float padding1;
		D3DXVECTOR2 normalMapTiling;
		D3DXVECTOR2 padding2;
	};

	//�ݻ縦 ���� ���� �������� �ݻ� ����� ����� ���� �ݻ� ���.
//struct ReflectionBufferType
//{
//	D3DXMATRIX reflection;
//};

	//�ȼ����̴����� ���� �׷��ֱ� ���� �� ����
	struct WaterBufferType
	{
		D3DXVECTOR4 refractionTint;
		D3DXVECTOR3 lightDirection;
		float waterTranslation;
		float reflectRefractScale;
		float specularShininess;
		D3DXVECTOR2 padding;
	};


public:
	WaterShaderClass();
	WaterShaderClass(const WaterShaderClass&);
	~WaterShaderClass();

	bool Initialize( HWND);
	void Shutdown();
	
	bool Render( int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		/*ID3D11ShaderResourceView*,*/ D3DXVECTOR3, D3DXVECTOR2, float, float, D3DXVECTOR4, D3DXVECTOR3, float);


private:
	virtual bool InitializeShader( HWND, WCHAR*, WCHAR*);
	virtual void ShutdownShader();


	bool SetShaderParameters( D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		/*ID3D11ShaderResourceView*,*/ D3DXVECTOR3, D3DXVECTOR2, float, float, D3DXVECTOR4, D3DXVECTOR3, float);

private:
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_camNormBuffer;
	ID3D11Buffer* m_waterBuffer;
	
};

