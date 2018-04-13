////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include "D3D.h"
#include "TextureClass.h" // 모델에 텍스처 파일을 붙여야 하기 때문에!
#include "MatrixClass.h"

using namespace std;
/*
	클래스명: ModelClass
	용도: 
	3D모델들의 복잡한 기하학들을 캡슐화하는 클래스
	정점 버퍼&인덱스 버퍼를 만든다.
	정점 버퍼&인덱스 버퍼를 만든 후, 렌더링 파이프라인의 첫번째 단계인 입력 조리기(I/A)에 보내준다.

	텍스트 파일이나 obj파일로 부터 3D모델을 읽어 그려낼 수 있다.

	텍스처링을 지원한다.
	방향광을 다룰 수 있게 되었다.

	18.01.28 범프 매핑(노말 매핑)을 추가하면서 vettextype에 tangent, binormal이 추가되었다.
	18.03.18 Picking 추가
	18.03.25 Instancing을 위해 InstancingClass에 상속시킴.
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

	
	//tangent와 binormal을 계산하는데 사용될 구조체들.
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

	// 3D모델의 정점 버퍼와 인덱스 버퍼들의 초기화와 종료 과정을 제어한다.
	virtual bool Initialize( char*, WCHAR*);

	//모델의 멀티텍스쳐도 지원한다.
	bool InitializeMulti(char*, WCHAR*, WCHAR*);

	//모델의 세 개의 텍스처(알파맵 포함)을 지원한다.
	bool InitializeTriple(char*, WCHAR*, WCHAR*, WCHAR*);

	//범프맵이 지원되는 텍스처를 초기화한다.(tangent, binormal사용)
	bool InitializeBumpMap(char*, WCHAR*, WCHAR*);

	//반사맵이 지원되는 텍스처를 초기화한다. (tangent, binormal사용)
	bool InitializeSpecMap( char*, WCHAR*, WCHAR*, WCHAR*);

	void Shutdown();

	// 그래픽 카드에 모델들의 기하정보를 넣고 셰이더로 그릴 준비를 한다.
	void Render();
	
	//셰이더에게 자신의 텍스처 자원을 전달하고 그리기 위한 GetTexure함수
	ID3D11ShaderResourceView* GetTexture();
	ID3D11ShaderResourceView* GetTripleTexture1();
	ID3D11ShaderResourceView* GetTripleTexture2();
	ID3D11ShaderResourceView* GetTripleTexture3();
	ID3D11ShaderResourceView** GetMultiTextureArray();
	ID3D11ShaderResourceView** GetTripleTextureArray();

	int GetIndexCount();

	//Picking 검사
	bool TestIntersection(int, int, int, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR3);
	bool RaySphereIntersect(D3DXVECTOR3 rayOrigin, D3DXVECTOR3 rayDirection, float radius);




protected:
	bool InitializeBuffers();
	void ShutdownBuffers();
	void RenderBuffers();

	//이 모델을 그릴 텍스처를 불러오고 반환하는데 사용할 LoadTexture&ReleaseTexture
	bool LoadTexture(WCHAR*);
	bool LoadMultiTexture( WCHAR*, WCHAR*);
	bool LoadTripleTexture( WCHAR*, WCHAR*, WCHAR*);
	void ReleaseTexture();

	//모델 데이터를 읽어들이고 해제하는 역할을 하는 함수들.
	bool LoadModel(char *);
	void ReleaseModel();

	//tangent와 binormal을 계산하기 위해 세 함수를 선언한다.
	void CalculateModelVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);
	void CalculateNormal(VectorType, VectorType, VectorType&);

	
protected:
	// 정점 버퍼와 인덱스 버퍼
	ID3D11Buffer *m_vertexBuffer;

	// 각 버퍼의 크기 정보를 갖고 있는 변수들(이걸 이용해서 나중에 합칠 수 있다.)
	int m_vertexCount;


	// 이 모델의 텍스처 자원을 불러오고, 반환하고, 접근하는데 사용된다.
	TextureClass* m_Texture;

	// ModelType형의 멤버 변수
	// 모델 데이터를 읽은 뒤 정점 버퍼에 놓여지기 전 까지 이 데이터를 저장해두는데 사용된다.
	ModelType* m_model;


private:
	ID3D11Buffer *m_indexBuffer;
	int m_indexCount;
};

#endif
/*
3D모델의 기하 정보가 써져있는 파일에 있는
3D모델의 포맷대로 MODELType을 지정해야한다.

struct ModelType
{
float x, y, z;
float tu, tv;
float nx, ny, nz;
};
*/

/*
/////초록색 삼각형을 그릴 때 쓴 vertextype.//////
// 정점 버퍼에 사용할 정점의 구조체를 선언한다.
// 이는 셰이더 클래스에서 사용할 정점의 구조체와 구조가 같아야 한다.
struct VertexType
{
D3DXVECTOR3 position;
D3DXVECTOR4 color;
};
*/

/*
/////텍스처만 입힌 삼각형을 그릴 때 쓴 vertextype//////
struct VertexType
{
D3DXVECTOR3 position;
D3DXVECTOR2 texture;
};
*/

/*
조명이 비추는 텍스처 입힌 삼각형을 그릴 때 쓴 Vertextype.
좌표 + 텍스쳐 좌표 + 법선벡터

struct VertexType
{
D3DXVECTOR3 position;
D3DXVECTOR2 texture;
D3DXVECTOR3 normal;

}; */