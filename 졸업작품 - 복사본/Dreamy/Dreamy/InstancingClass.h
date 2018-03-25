#pragma once
#include "MatrixClass.h"
#include <fstream> // 텍스트 파일로 부터 3D 모델을 읽어 그려내기 때문에
#include "TextureClass.h" // 모델에 텍스처 파일을 붙여야 하기 때문에!
#include <iostream>


/*
이름: InstancingClass
용도: 
- 나무, 파티클과 같이 위치만 바뀔 뿐 모델은 똑같은것들을 수행하기 위한 Class
- ModelClass를 상속한다.
- 나중에 Image도 상속해야할수도!

- 형상과 함께 단일 정점 버퍼를 수락하여 모델 지오메트리의 각 인스턴스에 대한 수정 정보를 전달하는
  인스턴스 버퍼라는 두번째 버퍼를 사용한다.
- 정점 버퍼는 비디오 카드에 캐시 된 다음 인스턴스 버퍼의 각 인스턴스에 대해 수정되고 렌더링된다.
**그래서 ModelClass의 IndexBuffer을 사용하지 않는다.

- GraphicsClass에서 초기화하기전에 SetInstanceCount(), SetInstancePosition() 부터 할 것.

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
	//인스턴싱 하고 싶은 요소를 구조체로 묶는다. (위치, 색상, 회전 등)
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

	//이 모델을 그릴 텍스처를 불러오고 반환하는데 사용할 LoadTexture&ReleaseTexture
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	//모델 데이터를 읽어들이고 해제하는 역할을 하는 함수들.
	bool LoadModel(char *);
	void ReleaseModel();

private:
	// 정점 버퍼와 인덱스 버퍼
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer* m_instanceBuffer;

	int m_vertexCount;

	TextureClass* m_Texture;

	ModelType* m_model;


	int m_instanceCount;

	D3DXVECTOR3* InstancePosition;
};

