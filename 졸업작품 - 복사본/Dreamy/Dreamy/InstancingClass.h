#pragma once
#include "MatrixClass.h"
#include <fstream> // �ؽ�Ʈ ���Ϸ� ���� 3D ���� �о� �׷����� ������
#include "TextureClass.h" // �𵨿� �ؽ�ó ������ �ٿ��� �ϱ� ������!
#include <iostream>


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

	bool Initialize(ID3D11Device*, char*, WCHAR*);

	int GetVertexCount();
	int GetInstanceCount();
	void SetInstanceCount(int);

	void Shutdown();

	void Render(ID3D11DeviceContext*);
	void SetInstancePosition(float, float, float);

	ID3D11ShaderResourceView* GetTexture();

protected:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	//�� ���� �׸� �ؽ�ó�� �ҷ����� ��ȯ�ϴµ� ����� LoadTexture&ReleaseTexture
	bool LoadTexture(ID3D11Device*, WCHAR*);
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
};

