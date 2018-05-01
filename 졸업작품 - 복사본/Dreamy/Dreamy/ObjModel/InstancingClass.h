#pragma once
#include "../System/D3D.h"
#include "MatrixClass.h"
#include "TextureClass.h" // �𵨿� �ؽ�ó ������ �ٿ��� �ϱ� ������!



/*
�̸�: InstancingClass
�뵵: 
- ����, ��ƼŬ�� ���� ��ġ�� �ٲ� �� ���� �Ȱ����͵��� �����ϱ� ���� Class
- ModelClass�� ����Ѵ�.
- ���߿� Image�� ����ؾ��Ҽ���!

- ����� �Բ� ���� ���� ���۸� �����Ͽ� �� ������Ʈ���� �� �ν��Ͻ��� ���� ���� ������ �����ϴ�
  �ν��Ͻ� ���۶�� �ι�° ���۸� ����Ѵ�.
- ���� ���۴� ���� ī�忡 ĳ�� �� ���� �ν��Ͻ� ������ �� �ν��Ͻ��� ���� �����ǰ� �������ȴ�.
**�׷��� ModelClass�� IndexBuffer�� ������� �ʴ´�.

- GraphicsClass���� �ʱ�ȭ�ϱ����� SetInstanceCount(), SetInstancePosition() ���� �� ��.

*/

class InstancingClass :
	public MatrixClass
{
private:
	
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};
	//�ν��Ͻ� �ϰ� ���� ��Ҹ� ����ü�� ���´�. (��ġ, ����, ȸ�� ��)
	struct InstanceType
	{
		D3DXVECTOR3 position;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	InstancingClass();
	InstancingClass(const InstancingClass&);
	~InstancingClass();

	bool Initialize( char*, WCHAR*);

	int GetVertexCount();
	int GetInstanceCount();
	void SetInstanceCount(int);


	void Shutdown();

	void Render();
	void SetInstancePosition(float, float, float);
	void SetInsatanceVariable(float, float, float);

	ID3D11ShaderResourceView* GetTexture();

protected:
	bool InitializeBuffers();
	void ShutdownBuffers();
	void RenderBuffers();

	//�� ���� �׸� �ؽ�ó�� �ҷ����� ��ȯ�ϴµ� ����� LoadTexture&ReleaseTexture
	bool LoadTexture( WCHAR*);
	void ReleaseTexture();

	//�� �����͸� �о���̰� �����ϴ� ������ �ϴ� �Լ���.
	bool LoadModel(char *);
	void ReleaseModel();

private:
	// ���� ���ۿ� �ε��� ����
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer* m_instanceBuffer;

	int m_vertexCount;

	TextureClass* m_Texture;

	ModelType* m_model;


	int m_instanceCount;

	D3DXVECTOR3* InstancePosition;

private:
	float _x, _y, _z;
};

