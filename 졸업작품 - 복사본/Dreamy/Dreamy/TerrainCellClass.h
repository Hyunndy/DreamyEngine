#pragma once


/*
이름: Terrain Cell
용도:
- 16비트 RAW형식의 Terrain의 컬링을 위해 Terrain을 쿼드 트리처럼 사각형으로 나눈다.
- 렌더링 및 개별 지형 셀에 대한 계산 기능을 캡슐화 한다.
*/

#include <d3d11.h>
#include <stdio.h>
#include <d3dx10math.h>



class TerrainCellClass
{
private:
	//Terrain 모델
	struct ModelType
	{
		float x, y, z;
		float tu, tv;// texture
		float nx, ny, nz; // normal
		float tx, ty, tz; // tangent
		float bx, by, bz; // binormal
		float r, g, b;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
		D3DXVECTOR3 color;
	};

	//조명 계산에 사용되는 공유 법선을 위한 구조체
	struct VectorType
	{
		float nx, ny, nz;
	};

	struct ColorVertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};

public:
	TerrainCellClass();
	TerrainCellClass(const TerrainCellClass&);
	~TerrainCellClass();

	bool Initialize(ID3D11Device*, void*, int, int, int, int, int);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	void RenderLineBuffers(ID3D11DeviceContext*);

	int GetVertexCount();
	int GetIndexCount();
	int GetLineBuffersIndexCount();
	void GetCellDimensions(float&, float&, float&, float&, float&, float&);

private:
	bool InitializeBuffers(ID3D11Device*, int, int, int, int, int, ModelType*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	void CalculateCellDimensions();
	bool BuildLineBuffers(ID3D11Device*);
	void ShutdownLineBuffers();

public:
	VectorType* m_vertexList;

private:
	int m_vertexCount, m_indexCount, m_lineIndexCount;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer, *m_lineVertexBuffer, *m_lineIndexBuffer;
	float m_maxWidth, m_maxHeight, m_maxDepth, m_minWidth, m_minHeight, m_minDepth;
	float m_positionX, m_positionY, m_positionZ;


};

