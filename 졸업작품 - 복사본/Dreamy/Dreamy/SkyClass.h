#pragma once



/*
�̸�: SkyClass
�뵵:
1) skydome
- ������ �߽����� �ϴ� �� ������ ���� ����� ������ ������.
- 1. ��ī�̵� ��(.obj��)�� �ҷ��´�.
- 2. �׻� ī�޶� ��ġ�� �߾ӿ� �� ������ �ҷ��鿩 ī�޶� ��� �̵��ϴ� �׻� ī�޶� �ѷ��ξ� �Ѵ�.
- 3. ��ī�� ���� ������ �� �� ��ǥ�� �ø��� ����, Z���۸� ����.
- 4. �ȼ����̴����� ��ī�̵��� �������� �� �ȼ��� ���� ��ġ�� �����Ѵ�.

2) skyplane
- ����
- �ϴ� ����� ������ �ϱ� ���� ���� ���۸� ���� ���� ������ �Ӹ����� �����Ͽ� �������� �������� �ξ� �� ū ȯ���� ������ �Ѵ�.
- �ϴð� ���� ������ �ణ�� ���� �����ڸ��� �� �� ��ó�� ���� �� �����Ƿ� ��, �ǹ� �� �ֺ� ������ �̿��Ͽ� ī�޶� �Ⱥ��̰� �ذ��ϰų� ����° �����ؽ�ó�� �𼭸��� ���̵� ȿ���� �����.
- ������ ��ī�̵��� ������ �������� �Ѵ�.

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
	//skydome�� ���� ����ü�� ��ġ�� ���Ѵ�.
	//�� ����ü�� ���� skydome�� ���̿� �ٰ��� color�� ������ �� �ִ�.
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

	//��ī�̵�
	bool InitializeSky(ID3D11Device*);
	void Shutdown();
	void RenderSky(ID3D11DeviceContext*);

	int GetIndexCount();

	D3DXVECTOR4 GetApexColor();
	D3DXVECTOR4 GetCenterColor();


	//skyplane(����)
	bool InitializeCloud(ID3D11Device*, WCHAR*, WCHAR*);
	void RenderCloud(ID3D11DeviceContext*);
	void Frame(float, float, float, float);

	int GetCloudIndexCount();
	ID3D11ShaderResourceView* GetCloudTexture1();
	ID3D11ShaderResourceView* GetCloudTexture2();

	float GetBrightness();
	float GetTranslation(int);
	

private:
	//��ī�̵�
	bool LoadSkyDomeModel(char*);
	void ReleaseSkyDomeModel();

	bool InitializeSkyBuffers(ID3D11Device*);
	void ReleaseBuffers();
	void RenderSkyBuffers(ID3D11DeviceContext*);

	//skyplane(����)
	bool InitializeSkyPlane(int, float, float, float, int);

	void RenderCloudBuffers(ID3D11DeviceContext*);
	void ShutdownSkyPlane();

	bool InitializeCloudBuffers(ID3D11Device*, int);



	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTextures();


private:

	//��ī�̵�
	int m_vertexCount, m_indexCount;
	ModelType* m_model;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	D3DXVECTOR4 m_apexColor, m_centerColor;

	//��ī���÷���
	SkyPlaneType* m_skyPlane;
	int m_SkyPlanevertexCount, m_SkyPlaneindexCount;
	ID3D11Buffer *m_SkyPlanevertexBuffer, *m_SkyPlaneindexBuffer;
	TextureClass *m_CloudTexture1, *m_CloudTexture2;
	float m_brightness;
	float m_translationSpeed[4]; // ���� �̵� ����
	float m_textureTranslation[4]; // ���� ��ġ

};

