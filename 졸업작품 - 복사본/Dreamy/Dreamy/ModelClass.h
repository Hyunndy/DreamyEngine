////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <fstream> // �ؽ�Ʈ ���Ϸ� ���� 3D ���� �о� �׷����� ������
#include "TextureClass.h" // �𵨿� �ؽ�ó ������ �ٿ��� �ϱ� ������!
#include <iostream>
#include "MatrixClass.h"

using namespace std;
/*
	Ŭ������: ModelClass
	�뵵: 
	3D�𵨵��� ������ �����е��� ĸ��ȭ�ϴ� Ŭ����
	���� ����&�ε��� ���۸� �����.
	���� ����&�ε��� ���۸� ���� ��, ������ ������������ ù��° �ܰ��� �Է� ������(I/A)�� �����ش�.

	�ؽ�Ʈ �����̳� obj���Ϸ� ���� 3D���� �о� �׷��� �� �ִ�.

	�ؽ�ó���� �����Ѵ�.
	���Ɽ�� �ٷ� �� �ְ� �Ǿ���.

	18.01.28 ���� ����(�븻 ����)�� �߰��ϸ鼭 vettextype�� tangent, binormal�� �߰��Ǿ���.
	18.03.18 Picking �߰�
	18.03.25 Instancing�� ���� InstancingClass�� ��ӽ�Ŵ.
*/


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////
class ModelClass : public MatrixClass
{
	
protected:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
	};

private:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float tx, ty, tz;
		float bx, by, bz;
	};

	
	//tangent�� binormal�� ����ϴµ� ���� ����ü��.
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	// 3D���� ���� ���ۿ� �ε��� ���۵��� �ʱ�ȭ�� ���� ������ �����Ѵ�.
	virtual bool Initialize(ID3D11Device*,  char*, WCHAR*);

	//���� ��Ƽ�ؽ��ĵ� �����Ѵ�.
	bool InitializeMulti(ID3D11Device*, char*, WCHAR*, WCHAR*);

	//���� �� ���� �ؽ�ó(���ĸ� ����)�� �����Ѵ�.
	bool InitializeTriple(ID3D11Device*, char*, WCHAR*, WCHAR*, WCHAR*);

	//�������� �����Ǵ� �ؽ�ó�� �ʱ�ȭ�Ѵ�.(tangent, binormal���)
	bool InitializeBumpMap(ID3D11Device*, char*, WCHAR*, WCHAR*);

	//�ݻ���� �����Ǵ� �ؽ�ó�� �ʱ�ȭ�Ѵ�. (tangent, binormal���)
	bool InitializeSpecMap(ID3D11Device*, char*, WCHAR*, WCHAR*, WCHAR*);

	void Shutdown();

	// �׷��� ī�忡 �𵨵��� ���������� �ְ� ���̴��� �׸� �غ� �Ѵ�.
	void Render(ID3D11DeviceContext*);
	
	//���̴����� �ڽ��� �ؽ�ó �ڿ��� �����ϰ� �׸��� ���� GetTexure�Լ�
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetTripleTexture1();
	ID3D11ShaderResourceView* GetTripleTexture2();
	ID3D11ShaderResourceView* GetTripleTexture3();
	ID3D11ShaderResourceView** GetMultiTextureArray();
	ID3D11ShaderResourceView** GetTripleTextureArray();

	int GetIndexCount();

	//Picking �˻�
	bool TestIntersection(int, int, int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR3);
	bool RaySphereIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, float radius);




protected:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	//�� ���� �׸� �ؽ�ó�� �ҷ����� ��ȯ�ϴµ� ����� LoadTexture&ReleaseTexture
	bool LoadTexture(ID3D11Device*, WCHAR*);
	bool LoadMultiTexture(ID3D11Device*, WCHAR*, WCHAR*);
	bool LoadTripleTexture(ID3D11Device*, WCHAR*, WCHAR*, WCHAR*);
	void ReleaseTexture();

	//�� �����͸� �о���̰� �����ϴ� ������ �ϴ� �Լ���.
	bool LoadModel(char *);
	void ReleaseModel();

	//tangent�� binormal�� ����ϱ� ���� �� �Լ��� �����Ѵ�.
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

	
protected:
	// ���� ���ۿ� �ε��� ����
	ID3D11Buffer *m_vertexBuffer;

	// �� ������ ũ�� ������ ���� �ִ� ������(�̰� �̿��ؼ� ���߿� ��ĥ �� �ִ�.)
	int m_vertexCount;


	// �� ���� �ؽ�ó �ڿ��� �ҷ�����, ��ȯ�ϰ�, �����ϴµ� ���ȴ�.
	TextureClass* m_Texture;

	// ModelType���� ��� ����
	// �� �����͸� ���� �� ���� ���ۿ� �������� �� ���� �� �����͸� �����صδµ� ���ȴ�.
	ModelType* m_model;


private:
	ID3D11Buffer *m_indexBuffer;
	int m_indexCount;
};

#endif
/*
3D���� ���� ������ �����ִ� ���Ͽ� �ִ�
3D���� ���˴�� MODELType�� �����ؾ��Ѵ�.

struct ModelType
{
float x, y, z;
float tu, tv;
float nx, ny, nz;
};
*/

/*
/////�ʷϻ� �ﰢ���� �׸� �� �� vertextype.//////
// ���� ���ۿ� ����� ������ ����ü�� �����Ѵ�.
// �̴� ���̴� Ŭ�������� ����� ������ ����ü�� ������ ���ƾ� �Ѵ�.
struct VertexType
{
D3DXVECTOR3 position;
D3DXVECTOR4 color;
};
*/

/*
/////�ؽ�ó�� ���� �ﰢ���� �׸� �� �� vertextype//////
struct VertexType
{
D3DXVECTOR3 position;
D3DXVECTOR2 texture;
};
*/

/*
������ ���ߴ� �ؽ�ó ���� �ﰢ���� �׸� �� �� Vertextype.
��ǥ + �ؽ��� ��ǥ + ��������

struct VertexType
{
D3DXVECTOR3 position;
D3DXVECTOR2 texture;
D3DXVECTOR3 normal;

}; */