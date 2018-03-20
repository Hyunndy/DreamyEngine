#pragma once
#include "ShaderClass.h"

/*------------------------------------------------------------------------------------------------------------------------
�̸�: FireShaderClass
�뵵:
- �غ�: 1) ȸ������ ������ �ؽ���(�Ϸ���) 2) ���� �������� ������ ������� ȭ���� ��谡 ���̴� �ؽ���(��) 3) �Ҳ��� ���¸� ���� �����ؽ���(���)
- ������ �ؽ��ĸ� �̿��� ��,����,���� ��� ���� �ؽ����� ���ø� ��ġ�� ��߷� ���´�.

- ���̴� ���� ����
 1) ������ �ؽ��ĸ� �����ٰ� ���� �ٸ��� �����ϸ��� �� ���� ���� �ٸ� �ؽ��ĸ� �����Ѵ�.
  *�̰͵��� �ܼ��� ���� �ؽ�ó�� �ݺ� �����Ͽ� �������� �ؽ��ĸ� ���� ���̹Ƿ� "��Ÿ��"��� �θ���.
  *�� ������ �ؽ��ĵ��� �������� ��ũ�ѽ��� �Ҳ��� �������� �Ϸ��̴µ��� ������ �ش�.
  *�� ���� �ӵ��� �ٸ��� �ؼ� ���߿� ȥ���Ѵ�.

 2) (0,1)������ ���� ������ �ؽ��ĵ��� (-1,+1)�� ��ȯ�Ѵ�.
  *��ȯ �� �Ϻ� ����� ������� �� �� �� �� ��翡 ������ �ȴ�.
  *��� ������ �ؽ�ó�� (-1,+1)�� ��ȯ �� �� �ؽ����� x��� y���� �ְ��ų �� �ִ�.
  *������ �ؽ��ĵ��� �ְ�ǰ� ���� 3���� �ؽ��ĸ� ���� ȥ���� ���� ������ �ؽ��� ����� �����.
  **�ְ��� ���� x,y�࿡ �̷������ ������ ���ø� ��ġ�� ���ϱ� ���� ��� ���̺� ó�� ���ɰ��̴�.
  **�� ������ �ؽ��� ��� ������ ���� ���� �ٸ� �ӵ��� ���� ��ũ�ѵǱ� ������ �̰͵��� ȥ���ϰ� �Ǹ� �帣�� ���� ����� ��������� �ȴ�.

 3) ���� �Ҳ� ���� �ؽ��ĸ� ��߸���.
  *������ �ؽ�ó�� ����� �ؽ�ó ���ø� ��ġ�� �ٲ۴�.
  *2)������ ������ ��ü�� ���ø� ��ġ�� ��߷ȴ�.
  *�ְ� �����ϰ� ���� ���� ����� �ְ��� �ؽ��� ���ʿ��� �� ũ��, �Ʒ��� ������ ���� �Ͼ���� �Ѵ�. 
  -> ������ ���� �ؽ�ó �Ʒ��ʺ��� ������ y���� �ö󰡸鼭 ���� ������ �� ū ������ �ְ��Ѵ�.
  *�ְ�� ���� ������ �ؽ��ĸ� ��� ���̺� ó�� ����Ͽ� �Ҳ� ���� �ؽ�ó�� ���ø��Ѵ�.
  **���� �ؽ�ó ���ø� �� Wrap��� Clamp�� ����� �ٴ����� �������� �ʰ� �Ѵ�.
 
 4) 3�� ���� ��Ÿ������ ����� �簢���� ���������Ƿ� ���� �Ҳ� ����� ������ ���� �ؽ�ó�� ����Ѵ�.
  *��߷� ���� ������ �ؽ�ó�� ����� ���� �ؽ�ó�� ���ø��ϸ� ���� �ؽ�ó�� Ÿ������ �Ҳ�ó�� ���̰� ���� �� �ִ�.
  *���ø� �� wrap��� clamp�� ����� �Ҳ��� �ٴ� �������� �ؽ�ó�� ������ ����ϴ� ���� �����ؾ� �Ѵ�.

  5) ����Ʈ�� �ϼ��ϱ� ���� ���� ���� ���� �Ҳ� �簢���� ���� ä�ο� ����Ͽ� ���ĺ������� �������� �� �ְ� �Ѵ�.
------------------------------------------------------------------------------------------------------------------------*/


class FireShaderClass :
	public ShaderClass
{
private:
	
	// fire.vs�� �ִ� NoiseBuffer.
	// ����� ����ϱ� ���� ������ �����Ѵ�.
	struct NoiseBufferType
	{
		float frameTime;
		D3DXVECTOR3 scrollSpeeds;
		D3DXVECTOR3 scales;
		float padding;
	};


	// fire.ps�� �ִ� �Ҳ� ȿ���� ���� �� ������ �ؽ�ó ������ ��߸��� ���� ������ �ִ� DistortionBuffer.
	struct DistortionBufferType
	{
		D3DXVECTOR2 distortion1;
		D3DXVECTOR2 distortion2;
		D3DXVECTOR2 distortion3;
		float distortionScale;
		float distortionBias;
	};



public:
	FireShaderClass();
	FireShaderClass(const FireShaderClass&);
	~FireShaderClass();


	virtual bool Initialize(ID3D11Device*, HWND);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
				float, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR2, D3DXVECTOR2, D3DXVECTOR2, float, float);

private:

	virtual bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);

	void ShutdownShader();
	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, ID3D11ShaderResourceView*, ID3D11ShaderResourceView*,
		ID3D11ShaderResourceView*, float, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR2, D3DXVECTOR2, D3DXVECTOR2, float, float);

private:
	ID3D11Buffer* m_noiseBuffer;
	ID3D11SamplerState* m_sampleState2; // wrap��� clamp�� ����ϴ� sampleState2.
	ID3D11Buffer* m_distortionBuffer;
};

