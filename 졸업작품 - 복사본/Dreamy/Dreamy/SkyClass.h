#pragma once



/*
이름: SkyClass
용도:
1) skydome
- 지형을 중심으로 하는 구 모형의 모델을 만들고 색상을 입힌다.
- 1. 스카이돔 모델(.obj모델)을 불러온다.
- 2. 항상 카메라 위치의 중앙에 구 모형을 불러들여 카메라가 어디를 이동하던 항상 카메라를 둘러싸야 한다.
- 3. 스카이 돔을 렌더링 할 때 뒷표면 컬링을 끄고, Z버퍼를 끈다.
- 4. 픽셀셰이더에서 스카이돔을 렌더링할 때 픽셀의 현재 위치를 전달한다.

2) skyplane
- 구름
- 하늘 평면을 렌더링 하기 위해 깊이 버퍼를 겹쳐 쓰고 배후으 ㅣ모든것을 결합하여 기하학이 실제보다 훨씬 더 큰 환상을 갖도록 한다.
- 하늘과 지형 사이의 약간의 공간 가장자리가 텅 빈 것처럼 보일 수 있으므로 벽, 건물 등 주변 지형을 이용하여 카메라가 안보이게 해결하거나 세번째 알파텍스처로 모서리에 페이드 효과를 만든다.
- 구름과 스카이돔의 색깔이 합쳐져야 한다.

*/


#include <d3d11.h>
#include <D3DX10math.h>
#include <fstream>
#include "TextureClass.h"



using namespace std;

class SkyClass
{
private:
	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};
	//skydome의 정점 구조체는 위치만 원한다.
	//이 구조체를 통해 skydome의 높이에 근거해 color을 생성할 수 있다.
	struct VertexType
	{
		D3DXVECTOR3 position;
	};

	struct SkyPlaneType
	{
		float x, y, z;
		float tu, tv;
	};

	struct SkyPlaneVertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};



public:
	SkyClass();
	SkyClass(const SkyClass&);
	~SkyClass();

	//스카이돔
	bool InitializeSky(ID3D11Device*);
	void Shutdown();
	void RenderSky(ID3D11DeviceContext*);

	int GetIndexCount();

	D3DXVECTOR4 GetApexColor();
	D3DXVECTOR4 GetCenterColor();


	//skyplane(구름)
	bool InitializeCloud(ID3D11Device*, WCHAR*, WCHAR*);
	void RenderCloud(ID3D11DeviceContext*);
	void Frame(float, float, float, float);

	int GetCloudIndexCount();
	ID3D11ShaderResourceView* GetCloudTexture1();
	ID3D11ShaderResourceView* GetCloudTexture2();

	float GetBrightness();
	float GetTranslation(int);
	

private:
	//스카이돔
	bool LoadSkyDomeModel(char*);
	void ReleaseSkyDomeModel();

	bool InitializeSkyBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderSkyBuffers(ID3D11DeviceContext*);

	//skyplane(구름)
	bool InitializeSkyPlane(int, float, float, float, int);

	void RenderCloudBuffers(ID3D11DeviceContext*);
	void ShutdownSkyPlane();

	bool InitializeCloudBuffers(ID3D11Device*, int);



	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTextures();


private:

	//스카이돔
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	D3DXVECTOR4 m_apexColor, m_centerColor;

	//스카이플레인
	SkyPlaneType* m_skyPlane;
	int m_SkyPlanevertexCount, m_SkyPlaneindexCount;
	ID3D11Buffer *m_SkyPlanevertexBuffer, *m_SkyPlaneindexBuffer;
	TextureClass *m_CloudTexture1, *m_CloudTexture2;
	float m_brightness;
	float m_translationSpeed[4]; // 구름 이동 방향
	float m_textureTranslation[4]; // 구름 위치

};

