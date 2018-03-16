#pragma once



// Terrain + TerrainShader + 쿼드 트리를 합칠 수 있지 않을까?

/*
--------------------------------------------------------------------------------------------
이름: TerrainClass
용도:
1) 오직 삼각형으로 이루어진 Terrain
- 100x100격자 선 그리기를 위한 모델 데이터 및 렌더링 기능을 캡슐화.
- 3D지형을 표현하기 위한 높이 맵을 구현한다.
- 높이맵은 파일에 저장된 높이 포인트 매핑을 말한다.
- 높이맵을 저장하는 가장 일반적인 방법 = 비트맵, 원시, 텍스트 또는 이진 파일
- 높이맵의 원리 : 지형의 높이의 범위가 0~255, 비트맵, .raw파일은 회색 색상의 밝기를 사용하여 높이를 나타낸다.
- Perlin Noise알고리즘을 통해 높이맵 생성 후 비트맵 저장 or .8비트 RAW형식

2) 방향+주변광이 설정된 Terrain
- VertexType에 법선 벡터 추가
- HeightMapType에 법선 벡터 추가
- 결과: 각 삼각형이 공유된 법선에 의해 조명에 비춰지는 큰 삼각형 목록으로 표시된다.

3) 텍스처 입힌 Terrain
- textureClass 헤더 추가
- TEXTURE_REPEAT 전역 변수 추가. 텍스처가 반복되는 빈도를 쉽게 제어할 수 있다.

4) 쿼드 트리가 적용된 Terrain
- 쿼드트리클래스랑 합칠 수 있지 않을까..?

5) 지형 컬러 매핑
- 지형 메쉬의 정점 색상을 결정하기 위해 컬러 비트맵 이미지를 사용한다.
- 비트맵 이미지는 일반적으로 높이맵 정점에 대한 비트맵 픽셀 색상의 1:1 변환을 보장하기 위해 높이맵과 동일한 크기 이다.
- 높이 맵에서 읽은 각 정점에 대해 컬러맵에서 동시에 그 정점의 색을 읽는다. -> 각 정점에 RGB구성 요소가 생긴다.
- VertexType에 color벡터 추가
- HeightMap에 r,g,b 추가

------------------------------------------------------여기까지 bitmap Terrain

1) Raw height map
- 비트맵 height map은 지형의 디테일이 매우 제한된다. -> 높이가 256으로 제한되어있으므로(8비트 이기 때문) -> 문제 해결을 위해 16비트 형식을 사용해야 한다.
- 16비트 높이맵을 사용하면 65536개의 높이를 가질 수 있다.
- RAW형식은 높이 값만 선형 방식으로 저장하므로 매우 쉽게 읽을 수 있다.(헤더OR기타 유형의 메타 데이터가 없다 -> 따라서 파일 열고 배열로 바로 읽을 수 있다)
※※메타데이터: 데이터에 관한 구조화된 데이터로, 다른 데이터를 설명해 주는 데이터(속성정보)

- RAW형식에는 Setup.txt파일이 필요하다.
- Terrain 파일 위치, 높이, 너비, 스캐일링, 컬러맵 값들이 들어있다.

- Terrain Cell 적용(bitmap파일의 쿼드 트리)
- Terrain Cell의 절두체 컬링을 처리한다.
--------------------------------------------------------------------------------------------
*/

#include <d3d11.h>
#include <d3dx10math.h>
#include <stdio.h>
#include <fstream>
#include "TextureClass.h"
#include "TerrainCellClass.h"
#include "FrustumClass.h"


const int TEXTURE_REPEAT = 4;

class TerrainClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR3 tangent;
		D3DXVECTOR3 binormal;
		D3DXVECTOR3 color;
	};

	//높이 맵 데이터를 보유하는 구조체.
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float r, g, b;
	};

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

	//조명 계산에 사용되는 공유 법선을 위한 구조체
	struct VectorType
	{
		float nx, ny, nz;
	};
	
	struct TempVertexType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	TerrainClass();
	TerrainClass(const TerrainClass&);
	~TerrainClass();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, char*, CHAR*, CHAR*);
	void Shutdown();
	//호수용Render
	bool Initializeforwater(ID3D11Device*, ID3D11DeviceContext*, char*, CHAR*, CHAR*);
	bool Renderforwater(ID3D11DeviceContext*);
	void Shutdownforwater();

	int GetVertexCount();
	int GetIndexCount();

	ID3D11ShaderResourceView* GetColorTexture();
	ID3D11ShaderResourceView* GetNormalMapTexture();

	//Terrain Cell
	bool RenderCell(ID3D11DeviceContext*, int, FrustumClass*);
	void RenderCellLines(ID3D11DeviceContext*, int);

	int GetCellIndexCount(int);
	int GetCellLinesIndexCount(int);
	int GetCellCount();

	//높이 기반 이동
	bool GetHeightAtPosition(float, float, float&);



	//// QuadTree
	//int GetVertexCount();
	//void CopyVertexArray(void*);
private:

	bool LoadSetupFile(char *);
	// 높이 맵
	bool LoadHeightMap_Bitmap(const char*);
	bool LoadHeightMap_Raw();

	//raw로 추가된애들
	void SetTerrainCoordinates();
	bool BuildTerrainModel();
	void ShutdownTerrainModel();
	void CalculateTerrainVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

	// HeightMap이 로드된 후 공유 법선을 계산하는 기능.
	void NormalizeHeightMap();
	bool CalculateNormal();
	void ShutdownHeightMap();

	// Terrain 텍스처
	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, CHAR*, CHAR*);
	void ReleaseTexture();
	
	// 컬러 맵
	bool LoadColorMap();

	//호수용
	bool InitializeBufferforwater(ID3D11Device*);
	void RenderBuffersforwater(ID3D11DeviceContext*);
	void ShutdownBufferforwater();


	//Terrain Cell
	bool LoadTerrainCells(ID3D11Device*);
	void ShutdownTerrainCells();

	//높이기반이동
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

private:
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	float m_heightScale;

	char *m_terrainFilename, *m_colorMapFilename;
	
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;
	// 높이 맵 파일에서 읽은 데이터를 저장한다.
	HeightMapType* m_heightMap;
	ModelType* m_terrainModel;

	TextureClass* m_ColorTexture;
	TextureClass* m_NormalMapTexture;

	//Terrain Cell
	TerrainCellClass* m_TerrainCells;
	int m_cellCount;


};

