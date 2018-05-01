#pragma once



// Terrain + TerrainShader + ���� Ʈ���� ��ĥ �� ���� ������?

/*
--------------------------------------------------------------------------------------------
�̸�: TerrainClass
�뵵:
1) ���� �ﰢ������ �̷���� Terrain
- 100x100���� �� �׸��⸦ ���� �� ������ �� ������ ����� ĸ��ȭ.
- 3D������ ǥ���ϱ� ���� ���� ���� �����Ѵ�.
- ���̸��� ���Ͽ� ����� ���� ����Ʈ ������ ���Ѵ�.
- ���̸��� �����ϴ� ���� �Ϲ����� ��� = ��Ʈ��, ����, �ؽ�Ʈ �Ǵ� ���� ����
- ���̸��� ���� : ������ ������ ������ 0~255, ��Ʈ��, .raw������ ȸ�� ������ ��⸦ ����Ͽ� ���̸� ��Ÿ����.
- Perlin Noise�˰����� ���� ���̸� ���� �� ��Ʈ�� ���� or .8��Ʈ RAW����

2) ����+�ֺ����� ������ Terrain
- VertexType�� ���� ���� �߰�
- HeightMapType�� ���� ���� �߰�
- ���: �� �ﰢ���� ������ ������ ���� ���� �������� ū �ﰢ�� ������� ǥ�õȴ�.

3) �ؽ�ó ���� Terrain
- textureClass ��� �߰�
- TEXTURE_REPEAT ���� ���� �߰�. �ؽ�ó�� �ݺ��Ǵ� �󵵸� ���� ������ �� �ִ�.

4) ���� Ʈ���� ����� Terrain
- ����Ʈ��Ŭ������ ��ĥ �� ���� ������..?

5) ���� �÷� ����
- ���� �޽��� ���� ������ �����ϱ� ���� �÷� ��Ʈ�� �̹����� ����Ѵ�.
- ��Ʈ�� �̹����� �Ϲ������� ���̸� ������ ���� ��Ʈ�� �ȼ� ������ 1:1 ��ȯ�� �����ϱ� ���� ���̸ʰ� ������ ũ�� �̴�.
- ���� �ʿ��� ���� �� ������ ���� �÷��ʿ��� ���ÿ� �� ������ ���� �д´�. -> �� ������ RGB���� ��Ұ� �����.
- VertexType�� color���� �߰�
- HeightMap�� r,g,b �߰�

------------------------------------------------------������� bitmap Terrain

1) Raw height map
- ��Ʈ�� height map�� ������ �������� �ſ� ���ѵȴ�. -> ���̰� 256���� ���ѵǾ������Ƿ�(8��Ʈ �̱� ����) -> ���� �ذ��� ���� 16��Ʈ ������ ����ؾ� �Ѵ�.
- 16��Ʈ ���̸��� ����ϸ� 65536���� ���̸� ���� �� �ִ�.
- RAW������ ���� ���� ���� ������� �����ϹǷ� �ſ� ���� ���� �� �ִ�.(���OR��Ÿ ������ ��Ÿ �����Ͱ� ���� -> ���� ���� ���� �迭�� �ٷ� ���� �� �ִ�)
�ءظ�Ÿ������: �����Ϳ� ���� ����ȭ�� �����ͷ�, �ٸ� �����͸� ������ �ִ� ������(�Ӽ�����)

- RAW���Ŀ��� Setup.txt������ �ʿ��ϴ�.
- Terrain ���� ��ġ, ����, �ʺ�, ��ĳ�ϸ�, �÷��� ������ ����ִ�.

- Terrain Cell ����(bitmap������ ���� Ʈ��)
- Terrain Cell�� ����ü �ø��� ó���Ѵ�.
--------------------------------------------------------------------------------------------
*/

#include "../System/D3D.h"
#include <stdio.h>
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

	//���� �� �����͸� �����ϴ� ����ü.
	struct HeightMapType
	{
		float x, y, z;
		float nx, ny, nz;
		float r, g, b;
	};

	//Terrain ��
	struct ModelType
	{
		float x, y, z;
		float tu, tv;// texture
		float nx, ny, nz; // normal
		float tx, ty, tz; // tangent
		float bx, by, bz; // binormal
		float r, g, b;
	};

	//���� ��꿡 ���Ǵ� ���� ������ ���� ����ü
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

	bool Initialize( char*, CHAR*, CHAR*);
	void Shutdown();
	//ȣ����Render
	//bool Initializeforwater(char*, CHAR*, CHAR*);
	//bool Renderforwater();
	//void Shutdownforwater();

	int GetVertexCount();
	int GetIndexCount();

	ID3D11ShaderResourceView* GetColorTexture();
	ID3D11ShaderResourceView* GetNormalMapTexture();

	//Terrain Cell
	bool RenderCell( int, FrustumClass*);
	void RenderCellLines( int);

	int GetCellIndexCount(int);
	int GetCellLinesIndexCount(int);
	int GetCellCount();

	//���� ��� �̵�
	bool GetHeightAtPosition(float, float, float&);



	//// QuadTree
	//int GetVertexCount();
	//void CopyVertexArray(void*);
private:

	bool LoadSetupFile(char *);
	// ���� ��
	bool LoadHeightMap_Bitmap(const char*);
	bool LoadHeightMap_Raw();

	//raw�� �߰��Ⱦֵ�
	void SetTerrainCoordinates();
	bool BuildTerrainModel();
	void ShutdownTerrainModel();
	void CalculateTerrainVectors();
	void CalculateTangentBinormal(TempVertexType, TempVertexType, TempVertexType, VectorType&, VectorType&);

	// HeightMap�� �ε�� �� ���� ������ ����ϴ� ���.
	void NormalizeHeightMap();
	bool CalculateNormal();
	void ShutdownHeightMap();

	// Terrain �ؽ�ó
	void CalculateTextureCoordinates();
	bool LoadTexture(  CHAR*, CHAR*);
	void ReleaseTexture();
	
	// �÷� ��
	bool LoadColorMap();

	//ȣ����
	//bool InitializeBufferforwater();
	//void RenderBuffersforwater();
	//void ShutdownBufferforwater();


	//Terrain Cell
	bool LoadTerrainCells();
	void ShutdownTerrainCells();

	//���̱���̵�
	bool CheckHeightOfTriangle(float, float, float&, float[3], float[3], float[3]);

private:
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	float m_heightScale;

	char *m_terrainFilename, *m_colorMapFilename;
	
	ID3D11Buffer* m_vertexBuffer, *m_indexBuffer;
	// ���� �� ���Ͽ��� ���� �����͸� �����Ѵ�.
	HeightMapType* m_heightMap;
	ModelType* m_terrainModel;

	TextureClass* m_ColorTexture;
	TextureClass* m_NormalMapTexture;

	//Terrain Cell
	TerrainCellClass* m_TerrainCells;
	int m_cellCount;


};

